// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the RenderManScript K-3D object, which can insert scripted data into RenderMan output
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/drawable_gl.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/irender_farm.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>

#define DEFAULT_SCRIPT "#python\n\n\
import k3d\n\
import math\n\
import sys\n\
import ri\n\
from ri import *\n\n\
# Redirect output to our RIB archive\n\
ri._ribout = open(Archive, \"w\")\n\n\
RiSphere(5, -5, 5, 360)\n\n\
ri._ribout.flush()\n"

namespace
{

/////////////////////////////////////////////////////////////////////////////
// renderman_script

class renderman_script :
	public k3d::scripted_node<k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef k3d::scripted_node<k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	renderman_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		glDisable(GL_LIGHTING);
		glColor3d(1, 1, 0);

		glBegin(GL_POINTS);
		glVertex3d(0, 0, 0);
		glEnd();
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);

		glBegin(GL_POINTS);
		glVertex3d(0, 0, 0);
		glEnd();

		k3d::gl::pop_selection_token();
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		// Get a path where we can store the scripted RIB output ...
		const k3d::filesystem::path archive_path = State.frame.add_input_file("renderman_script");

		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Archive"] = archive_path;
		context["RenderState"] = &State;

		return_if_fail(execute_script(context));

		State.engine.RiReadArchive(archive_path);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<renderman_script,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
			k3d::uuid(0xded017f6, 0xa08849fd, 0xa9d8dd63, 0xe3cd6f1d),
			"RenderManScript",
			_("Creates RenderMan output under script control"),
			"RenderMan Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

} // namespace

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// renderman_script_factory

k3d::iplugin_factory& renderman_script_factory()
{
	return renderman_script::get_factory();
}

} // namespace libk3drenderman

