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
#include <k3dsdk/gl.h>
#include <k3dsdk/imatrix_sink.h>
#include <k3dsdk/imatrix_source.h>
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
#include <k3dsdk/utility_gl.h>

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
/// Adapted from render_engine.cpp
/// TODO: Do we need some convention on what light is used for the 3D scene and the UI modes?
/// We use LIGHT1 here.
void gl_setup_headlight()
{
	// Setup lights ...
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	// Make sure all lights are turned off initially ...
	GLint maxlights = 0;
	glGetIntegerv(GL_MAX_LIGHTS, &maxlights);
	for(GLint i = 0; i < maxlights; ++i)
		glDisable(GLenum(GL_LIGHT0 + i));

	// Setup the headlight ...
	glEnable(GL_LIGHT1);

	// Setup color ...
	const GLfloat color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, color);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color);

	// Setup light direction ...
	const GLfloat position[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, position);
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
	
	
	
	std::vector<k3d::icamera*> cameras = k3d::node::lookup<k3d::icamera>(Document);
	return_if_fail(cameras.size());
	m_move_manipulators.set_camera(cameras.front());
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
	k3d::gl::store_attributes attributes;
	
	// Setup the projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	k3d::rectangle window_rect, cam_rect;
	k3d::double_t near, far;
	k3d::bool_t ortho;
	k3d::gl::calculate_projection(*m_camera, m_rect.width(), m_rect.height(), window_rect, cam_rect, near, far, ortho);
	if(ortho)
	{
		const k3d::matrix4 transform_matrix = k3d::property::pipeline_value<k3d::matrix4>(m_camera->transformation().matrix_source_output());
		const k3d::point3 world_position = transform_matrix * k3d::point3(0, 0, 0);
		const k3d::point3 world_target = boost::any_cast<k3d::point3>(m_camera->world_target().property_internal_value());
		const double distance = k3d::distance(world_position, world_target);
		const double window_aspect = (window_rect.x2 - window_rect.x1) / (window_rect.y1 - window_rect.y2);
		const double window_tan_fov = (window_rect.y1 - window_rect.y2) * 0.5 / near;
		const double window_size = distance * window_tan_fov;
		glOrtho(-window_size * window_aspect, window_size * window_aspect, -window_size, window_size, near, far);
	}
	else
	{
		glFrustum(window_rect.x1, window_rect.x2, window_rect.y2, window_rect.y1, near, far);
	}
	
	// Setup the light
	detail::gl_setup_headlight();
	
	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	const k3d::matrix4 transform_matrix = k3d::property::pipeline_value<k3d::matrix4>(m_camera->transformation().matrix_source_output());
	const k3d::angle_axis orientation(k3d::euler_angles(transform_matrix, k3d::euler_angles::ZXYstatic));
	const k3d::point3 position(k3d::position(transform_matrix));

	glScaled(1.0, 1.0, -1.0);
	glRotated(-k3d::degrees(orientation.angle), orientation.axis[0], orientation.axis[1], orientation.axis[2]);
	glTranslated(-position[0], -position[1], -position[2]);
	
	// Draw the manipulators
	m_move_manipulators.draw(move_manipulators::NO);
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void mode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mousePressEvent(event);
}


} // namespace move

} // namespace qtui

} // namespace module


