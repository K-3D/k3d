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
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
import k3d\n\n\
positions = [(-5, -5, 0), (5, -5, 0), (5, 5, 0), (-5, 5, 0)]\n\n\
points = Output.create_points()\n\
point_selection = Output.create_point_selection()\n\
for position in positions:\n\
	points.append(k3d.point3(position[0], position[1], position[2]))\n\n\
	point_selection.append(0.0)\n\
polyhedra = Output.create_polyhedra()\n\n\
first_faces = polyhedra.create_first_faces()\n\
first_faces.assign([0])\n\n\
face_counts = polyhedra.create_face_counts()\n\
face_counts.assign([1])\n\n\
types = polyhedra.create_types()\n\
types.assign([k3d.polyhedron_type.polygons])\n\n\
face_first_loops = polyhedra.create_face_first_loops()\n\
face_first_loops.assign([0])\n\n\
face_loop_counts = polyhedra.create_face_loop_counts()\n\
face_loop_counts.assign([1])\n\n\
face_materials = polyhedra.create_face_materials()\n\
face_materials.assign([None])\n\n\
face_selection = polyhedra.create_face_selection()\n\
face_selection.assign([0.0])\n\n\
loop_first_edges = polyhedra.create_loop_first_edges()\n\
loop_first_edges.assign([0])\n\n\
edge_points = polyhedra.create_edge_points()\n\
edge_points.assign([0, 1, 2, 3])\n\n\
clockwise_edges = polyhedra.create_clockwise_edges()\n\
clockwise_edges.assign([1, 2, 3, 0])\n\n\
edge_selection = polyhedra.create_edge_selection()\n\
edge_selection.assign([0.0, 0.0, 0.0, 0.0])\n\n"

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// mesh_source_script

class mesh_source_script :
	public k3d::scripted_node<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	mesh_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);

		connect_script_changed_signal(make_topology_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Output"] = &Mesh;

		execute_script(context);

		k3d::validate(Mesh);
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_source_script, k3d::interface_list<k3d::imesh_source> > factory(
			k3d::uuid(0xc68d0187, 0xbb334026, 0xb4ca6a9f, 0x0d980cc9),
			"MeshSourceScript",
			_("Mesh source that uses a script to create geometry"),
			"Scripting Mesh",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_source_script_factory

k3d::iplugin_factory& mesh_source_script_factory()
{
	return mesh_source_script::get_factory();
}

} // namespace scripting

} // namespace module


