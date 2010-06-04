#ifndef MODULES_QTUI_OPENGL_WINDOW_H
#define MODULES_QTUI_OPENGL_WINDOW_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/application_widget.h>

#include <ui_window.h>

#include <QMainWindow>

#include <boost/scoped_ptr.hpp>

namespace k3d { class iplugin_factory; }

namespace module
{

namespace qtui
{

namespace opengl
{

/////////////////////////////////////////////////////////////////////////////
// window

/// Displays output from an embedded Aqsis render engine
class window :
	public QMainWindow,
	public k3d::iunknown
{
	Q_OBJECT;

public:
	window();

	static k3d::iplugin_factory& get_factory();

public Q_SLOTS:
	void on_render_offscreen_clicked();

private:
	Ui::QTUIOpenGLWindow ui;
	k3d::qtui::application_widget application_widget;
};

} // namespace opengl

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_OPENGL_WINDOW_H
