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
		\brief Implements the renderjob application, which runs jobs scheduled with the virtual render farm
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/path.h>
#include <k3dsdk/system.h>
#include <k3dsdk/utility.h>

#include <algorithm>
#include <iostream>
#include <vector>

namespace
{

typedef std::vector<std::string> string_array;

bool g_show_timestamps = false;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = false;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////
// render_job

bool render_job(const k3d::filesystem::path& GlobalOptionsPath, const k3d::filesystem::path& JobDirectory)
{
	// Sanity checks ...
	if(!k3d::filesystem::exists(GlobalOptionsPath))
	{
		k3d::log() << error << "Options file " << GlobalOptionsPath.native_console_string() << " does not exist" << std::endl;
		return false;
	}

	if(!k3d::filesystem::exists(JobDirectory))
	{
		k3d::log() << error << "Job directory " << JobDirectory.native_console_string() << " does not exist" << std::endl;
		return false;
	}

	if(!k3d::filesystem::is_directory(JobDirectory))
	{
		k3d::log() << error << "Job directory " << JobDirectory.native_console_string() << " is not a directory" << std::endl;
		return false;
	}

	// Skip the job if it's complete ...
	if(k3d::filesystem::exists(JobDirectory / k3d::filesystem::generic_path("complete")))
		return true;

	// Skip the job if it errored out ...
	if(k3d::filesystem::exists(JobDirectory / k3d::filesystem::generic_path("error")))
		return true;

	// Skip the job if it's running ...
	if(k3d::filesystem::exists(JobDirectory / k3d::filesystem::generic_path("running")))
		return true;

	// Make sure the job is ready ...
	if(!k3d::filesystem::exists(JobDirectory / k3d::filesystem::generic_path("ready")))
	{
		k3d::log() << error << "Job " << JobDirectory.native_console_string() << " is not ready" << std::endl;
		return false;
	}

	// Standard logging ...
	k3d::log() << info << "Starting Job " << JobDirectory.native_console_string() << std::endl;

	// Switch the job status to running ...
	k3d::filesystem::rename(JobDirectory / k3d::filesystem::generic_path("ready"), JobDirectory / k3d::filesystem::generic_path("running"));

	// For each directory in the job directory (non-recursive) ...
	for(k3d::filesystem::directory_iterator frame(JobDirectory); frame != k3d::filesystem::directory_iterator(); ++frame)
	{
		if(!k3d::filesystem::is_directory(*frame))
			continue;

		const std::string commandline("k3d-renderframe \"" + GlobalOptionsPath.native_filesystem_string() + "\" \"" + frame->native_filesystem_string() + "\"");
		k3d::system::spawn_sync(commandline);
	}

	// Switch the job status to complete ...
	k3d::filesystem::rename(JobDirectory / k3d::filesystem::generic_path("running"), JobDirectory / k3d::filesystem::generic_path("complete"));

	// Standard logging ...
	k3d::log() << info << "Completed Job " << JobDirectory.native_console_string() << std::endl;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////
// version

/// Prints version info
void print_version(std::ostream& Stream)
{
	Stream << "K-3D Version " << K3D_VERSION << std::endl;
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

/// Program main
int main(int argc, char* argv[])
{
#ifndef K3D_API_WIN32
	// Fork ourselves so we don't become a zombie of the K-3D executable
	const int fork_result = fork();
	if(fork_result < 0)
		return 1;
	else if(fork_result > 0)
		return 0;
#endif // !K3D_API_WIN32

	const std::string program_name = k3d::filesystem::native_path(k3d::ustring::from_utf8(std::string(argv[0]))).leaf().raw();

	// Put our command-line arguments in a more useable form ...
	string_array options(&argv[1], &argv[argc]);

	// Print a "help" message ...
	if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
	{
		usage(program_name, std::cout);
		return 0;
	}

	// Print version data ...
	if(std::count(options.begin(), options.end(), "--version"))
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

	// Each remaining argument should be a job path to render ...
	int result = 0;
	for(unsigned long j = 1; j < options.size(); j++)
	{
		if(!render_job(
			k3d::filesystem::native_path(k3d::ustring::from_utf8(options[0])),
			k3d::filesystem::native_path(k3d::ustring::from_utf8(options[j]))))
			result = 1;
	}

	return result;
}

