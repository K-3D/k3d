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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "array_helpers.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace module
{

namespace renderman
{

namespace painters
{

/////////////////////////////////////////////////////////////////////////////
// scripted_painter

class scripted_painter :
	public k3d::scripted_node<k3d::node>,
	public k3d::ri::imesh_painter
{
	typedef k3d::scripted_node<k3d::node> base;

public:
	scripted_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		// Get a path where we can store the scripted RIB output ...
		const k3d::filesystem::path archive_path = RenderState.frame.add_file("scripted_painter");

		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Input"] = const_cast<k3d::mesh*>(&Mesh);
		context["Archive"] = archive_path;
		context["RenderState"] = &RenderState;

		return_if_fail(execute_script(context));

		RenderState.stream.RiReadArchive(archive_path);
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scripted_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x211913e0, 0xa54aec62, 0x938b32a0, 0x0e5858d9),
			"RenderManScriptedPainter",
			_("Uses a script to render meshes"),
			"RenderMan Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// scripted_painter.cpp_factory

k3d::iplugin_factory& scripted_painter_factory()
{
	return scripted_painter::get_factory();
}

} // namespace painters

} // namespace renderman

} // namespace module

