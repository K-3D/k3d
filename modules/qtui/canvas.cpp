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

#include "canvas.h"
#include <ui_extrude_faces.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/share.h>

#include <QCheckBox>
#include <QGLWidget>
#include <QGraphicsProxyWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// viewport_scene

viewport_scene::viewport_scene() :
	m_camera(init_value<k3d::icamera*>(0)),
	m_gl_engine(init_value<k3d::gl::irender_viewport*>(0)),
	m_fps(0)
{
	QGraphicsProxyWidget* const button = addWidget(new QPushButton("Hello, World!"));
	button->setPos(100, 10);

	m_fps = new QGraphicsTextItem();
	m_fps->setPos(5, 5);
	addItem(m_fps);

	QGraphicsProxyWidget* const extrude_faces_widget = addWidget(new QWidget());
	Ui::ExtrudeFaces* const extrude_faces = new Ui::ExtrudeFaces();
	extrude_faces->setupUi(extrude_faces_widget->widget());

/*
	QGroupBox* const group_box = new QGroupBox("ExtrudeFaces");
	QVBoxLayout* const vbox = new QVBoxLayout();
	vbox->addWidget(new QCheckBox("Option A"));
	vbox->addWidget(new QCheckBox("Option B"));
	vbox->addWidget(new QCheckBox("Option C"));
	group_box->setLayout(vbox);
	QGraphicsProxyWidget* const group_box_proxy = addWidget(group_box);
*/
	extrude_faces_widget->setPos(10, 200);
	extrude_faces_widget->setOpacity(0.8);
}

void viewport_scene::drawBackground(QPainter *painter, const QRectF &rect)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

	if(m_gl_engine.internal_value() && m_camera.internal_value())
	{
		k3d::timer timer;

		m_gl_engine.internal_value()->render_viewport(*m_camera.internal_value(), width(), height(), m_gl_view_matrix, m_gl_projection_matrix, m_gl_viewport);

		const k3d::double_t elapsed = timer.elapsed();
		std::stringstream buffer;
		if(elapsed)
			buffer << std::fixed << std::setprecision(1) << 1.0 / elapsed << "fps";
//		m_fps->setPlainText(buffer.str().c_str());
	}
	else
	{
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
	glMatrixMode(GL_PROJECTION);
        glPopMatrix();
	glPopAttrib();
}

void viewport_scene::on_camera_changed(k3d::icamera* const Camera)
{
	m_camera.set_value(Camera);
	update();
}

void viewport_scene::on_render_engine_changed(k3d::gl::irender_viewport* const Engine)
{
	m_gl_engine.set_value(Engine);
	update();
}

//////////////////////////////////////////////////////////////////////////
// viewport_view

viewport_view::viewport_view(QWidget* parent) :
	QGraphicsView(parent)
{
//	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setViewport(new QGLWidget(QGLFormat()));
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void viewport_view::resizeEvent(QResizeEvent *event)
{
	if(scene())
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
	QGraphicsView::resizeEvent(event);
}

} // namespace qtui

} // namespace module

