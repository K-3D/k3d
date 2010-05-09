#ifndef MODULES_QTUI_SCENE_H
#define MODULES_QTUI_SCENE_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/qtui/mode.h>
#include <k3dsdk/signal_system.h>

#include <boost/scoped_ptr.hpp>

#include <QGraphicsScene>

namespace k3d { class idocument; }

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// scene

class scene :
	public QGraphicsScene,
	public sigc::trackable
{
	Q_OBJECT;

public:
	scene(k3d::idocument& Document);
 
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

	void set_active_mode(k3d::qtui::mode* const Mode);

private Q_SLOTS:
	void on_camera_changed(int Index);
	void on_render_engine_changed(int Index);
	
private:
	/// Called when the pipeline needs to be rendered.
	void on_redraw_pipeline(k3d::gl::irender_viewport::redraw_type_t RedrawType);
 
	/// Stores the document camera for drawing
	k3d_data(k3d::icamera*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_camera;
	/// Stores the document OpenGL render engine for drawing
	k3d_data(k3d::gl::irender_viewport*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_gl_engine;
	// Buffers parameters from the most-recent render
	GLdouble m_gl_view_matrix[16];
	GLdouble m_gl_projection_matrix[16];
	GLint m_gl_viewport[4];
	std::vector<k3d::icamera*> m_cameras;
	std::vector<k3d::gl::irender_viewport*> m_render_engines;
	boost::scoped_ptr<k3d::qtui::mode> m_active_mode;
};

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_SCENE_H

