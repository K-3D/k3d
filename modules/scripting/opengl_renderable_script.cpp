// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// opengl_renderable_script

class opengl_renderable_script :
	public k3d::scripted_node<k3d::gl::renderable<k3d::transformable<k3d::node > > >
{
	typedef k3d::scripted_node<k3d::gl::renderable<k3d::transformable<k3d::node > > > base;

public:
	opengl_renderable_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/opengl_renderable_script.py"));
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::iscript_engine::context context;
		context["document"] = &document();
		context["node"] = static_cast<k3d::inode*>(this);
		context["draw"] = true;
		context["select"] = false;
		execute_script(context);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);

		k3d::iscript_engine::context context;
		context["document"] = &document();
		context["node"] = static_cast<k3d::inode*>(this);
		context["draw"] = false;
		context["select"] = true;
		execute_script(context);

		k3d::gl::pop_selection_token();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<opengl_renderable_script,
			k3d::interface_list<k3d::imatrix_source,
			k3d::interface_list<k3d::imatrix_sink > > > factory(
			k3d::uuid(0xd57c84cc, 0xa9474b12, 0xa344763c, 0x8f4e7c8e),
			"OpenGLRenderableScript",
			_("Scriped node that can do arbitrary rendering using OpenGL."),
			"OpenGL Script",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// opengl_renderable_script_factory

k3d::iplugin_factory& opengl_renderable_script_factory()
{
	return opengl_renderable_script::get_factory();
}

} // namespace scripting

} // namespace module


