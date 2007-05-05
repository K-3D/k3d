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

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/selection.h>

#define DEFAULT_SCRIPT "#python\n\n\
from OpenGL.GL import *\n\n\
glPushAttrib(GL_ALL_ATTRIB_BITS)\n\
glDisable(GL_LIGHTING)\n\
glPointSize(5)\n\
glColor3d(0, 0, 1)\n\n\
glBegin(GL_POINTS)\n\
for point in Mesh.points():\n\
	glVertex3d(point[0], point[1], point[2])\n\
glEnd()\n\n\
glPopAttrib()\n\n"

namespace libk3dscripting
{

/////////////////////////////////////////////////////////////////////////////
// mesh_painter_script

class mesh_painter_script :
	public k3d::scripted_node<k3d::persistent<k3d::node> >,
	public k3d::gl::imesh_painter
{
	typedef k3d::scripted_node<k3d::persistent<k3d::node> > base;

public:
	mesh_painter_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);
	}

	void paint_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
		context["Mesh"] = &const_cast<k3d::dev::mesh&>(Mesh);
		context["PaintMesh"] = true;
		context["SelectMesh"] = false;
		execute_script(context);
	}

	void select_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		k3d::gl::push_selection_token(this);

		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
		context["Mesh"] = &const_cast<k3d::dev::mesh&>(Mesh);
		context["PaintMesh"] = false;
		context["SelectMesh"] = true;
		execute_script(context);

		k3d::gl::pop_selection_token();
	}

	void mesh_changed(const k3d::dev::mesh& Mesh, k3d::iunknown* Hint)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_painter_script,
			k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0xd841c417, 0x889a44d7, 0x8fe520eb, 0xf26dc650),
			"MeshPainterScript",
			_("Scripted Mesh Painter"),
			"OpenGL Painters Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_painter_script_factory

k3d::iplugin_factory& mesh_painter_script_factory()
{
	return mesh_painter_script::get_factory();
}

} // namespace libk3dscripting

