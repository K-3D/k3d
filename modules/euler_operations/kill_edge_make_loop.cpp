// K-3D
// Copyright (c) 2005-2009 Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/euler_operations.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace euler
{

/////////////////////////////////////////////////////////////////////////////
// kill_edge_make_loop

class kill_edge_make_loop :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	kill_edge_make_loop(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// If there are no valid polyhedra, we give up
		if(!k3d::validate_polyhedra(Input))
		{
			return;
		}
		
		Output = Input; // shallow copy, so all non-polyhedra are untouched
		
		k3d::mesh::selection_t input_edge_selection = *Input.polyhedra->edge_selection;
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::mesh_selection::merge(mesh_selection.edges, input_edge_selection);
		
		const k3d::mesh::points_t& points = *Input.points;
		const k3d::mesh::indices_t& edge_points = *Input.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::mesh::indices_t& loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& face_first_loops = *Input.polyhedra->face_first_loops;
		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_first_loops.size();
		
		k3d::mesh::bools_t boundary_edges;
		k3d::mesh::indices_t companions;
		k3d::polyhedron::create_edge_adjacency_lookup(edge_points, clockwise_edges, boundary_edges, companions);
		
		// Calculate the face normals
		k3d::mesh::normals_t face_normals(face_first_loops.size());
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
				face_normals[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));
		}
		
		k3d::euler::kill_edge_make_loop(Output.polyhedra.writable(), points, input_edge_selection, boundary_edges, companions, face_normals);
		
		k3d::mesh::delete_unused_points(Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<kill_edge_make_loop,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xe7bd38c1, 0xa841a725, 0x22e170b0, 0x417ba28a),
				"EulerKillEdgeMakeLoop",
				_("Apply the Kill Edge Make Loop (KEML) Euler operation to the selected edges"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// kill_edge_make_loop_factory

k3d::iplugin_factory& kill_edge_make_loop_factory()
{
	return kill_edge_make_loop::get_factory();
}

} // namespace euler

} // namespace module

