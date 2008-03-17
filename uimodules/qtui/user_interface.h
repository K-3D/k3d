#ifndef QTUI_USER_INTERFACE_H
#define QTUI_USER_INTERFACE_H

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

#include <k3dsdk/data.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/ievent_loop.h>

#include <QComboBox>
#include <QGLWidget>
#include <QMainWindow>
#include <QSplashScreen>
#include <QTimer>

namespace module
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////
// timer_helper

/// Converts a Qt timer into a libsigc++ signal
class timer_helper :
	public QObject
{
	Q_OBJECT

public:
	timer_helper(const unsigned int Interval);

	sigc::signal<void> m_signal;

private:
	std::auto_ptr<QTimer> timer;
	
private slots:
	void on_timer();
};
	
//////////////////////////////////////////////////////////////////////////
// viewport

class viewport :
	public QGLWidget
{
	Q_OBJECT

public:
	viewport(QWidget* parent = 0);

private slots:
	void on_camera_changed(k3d::icamera* const Camera);
	void on_render_engine_changed(k3d::gl::irender_viewport* const Engine);
	
private:
	void createFont();
	void initializeGL();
	void paintGL();

	/// Stores the document camera for drawing
	k3d_data(k3d::icamera*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_camera;
	/// Stores the document OpenGL render engine for drawing
	k3d_data(k3d::gl::irender_viewport*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_gl_engine;
	/// Stores the current set of OpenGL font glyphs
	unsigned long m_font_begin;
	/// Stores the current set of OpenGL font glyphs
	unsigned long m_font_end;
	// Buffers parameters from the most-recent render
	GLdouble m_gl_view_matrix[16];
	GLdouble m_gl_projection_matrix[16];
	GLint m_gl_viewport[4];
};
	
//////////////////////////////////////////////////////////////////////////
// main_window
	
class main_window :
	public QMainWindow
{
	Q_OBJECT

public:
	main_window(QApplication& Application);

signals:
	void camera_changed(k3d::icamera* const);
	void render_engine_changed(k3d::gl::irender_viewport* const);
	
private slots:
	void on_file_open();
	void on_camera_changed(int Index);
	void on_render_engine_changed(int Index);

private:
	k3d::idocument* m_document;
	QComboBox* m_camera_combo;
	QComboBox* m_render_engine_combo;
	viewport* m_viewport;
	k3d::nodes_t m_cameras;
	k3d::nodes_t m_render_engines;
};
	
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
	bool batch_mode();
	void open_uri(const k3d::string_t& URI);
	void message(const k3d::string_t& Message);
	void warning_message(const k3d::string_t& Message);
	void error_message(const k3d::string_t& Message);
	unsigned int query_message(const k3d::string_t& Message, const unsigned int DefaultOption, const std::vector<k3d::string_t>& Options);
	bool tutorial_message(const k3d::string_t& Message);
	bool get_file_path(const k3d::ipath_property::mode_t Mode, const k3d::string_t& Type, const k3d::string_t& Prompt, const k3d::filesystem::path& OldPath, k3d::filesystem::path& Result);
	bool show(iunknown& Object);
	void synchronize();
	sigc::connection get_timer(const double FrameRate, sigc::slot<void> Slot);
	k3d::iplugin_factory& factory();
	static k3d::iplugin_factory& get_factory();

private:
	std::auto_ptr<QApplication> m_application;
	std::auto_ptr<QSplashScreen> m_splash_box;
	std::auto_ptr<QWidget> m_window;
};

} // namespace qtui

} // namespace module

#endif // !QTUI_USER_INTERFACE_H

