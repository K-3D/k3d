#ifndef MODULES_QTUI_MAIN_WINDOW_H
#define MODULES_QTUI_MAIN_WINDOW_H

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

#include <QMainWindow>

class QComboBox;

namespace k3d { class icamera; }
namespace k3d { class idocument; }
namespace k3d { namespace gl { class irender_viewport; } }

namespace module
{

namespace qtui
{

class viewport_scene;
class viewport_view;

//////////////////////////////////////////////////////////////////////////
// main_window
	
class main_window :
	public QMainWindow
{
	Q_OBJECT

public:
	main_window(QApplication& Application);

Q_SIGNALS:
	void camera_changed(k3d::icamera* const);
	void render_engine_changed(k3d::gl::irender_viewport* const);
	
private Q_SLOTS:
	void on_file_open();
	void on_camera_changed(int Index);
	void on_render_engine_changed(int Index);

private:
	k3d::idocument* m_document;
	QComboBox* m_camera_combo;
	QComboBox* m_render_engine_combo;
	viewport_scene* m_viewport_scene;
	viewport_view* m_viewport_view;
	std::vector<k3d::icamera*> m_cameras;
	std::vector<k3d::gl::irender_viewport*> m_render_engines;
};
	
} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_MAIN_WINDOW_H

