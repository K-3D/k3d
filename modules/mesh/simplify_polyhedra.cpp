// K-3D
// Copyright (c) 2005-2008 Timothy M. Shead
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

namespace detail
{

template<class ArrayT> void cumulative_sum(const ArrayT& InputArray, ArrayT& OutputArray)
{
	return_if_fail(InputArray.size() == OutputArray.size());
	const k3d::uint_t array_begin = 0;
	const k3d::uint_t array_end = InputArray.size();
	OutputArray[0] = InputArray[0];
	for(k3d::uint_t i = array_begin + 1; i != array_end; ++i)
		OutputArray[i] = OutputArray[i-1] + InputArray[i];
}

/// Sets the loop number associated with each edge in the loop to Loop
void set_edge_loop(const uint_t Loop, const uint_t FirstEdge, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& EdgeLoops)
{
	for(uint_t edge = FirstEdge; ;)
	{
		EdgeLoops[edge] = Loop;
		
		edge = ClockwiseEdges[edge];
		if(edge == FirstEdge)
			break;
	}
}

/// True if the given loop is really a polyline of adjacent edges
bool_t is_polyline(const uint_t FirstEdge, const mesh::indices_t& ClockwiseEdges, const mesh::indices_t& EdgeLoops, const mesh::indices_t& AdjacentEdges)
{
	for(uint_t edge = FirstEdge; ;)
	{
		if(EdgeLoops[AdjacentEdges[edge]] != EdgeLoops[edge])
			return false;
		
		edge = ClockwiseEdges[edge];
		if(edge == FirstEdge)
			break;
	}
	
	return true;
}

}

/// Marks edges that are shared by coplanar faces among the selected faces
void mark_redundant_edges(const mesh::indices_t& Companions,
		const mesh::bools_t& BoundaryEdges,
		const mesh::normals_t& Normals,
		const mesh::indices_t& EdgeFaces,
		const mesh::selection_t& FaceSelection,
		mesh::selection_t& RedundantEdges,
		const double_t Threshold = 1e-8)
{
	RedundantEdges.assign(Companions.size(), 0.0);
	for(uint_t edge = 0; edge != Companions.size(); ++edge)
	{
		if(BoundaryEdges[edge])
			continue;
		
		const uint_t face = EdgeFaces[edge];
		if(!FaceSelection[face])
			continue;
		
		const uint_t companion = Companions[edge];
		const uint_t companion_face = EdgeFaces[companion];
		if(!FaceSelection[companion_face])
			continue;
		
		if(std::abs((Normals[face] * Normals[companion_face]) - 1) < Threshold)
			RedundantEdges[edge] = 1.0;
	}
}

