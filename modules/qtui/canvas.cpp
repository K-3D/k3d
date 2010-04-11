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

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

//////////////////////////////////////////////////////////////////////////
// canvas

canvas::canvas(QWidget* parent) :
	QGLWidget(parent),
	m_camera(init_value<k3d::icamera*>(0)),
	m_gl_engine(init_value<k3d::gl::irender_viewport*>(0)),
	m_font_end(0)
{
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
}

void canvas::on_camera_changed(k3d::icamera* const Camera)
{
	m_camera.set_value(Camera);
	update();
}

void canvas::on_render_engine_changed(k3d::gl::irender_viewport* const Engine)
{
	m_gl_engine.set_value(Engine);
	update();
}

void canvas::initializeGL()
{
}

void canvas::paintGL()
{
	glViewport(0, 0, width(), height());
	if(m_gl_engine.internal_value() && m_camera.internal_value())
	{
		k3d::timer timer;

		m_gl_engine.internal_value()->render_viewport(*m_camera.internal_value(), width(), height(), m_gl_view_matrix, m_gl_projection_matrix, m_gl_viewport);

		const double elapsed = timer.elapsed();
		if(elapsed)
		{
			std::stringstream buffer;
			buffer << std::fixed << std::setprecision(1) << 1.0 / elapsed << "fps";

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1, 1, -1, 1, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_1D);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);

			glColor3d(0, 0, 0);
			renderText(-0.95, -0.95, 0.0, buffer.str().c_str());
		}
	}
	else
	{
		glClearColor(0.6f, 0.6f, 0.6f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glColor3d(0, 0, 0);
		renderText(-0.95, 0.0, 0.0, _("Use File > Open to load a document"));
	}
	glFlush();
}

} // namespace qtui

} // namespace module

