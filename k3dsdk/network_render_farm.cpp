// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "fstream.h"
#include "inetwork_render_frame.h"
#include "inetwork_render_job.h"
#include "network_render_farm.h"
#include "network_render_farm_detail.h"
#include "options.h"
#include "result.h"
#include "shader_cache.h"
#include "share.h"
#include "string_cast.h"
#include "system.h"
#include "xml.h"
using namespace k3d::xml;

#include <list>
#include <iostream>
#include <vector>

//#include <errno.h>

namespace k3d
{

namespace detail
{

/// Stores a reference to the global render farm instance
inetwork_render_farm* g_render_farm = 0;

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// network_render_frame

class network_render_frame :
	public inetwork_render_frame
{
public:
	network_render_frame(const filesystem::path& JobPath, const string_t& Name) :
		m_Path(JobPath / filesystem::native_path(ustring::from_utf8(Name)))
	{
		if(!filesystem::create_directory(m_Path))
			log() << error << "Error creating frame directory [" << m_Path.native_console_string() << "]" << std::endl;
	}

	~network_render_frame()
	{
		for(commands_t::const_iterator command = m_commands.begin(); command != m_commands.end(); ++command)
			delete *command;
	}

	const filesystem::path add_file(const string_t& Name)
	{
		// Sanity checks ...
		assert_warning(Name.size());

		// Make sure the filepath is unique ...
		unsigned long index = 0;
		string_t name = Name;
		while(std::count(m_files.begin(), m_files.end(), name))
			name = Name + '-' + string_cast(++index);

		m_files.push_back(name);

		return m_Path / filesystem::generic_path(name);
	}

	void add_exec_command(const string_t& Binary, const environment& Environment, const arguments& Arguments)
	{
		// Sanity checks ...
		assert_warning(!Binary.empty());

		m_commands.push_back(new exec_command(Binary, Environment, Arguments));
	}

	void add_copy_command(const filesystem::path& Source, const filesystem::path& Target)
	{
		// Sanity checks ...
		assert_warning(!Source.empty());
		assert_warning(!Target.empty());

		m_commands.push_back(new copy_command(Source, Target));
	}

	void add_view_command(const filesystem::path& File)
	{
		m_commands.push_back(new view_command(File));
	}

	void write_control_file()
	{
		// Create the empty control file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("control.k3d"));

		// Create the XML document ...
		element xml_document("k3dml");
		element& xml_frame = xml_document.append(element("frame"));

		for(commands_t::const_iterator command = m_commands.begin(); command != m_commands.end(); ++command)
		{
			if(exec_command* const exec = dynamic_cast<exec_command*>(*command))
			{
				element& xml_command = xml_frame.append(element("command"));
				element& xml_exec = xml_command.append(element("exec",
					attribute("binary", exec->m_binary)));

				element& xml_environment = xml_exec.append(element("environment"));
				for(environment::const_iterator variable = exec->m_environment.begin(); variable != exec->m_environment.end(); ++variable)
				{
					xml_environment.append(element("variable",
						attribute("name", variable->name),
						attribute("value", variable->value)));
				}

				element& xml_arguments = xml_exec.append(element("arguments"));
				for(arguments::const_iterator argument = exec->m_arguments.begin(); argument != exec->m_arguments.end(); ++argument)
				{
					xml_arguments.append(element("argument",
						attribute("value", argument->value)));
				}
			}
			else if(copy_command* const copy = dynamic_cast<copy_command*>(*command))
			{
				xml_frame.append(element("command",
					element("copy",
						attribute("source", copy->m_source.native_utf8_string().raw()),
						attribute("target", copy->m_target.native_utf8_string().raw()))));
			}
			else if(view_command* const view = dynamic_cast<view_command*>(*command))
			{
				xml_frame.append(element("command",
					element("view",
						attribute("file", view->m_file.native_utf8_string().raw()))));
			}
			else
			{
				assert_not_reached();
			}
		}

		// Write the document ...
		file << xml_document << std::endl;
	}

	void mark_ready()
	{
		// Create the empty "ready" status file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("ready"));

		// Boost::filesystem does not rename empty files
		file << "K-3D renderfarm status file";
	}

private:
	/// Stores the filesystem path for this job
	const filesystem::path m_Path;
	/// Stores the set of input files used during rendering
	typedef std::vector<string_t> files_t;
	files_t m_files;

	/// Defines a command to be executed
	class command
	{
	public:
		virtual ~command() {}

	protected:
		command() {}
		command(const command&) {}
		command& operator=(const command&) { return *this; }
	};

	class exec_command :
		public command
	{
	public:
		exec_command(const string_t& Binary, const environment& Environment, const arguments& Arguments) :
			m_binary(Binary),
			m_environment(Environment),
			m_arguments(Arguments)
		{
		}

		const string_t m_binary;
		const environment m_environment;
		const arguments m_arguments;
	};
	
	class copy_command :
		public command
	{
	public:
		copy_command(const filesystem::path& Source, const filesystem::path& Target) :
			m_source(Source),
			m_target(Target)
		{
		}

		const filesystem::path m_source;
		const filesystem::path m_target;
	};

	class view_command :
		public command
	{
	public:
		view_command(const filesystem::path& File) :
			m_file(File)
		{
		}

		const filesystem::path m_file;
	};

	typedef std::vector<command*> commands_t;
	commands_t m_commands;
};

/////////////////////////////////////////////////////////////////////////////
// network_render_job

class network_render_job :
	public inetwork_render_job
{
public:
	network_render_job(const filesystem::path Path, const string_t JobName) :
		m_Path(Path / filesystem::generic_path(JobName))
	{
		try
		{
			create_directory(m_Path);
		}
		catch(const std::exception& ex)
		{
			log() << error << k3d_file_reference << ": Error creating job directory [" << m_Path.native_console_string() << "]" << std::endl;
		}
	}

	inetwork_render_frame& create_frame(const string_t& FrameName)
	{
		m_frames.push_back(network_render_frame(m_Path, FrameName));
		return m_frames.back();
	}

	bool write_control_files()
	{
		// Create a control file for each frame ...
		std::for_each(m_frames.begin(), m_frames.end(), std::mem_fun_ref(&network_render_frame::write_control_file));

		// Create the control file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("control.k3d"));

		// Create the XML document ...
		element document("k3dml");

		// Write the document ...
		file << xml::declaration() << document << std::endl;

		return true;
	}

	bool mark_ready()
	{
		// Mark each frame as "ready" ...
		std::for_each(m_frames.begin(), m_frames.end(), std::mem_fun_ref(&network_render_frame::mark_ready));

		// Create the "ready" status file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("ready"));

		// Boost::filesystem does not rename empty files
		file << "Status file." << std::ends;

		return true;
	}

	const string_t path()
	{
		return m_Path.native_utf8_string().raw();
	}

private:
	const filesystem::path m_Path;

	typedef std::list<network_render_frame> frames_t;
	frames_t m_frames;
};

/////////////////////////////////////////////////////////////////////////////
// network_render_farm::implementation

class network_render_farm::implementation
{
public:
	implementation(const filesystem::path& OptionsPath) :
		m_options_path(OptionsPath)
	{
	}

