#ifndef MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H
#define MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H

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

#include <k3dsdk/icamera.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/mode.h>
#include <k3dsdk/types.h>

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QScriptEngine>

#include <boost/scoped_ptr.hpp>

namespace k3d
{ 

class iplugin_factory;
class imatrix_sink;

}

class QGraphicsProxyWidget;

namespace module
{

namespace qtui
{

namespace move
{

/////////////////////////////////////////////////////////////////////////////
// mode

/// Provides a move edit mode
class mode :
	public k3d::qtui::mode,
	public QGraphicsItem,
	public k3d::iunknown
{
	Q_OBJECT;

	typedef k3d::qtui::mode base;

public:
	mode();

	void enable(k3d::idocument& Document, QGraphicsScene& Scene);
	
	virtual QRectF boundingRect() const;
	
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

	static k3d::iplugin_factory& get_factory();

private Q_SLOTS:
	void on_reload();
	void on_scene_rect_changed(const QRectF& Rect);

private:
	void redraw();
	void render_selection(const k3d::rectangle& Region);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	k3d::idocument* m_document;
	QGraphicsScene* m_scene;
	QRectF m_rect;
	
	// cache some required document-related data
	k3d::imatrix_sink* m_manipulators;
	k3d::gl::irender_viewport* m_gl_engine;
	k3d::icamera* m_camera;
};

} // namespace move

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H
