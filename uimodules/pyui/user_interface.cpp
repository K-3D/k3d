// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
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

#include <Python.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/iuser_interface_plugin.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3d-platform-config.h>

#include <k3dsdk_python/object_model_python.h>

#include <iostream>

#if defined K3D_API_DARWIN
	#define PYTHON_INITIALIZE PyMac_Initialize
#else
	#define PYTHON_INITIALIZE Py_Initialize
#endif

namespace libk3dpyui
{

/////////////////////////////////////////////////////////////////////////////
// user_interface

/// Provides an interactive python shell user interface to K-3D, useful for prototyping and executing scripts when a graphical UI isn't needed
class user_interface :
	public k3d::iuser_interface_plugin,
	public k3d::iuser_interface,
	public k3d::ideletable
{
public:
	void get_command_line_arguments(boost::program_options::options_description& Description)
	{
	}

	const arguments_t parse_startup_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
	{
		return Arguments;
	}

	const arguments_t parse_runtime_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
	{
		return Arguments;
	}

	void startup_message_handler(const std::string& Message)
	{
	}

	void display_user_interface()
	{
	}

	void start_event_loop()
	{
		std::vector<char*> argv_buffer;
		argv_buffer.push_back("k3d");
		int argc = argv_buffer.size();
		char** argv = &argv_buffer[0];

		try
		{
			PYTHON_INITIALIZE();

			initk3d();

			Py_Main(argc, argv);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << "Unknown exception" << std::endl;
		}

	}

	void stop_event_loop()
	{
		k3d::log() << debug << k3d_file_reference << std::endl;
	}

	bool batch_mode()
	{
		return true;
	}

	void browser_navigate(const std::string& URL)
	{
	}

	void message(const std::string& Message)
	{
		std::cout << "message: " << Message << std::endl;
	}

	void warning_message(const std::string& Message)
	{
		std::cout << "warning: " << Message << std::endl;
	}

	void error_message(const std::string& Message)
	{
		std::cout << "error: " << Message << std::endl;
	}

	unsigned int query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options)
	{
		return 0;
	}

	bool tutorial_message(const std::string& Message)
	{
		return false;
	}

	bool get_file_path(const k3d::ipath_property::mode_t Mode, const std::string& Type, const std::string& Prompt, const k3d::filesystem::path& OldPath, k3d::filesystem::path& Result)
	{
		return false;
	}

	bool show(iunknown& Object)
	{
		return false;
	}

	sigc::connection get_timer(const double FrameRate, sigc::slot<void> Slot)
	{
		return sigc::connection();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<user_interface,
			k3d::interface_list<k3d::iuser_interface_plugin> > factory(
			k3d::uuid(0x337facd7, 0xbd1645b6, 0x86957382, 0xcb2d53cb),
			"NullUI",
			"Null User Interface (NUI)",
			"");

		return factory;
	}
};

} // namespace libk3dpyui

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dpyui::user_interface::get_factory());
K3D_MODULE_END

