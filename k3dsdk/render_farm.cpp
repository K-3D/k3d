// K-3D
// Copyright © 1995-2006, Timothy M. Shead
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
#include "irender_frame.h"
#include "irender_job.h"
#include "options.h"
#include "render_farm.h"
#include "render_farm_detail.h"
#include "result.h"
#include "shader_cache.h"
#include "share.h"
#include "string_cast.h"
#include "system.h"
#include "xml.h"
using namespace k3d::xml;

#include <list>
#include <iostream>

#include <errno.h>

namespace k3d
{

namespace detail
{

/// Stores a reference to the global render farm instance
irender_farm* g_render_farm = 0;

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// render_frame_implementation

class render_frame_implementation :
	public irender_frame
{
public:
	render_frame_implementation(const filesystem::path& JobPath, const std::string& Name) :
		m_Path(JobPath / filesystem::native_path(ustring::from_utf8(Name)))
	{
		if(!filesystem::create_directory(m_Path))
			log() << error << "Error creating frame directory [" << m_Path.native_console_string() << "]" << std::endl;
	}

	const filesystem::path add_input_file(const std::string& Name)
	{
		// Sanity checks ...
		assert_warning(Name.size());

		// Make sure the filepath is unique ...
		unsigned long index = 0;
		std::string name(Name);
		while(std::find(m_input_files.begin(), m_input_files.end(), name) != m_input_files.end())
			name = Name + '-' + string_cast(++index);

		m_input_files.push_back(name);

		return m_Path / filesystem::generic_path(name);
	}

	const filesystem::path add_output_file(const std::string& Name)
	{
		// Sanity checks ...
		assert_warning(Name.size());

		// Make sure the filepath is unique ...
		unsigned long index = 0;
		std::string name(Name);
		while(std::find(m_output_files.begin(), m_output_files.end(), name) != m_output_files.end())
			name = Name + '-' + string_cast(++index);

		m_output_files.push_back(name);

		return m_Path / filesystem::generic_path(name);
	}

	void add_render_operation(const std::string& Type, const std::string& Name, const filesystem::path& SourceFilePath, const bool VisibleRender)
	{
		// Sanity checks ...
		assert_warning(!Type.empty());
		assert_warning(!Name.empty());
		assert_warning(!SourceFilePath.empty());

		m_render_operations.push_back(render_operation(Type, Name, SourceFilePath, VisibleRender));
	}

	void add_copy_operation(const filesystem::path& SourceFilePath, const filesystem::path& TargetFilePath)
	{
		// Sanity checks ...
		assert_warning(!SourceFilePath.empty());
		assert_warning(!TargetFilePath.empty());

		m_copy_operations.push_back(copy_operation(SourceFilePath, TargetFilePath));
	}

	void add_view_operation(const filesystem::path& FilePath)
	{
		m_view_files.push_back(FilePath);
	}

	void write_control_file()
	{
		// Create the empty control file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("control.k3d"));

		// Create the XML document ...
		element document("k3dml");
		element& frame = document.append(element("frame"));

		// Specify which files will be used as input for rendering the frame ...
		for(input_files_t::const_iterator inputfile = m_input_files.begin(); inputfile != m_input_files.end(); ++inputfile)
		{
			frame.append(element("inputfile", attribute("path", *inputfile)));
		}

		// Specify which files will be generated during the render process ...
		for(output_files_t::const_iterator outputfile = m_output_files.begin(); outputfile != m_output_files.end(); ++outputfile)
		{
			frame.append(element("outputfile", attribute("path", *outputfile)));
		}

		// Specify the steps necessary to render ...
		for(render_operations_t::const_iterator renderop = m_render_operations.begin(); renderop != m_render_operations.end(); ++renderop)
		{
			frame.append(element("renderoperation",
				attribute("type", renderop->type),
				attribute("name", renderop->name),
				attribute("sourcepath", renderop->source_path.native_utf8_string().raw()),
				attribute("shaderspath", shader_cache_path().native_utf8_string().raw()),
				attribute("sharepath", share_path().native_utf8_string().raw()),
				attribute("visiblerender", renderop->visible_render)));
		}

		// Specify any copy operations to be performed after the render ...
		for(copy_operations_t::const_iterator copyop = m_copy_operations.begin(); copyop != m_copy_operations.end(); ++copyop)
		{
			frame.append(element("copyoperation",
				attribute("from", copyop->from.native_utf8_string().raw()),
				attribute("to", copyop->to.native_utf8_string().raw())));
		}

		// Specify any viewing operations to be performed after the render ...
		for(view_files_t::const_iterator viewop = m_view_files.begin(); viewop != m_view_files.end(); ++viewop)
			frame.append(element("viewoperation", attribute("path", viewop->native_utf8_string().raw())));

		// Write the document ...
		file << document << std::endl;
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
	typedef std::vector<std::string> input_files_t;
	input_files_t m_input_files;
	/// Stores the set of output files generated by rendering
	typedef std::vector<std::string> output_files_t;
	output_files_t m_output_files;
	/// Defines a render operation to be performed
	struct render_operation
	{
		explicit render_operation(const std::string& Type, const std::string& Name, const filesystem::path& SourcePath, const bool VisibleRender) :
			type(Type),
			name(Name),
			source_path(SourcePath),
			visible_render(VisibleRender)
		{
		}

		const std::string type;
		const std::string name;
		const filesystem::path source_path;
		const bool visible_render;
	};
	typedef std::list<render_operation> render_operations_t;
	render_operations_t m_render_operations;
	/// Defines a post-render copy operation
	struct copy_operation
	{
		explicit copy_operation(const filesystem::path& From, const filesystem::path& To) :
			from(From),
			to(To)
		{
		}

		const filesystem::path from;
		const filesystem::path to;
	};
	typedef std::list<copy_operation> copy_operations_t;
	copy_operations_t m_copy_operations;
	/// Stores an array of files to view once rendering is complete
	typedef std::vector<filesystem::path> view_files_t;
	view_files_t m_view_files;
};

/////////////////////////////////////////////////////////////////////////////
// render_job_implementation

class render_job_implementation :
	public irender_job
{
public:
	render_job_implementation(const filesystem::path Path, const std::string JobName) :
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

	irender_frame& create_frame(const std::string& FrameName)
	{
		m_frames.push_back(render_frame_implementation(m_Path, FrameName));
		return m_frames.back();
	}

	bool write_control_files()
	{
		// Create a control file for each frame ...
		std::for_each(m_frames.begin(), m_frames.end(), std::mem_fun_ref(&render_frame_implementation::write_control_file));

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
		std::for_each(m_frames.begin(), m_frames.end(), std::mem_fun_ref(&render_frame_implementation::mark_ready));

		// Create the "ready" status file ...
		filesystem::ofstream file(m_Path / filesystem::generic_path("ready"));

		// Boost::filesystem does not rename empty files
		file << "Status file." << std::ends;

		return true;
	}

	const std::string path()
	{
		return m_Path.native_utf8_string().raw();
	}

private:
	const filesystem::path m_Path;

	typedef std::list<render_frame_implementation> frames_t;
	frames_t m_frames;
};

/////////////////////////////////////////////////////////////////////////////
// render_farm_implementation::implementation

class render_farm_implementation::implementation
{
public:
	implementation(const filesystem::path& OptionsPath) :
		m_options_path(OptionsPath)
	{
	}

	irender_job& create_job(const std::string& JobName)
	{
		// Sanity checks ...
		assert_warning(JobName.size());

		// Render farm path ...
		const filesystem::path job_path = options::get_path(options::path::render_farm());

		// Ensure that the job gets a unique name ...
		unsigned long index = 0;
		std::string job_name(JobName);
		while(filesystem::exists(job_path / filesystem::generic_path(job_name)))
			job_name = JobName + '-' + string_cast(index++);

		m_jobs.push_back(render_job_implementation(job_path, job_name));
		return m_jobs.back();
	}

	void start_job(irender_job& Job)
	{
		// Make sure it's one of ours ...
		render_job_implementation* const job = dynamic_cast<render_job_implementation*>(&Job);
		assert_warning(job);

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
		std::string commandline = "k3d-renderjob \"";
		commandline += m_options_path.native_filesystem_string();
		commandline += "\" \"";
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
	typedef std::list<render_job_implementation> jobs_t;
	jobs_t m_jobs;
};


/////////////////////////////////////////////////////////////////////////////
// render_farm_implementation

render_farm_implementation::render_farm_implementation(const filesystem::path& OptionsPath) :
	m_implementation(new implementation(OptionsPath))
{
}

render_farm_implementation::~render_farm_implementation()
{
	delete m_implementation;
}

irender_job& render_farm_implementation::create_job(const std::string& JobName)
{
	return m_implementation->create_job(JobName);
}

void render_farm_implementation::start_job(irender_job& Job)
{
	m_implementation->start_job(Job);
}

/////////////////////////////////////////////////////////////////////////////
// set_render_farm

void set_render_farm(irender_farm& RenderFarm)
{
	return_if_fail(!detail::g_render_farm);
	detail::g_render_farm = &RenderFarm;
}

/////////////////////////////////////////////////////////////////////////////
// render_farm

irender_farm& render_farm()
{
	assert_critical(detail::g_render_farm);
	return *detail::g_render_farm;
}

} // namespace k3d