/// Adapts face loops so the straight edges on the selected faces have two vertices
void simplify_straight_edges(const mesh::points_t& Points, mesh::polyhedra_t& Output, const mesh::selection_t& FaceSelection, const mesh::counts_t& VertexValences, const double_t Threshold = 1e-8)
{
	const mesh::indices_t& input_face_first_loops = *Output.face_first_loops;
	const mesh::counts_t& input_face_loop_counts = *Output.face_loop_counts;
	const mesh::indices_t input_loop_first_edges = *Output.loop_first_edges;
	const mesh::indices_t input_clockwise_edges = *Output.clockwise_edges;
	const mesh::indices_t input_edge_points = *Output.edge_points;
	
	mesh::indices_t edges_to_delete(input_edge_points.size(), 0);
	mesh::indices_t new_clockwise_edges = input_clockwise_edges;
	mesh::indices_t& output_loop_first_edges = Output.loop_first_edges.writable();
	
	const uint_t face_begin = 0;
	const uint_t face_end = input_face_first_loops.size();
	for(uint_t face = face_begin; face != face_end; ++face)
	{
		if(!FaceSelection[face])
			continue;
		const uint_t loop_begin = input_face_first_loops[face];
		const uint_t loop_end = loop_begin + input_face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const uint_t first_edge = input_loop_first_edges[loop];
			for(uint_t edge = first_edge; ;)
			{
				if(!edges_to_delete[edge])
				{
					uint_t clockwise = input_clockwise_edges[edge];
					const k3d::point3& vertex = Points[input_edge_points[edge]];
					const k3d::point3& clockwise_vertex = Points[input_edge_points[clockwise]];
					const k3d::vector3 start_vector = k3d::normalize(clockwise_vertex - vertex);
					k3d::vector3 clockwise_vector = k3d::normalize(Points[input_edge_points[input_clockwise_edges[clockwise]]] - clockwise_vertex);
					while(std::abs((start_vector * clockwise_vector) - 1) < Threshold)
					{
						if(clockwise == first_edge)
							output_loop_first_edges[loop] = edge;
						edges_to_delete[clockwise] = 1;
						clockwise = input_clockwise_edges[clockwise];
						new_clockwise_edges[edge] = clockwise;
						clockwise_vector = k3d::normalize(Points[input_edge_points[input_clockwise_edges[clockwise]]] - clockwise_vertex);
					}
				}

				edge = input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	
	mesh::indices_t edges_to_delete_sum(edges_to_delete.size());
	detail::cumulative_sum(edges_to_delete, edges_to_delete_sum);
	mesh::indices_t& output_edge_points = Output.edge_points.create();
	mesh::indices_t& output_clockwise_edges = Output.clockwise_edges.create();
	for(uint_t edge = 0; edge != input_edge_points.size(); ++edge)
	{
		if(!edges_to_delete[edge])
		{
			output_edge_points.push_back(input_edge_points[edge]);
			output_clockwise_edges.push_back(new_clockwise_edges[edge] - edges_to_delete_sum[new_clockwise_edges[edge]]);
		}
	}
	Output.edge_selection.create(new mesh::selection_t(output_edge_points.size(), 0.0));
	for(uint_t loop = 0; loop != output_loop_first_edges.size(); ++loop)
		output_loop_first_edges[loop] -= edges_to_delete_sum[output_loop_first_edges[loop]];
}

/// Apply the Kill Edge Make Loop (KEML) Euler operation to all selected edges that are not boundary edges 
void kill_edge_make_loop(mesh::polyhedra_t& Output, const mesh::points_t& Points, const mesh::selection_t& EdgeSelection, const mesh::bools_t BoundaryEdges, const mesh::indices_t& AdjacentEdges)
{
	// Copies, so we can use them as temp storage between the individiual KEML operations
	const mesh::indices_t face_first_loops = *Output.face_first_loops;
	mesh::counts_t face_loop_counts = *Output.face_loop_counts;
	mesh::indices_t loop_first_edges = *Output.loop_first_edges;
	mesh::indices_t clockwise_edges = *Output.clockwise_edges;
	const mesh::indices_t edge_points = *Output.edge_points;
	
	// Initiate some helper arrays
	const uint_t face_count = face_first_loops.size();
	const uint_t loop_count = loop_first_edges.size();
	const uint_t edge_count = clockwise_edges.size();
	mesh::selection_t face_selection(face_count, 0.0);
	mesh::counts_t faces_to_delete(face_count, 0);
	mesh::counts_t loops_to_delete(loop_count, 0);
	mesh::counts_t edges_to_delete(edge_count, 0);
	mesh::indices_t loop_faces(loop_count); // for each loop, the face it belongs to
	mesh::indices_t edge_loops(edge_count); // for each edge, the loop it belongs to
	mesh::indices_t counter_clockwise_edges(edge_count);
	for(uint_t face = 0; face != face_count; ++face) // fill the above arrays
	{
		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			loop_faces[loop] = face;
			const uint_t first_edge = loop_first_edges[loop];
			for(uint_t edge = first_edge; ;)
			{
				edge_loops[edge] = loop;
				const uint_t clockwise = clockwise_edges[edge];
				counter_clockwise_edges[clockwise] = edge;

				edge = clockwise;
				if(edge == first_edge)
					break;
			}
		}
	}
	
	mesh::indices_t affected_edges; // keeps one edge for each adjacent edge pair that is selected and not on the boundary
	for(uint_t edge = 0; edge != edge_count; ++edge)
		if(edge < AdjacentEdges[edge] && EdgeSelection[edge])
			affected_edges.push_back(edge);
	
	for(uint_t i = 0; i != affected_edges.size(); ++i)
	{
		const uint_t edge = affected_edges[i];
		const uint_t companion = AdjacentEdges[edge];
		const uint_t loop = edge_loops[edge];
		const uint_t face = loop_faces[loop];
		const uint_t companion_loop = edge_loops[companion];
		const uint_t companion_face = loop_faces[companion_loop];
		
		return_if_fail(clockwise_edges[counter_clockwise_edges[edge]] == edge);
		return_if_fail(clockwise_edges[counter_clockwise_edges[companion]] == companion);
		
		// Mark edges to delete
		edges_to_delete[edge] = 1;
		edges_to_delete[companion] = 1;
		
		face_selection[face] = 1.0;
		
		// if a new hole will be created, we calculate the face normal
		normal3 face_normal;
		if(loop == companion_loop && loop == face_first_loops[face])
			face_normal = normalize(normal(edge_points, clockwise_edges, Points, edge));
		
		// last two edges of a loop -> just delete it. This should never happen to a face_first_loop
		if(loop == companion_loop && companion == clockwise_edges[edge] && companion == counter_clockwise_edges[edge])
		{
			k3d::log() << debug << "kill_edge_make_loop: use case: last two edges (edge " << edge << ")" << std::endl;
			return_if_fail(loop != face_first_loops[face]);
			loops_to_delete[loop] = 1;
			--face_loop_counts[face];
			continue;
		}
		
		// antenna's
		if(loop == companion_loop && companion == clockwise_edges[edge])
		{
			k3d::log() << debug << "kill_edge_make_loop: use case: antenna 1 (edge " << edge << ")" << std::endl;
			clockwise_edges[counter_clockwise_edges[edge]] = clockwise_edges[companion];
			counter_clockwise_edges[clockwise_edges[companion]] = counter_clockwise_edges[edge];
			loop_first_edges[loop] = counter_clockwise_edges[edge];
			continue;
		}
		if(loop == companion_loop && companion == counter_clockwise_edges[edge])
		{
			k3d::log() << debug << "kill_edge_make_loop: use case: antenna 2 (edge " << edge << ")" << std::endl;
			clockwise_edges[counter_clockwise_edges[companion]] = clockwise_edges[edge];
			counter_clockwise_edges[clockwise_edges[edge]] = counter_clockwise_edges[companion];
			loop_first_edges[loop] = clockwise_edges[edge];
			continue;
		}
		                                                          
		// Reconnect edges
		clockwise_edges[counter_clockwise_edges[edge]] = clockwise_edges[companion];
		clockwise_edges[counter_clockwise_edges[companion]] = clockwise_edges[edge];
		counter_clockwise_edges[clockwise_edges[edge]] = counter_clockwise_edges[companion];
		counter_clockwise_edges[clockwise_edges[companion]] = counter_clockwise_edges[edge];
		
		// First loop contained an adjacent edge that was part of that same loop -> a hole must be created or split
		if(loop == companion_loop)
		{
			const uint_t first_edge_1 = counter_clockwise_edges[edge];
			const uint_t first_edge_2 = counter_clockwise_edges[companion];
			// Create a real hole
			if(loop == face_first_loops[face] && !detail::is_polyline(first_edge_1, clockwise_edges, edge_loops, AdjacentEdges) && !detail::is_polyline(first_edge_2, clockwise_edges, edge_loops, AdjacentEdges))
			{
				k3d::log() << debug << "kill_edge_make_loop: use case: new hole (edge " << edge << ")" << std::endl;
				const uint_t hole = loop_first_edges.size();
				const double threshold = 1e-8;
				const normal3 normal_1 = normalize(normal(edge_points, clockwise_edges, Points, first_edge_1));
				const normal3 normal_2 = normalize(normal(edge_points, clockwise_edges, Points, first_edge_2));
				return_if_fail(std::abs((normal_1 * normal_2) + 1) < threshold);
				if(std::abs((normal_1 * face_normal) + 1) < threshold)
				{
					loop_first_edges[loop] = first_edge_2;
					loop_first_edges.push_back(first_edge_1);
					detail::set_edge_loop(loop, first_edge_2, clockwise_edges, edge_loops);
					detail::set_edge_loop(hole, first_edge_1, clockwise_edges, edge_loops);
				}
				else
				{
					loop_first_edges[loop] = first_edge_1;
					loop_first_edges.push_back(first_edge_2);
					detail::set_edge_loop(loop, first_edge_1, clockwise_edges, edge_loops);
					detail::set_edge_loop(hole, first_edge_2, clockwise_edges, edge_loops);
				}
				loops_to_delete.push_back(0);
				loop_faces.push_back(face);
				++face_loop_counts[face];
			}
			else // Create or split a polyline hole
			{
				k3d::log() << debug << "kill_edge_make_loop: use case: polyline hole (edge " << edge << ")" << std::endl;
				const uint_t new_hole = loop_first_edges.size();
				if(detail::is_polyline(first_edge_1, clockwise_edges, edge_loops, AdjacentEdges))
				{
					return_if_fail(!detail::is_polyline(first_edge_2, clockwise_edges, edge_loops, AdjacentEdges));
					loop_first_edges[loop] = first_edge_2;
					loop_first_edges.push_back(first_edge_1);
					detail::set_edge_loop(loop, first_edge_2, clockwise_edges, edge_loops);
					detail::set_edge_loop(new_hole, first_edge_1, clockwise_edges, edge_loops);
				}
				else
				{
					loop_first_edges[loop] = first_edge_1;
					loop_first_edges.push_back(first_edge_2);
					detail::set_edge_loop(loop, first_edge_1, clockwise_edges, edge_loops);
					detail::set_edge_loop(new_hole, first_edge_2, clockwise_edges, edge_loops);
				}
				loop_faces.push_back(face);
				loops_to_delete.push_back(0);
				++face_loop_counts[face];
			}
		}
		else // edges belong to different loops
		{
			k3d::log() << debug << "kill_edge_make_loop: use case: different loops (edge " << edge << ")" << std::endl;
			loop_first_edges[loop] = counter_clockwise_edges[edge];
			detail::set_edge_loop(loop, loop_first_edges[loop], clockwise_edges, edge_loops);
			loops_to_delete[companion_loop] = 1;
			--face_loop_counts[companion_face];
			if(companion_loop == face_first_loops[companion_face])
			{
				faces_to_delete[companion_face] = 1;
				const uint_t loop_begin = face_first_loops[companion_face] + 1;
				const uint_t loop_end = face_first_loops[companion_face] + Output.face_loop_counts->at(companion_face);
				for(uint_t l = loop_begin; l != loop_end; ++l)
				{
					if(!loops_to_delete[l])
					{
						loop_faces[l] = face;
						++face_loop_counts[face];
					}
				}
				// Check if loops were added to the face that will be deleted
				if(Output.face_loop_counts->at(companion_face) < face_loop_counts[companion_face])
				{
					for(uint_t l = loop_count; l != face_loop_counts.size(); ++l)
					{
						if(loop_faces[l] == companion_face && !loops_to_delete[l])
						{
							loop_faces[l] = face;
							++face_loop_counts[face];
						}
					}
				}
			}
		}
	}
	
	// Update the output arrays
	mesh::counts_t edges_to_delete_sum(edge_count);
	detail::cumulative_sum(edges_to_delete, edges_to_delete_sum);
	mesh::indices_t loop_map(loop_first_edges.size()); // mapping between old and new loop numbers
	mesh::indices_t& output_loop_first_edges = Output.loop_first_edges.create();
	mesh::indices_t& output_face_first_loops = Output.face_first_loops.create();
	mesh::indices_t& output_face_loop_counts = Output.face_loop_counts.create();
	mesh::selection_t& output_face_selection = Output.face_selection.create();
	mesh::indices_t face_map(face_count);
	for(uint_t loop = 0; loop != loop_first_edges.size(); ++loop)
	{
		if(!loops_to_delete[loop])
		{
			const uint_t face = loop_faces[loop];
			return_if_fail(!faces_to_delete[face]);
			if(loop == face_first_loops[face])
			{
				face_map[face] = output_face_first_loops.size();
				output_face_first_loops.push_back(output_loop_first_edges.size());
				output_face_loop_counts.push_back(face_loop_counts[face]);
				output_face_selection.push_back(face_selection[face]);
				return_if_fail(!edges_to_delete[loop_first_edges[loop]]);
				output_loop_first_edges.push_back(loop_first_edges[loop] - edges_to_delete_sum[loop_first_edges[loop]]);
				output_loop_first_edges.resize(output_loop_first_edges.size() + face_loop_counts[face] - 1);
			}
		}
	}
	mesh::counts_t added_loops(output_face_first_loops.size(), 1);
	for(uint_t loop = 0; loop != loop_first_edges.size(); ++loop)
	{
		if(!loops_to_delete[loop])
		{
			const uint_t face = loop_faces[loop];
			const uint_t new_face = face_map[face];
			if(loop != face_first_loops[face])
			{
				output_loop_first_edges[output_face_first_loops[new_face] + added_loops[new_face]] = loop_first_edges[loop] - edges_to_delete_sum[loop_first_edges[loop]];
				++added_loops[new_face];
			}
		}
	}
	mesh::indices_t& output_edge_points = Output.edge_points.create();
	mesh::indices_t& output_clockwise_edges = Output.clockwise_edges.create();
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		if(!edges_to_delete[edge])
		{
			output_edge_points.push_back(edge_points[edge]);
			output_clockwise_edges.push_back(clockwise_edges[edge] - edges_to_delete_sum[clockwise_edges[edge]]);
		}
	}
	const mesh::indices_t first_faces = *Output.first_faces;
	const mesh::indices_t face_counts = *Output.face_counts;
	const mesh::polyhedra_t::types_t types = *Output.types;
	mesh::indices_t& output_first_faces = Output.first_faces.create();
	mesh::indices_t& output_face_counts = Output.face_counts.create();
	mesh::polyhedra_t::types_t& output_types = Output.types.create();
	mesh::counts_t faces_to_delete_sum(faces_to_delete.size());
	detail::cumulative_sum(faces_to_delete, faces_to_delete_sum);
	uint_t new_first_face = 0;
	uint_t last_delete_count = 0;
	for(uint_t polyhedron = 0; polyhedron != first_faces.size(); ++polyhedron)
	{
		const uint_t last_face = first_faces[polyhedron] + face_counts[polyhedron] - 1;
		const uint_t new_face_count = face_counts[polyhedron] - (faces_to_delete_sum[last_face] - last_delete_count);
		last_delete_count = faces_to_delete_sum[last_face];
		if(new_face_count != 0)
		{
			output_first_faces.push_back(new_first_face);
			output_face_counts.push_back(new_face_count);
			output_types.push_back(types[polyhedron]);
			new_first_face += new_face_count; 
		}
	}
	Output.face_materials.create(new mesh::materials_t(output_face_counts.back(), static_cast<imaterial*>(0)));
	Output.edge_selection.create(new mesh::selection_t(output_clockwise_edges.size()));
}

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
		
		k3d::mesh::indices_t edge_faces;
		k3d::polyhedron::create_edge_face_lookup(face_first_loops, *Input.polyhedra->face_loop_counts, loop_first_edges, clockwise_edges, edge_faces);
		k3d::mesh::selection_t redundant_edges;
		k3d::polyhedron::mark_redundant_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges);
		
		k3d::mesh::polyhedra_t& output_polyhedra = Output.polyhedra.writable();
		k3d::polyhedron::kill_edge_make_loop(output_polyhedra, points, redundant_edges, boundary_edges, companions);
		
		k3d::mesh::counts_t vertex_valences;
		k3d::polyhedron::create_vertex_valence_lookup(Output.points->size(), *Output.polyhedra->edge_points, vertex_valences);
		k3d::polyhedron::simplify_straight_edges(*Output.points, output_polyhedra, *Output.polyhedra->face_selection, vertex_valences);
		
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

