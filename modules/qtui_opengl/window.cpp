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

#include "window.h"

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/gl/api.h>
#include <k3dsdk/gl/context_factory.h>
#include <k3dsdk/gl/offscreen_context_factory.h>
#include <k3dsdk/gl/offscreen_context.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/qtui/application_widget.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>
#include <sstream>

namespace module
{

namespace qtui
{

namespace opengl
{

/////////////////////////////////////////////////////////////////////////////
// window

window::window() :
	application_widget(*this)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
}

void window::on_render_offscreen_clicked()
{
	try
	{
		const boost::scoped_ptr<k3d::gl::offscreen_context_factory> factory(k3d::plugin::create<k3d::gl::offscreen_context_factory>("OSMesaContextFactory"));
		if(!factory)
			throw std::runtime_error("Error creating context factory");

		const boost::scoped_ptr<k3d::gl::offscreen_context> context(factory->create(5, 5));
		if(!context)
			throw std::runtime_error("Error creating context");

		k3d::log() << debug;
		std::copy(context->buffer_begin(), context->buffer_end(), std::ostream_iterator<int>(k3d::log(), " "));
		k3d::log() << std::endl;

		const k3d::gl::api& gl = context->begin();
		gl.glClearColor(1.0, 0.5, 0.25, 0.125);
		gl.glClear(gl.GL_COLOR_BUFFER_BIT);
		gl.glFlush();
		context->end();

		k3d::log() << debug;
		std::copy(context->buffer_begin(), context->buffer_end(), std::ostream_iterator<int>(k3d::log(), " "));
		k3d::log() << std::endl;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << e.what() << std::endl;
	}
}

k3d::iplugin_factory& window::get_factory()
{
	static k3d::application_plugin_factory<window> factory(
		k3d::uuid(0x9704d807, 0xec420781, 0x96396b9e, 0xfe8f786e),
		"QTUIOpenGLWindow",
		_("Used to test OpenGL development."),
		"QTUI Dialog",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "dialog"));

	return factory;
}

} // namespace opengl

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::opengl::window::get_factory());
K3D_MODULE_END