	inetwork_render_job& create_job(const string_t& JobName)
	{
		// Sanity checks ...
		assert_warning(JobName.size());

		// Render farm path ...
		const filesystem::path job_path = options::get_path(options::path::render_farm());

		// Ensure that the job gets a unique name ...
		unsigned long index = 0;
		string_t job_name(JobName);
		while(filesystem::exists(job_path / filesystem::generic_path(job_name)))
			job_name = JobName + '-' + string_cast(index++);

		m_jobs.push_back(network_render_job(job_path, job_name));
		return m_jobs.back();
	}

	void start_job(inetwork_render_job& Job)
	{
		// Make sure it's one of ours ...
		network_render_job* const job = dynamic_cast<network_render_job*>(&Job);
		return_if_fail(job);

		// Update control files for the job and all its frames ...
		if(!job->write_control_files())
		{
			log() << error << "Error writing render job control files" << std::endl;
			return;
		}

		// Mark the job ready-to-run ...
		if(!job->mark_ready())
		{
			log() << error << "Error marking render job ready" << std::endl;
			return;
		}

		// Start the local rendering process ...
		string_t commandline = "k3d-renderjob \"";
		commandline += job->path();
		commandline += "\"";

		if(!system::spawn_async(commandline))
		{
			log() << error << "Error starting render job " << commandline << std::endl;
			return;
		}
	}

private:
	const filesystem::path m_options_path;
	typedef std::list<network_render_job> jobs_t;
	jobs_t m_jobs;
};


/////////////////////////////////////////////////////////////////////////////
// network_render_farm

network_render_farm::network_render_farm(const filesystem::path& OptionsPath) :
	m_implementation(new implementation(OptionsPath))
{
}

network_render_farm::~network_render_farm()
{
	delete m_implementation;
}

inetwork_render_job& network_render_farm::create_job(const string_t& JobName)
{
	return m_implementation->create_job(JobName);
}

void network_render_farm::start_job(inetwork_render_job& Job)
{
	m_implementation->start_job(Job);
}

/////////////////////////////////////////////////////////////////////////////
// set_network_render_farm

void set_network_render_farm(inetwork_render_farm& RenderFarm)
{
	return_if_fail(!detail::g_render_farm);
	detail::g_render_farm = &RenderFarm;
}

/////////////////////////////////////////////////////////////////////////////
// render_farm

inetwork_render_farm& get_network_render_farm()
{
	assert_critical(detail::g_render_farm);
	return *detail::g_render_farm;
}

} // namespace k3d

