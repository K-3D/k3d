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

#include "scene.h"
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
#include <QComboBox>
#include <QGLWidget>
#include <QGraphicsProxyWidget>
#include <QGroupBox>
#include <QMenu>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// scene

scene::scene() :
	m_camera(init_value<k3d::icamera*>(0)),
	m_gl_engine(init_value<k3d::gl::irender_viewport*>(0)),
	m_fps(0),
	m_camera_combo(0),
	m_engine_combo(0)
{
	QToolButton* const file_menu_button = new QToolButton();
	file_menu_button->setText("Stuff");
	file_menu_button->setPopupMode(QToolButton::MenuButtonPopup);

	QMenu* const file_menu = new QMenu(file_menu_button);
	file_menu->addAction("Stampede");
	file_menu->addAction("Framulize");
	file_menu->addAction("Transmogrify");

	file_menu_button->setMenu(file_menu);

	QGraphicsProxyWidget* const button_proxy = addWidget(file_menu_button);
	button_proxy->setPos(10, 10);

	m_fps = new QGraphicsTextItem();
	m_fps->setPos(5, 5);
	addItem(m_fps);

	QGraphicsProxyWidget* const extrude_faces_proxy = addWidget(new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint));
	Ui::ExtrudeFaces* const extrude_faces = new Ui::ExtrudeFaces();
	extrude_faces->setupUi(static_cast<QDialog*>(extrude_faces_proxy->widget()));
	extrude_faces_proxy->setFlag(QGraphicsItem::ItemIsMovable);
	extrude_faces_proxy->setPos(10, 200);
	extrude_faces_proxy->setOpacity(0.8);

	m_camera_combo = new QComboBox();
	connect(m_camera_combo, SIGNAL(activated(int)), this, SLOT(on_camera_changed(int)));
	QGraphicsProxyWidget* const camera_combo_proxy = addWidget(m_camera_combo);
	camera_combo_proxy->setPos(100, 10);

	m_engine_combo = new QComboBox();
	connect(m_engine_combo, SIGNAL(activated(int)), this, SLOT(on_render_engine_changed(int)));
	QGraphicsProxyWidget* const engine_combo_proxy = addWidget(m_engine_combo);
	engine_combo_proxy->setPos(200, 10);
}

void scene::drawBackground(QPainter *painter, const QRectF &rect)
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

void scene::on_document_changed(k3d::idocument& Document)
{
	m_cameras = k3d::node::lookup<k3d::icamera>(Document);
	m_render_engines = k3d::node::lookup<k3d::gl::irender_viewport>(Document);

	m_camera_combo->clear();
	m_camera_combo->setEnabled(m_cameras.size());
	for(std::vector<k3d::icamera*>::iterator camera = m_cameras.begin(); camera != m_cameras.end(); ++camera)
		m_camera_combo->addItem(dynamic_cast<k3d::inode*>(*camera)->name().c_str());
	m_camera_combo->adjustSize();

	m_engine_combo->clear();
	m_engine_combo->setEnabled(m_render_engines.size());
	for(std::vector<k3d::gl::irender_viewport*>::iterator render_engine = m_render_engines.begin(); render_engine != m_render_engines.end(); ++render_engine)
		m_engine_combo->addItem(dynamic_cast<k3d::inode*>(*render_engine)->name().c_str());
	m_engine_combo->adjustSize();

	m_camera.set_value(m_cameras.size() ? m_cameras[0] : 0);
	m_gl_engine.set_value(m_render_engines.size() ? m_render_engines[0] : 0);	
	update();
}

void scene::on_camera_changed(int Index)
{
	m_camera.set_value(m_cameras[Index]);
	update();
}

void scene::on_render_engine_changed(int Index)
{
	m_gl_engine.set_value(m_render_engines[Index]);
	update();
}

} // namespace qtui

} // namespace module

