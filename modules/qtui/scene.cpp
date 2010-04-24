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

#include <boost/scoped_ptr.hpp>
#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// scene

scene::scene(k3d::idocument& Document) :
	m_camera(init_value<k3d::icamera*>(0)),
	m_gl_engine(init_value<k3d::gl::irender_viewport*>(0))
{
	m_cameras = k3d::node::lookup<k3d::icamera>(Document);
	m_render_engines = k3d::node::lookup<k3d::gl::irender_viewport>(Document);

	m_camera.set_value(m_cameras.size() ? m_cameras[0] : 0);
	m_gl_engine.set_value(m_render_engines.size() ? m_render_engines[0] : 0);

	set_active_mode(k3d::plugin::create<k3d::qtui::mode>("QTUIDefaultMode"));

	update();
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

void scene::set_active_mode(k3d::qtui::mode* const Mode)
{
	m_active_mode.reset(Mode);
	if(m_active_mode)
		m_active_mode->enable(*this);
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

