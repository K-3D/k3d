// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Implements the renderframe application, which renders frames scheduled as part of a job with the virtual render farm
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/fstream.h>
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/options.h>
#include <k3dsdk/options_policy.h>
#include <k3dsdk/path.h>
#include <k3dsdk/system.h>
#include <k3dsdk/utility.h>

#include <k3dsdk/xml.h>
using namespace k3d::xml;

#ifdef K3D_API_WIN32

	#include <k3dsdk/win32.h>
	#include <direct.h>
	#define chdir _chdir

#endif // K3D_API_WIN32

#include <boost/format.hpp>

#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>

namespace
{

typedef std::vector<std::string> string_array;

bool g_show_timestamps = false;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = true;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// input_file

bool input_file(const element& XMLOperation)
{
	// Sanity checks ...
	assert(XMLOperation.name == "inputfile");

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// output_file

bool output_file(const element& XMLOperation)
{
	// Sanity checks ...
	assert(XMLOperation.name == "outputfile");

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// render_operation

bool render_operation(const element& XMLOperation)
{
	// Sanity checks ...
	assert(XMLOperation.name == "renderoperation");

	// Load paths for use during rendering ...


	// Load render data ...
	const std::string type = attribute_text(XMLOperation, "type");
	const std::string name = attribute_text(XMLOperation, "name");
	const std::string sourcepath = attribute_text(XMLOperation, "sourcepath");
	const std::string shaderspath = attribute_text(XMLOperation, "shaderspath");
	const std::string sharepath = attribute_text(XMLOperation, "sharepath");

	// Poke through global options and look for a render engine that matches ...
	std::string command_line;
	const k3d::options::render_engines_t render_engines = k3d::options::render_engines();
	for(k3d::options::render_engines_t::const_iterator render_engine = render_engines.begin(); render_engine != render_engines.end(); ++render_engine)
	{
		if(type != render_engine->type)
			continue;

		if(name != render_engine->name)
			continue;

		command_line = render_engine->render_command;
		break;
	}

	if(command_line.empty())
	{
		k3d::log() << error << "Could not find requested render engine [" << type << "] [" << name << "]" << std::endl;
		return false;
	}

	// Substitute the input source file, shaders, and share paths ...
	boost::format command_line2(command_line);
	command_line2.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);

	command_line2 % sourcepath;
	command_line2 % shaderspath;
	command_line2 % sharepath;

	// Execute the command ...
	k3d::system::spawn_sync(command_line2.str());

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// copy_operation

bool copy_operation(const element& XMLOperation)
{
	// Sanity checks ...
	assert(XMLOperation.name == "copyoperation");

	try
	{
		const k3d::filesystem::path from = k3d::filesystem::native_path(k3d::ustring::from_utf8(attribute_text(XMLOperation, "from")));
		const k3d::filesystem::path to = k3d::filesystem::native_path(k3d::ustring::from_utf8(attribute_text(XMLOperation, "to")));

		k3d::filesystem::remove(to);
		k3d::filesystem::copy_file(from, to);
		return true;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "exception copying file: " << e.what();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// view_operation

bool view_operation(const element& XMLOperation)
{
	// Sanity checks ...
	assert(XMLOperation.name == "viewoperation");

	const std::string path = attribute_text(XMLOperation, "path");

#ifndef K3D_API_WIN32

	// View the image ...
	boost::format command_line(k3d::options::get_command(k3d::options::command::bitmap_viewer()));
	command_line % path;

	// Execute the command ...
	k3d::system::spawn_async(command_line.str());

#else // !K3D_API_WIN32

	ShellExecute(0, "open", path.c_str(), 0, 0, SW_SHOWDEFAULT);

#endif // K3D_API_WIN32

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// frame_operation

/// Handles a single operation during processing for the frame ...
bool frame_operation(const element& XMLOperation)
{
	// Sanity checks ...
	const std::string operation = XMLOperation.name;

	if(operation == "inputfile")
		return input_file(XMLOperation);
	else if(operation == "outputfile")
		return output_file(XMLOperation);
	else if(operation == "renderoperation")
		return render_operation(XMLOperation);
	else if(operation == "copyoperation")
		return copy_operation(XMLOperation);
	else if(operation == "viewoperation")
		return view_operation(XMLOperation);

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

	for(element::elements_t::iterator xml_operation = xml_frame->children.begin(); xml_operation != xml_frame->children.end(); ++xml_operation)
		frame_operation(*xml_operation);

	// Switch the frame status to complete ...
	k3d::filesystem::rename(FrameDirectory / k3d::filesystem::generic_path("running"), FrameDirectory / k3d::filesystem::generic_path("complete"));

	// Standard logging ...
	k3d::log() << info << "Completed Frame " << FrameDirectory.native_console_string() << std::endl;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(const std::string& Name, std::ostream& Stream)
{
	Stream << "usage: " << Name << " [options]" << std::endl;
	Stream << "       " << Name << " [optionspath] [directory ...]" << std::endl;
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
void setup_logging(const std::string& ProcessName)
{
	k3d::log_show_timestamps(g_show_timestamps);
	k3d::log_set_tag(g_show_process ? "[" + ProcessName + "]" : std::string());
	k3d::log_color_level(g_color_level);
	k3d::log_show_level(true);
	k3d::log_syslog(g_syslog);
	k3d::log_minimum_level(g_minimum_log_level);
}

} // namespace

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main

/// Program main
int main(int argc, char* argv[])
{
	const std::string program_name = k3d::filesystem::native_path(k3d::ustring::from_utf8(std::string(argv[0]))).leaf().raw();

	// Put our arguments in a more useable form ...
	string_array options(&argv[1], &argv[argc]);

	// Print a "help" message ...
	if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
	{
		usage(program_name, std::cout);
		return 0;
	}

	// Print version data ...
	if(options.end() != std::find(options.begin(), options.end(), "--version"))
	{
		print_version(std::cout);
		return 0;
	}

	// Otherwise we should have a minimum of two arguments ...
	if(options.size() < 2)
	{
		usage(program_name, k3d::log());
		return 1;
	}

	// Setup logging right away ...
	setup_logging(program_name);

	// Open the global options file ...
	const k3d::filesystem::path options_path = k3d::filesystem::native_path(k3d::ustring::from_utf8(options[0]));
	if(!k3d::filesystem::exists(options_path))
	{
		k3d::log() << error << "User options file [" << options_path.native_console_string() << "] does not exist" << std::endl;
		return 1;
	}

	k3d::options::file_storage user_options(options_path);
	k3d::options::set_storage(user_options);

	// Each remaining argument should be a frame path to render ...
	int result = 0;
	for(unsigned long j = 1; j < options.size(); j++)
	{
		if(!render_frame(k3d::filesystem::native_path(k3d::ustring::from_utf8(options[j]))))
		    result = 1;
	}

	return result;
}

