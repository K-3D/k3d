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

#include <QGraphicsScene>
#include <QGraphicsView>

class QComboBox;
class QGraphicsTextItem;

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// viewport_scene

class viewport_scene :
	public QGraphicsScene
{
	Q_OBJECT

public:
	viewport_scene();
 
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

private Q_SLOTS:
	void on_document_changed(k3d::idocument&);
	void on_camera_changed(int Index);
	void on_render_engine_changed(int Index);
	
private:
	/// Stores the document camera for drawing
	k3d_data(k3d::icamera*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_camera;
	/// Stores the document OpenGL render engine for drawing
	k3d_data(k3d::gl::irender_viewport*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_gl_engine;
	// Buffers parameters from the most-recent render
	GLdouble m_gl_view_matrix[16];
	GLdouble m_gl_projection_matrix[16];
	GLint m_gl_viewport[4];
	QGraphicsTextItem* m_fps;
	QComboBox* m_camera_combo;
	QComboBox* m_engine_combo;
	std::vector<k3d::icamera*> m_cameras;
	std::vector<k3d::gl::irender_viewport*> m_render_engines;
};

//////////////////////////////////////////////////////////////////////////
// viewport_view

class viewport_view :
	public QGraphicsView
{
	Q_OBJECT

public:
	viewport_view(QWidget* parent = 0);

protected:
	void resizeEvent(QResizeEvent *event);
};

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_CANVAS_H

