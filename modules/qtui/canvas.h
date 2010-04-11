#ifndef MODULES_QTUI_CANVAS_H
#define MODULES_QTUI_CANVAS_H

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

#include <QGLWidget>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// canvas

class canvas :
	public QGLWidget
{
	Q_OBJECT

public:
	canvas(QWidget* parent = 0);

private Q_SLOTS:
	void on_camera_changed(k3d::icamera* const Camera);
	void on_render_engine_changed(k3d::gl::irender_viewport* const Engine);
	
private:
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

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_CANVAS_H

