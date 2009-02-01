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

#include "euler_operations.h"
#include "mesh_operations.h"
#include "polyhedron.h"

#include <set>

namespace k3d
{

namespace euler
{

namespace detail
{

template<class ArrayT> void cumulative_sum(const ArrayT& InputArray, ArrayT& OutputArray)
{
	return_if_fail(InputArray.size() == OutputArray.size());
	const uint_t array_begin = 0;
	const uint_t array_end = InputArray.size();
	OutputArray[0] = InputArray[0];
	for(uint_t i = array_begin + 1; i != array_end; ++i)
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
		if(AdjacentEdges[edge] == edge || EdgeLoops[AdjacentEdges[edge]] != EdgeLoops[edge])
			return false;
		
		edge = ClockwiseEdges[edge];
		if(edge == FirstEdge)
			break;
	}
	
	return true;
}

/// Deletes the given loop
void delete_loop(const uint_t Loop, const mesh::indices_t& LoopFaces, const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& FacesToDelete, mesh::indices_t& LoopsToDelete, mesh::indices_t& EdgesToDelete, mesh::indices_t& Companions, mesh::indices_t& OutputFaceLoopCounts)
{
	const uint_t face = LoopFaces[Loop];
	--OutputFaceLoopCounts[face];
	if(Loop == FaceFirstLoops[face])
	{
		FacesToDelete[face] = 1;
		for(uint_t loop = 0; loop != LoopFaces.size(); ++loop)
		{
			if(LoopFaces[loop] == face && !LoopsToDelete[loop])
			{
				LoopsToDelete[loop] = 1;
				--OutputFaceLoopCounts[face];
				const uint_t first_edge2 = LoopFirstEdges[loop];
				for(uint_t edge = first_edge2; ;)
				{
					//return_if_fail(!EdgesToDelete[edge]);
					EdgesToDelete[edge] = 1;
					
					edge = ClockwiseEdges[edge];
					if(edge == first_edge2)
						break;
				}
			}
		}
		LoopsToDelete[Loop] = 1;
	}
}

/// Deletes the given loop if it consists of two edges, as well as the face if tis was the last loop
void delete_loop_if_degenerate(const uint_t Loop, const mesh::indices_t& LoopFaces, const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& FacesToDelete, mesh::indices_t& LoopsToDelete, mesh::indices_t& EdgesToDelete, mesh::indices_t& Companions, mesh::indices_t& OutputFaceLoopCounts)
{
	const uint_t first_edge = LoopFirstEdges[Loop];
	if(first_edge != ClockwiseEdges[ClockwiseEdges[first_edge]]) // not a degenerate loop
		return;
	
	delete_loop(Loop, LoopFaces, FaceFirstLoops, LoopFirstEdges, ClockwiseEdges, FacesToDelete, LoopsToDelete, EdgesToDelete, Companions, OutputFaceLoopCounts);
	const uint_t companion1 = Companions[first_edge];
	const uint_t companion2 = Companions[ClockwiseEdges[first_edge]];
	Companions[companion1] = companion2;
	Companions[companion2] = companion1;
}

/// Update output arrays based on geometry that is marked for deletion
void remove_deleted_geometry(mesh::polyhedra_t& Output,
		const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const mesh::indices_t& FacesToDelete,
		const mesh::indices_t& LoopsToDelete,
		const mesh::indices_t& EdgesToDelete,
		const mesh::indices_t& LoopFaces,
		const mesh::selection_t& FaceSelection)
{
	const uint_t edge_count = EdgePoints.size();
	const uint_t face_count = FaceFirstLoops.size();
	mesh::counts_t edges_to_delete_sum(edge_count);
	detail::cumulative_sum(EdgesToDelete, edges_to_delete_sum);
	mesh::indices_t loop_map(LoopFirstEdges.size()); // mapping between old and new loop numbers
	mesh::indices_t& output_loop_first_edges = Output.loop_first_edges.create();
	mesh::indices_t& output_face_first_loops = Output.face_first_loops.create();
	mesh::indices_t& output_face_loop_counts = Output.face_loop_counts.create();
	mesh::selection_t& output_face_selection = Output.face_selection.create();
	mesh::indices_t face_map(face_count);
	for(uint_t loop = 0; loop != LoopFirstEdges.size(); ++loop)
	{
		if(!LoopsToDelete[loop] && !FacesToDelete[LoopFaces[loop]])
		{
			const uint_t face = LoopFaces[loop];
			//return_if_fail(!FacesToDelete[face]);
			if(loop == FaceFirstLoops[face])
			{
				face_map[face] = output_face_first_loops.size();
				output_face_first_loops.push_back(output_loop_first_edges.size());
				output_face_loop_counts.push_back(FaceLoopCounts[face]);
				output_face_selection.push_back(FaceSelection[face]);
				return_if_fail(!EdgesToDelete[LoopFirstEdges[loop]]);
				output_loop_first_edges.push_back(LoopFirstEdges[loop] - edges_to_delete_sum[LoopFirstEdges[loop]]);
				output_loop_first_edges.resize(output_loop_first_edges.size() + FaceLoopCounts[face] - 1);
			}
		}
	}
	mesh::counts_t added_loops(output_face_first_loops.size(), 1);
	for(uint_t loop = 0; loop != LoopFirstEdges.size(); ++loop)
	{
		if(!LoopsToDelete[loop] && !FacesToDelete[LoopFaces[loop]])
		{
			const uint_t face = LoopFaces[loop];
			const uint_t new_face = face_map[face];
			if(loop != FaceFirstLoops[face])
			{
				output_loop_first_edges[output_face_first_loops[new_face] + added_loops[new_face]] = LoopFirstEdges[loop] - edges_to_delete_sum[LoopFirstEdges[loop]];
				++added_loops[new_face];
			}
		}
	}
	mesh::indices_t& output_edge_points = Output.edge_points.create();
	mesh::indices_t& output_clockwise_edges = Output.clockwise_edges.create();
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		if(!EdgesToDelete[edge])
		{
			output_edge_points.push_back(EdgePoints[edge]);
			output_clockwise_edges.push_back(ClockwiseEdges[edge] - edges_to_delete_sum[ClockwiseEdges[edge]]);
		}
	}
	const mesh::indices_t first_faces = *Output.first_faces;
	const mesh::indices_t face_counts = *Output.face_counts;
	const mesh::polyhedra_t::types_t types = *Output.types;
	mesh::indices_t& output_first_faces = Output.first_faces.create();
	mesh::indices_t& output_face_counts = Output.face_counts.create();
	mesh::polyhedra_t::types_t& output_types = Output.types.create();
	mesh::counts_t faces_to_delete_sum(FacesToDelete.size());
	detail::cumulative_sum(FacesToDelete, faces_to_delete_sum);
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
}

/// Creates lookup arrays linking edges to their loops, and loops to their faces
void create_edge_loop_face_lookup(const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& ClockwiseEdges,
		mesh::indices_t& EdgeLoops,
		mesh::indices_t& LoopFaces)
{
	const uint_t face_count = FaceFirstLoops.size();
	EdgeLoops.resize(ClockwiseEdges.size());
	LoopFaces.resize(LoopFirstEdges.size());
	for(uint_t face = 0; face != face_count; ++face) // fill the above arrays
	{
		const uint_t loop_begin = FaceFirstLoops[face];
		const uint_t loop_end = loop_begin + FaceLoopCounts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			LoopFaces[loop] = face;
			const uint_t first_edge = LoopFirstEdges[loop];
			for(uint_t edge = first_edge; ;)
			{
				EdgeLoops[edge] = loop;

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

} // namespace detail

const uint_t number(const mesh& Mesh, const uint_t Polyhedron)
{
	return_val_if_fail(Polyhedron < polyhedron::count(Mesh), 0);
	return_val_if_fail(polyhedron::is_solid(Mesh, Polyhedron), 0);

	const mesh::indices_t& first_faces = *Mesh.polyhedra->first_faces;
	const mesh::counts_t& face_counts = *Mesh.polyhedra->face_counts;
	const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	uint_t vertex_count = 0;
	uint_t edge_count = 0;
	uint_t face_count = face_counts[Polyhedron];
	uint_t loop_count = 0;

	std::set<uint_t> vertices;
	const uint_t face_begin = first_faces[Polyhedron];
	const uint_t face_end = face_begin + face_counts[Polyhedron];
	for(uint_t face = face_begin; face != face_end; ++face)
	{
		loop_count += face_loop_counts[face];

		const uint_t loop_begin = face_first_loops[face];
		const uint_t loop_end = loop_begin + face_loop_counts[face];
		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const uint_t first_edge = loop_first_edges[loop];
			for(uint_t edge = first_edge; ;)
			{
				vertices.insert(edge_points[edge]);

				++edge_count;

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	vertex_count = vertices.size();
	edge_count /= 2;

	return vertex_count - edge_count + face_count - (loop_count - face_count);
}

void kill_edge_make_loop(mesh::polyhedra_t& Output, const mesh::indices_t& EdgeList, const mesh::bools_t BoundaryEdges, const mesh::indices_t& AdjacentEdges, const mesh::points_t& Points, const mesh::normals_t& FaceNormals)
{
	// Copies, so we can use them as temp storage between the individiual KEML operations
	const mesh::indices_t face_first_loops = *Output.face_first_loops;
	mesh::counts_t face_loop_counts = *Output.face_loop_counts;
	mesh::indices_t loop_first_edges = *Output.loop_first_edges;
	mesh::indices_t clockwise_edges = *Output.clockwise_edges;
	const mesh::indices_t edge_points = *Output.edge_points;
	
	const double_t threshold = 1e-1; // Threshold for normal comparison
	
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
	detail::create_edge_loop_face_lookup(face_first_loops, face_loop_counts, loop_first_edges, clockwise_edges, edge_loops, loop_faces);
	mesh::indices_t counter_clockwise_edges(edge_count);
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		counter_clockwise_edges[clockwise_edges[edge]] = edge;
	}
	
	mesh::selection_t marked_edges(edge_count, 0.0); 
	
	for(uint_t i = 0; i != EdgeList.size(); ++i)
		marked_edges[EdgeList[i]] = 1.0;
	
	
	for(uint_t edge_list_index = 0; edge_list_index != EdgeList.size(); ++edge_list_index)
	{
		const uint_t edge = EdgeList[edge_list_index];
		const uint_t companion = AdjacentEdges[edge];
		if(!marked_edges[edge] && !marked_edges[companion])
			continue;
		if(BoundaryEdges[edge])
			continue;
		if(edges_to_delete[edge])
			continue;
		return_if_fail(!edges_to_delete[companion]);
		
		const uint_t loop = edge_loops[edge];
		const uint_t companion_loop = edge_loops[companion];
		const uint_t face = loop_faces[loop];
		const uint_t companion_face = loop_faces[companion_loop];
		
		// Treat this case when we hit the companion edge
		if(loop == face_first_loops[face] && companion_loop != face_first_loops[companion_face])
		{  
			continue;
		}
		
		return_if_fail(!faces_to_delete[face]);
		return_if_fail(!faces_to_delete[companion_face]);
		return_if_fail(!loops_to_delete[loop]);
		return_if_fail(!loops_to_delete[companion_loop]);
		
		return_if_fail(clockwise_edges[counter_clockwise_edges[edge]] == edge);
		return_if_fail(clockwise_edges[counter_clockwise_edges[companion]] == companion);
		return_if_fail(counter_clockwise_edges[clockwise_edges[edge]] == edge);
		return_if_fail(counter_clockwise_edges[clockwise_edges[companion]] == companion);
		
		edges_to_delete[edge] = 1;
		edges_to_delete[companion] = 1;
		
		face_selection[face] = 1.0;
		
		if(edge == clockwise_edges[clockwise_edges[edge]])
		{
			return_if_fail(loop == companion_loop);
			return_if_fail(loop != face_first_loops[face]);
			return_if_fail(counter_clockwise_edges[counter_clockwise_edges[edge]] == edge);
			loops_to_delete[loop] = 1;
			--face_loop_counts[face];
			continue;
		}
		
		bool_t loop_is_polyline = detail::is_polyline(edge, clockwise_edges, edge_loops, AdjacentEdges);
		bool_t companion_loop_is_polyline = detail::is_polyline(companion, clockwise_edges, edge_loops, AdjacentEdges);
		
		// Reconnect edges
		clockwise_edges[counter_clockwise_edges[edge]] = clockwise_edges[companion];
		clockwise_edges[counter_clockwise_edges[companion]] = clockwise_edges[edge];
		counter_clockwise_edges[clockwise_edges[edge]] = counter_clockwise_edges[companion];
		counter_clockwise_edges[clockwise_edges[companion]] = counter_clockwise_edges[edge];
		
		if(loop == companion_loop)
		{
			return_if_fail(face == companion_face);
			if(clockwise_edges[edge] == companion)
			{
				loop_first_edges[loop] = clockwise_edges[companion];
				continue;
			}
			if(clockwise_edges[companion] == edge)
			{
				loop_first_edges[loop] = clockwise_edges[edge];
				continue;
			}
			if(!detail::is_polyline(clockwise_edges[companion], clockwise_edges, edge_loops, AdjacentEdges)
				&& std::abs(normalize(normal(edge_points, clockwise_edges, Points, clockwise_edges[companion])) * FaceNormals[face] - 1) < threshold)
			{
				loop_first_edges[loop] = clockwise_edges[companion];
				const uint_t new_loop = loop_first_edges.size();
				loop_first_edges.push_back(clockwise_edges[edge]);
				detail::set_edge_loop(new_loop, clockwise_edges[edge], clockwise_edges, edge_loops);
				loop_faces.push_back(face);
			}
			else
			{
				if(loop == face_first_loops[face]) // first loops should not degenerate to a polyline
				{
					return_if_fail(!detail::is_polyline(clockwise_edges[edge], clockwise_edges, edge_loops, AdjacentEdges));
					return_if_fail(std::abs(normalize(normal(edge_points, clockwise_edges, Points, clockwise_edges[edge])) * FaceNormals[face] - 1) < threshold);
				}
				loop_first_edges[loop] = clockwise_edges[edge];
				const uint_t new_loop = loop_first_edges.size();
				loop_first_edges.push_back(clockwise_edges[companion]);
				detail::set_edge_loop(new_loop, clockwise_edges[companion], clockwise_edges, edge_loops);
				loop_faces.push_back(face);
			}
			++face_loop_counts[face];
			loops_to_delete.push_back(0);
		}
		else
		{
			if(companion_loop == face_first_loops[companion_face])
			{
				loop_first_edges[loop] = counter_clockwise_edges[edge];
				detail::set_edge_loop(loop, loop_first_edges[loop], clockwise_edges, edge_loops);
				loops_to_delete[companion_loop] = 1;
				--face_loop_counts[companion_face];
				faces_to_delete[companion_face] = 1;
				for(uint_t l = 0; l != loop_faces.size(); ++l)
				{
					if(loop_faces[l] == companion_face && !loops_to_delete[l])
					{
						loop_faces[l] = face;
						++face_loop_counts[face];
					}
				}
			}
			else
			{
				assert_not_implemented();
			}
		}
	}
	return_if_fail(loops_to_delete.size() == loop_first_edges.size());
	return_if_fail(loop_faces.size() == loop_first_edges.size());
	mesh::indices_t edges_to_delete_sum(edges_to_delete.size());
	detail::cumulative_sum(edges_to_delete, edges_to_delete_sum);
	// Update the output arrays
	detail::remove_deleted_geometry(Output, face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, faces_to_delete, loops_to_delete, edges_to_delete, loop_faces, face_selection);
	Output.face_materials.create(new mesh::materials_t(Output.face_first_loops->size(), static_cast<imaterial*>(0)));
	mesh::selection_t& edge_selection = Output.edge_selection.create(new mesh::selection_t(Output.edge_points->size(), 0.0));
}

void kill_edge_and_vertex(mesh::polyhedra_t& Output, const mesh::indices_t& EdgeList, const mesh::bools_t BoundaryEdges, const mesh::indices_t& AdjacentEdges, const uint_t PointCount)
{
	const mesh::indices_t face_first_loops = *Output.face_first_loops;
	mesh::counts_t face_loop_counts = *Output.face_loop_counts;
	mesh::indices_t loop_first_edges = *Output.loop_first_edges;
	mesh::indices_t clockwise_edges = *Output.clockwise_edges;
	mesh::indices_t edge_points = *Output.edge_points;
	// Copy the companions array, since it will change
	mesh::indices_t companions = AdjacentEdges;
	
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
	detail::create_edge_loop_face_lookup(face_first_loops, face_loop_counts, loop_first_edges, clockwise_edges, edge_loops, loop_faces);
	mesh::indices_t counter_clockwise_edges(edge_count);
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		const uint_t clockwise = clockwise_edges[edge];
		counter_clockwise_edges[clockwise] = edge;
	}
	
	// maps old point indices to the indices that replace them.
	mesh::indices_t point_map(PointCount, 0.0);
	for(uint_t point = 0; point != PointCount; ++point)
		point_map[point] = point;
	
	for(uint_t edge_list_index = 0; edge_list_index != EdgeList.size(); ++edge_list_index)
	{
		const uint_t edge = EdgeList[edge_list_index];
		//const uint_t edge = affected_edges[i];
		const uint_t companion = companions[edge];
		const uint_t loop = edge_loops[edge];
		const uint_t face = loop_faces[loop];
		
		if(!edges_to_delete[companion])
		{
			const uint_t edge_point = edge_points[edge];
			if(edge_point == point_map[edge_point])
				point_map[edge_point] = edge_points[clockwise_edges[edge]];
		}
		
		return_if_fail(clockwise_edges[counter_clockwise_edges[edge]] == edge);

		// Mark edge to delete
		edges_to_delete[edge] = 1;
		
		// Reconnect edges
		if(companion == counter_clockwise_edges[edge]) // first antenna case
		{
			counter_clockwise_edges[clockwise_edges[edge]] = companions[clockwise_edges[edge]];
			loop_first_edges[loop] = companions[clockwise_edges[edge]];
		}
		else if(companion == clockwise_edges[edge]) // second antenna case
		{
			clockwise_edges[counter_clockwise_edges[edge]] = companions[counter_clockwise_edges[edge]];
			loop_first_edges[loop] = companions[counter_clockwise_edges[edge]];
		}
		else // normal edge
		{
			clockwise_edges[counter_clockwise_edges[edge]] = clockwise_edges[edge];
			counter_clockwise_edges[clockwise_edges[edge]] = counter_clockwise_edges[edge];
			return_if_fail(edge_loops[counter_clockwise_edges[edge]] == edge_loops[clockwise_edges[edge]]);
			loop_first_edges[loop] = clockwise_edges[edge];
		}
		// Check if we created a loop that has only 2 edges, and if we did, delete it
		detail::delete_loop_if_degenerate(loop, loop_faces, face_first_loops, loop_first_edges, clockwise_edges, faces_to_delete, loops_to_delete, edges_to_delete, companions, face_loop_counts);
	}
	
	// Set edge points
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		if(!edges_to_delete[edge] && edge_points[edge] != edge_points[clockwise_edges[edge]])
		{
			uint_t new_edge_point = edge_points[edge];
			// recursively get the new edge point
			while(new_edge_point != point_map[new_edge_point])
			{
				new_edge_point = point_map[new_edge_point];
				// if we get a loop, leave the point unchanged
				if(new_edge_point != point_map[new_edge_point] && new_edge_point == point_map[point_map[new_edge_point]])
				{
					new_edge_point = edge_points[edge];
					break;
				}
			}
			if(edge_points[edge] != new_edge_point)
			{
				face_selection[loop_faces[edge_loops[edge]]] = 1.0;
			}
			edge_points[edge] = new_edge_point;
		}
	}
	
	// Remove edges that have the same start and end
	for(uint_t edge = 0; edge != edge_count; ++edge)
	{
		if(edge_points[edge] == edge_points[clockwise_edges[edge]])
		{
			edges_to_delete[edge] = 1;
			clockwise_edges[counter_clockwise_edges[edge]] = clockwise_edges[edge];
			counter_clockwise_edges[clockwise_edges[edge]] = counter_clockwise_edges[edge];
			loop_first_edges[edge_loops[edge]] = clockwise_edges[edge];
		}
	}
	
	// Update output arrays
	detail::remove_deleted_geometry(Output,face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, faces_to_delete, loops_to_delete, edges_to_delete, loop_faces, face_selection);
	Output.face_materials.create(new mesh::materials_t(Output.face_first_loops->size(), static_cast<imaterial*>(0)));
	Output.edge_selection.create(new mesh::selection_t(Output.clockwise_edges->size()));
}

} // namespace euler

} // namespace k3d

