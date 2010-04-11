#ifndef MODULES_QTUI_USER_INTERFACE_H
#define MODULES_QTUI_USER_INTERFACE_H

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

#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/ievent_loop.h>

#include <QComboBox>
#include <QMainWindow>
#include <QSplashScreen>

namespace k3d { class iplugin_factory; }

namespace module
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// user_interface

/// Implements the Qt User Interface plugin
class user_interface :
	public k3d::ievent_loop,
	public k3d::iuser_interface
{
public:
	void get_command_line_arguments(boost::program_options::options_description& Description);
	const arguments_t parse_startup_arguments(const arguments_t& Arguments, bool& Quit, bool& Error);
	void startup_message_handler(const k3d::string_t& Message);
	void display_user_interface();
	const arguments_t parse_runtime_arguments(const arguments_t& Arguments, bool& Quit, bool& Error);
	void start_event_loop();
	void stop_event_loop();

	void open_uri(const k3d::string_t& URI);
	void message(const k3d::string_t& Message);
	void warning_message(const k3d::string_t& Message);
	void error_message(const k3d::string_t& Message);
	unsigned int query_message(const k3d::string_t& Message, const unsigned int DefaultOption, const std::vector<k3d::string_t>& Options);
	void nag_message(const k3d::string_t& Type, const k3d::ustring& Message, const k3d::ustring& SecondaryMessage);
	bool get_file_path(const k3d::ipath_property::mode_t Mode, const k3d::string_t& Type, const k3d::string_t& Prompt, const k3d::filesystem::path& OldPath, k3d::filesystem::path& Result);
	bool show(iunknown& Object);
	void synchronize();
	sigc::connection get_timer(const double FrameRate, sigc::slot<void> Slot);
	k3d::uint_t watch_path(const k3d::filesystem::path& Path, const sigc::slot<void>& Slot);
	void unwatch_path(const k3d::uint_t WatchID);

	static k3d::iplugin_factory& get_factory();

private:
	std::auto_ptr<QApplication> m_application;
	std::auto_ptr<QSplashScreen> m_splash_box;
	std::auto_ptr<QWidget> m_window;
};

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_USER_INTERFACE_H

