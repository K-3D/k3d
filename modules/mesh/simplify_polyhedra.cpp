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

namespace k3d
{

namespace polyhedron
{

/// Adapts face loops so the straight edges on the selected faces have two vertices
//void simplify_straight_edges(const mesh::points_t& Points, mesh::polyhedra_t& Output, const mesh::selection_t& FaceSelection, const mesh::counts_t& VertexValences, const double_t Threshold = 1e-8)
//{
//	const mesh::indices_t& input_face_first_loops = *Output.face_first_loops;
//	const mesh::counts_t& input_face_loop_counts = *Output.face_loop_counts;
//	const mesh::indices_t input_loop_first_edges = *Output.loop_first_edges;
//	const mesh::indices_t input_clockwise_edges = *Output.clockwise_edges;
//	const mesh::indices_t input_edge_points = *Output.edge_points;
//	
//	mesh::indices_t edges_to_delete(input_edge_points.size(), 0);
//	mesh::indices_t new_clockwise_edges = input_clockwise_edges;
//	mesh::indices_t& output_loop_first_edges = Output.loop_first_edges.writable();
//	
//	const uint_t face_begin = 0;
//	const uint_t face_end = input_face_first_loops.size();
//	for(uint_t face = face_begin; face != face_end; ++face)
//	{
//		if(!FaceSelection[face])
//			continue;
//		const uint_t loop_begin = input_face_first_loops[face];
//		const uint_t loop_end = loop_begin + input_face_loop_counts[face];
//		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
//		{
//			const uint_t first_edge = input_loop_first_edges[loop];
//			for(uint_t edge = first_edge; ;)
//			{
//				if(!edges_to_delete[edge])
//				{
//					uint_t clockwise = input_clockwise_edges[edge];
//					const k3d::point3& vertex = Points[input_edge_points[edge]];
//					const k3d::point3& clockwise_vertex = Points[input_edge_points[clockwise]];
//					const k3d::vector3 start_vector = k3d::normalize(clockwise_vertex - vertex);
//					k3d::vector3 clockwise_vector = k3d::normalize(Points[input_edge_points[input_clockwise_edges[clockwise]]] - clockwise_vertex);
//					while(std::abs((start_vector * clockwise_vector) - 1) < Threshold)
//					{
//						if(clockwise == first_edge)
//							output_loop_first_edges[loop] = edge;
//						edges_to_delete[clockwise] = 1;
//						clockwise = input_clockwise_edges[clockwise];
//						new_clockwise_edges[edge] = clockwise;
//						clockwise_vector = k3d::normalize(Points[input_edge_points[input_clockwise_edges[clockwise]]] - clockwise_vertex);
//					}
//				}
//
//				edge = input_clockwise_edges[edge];
//				if(edge == first_edge)
//					break;
//			}
//		}
//	}
//	
//	mesh::indices_t edges_to_delete_sum(edges_to_delete.size());
//	detail::cumulative_sum(edges_to_delete, edges_to_delete_sum);
//	mesh::indices_t& output_edge_points = Output.edge_points.create();
//	mesh::indices_t& output_clockwise_edges = Output.clockwise_edges.create();
//	for(uint_t edge = 0; edge != input_edge_points.size(); ++edge)
//	{
//		if(!edges_to_delete[edge])
//		{
//			output_edge_points.push_back(input_edge_points[edge]);
//			output_clockwise_edges.push_back(new_clockwise_edges[edge] - edges_to_delete_sum[new_clockwise_edges[edge]]);
//		}
//	}
//	Output.edge_selection.create(new mesh::selection_t(output_edge_points.size(), 0.0));
//	for(uint_t loop = 0; loop != output_loop_first_edges.size(); ++loop)
//		output_loop_first_edges[loop] -= edges_to_delete_sum[output_loop_first_edges[loop]];
//}


} // namespace polyhedron
		
} // namespace k3d

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// simplify_polyhedra

class simplify_polyhedra :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	simplify_polyhedra(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
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
		
		k3d::mesh::selection_t input_face_selection = *Input.polyhedra->face_selection;
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::mesh_selection::merge(mesh_selection.faces, input_face_selection);
		
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
		
		// Calculate the face normals of the selected faces
		k3d::mesh::normals_t face_normals(face_first_loops.size());
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			if(input_face_selection[face])
				face_normals[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));
		}
		
//		k3d::mesh::indices_t edge_faces;
//		k3d::polyhedron::create_edge_face_lookup(face_first_loops, *Input.polyhedra->face_loop_counts, loop_first_edges, clockwise_edges, edge_faces);
//		k3d::mesh::selection_t redundant_edges = *Input.polyhedra->edge_selection;
//		k3d::polyhedron::mark_redundant_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges);
//		k3d::mesh_selection::merge(mesh_selection.edges, redundant_edges);
//		
//		k3d::mesh::polyhedra_t& output_polyhedra = Output.polyhedra.writable();
//		k3d::polyhedron::kill_edge_make_loop(output_polyhedra, points, redundant_edges, boundary_edges, companions, face_normals);
//		
//		k3d::mesh::counts_t vertex_valences;
//		k3d::polyhedron::create_vertex_valence_lookup(Output.points->size(), *Output.polyhedra->edge_points, vertex_valences);
//		k3d::polyhedron::kill_edge_and_vertex(Output.polyhedra.writable(), redundant_edges, boundary_edges, companions, vertex_valences);
//		k3d::polyhedron::simplify_straight_edges(*Output.points, output_polyhedra, *Output.polyhedra->face_selection, vertex_valences);
		
		k3d::mesh::delete_unused_points(Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<simplify_polyhedra,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x619a9cc1, 0x134fd447, 0x08878ebd, 0x931f927e),
				"SimplifyPolyhedra",
				_("Simplifies polyhedra, retaining the geometry"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// simplify_polyhedra_factory

k3d::iplugin_factory& simplify_polyhedra_factory()
{
	return simplify_polyhedra::get_factory();
}

} // namespace mesh

} // namespace module

