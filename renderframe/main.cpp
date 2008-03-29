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

#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/fstream.h>
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/path.h>
#include <k3dsdk/system.h>
#include <k3dsdk/types.h>
#include <k3dsdk/utility.h>

#include <k3dsdk/xml.h>
using namespace k3d::xml;

#ifdef K3D_API_WIN32

	#include <k3dsdk/win32.h>
	#include <direct.h>
	#define chdir _chdir

#endif // K3D_API_WIN32

#include <glibmm/spawn.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>

extern char** environ;

namespace detail
{

typedef std::vector<k3d::string_t> string_array;

bool g_show_timestamps = false;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = true;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// expand

const k3d::string_t expand(const k3d::string_t& Value)
{
	static boost::regex variable_regex("[$]([^$].*)[$]");
	static std::map<k3d::string_t, k3d::string_t> builtin_variables;

	k3d::string_t value = Value;
	for(boost::sregex_iterator variable(value.begin(), value.end(), variable_regex); variable != boost::sregex_iterator(); ++variable)
	{
		const k3d::string_t variable_expression = (*variable)[0].str();
		const k3d::string_t variable_name = (*variable)[1].str();

		k3d::string_t variable_value;
		if(builtin_variables.count(variable_name))
		{
			variable_value = builtin_variables[variable_name];
		}
		else
		{
			variable_value = k3d::system::getenv(variable_name);
		}

		boost::algorithm::replace_first(value, variable_expression, variable_value);
	}
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// exec_command

bool exec_command(const element& XMLCommand, const k3d::filesystem::path& FrameDirectory)
{
	k3d::string_t working_directory = FrameDirectory.native_filesystem_string();
	std::vector<k3d::string_t> arguments;
	std::vector<k3d::string_t> environment;
	k3d::string_t standard_output;
	k3d::string_t standard_error;
	int exit_status = 0;

	// Setup program arguments ...
	arguments.push_back(attribute_text(XMLCommand, "binary"));
	if(const element* const xml_arguments = find_element(XMLCommand, "arguments"))
	{
		for(element::elements_t::const_iterator xml_argument = xml_arguments->children.begin(); xml_argument != xml_arguments->children.end(); ++xml_argument)
		{
			k3d::string_t value = expand(attribute_text(*xml_argument, "value"));
			arguments.push_back(value);
		}
	}

	// Setup the application environment ...
	for(int i = 0; environ[i]; ++i)
		environment.push_back(environ[i]);

	if(const element* const xml_environment = find_element(XMLCommand, "environment"))
	{
		for(element::elements_t::const_iterator xml_variable = xml_environment->children.begin(); xml_variable != xml_environment->children.end(); ++xml_variable)
		{
			k3d::string_t name = attribute_text(*xml_variable, "name");
			k3d::string_t value = expand(attribute_text(*xml_variable, "value"));

			// Ensure that duplicates don't creep in ...
			for(int i = 0; i != environment.size(); ++i)
			{
				if(0 == environment[i].find(name + "="))
				{
					environment.erase(environment.begin() + i);
					break;
				}
			}

			environment.push_back(name + "=" + value);
		}
	}

	try
	{
		k3d::log() << info;
		std::copy(environment.begin(), environment.end(), std::ostream_iterator<k3d::string_t>(k3d::log(), " "));
		std::copy(arguments.begin(), arguments.end(), std::ostream_iterator<k3d::string_t>(k3d::log(), " "));
		k3d::log() << std::endl;

		Glib::spawn_sync(working_directory, arguments, environment, Glib::SPAWN_SEARCH_PATH, sigc::slot<void>(), &standard_output, &standard_error, &exit_status);

		if(!standard_output.empty())
			k3d::log() << info << "stdout: " << standard_output << std::endl;

		if(!standard_error.empty())
			k3d::log() << error << "stderr: " << standard_error << std::endl;

		return true;
	}
	catch(Glib::SpawnError& e)
	{
		k3d::log() << error << e.what().raw() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "caught unknown exception spawning process" << std::endl;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// copy_command

bool copy_command(const element& XMLCommand, const k3d::filesystem::path& FrameDirectory)
{
	try
	{
		const k3d::filesystem::path source = k3d::filesystem::native_path(k3d::ustring::from_utf8(attribute_text(XMLCommand, "source")));
		const k3d::filesystem::path target = k3d::filesystem::native_path(k3d::ustring::from_utf8(attribute_text(XMLCommand, "target")));

		k3d::filesystem::remove(target);
		k3d::filesystem::copy_file(source, target);

		return true;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "exception copying file: " << e.what();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// view_command

bool view_command(const element& XMLCommand, const k3d::filesystem::path& FrameDirectory)
{
	const k3d::string_t path = attribute_text(XMLCommand, "file");

#ifndef K3D_API_WIN32

/*
	// View the image ...
	boost::format command_line(k3d::options::get_command(k3d::options::command::bitmap_viewer()));
	command_line % path;

	// Execute the command ...
	k3d::system::spawn_async(command_line.str());
*/

#else // !K3D_API_WIN32

	ShellExecute(0, "open", path.c_str(), 0, 0, SW_SHOWDEFAULT);

#endif // K3D_API_WIN32

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// execute_command

/// Handles a single command during processing for the frame ...
bool execute_command(const element& XMLCommand, const k3d::filesystem::path& FrameDirectory)
{
	for(element::elements_t::const_iterator xml_command = XMLCommand.children.begin(); xml_command != XMLCommand.children.end(); ++xml_command)
	{
		if(xml_command->name == "exec")
			return exec_command(*xml_command, FrameDirectory);
		else if(xml_command->name == "copy")
			return copy_command(*xml_command, FrameDirectory);
		else if(xml_command->name == "view")
			return view_command(*xml_command, FrameDirectory);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// render_frame

/// Handles all processing for the given frame
bool render_frame(const k3d::filesystem::path& FrameDirectory)
{
	if(!k3d::filesystem::exists(FrameDirectory))
	{
		k3d::log() << error << "Frame directory " << FrameDirectory.native_console_string() << " does not exist" << std::endl;
		return false;
	}

	if(!k3d::filesystem::is_directory(FrameDirectory))
	{
		k3d::log() << error << "Frame directory " << FrameDirectory.native_console_string() << " is not a directory" << std::endl;
		return false;
	}

	// Skip the frame if it's complete ...
	if(k3d::filesystem::exists(FrameDirectory / k3d::filesystem::generic_path("complete")))
		return true;

	// Skip the frame if it errored out ...
	if(k3d::filesystem::exists(FrameDirectory / k3d::filesystem::generic_path("error")))
		return true;

	// Skip the frame if it's running ...
	if(k3d::filesystem::exists(FrameDirectory / k3d::filesystem::generic_path("running")))
		return true;

	// Make sure the frame is ready ...
	if(!k3d::filesystem::exists(FrameDirectory / k3d::filesystem::generic_path("ready")))
	{
		k3d::log() << error << "Frame " << FrameDirectory.native_console_string() << " is not ready" << std::endl;
		return false;
	}

	// Standard logging ...
	k3d::log() << info << "Starting Frame " << FrameDirectory.native_console_string() << std::endl;

	// Switch the frame status to running ...
	k3d::filesystem::rename(FrameDirectory / k3d::filesystem::generic_path("ready"), FrameDirectory / k3d::filesystem::generic_path("running"));

	// Load the frame options file ...
	element xml_frame_options("empty");
	const k3d::filesystem::path control_file_path = FrameDirectory / k3d::filesystem::generic_path("control.k3d");
	try
	{
		k3d::filesystem::ifstream stream(control_file_path);
		hide_progress progress;
		parse(xml_frame_options, stream, control_file_path.native_console_string(), progress);
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Frame " << FrameDirectory.native_console_string() << " error parsing control file " << control_file_path.native_console_string() << " " << e.what() << std::endl;
		return false;
	}

	// Get the frame data ...
	element* const xml_frame = find_element(xml_frame_options, "frame");
	if(!xml_frame)
	{
		k3d::log() << error << "Missing <frame> data in control file " << control_file_path.native_console_string() << std::endl;
		return false;
	}

	// Setup our execution environment ...
	chdir(FrameDirectory.native_filesystem_string().c_str());

	for(element::elements_t::iterator xml_command = xml_frame->children.begin(); xml_command != xml_frame->children.end(); ++xml_command)
		execute_command(*xml_command, FrameDirectory);

	// Switch the frame status to complete ...
	k3d::filesystem::rename(FrameDirectory / k3d::filesystem::generic_path("running"), FrameDirectory / k3d::filesystem::generic_path("complete"));

	// Standard logging ...
	k3d::log() << info << "Completed Frame " << FrameDirectory.native_console_string() << std::endl;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(const k3d::string_t& Name, std::ostream& Stream)
{
	Stream << "usage: " << Name << " [options]" << std::endl;
	Stream << "       " << Name << " [directory ...]" << std::endl;
	Stream << std::endl;
	Stream << "  -h, --help               prints this help information and exits" << std::endl;
	Stream << "      --version            prints program version information and exits" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// version

/// Prints version info
void print_version(std::ostream& Stream)
{
	Stream << "K-3D version " << K3D_VERSION << std::endl;
	Stream << K3D_COPYRIGHT << "  See the AUTHORS file for contributors." << std::endl;
	Stream << "Licensed by the GNU General Public License.  See the COPYING file for details." << std::endl;
	Stream << "K-3D Home Page: http://www.k-3d.org" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// setup_logging

/// Sets-up options for logging our output
void setup_logging(const k3d::string_t& ProcessName)
{
	k3d::log_show_timestamps(g_show_timestamps);
	k3d::log_set_tag(g_show_process ? "[" + ProcessName + "]" : k3d::string_t());
	k3d::log_color_level(g_color_level);
	k3d::log_show_level(true);
	k3d::log_syslog(g_syslog);
	k3d::log_minimum_level(g_minimum_log_level);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main

/// Program main
int main(int argc, char* argv[])
{
	const k3d::string_t program_name = k3d::filesystem::native_path(k3d::ustring::from_utf8(k3d::string_t(argv[0]))).leaf().raw();

	// Put our arguments in a more useable form ...
	detail::string_array options(&argv[1], &argv[argc]);

	// Print a "help" message ...
	if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
	{
		detail::usage(program_name, std::cout);
		return 0;
	}

	// Print version data ...
	if(options.end() != std::find(options.begin(), options.end(), "--version"))
	{
		detail::print_version(std::cout);
		return 0;
	}

	// Otherwise we should have a minimum of two arguments ...
	if(options.size() < 1)
	{
		detail::usage(program_name, k3d::log());
		return 1;
	}

	// Setup logging right away ...
	detail::setup_logging(program_name);

	// Each remaining argument should be a frame path to render ...
	int result = 0;
	for(unsigned long j = 0; j < options.size(); j++)
	{
		if(!detail::render_frame(k3d::filesystem::native_path(k3d::ustring::from_utf8(options[j]))))
		    result = 1;
	}

	return result;
}

