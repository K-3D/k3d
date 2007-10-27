// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>

#define DEFAULT_SCRIPT "#python\n\n\
try:\n\
  from OpenGL.GL import *\n\n\
  glDisable(GL_LIGHTING)\n\
  glColor3f(1, 1, 0)\n\n\
  glBegin(GL_POLYGON)\n\
  glVertex3f(-1, -1, 0)\n\
  glVertex3f(1, -1, 0)\n\
  glVertex3f(1, 1, 0)\n\
  glVertex3f(-1, 1, 0)\n\
  glEnd()\n\n\
except:\n\
  print \"Couldn't load OpenGL module\"\n\n"

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// gl_renderable_script

class gl_renderable_script :
	public k3d::scripted_node<k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > >
{
	typedef k3d::scripted_node<k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > base;

public:
	gl_renderable_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		execute_script(context);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);

		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		execute_script(context);

		k3d::gl::pop_selection_token();
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<gl_renderable_script,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
			k3d::uuid(0xd57c84cc, 0xa9474b12, 0xa344763c, 0x8f4e7c8e),
			"GLRenderableScript",
			_("Scripted OpenGL Drawable"),
			"OpenGL Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// gl_renderable_script_factory

k3d::iplugin_factory& gl_renderable_script_factory()
{
	return gl_renderable_script::get_factory();
}

} // namespace scripting

} // namespace module


