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

#include "mode.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/imatrix_sink.h>
#include <k3dsdk/irenderable_gl.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/property.h>
#include <k3dsdk/qtui/file_dialog.h>
#include <k3dsdk/qtui/modal_text_editor.h>
#include <k3dsdk/qtui/script/engine.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/selection_state_gl.h>
#include <k3dsdk/types.h>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QMetaType>
#include <QTimer>
#include <QToolButton>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>

Q_DECLARE_METATYPE(k3d::iunknown*);

namespace module
{

namespace qtui
{

namespace move
{

namespace detail
{

typedef std::vector<GLuint> selection_buffer_t;
	
	// based on the NGUI viewport::select function
k3d::int32_t select(k3d::gl::irender_viewport& Engine, k3d::icamera& Camera, selection_buffer_t& Buffer, const k3d::gl::selection_state& SelectState, const k3d::uint_t Width, const k3d::uint_t Height, const k3d::rectangle& Region)
{
	// Set our selection buffer to a sensible minimum size ...
	if(Buffer.size() < 8096)
		Buffer.resize(8096);
	
	k3d::log() << debug << "rendering selection in window of " << Width << " x " << Height << " with rect around " << Region.center() << std::endl;

	// Set an (arbitrary) upper-limit on how large we let the buffer grow ...
	while(Buffer.size() < 10000000)
	{
		glSelectBuffer(Buffer.size(), &Buffer[0]);
		glRenderMode(GL_SELECT);
		glInitNames();

		GLdouble view_matrix[16];
		GLdouble projection_matrix[16];
		GLint viewport[4];
		
		Engine.render_viewport_selection(SelectState, Camera, Width, Height, k3d::rectangle::normalize(Region), view_matrix, projection_matrix, viewport);
		
		const GLint hits = glRenderMode(GL_RENDER);
		glFlush();

		// If we got a positive number of hits, we're done ...
		if(hits >= 0)
			return hits;

		// A negative number means there was buffer overflow, so try again ...
		Buffer.resize(Buffer.size() * 2);
	}

	// Ran out of buffer space!
	k3d::log() << error << "Ran out of selection-buffer space" << std::endl;

	return 0;
}

}

/////////////////////////////////////////////////////////////////////////////
// mode

mode::mode() :
	m_document(0),
	m_scene(0)
{
}

void mode::enable(k3d::idocument& Document, QGraphicsScene& Scene)
{
	if(m_scene)
		disconnect(m_scene, 0, this, 0);

	m_document = &Document;
	m_scene = &Scene;
	
	std::vector<k3d::gl::irender_viewport*> gl_engines = k3d::node::lookup<k3d::gl::irender_viewport>(Document);
	return_if_fail(gl_engines.size());
	m_gl_engine = gl_engines.front();
	
	std::vector<k3d::icamera*> cameras = k3d::node::lookup<k3d::icamera>(Document);
	return_if_fail(cameras.size());
	m_camera = cameras.front();

	connect(m_scene, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(on_scene_rect_changed(const QRectF&)));

	on_reload();
}

void mode::on_reload()
{
	return_if_fail(m_scene);

	QList<QGraphicsItem*> all_items = m_scene->items();
	for(int i = 0; i != all_items.size(); ++i)
		delete all_items[i];
	
	setAcceptedMouseButtons(Qt::LeftButton);
	m_scene->addItem(this);
	
	// Add the manipulator plugin
	m_manipulators = k3d::plugin::create<k3d::imatrix_sink>("MoveManipulators", *m_document, "MoveManipulators");
	
	on_scene_rect_changed(m_scene->sceneRect());
}

void mode::on_scene_rect_changed(const QRectF& Rect)
{
	m_rect = Rect;
	redraw();
}

void mode::redraw()
{

}


k3d::iplugin_factory& mode::get_factory()
{
	static k3d::application_plugin_factory<mode> factory(
		k3d::uuid(0x5155a2c8, 0x5af54dc7, 0xa9efcc88, 0xd7239206),
		"QTUIMoveMode",
		_("Provides an edit-mode to interactively move items in the viewport."),
		"QTUI Mode",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "mode"));

	return factory;
}

k3d::iplugin_factory& mode_factory()
{
	return mode::get_factory();
}

QRectF mode::boundingRect() const
{
	return m_rect;
}

void mode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}

void mode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	k3d::rectangle rect(k3d::point2(event->pos().x(), event->pos().y()), 300., 300.);
	render_selection(rect);
	QGraphicsItem::mousePressEvent(event);
}

void mode::render_selection(const k3d::rectangle& Region)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	std::vector<GLuint> buffer;
	k3d::gl::selection_state state;
	state.select_component.insert(k3d::selection::USER1);
	const k3d::int32_t hits = detail::select(*m_gl_engine, *m_camera, buffer, state, static_cast<k3d::uint_t>(boundingRect().width()), static_cast<k3d::uint_t>(boundingRect().height()), Region);
	k3d::log() << debug << "got " << hits << " hits" << std::endl;
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}


} // namespace move

} // namespace qtui

} // namespace module


