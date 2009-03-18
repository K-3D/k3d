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

/** \file catmull_clark.cpp
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\created Feb 18, 2009
*/

#include "catmull_clark.h"

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace sds
{

namespace detail
{

/// Replace the elements of Array with their cumulative sum
template<class ArrayT> void cumulative_sum(ArrayT& Array)
{
	const k3d::uint_t array_begin = 0;
	const k3d::uint_t array_end = Array.size();
	for(k3d::uint_t i = array_begin + 1; i != array_end; ++i)
		Array[i] += Array[i-1];
}

/// For each point, create a fast lookup of the outbound edges
// TODO: Move to mesh_topology_data and provide an alternate create_edge_adjacency_lookup that uses the result from this.
void create_vertex_edge_lookup(const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::counts_t& Valences, k3d::mesh::indices_t& PointFirstEdges, k3d::mesh::indices_t& PointEdges)
{
	k3d::mesh::counts_t found_edges(Valences.size(), 0);
	PointFirstEdges.resize(Valences.size(), 0);
	PointEdges.resize(EdgePoints.size(), 0);
	k3d::uint_t count = 0;
	for(k3d::uint_t point = 0; point != Valences.size(); ++point)
	{
		PointFirstEdges[point] = count;
		count += Valences[point];
	}
	
	const k3d::uint_t edge_begin = 0;
	const k3d::uint_t edge_end = edge_begin + EdgePoints.size();
	for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const k3d::uint_t point = EdgePoints[edge];
		PointEdges[PointFirstEdges[point] + found_edges[point]] = edge;
		++found_edges[point];
	}
}

/// True if Face is the first face containing Point
k3d::bool_t first_corner(const k3d::uint_t Face, const k3d::uint_t Point, const k3d::mesh::indices_t& PointFirstFaces, const k3d::mesh::counts_t& PointFaceCounts, const k3d::mesh::indices_t& PointFaces)
{
	const k3d::uint_t face_begin = PointFirstFaces[Point];
	const k3d::uint_t face_end = face_begin + PointFaceCounts[Point];
	for(k3d::uint_t i = face_begin; i != face_end; ++i)
	{
		if(PointFaces[i] < Face)
			return false;
	}
	return true;
}

/// Stores references to commonly used arrays, and defines some common checks for faces and edges
class mesh_arrays
{
public:
	mesh_arrays(const k3d::mesh::selection_t& FaceSelection,
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::counts_t& FaceLoopCounts,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgeFaces,
			const k3d::mesh::indices_t& Companions) :
				face_selection(FaceSelection),
				face_first_loops(FaceFirstLoops),
				face_loop_counts(FaceLoopCounts),
				loop_first_edges(LoopFirstEdges),
				clockwise_edges(ClockwiseEdges),
				edge_faces(EdgeFaces),
				companions(Companions)
	{}
	
	/// True if the given face is affected by the operation
	k3d::bool_t is_affected(const k3d::uint_t Face) const
	{
		return face_selection[Face] && face_loop_counts[Face] == 1;
	}
	
	/// True if the face adjacent to Edge is affected. In case of a boundary edge, returns the affected status of the face itself
	k3d::bool_t is_companion_affected(const k3d::uint_t Edge) const
	{
		return is_affected(edge_faces[companions[Edge]]);
	}
	
	/// True if the face that Edge belongs to comes before the face its companion belongs to, when considering midpoints
	k3d::bool_t first_midpoint(const k3d::uint_t Edge) const
	{
		const k3d::uint_t face = edge_faces[Edge];
		const k3d::uint_t companion_face = edge_faces[companions[Edge]];
		return ((face <= companion_face && !is_affected(face))
						|| is_affected(face) && (face <= companion_face || !is_affected(companion_face)));
	}
	
	/// True if the edge is part of the boundary of the mesh that is to be subdivided
	/**
	 * This means it is one of the following:
	 * - A real boundary edge
	 * - An edge next to an unselected face
	 * - An edge next to a face with holes
	 */
	k3d::bool_t boundary(const k3d::uint_t Edge) const
	{
		const k3d::uint_t companion = companions[Edge];
		return companion == Edge
					|| (is_affected(edge_faces[Edge]) && !is_affected(edge_faces[companion]))
					|| (!is_affected(edge_faces[Edge]) && is_affected(edge_faces[companion]));
	}
	
	const k3d::mesh::selection_t& face_selection;
	const k3d::mesh::indices_t& face_first_loops;
	const k3d::mesh::counts_t& face_loop_counts;
	const k3d::mesh::indices_t& loop_first_edges;
	const k3d::mesh::indices_t& clockwise_edges;
	const k3d::mesh::indices_t& edge_faces;
	const k3d::mesh::indices_t& companions;
};

/// For each old face index, count the number of subfaces, loops, edges and distinct points that will be in the new mesh
/**
 * The parameters FaceSubfaceCounts, FaceSubloopCounts, FaceEdgeCounts and FacePointCounts store the result and must
 * have the same length as the number of faces of the input mesh
 */
class per_face_component_counter
{
public:
	per_face_component_counter(const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& PointFirstFaces,
			const k3d::mesh::counts_t& PointFaceCounts,
			const k3d::mesh::indices_t& PointFaces,
			k3d::mesh::counts_t& FaceSubfaceCounts,
			k3d::mesh::counts_t& FaceSubloopCounts,
			k3d::mesh::counts_t& FaceEdgeCounts,
			k3d::mesh::counts_t& FacePointCounts) :
		m_mesh_arrays(MeshArrays),
		m_edge_points(EdgePoints),
		m_point_first_faces(PointFirstFaces),
		m_point_face_counts(PointFaceCounts),
		m_point_faces(PointFaces),
		m_face_subface_counts(FaceSubfaceCounts),
		m_face_subloop_counts(FaceSubloopCounts),
		m_face_edge_counts(FaceEdgeCounts),
		m_face_point_counts(FacePointCounts)
	{}
	
	void operator()(const unsigned int Face)
	{
		k3d::uint_t& subface_count = m_face_subface_counts[Face];
		k3d::uint_t& subloop_count = m_face_subloop_counts[Face];
		k3d::uint_t& edge_count = m_face_edge_counts[Face];
		k3d::uint_t& point_count = m_face_point_counts[Face];
		subface_count = 0;
		subloop_count = 0;
		edge_count = 0;
		point_count = 0;
		if(!m_mesh_arrays.is_affected(Face)) // Faces that will not be split
		{
			++subface_count;
			subloop_count += m_mesh_arrays.face_loop_counts[Face];
			const k3d::uint_t loop_begin = m_mesh_arrays.face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_mesh_arrays.face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					++edge_count; // Count the original edge
					// Note: we assume here that in case of a boundary edge, companion == edge
					if(m_mesh_arrays.is_companion_affected(edge))
					{
						++edge_count; // Reserve space for an edge that is split because this or the adjacent face is selected
					} 
					
					// Count the new corner points, respecting face order
					
					if(first_corner(Face, m_edge_points[m_mesh_arrays.clockwise_edges[edge]], m_point_first_faces, m_point_face_counts, m_point_faces))
					{
						++point_count;
					}
					
					edge = m_mesh_arrays.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		else // Faces to split
		{
			const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
			// Count the face point
			++point_count;
			for(k3d::uint_t edge = first_edge; ; )
			{
				++subface_count;
				++subloop_count;
				edge_count += 4;
				
				if(m_mesh_arrays.first_midpoint(edge))
				{
					++point_count; // Count the midpoint
				}
				
				if(first_corner(Face, m_edge_points[m_mesh_arrays.clockwise_edges[edge]], m_point_first_faces, m_point_face_counts, m_point_faces))
					++point_count;
				
				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	
private:
	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_point_first_faces;
	const k3d::mesh::counts_t& m_point_face_counts;
	const k3d::mesh::indices_t& m_point_faces;
	k3d::mesh::counts_t& m_face_subface_counts;
	k3d::mesh::counts_t& m_face_subloop_counts;
	k3d::mesh::counts_t& m_face_edge_counts;
	k3d::mesh::counts_t& m_face_point_counts;
};

/// Calculates the new indices of corner points, edge midpoints and face centers 
class point_index_calculator
{
public:
	point_index_calculator(const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& PointFirstFaces,
			const k3d::mesh::counts_t& PointFaceCounts,
			const k3d::mesh::indices_t& PointFaces,
			const k3d::mesh::counts_t& FacePointCounts,
			k3d::mesh::indices_t& CornerPoints,
			k3d::mesh::indices_t& EdgeMidpoints,
			k3d::mesh::indices_t& FaceCenters
			) :
				m_mesh_arrays(MeshArrays),
				m_edge_points(EdgePoints),
				m_point_first_faces(PointFirstFaces),
				m_point_face_counts(PointFaceCounts),
				m_point_faces(PointFaces),
				m_face_point_counts(FacePointCounts),
				m_corner_points(CornerPoints),
				m_edge_midpoints(EdgeMidpoints),
				m_face_centers(FaceCenters)
			{}
	
	void operator()(const k3d::uint_t Face)
	{
		k3d::uint_t point_count = Face == 0 ? 0 : m_face_point_counts[Face - 1];
		if(!m_mesh_arrays.is_affected(Face)) // Faces that will not be split
		{
			const k3d::uint_t loop_begin = m_mesh_arrays.face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_mesh_arrays.face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					if(first_corner(Face, m_edge_points[m_mesh_arrays.clockwise_edges[edge]], m_point_first_faces, m_point_face_counts, m_point_faces))
					{
						const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[edge]; 
						m_corner_points[m_edge_points[clockwise]] = point_count;
						++point_count;
					}
					
					edge = m_mesh_arrays.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		else // Faces to split
		{
			const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
			m_face_centers[Face] = point_count;
			++point_count;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[edge];
				if(m_mesh_arrays.first_midpoint(edge))
				{
					m_edge_midpoints[edge] = point_count;
					m_edge_midpoints[m_mesh_arrays.companions[edge]] = point_count;
					++point_count; // Count the midpoint
				}
				
				if(first_corner(Face, m_edge_points[m_mesh_arrays.clockwise_edges[edge]], m_point_first_faces, m_point_face_counts, m_point_faces))
				{ 
					m_corner_points[m_edge_points[clockwise]] = point_count;
					++point_count;
				}
				
				edge = clockwise;
				if(edge == first_edge)
					break;
			}
		}
	}
	
private:
	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_point_first_faces;
	const k3d::mesh::counts_t& m_point_face_counts;
	const k3d::mesh::indices_t& m_point_faces;
	const k3d::mesh::counts_t& m_face_point_counts;
	k3d::mesh::indices_t& m_corner_points;
	k3d::mesh::indices_t& m_edge_midpoints;
	k3d::mesh::indices_t& m_face_centers;
};

/// Create the subdivided mesh topology
class topology_subdivider
{
public:
	topology_subdivider(const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::materials_t& InputFaceMaterials,
			const k3d::mesh::counts_t& FaceSubfaceCounts,
			const k3d::mesh::counts_t& FaceSubloopCounts,
			const k3d::mesh::counts_t& FaceEdgeCounts,
			const k3d::mesh::indices_t& CornerPoints,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			k3d::mesh::indices_t& OutputEdgePoints,
			k3d::mesh::indices_t& OutputClockwiseEdges,
			k3d::mesh::indices_t& OutputLoopFirstEdges,
			k3d::mesh::indices_t& OutputFaceFirstLoops,
			k3d::mesh::counts_t& OutputFaceLoopCounts,
			k3d::mesh::materials_t& OutputFaceMaterials,
			k3d::mesh::selection_t& OutputFaceSelection
			) :
				m_mesh_arrays(MeshArrays),
				m_input_edge_points(InputEdgePoints),
				m_input_face_materials(InputFaceMaterials),
				m_face_subface_counts(FaceSubfaceCounts),
				m_face_subloop_counts(FaceSubloopCounts),
				m_face_edge_counts(FaceEdgeCounts),
				m_corner_points(CornerPoints),
				m_edge_midpoints(EdgeMidpoints),
				m_face_centers(FaceCenters),
				m_output_edge_points(OutputEdgePoints),
				m_output_clockwise_edges(OutputClockwiseEdges),
				m_output_loop_first_edges(OutputLoopFirstEdges),
				m_output_face_first_loops(OutputFaceFirstLoops),
				m_output_face_loop_counts(OutputFaceLoopCounts),
				m_output_face_materials(OutputFaceMaterials),
				m_output_face_selection(OutputFaceSelection)
				{}
	
	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = Face == 0 ? 0 : m_face_subface_counts[Face - 1];
		const k3d::uint_t first_new_loop = Face == 0 ? 0 : m_face_subloop_counts[Face - 1];

		if(!m_mesh_arrays.is_affected(Face))
		{ // copy unaffected face, splitting edges adjacent to affected faces
			copy_face(Polyhedron, Face);
		}
		else
		{ // subdivide affected faces
			const k3d::uint_t face_center = m_face_centers[Face];
			const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
			k3d::uint_t edgenumber = 0;
			k3d::uint_t first_new_edge = Face == 0 ? 0 : m_face_edge_counts[Face - 1];
			m_output_loop_first_edges[first_new_loop] = first_new_edge;
			m_output_face_first_loops[first_new_face] = first_new_loop;
			m_output_face_materials[first_new_face] = m_input_face_materials[Face];
			m_output_face_selection[first_new_face] = m_mesh_arrays.face_selection[Face];

			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[edge];
				const k3d::uint_t mid1 = m_edge_midpoints[edge];
				const k3d::uint_t mid2 = m_edge_midpoints[clockwise];
				const k3d::uint_t center_to_mid1 = first_new_edge;
				const k3d::uint_t mid1_to_corner = center_to_mid1 + 1;
				const k3d::uint_t corner_to_mid2 = mid1_to_corner + 1;
				const k3d::uint_t mid2_to_center = corner_to_mid2 + 1;
				m_output_edge_points[mid1_to_corner] = mid1;
				m_output_edge_points[corner_to_mid2] = m_corner_points[m_input_edge_points[clockwise]];
				m_output_edge_points[mid2_to_center] = mid2;
				m_output_edge_points[center_to_mid1] = face_center;
				m_output_clockwise_edges[mid1_to_corner] = corner_to_mid2;
				m_output_clockwise_edges[corner_to_mid2] = mid2_to_center;
				m_output_clockwise_edges[mid2_to_center] = center_to_mid1;
				m_output_clockwise_edges[center_to_mid1] = mid1_to_corner;

				// Append face data
				const k3d::uint_t newloop = first_new_loop + edgenumber;
				m_output_loop_first_edges[newloop] = center_to_mid1; 
				m_output_face_first_loops[first_new_face + edgenumber] = newloop;
				m_output_face_materials[first_new_face + edgenumber] = m_input_face_materials[Face];
				m_output_face_selection[first_new_face + edgenumber] = m_mesh_arrays.face_selection[Face];

				first_new_edge += 4;

				edge = clockwise;
				++edgenumber;
				if(edge == first_edge)
					break;
			}
		}
	}

private:
	/// Copies the face to the correct location in the output
	void copy_face(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_edge = Face == 0 ? 0 : m_face_edge_counts[Face - 1];
		const k3d::uint_t first_new_face = Face == 0 ? 0 : m_face_subface_counts[Face - 1];
		const k3d::uint_t first_new_loop = Face == 0 ? 0 : m_face_subloop_counts[Face - 1];

		const k3d::uint_t loop_begin = m_mesh_arrays.face_first_loops[Face];
		const k3d::uint_t loop_end = loop_begin + m_mesh_arrays.face_loop_counts[Face];
		k3d::uint_t edgenumber = 0;
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[loop];
			k3d::uint_t loopedgenumber = 0;
			m_output_loop_first_edges[first_new_loop + loop - loop_begin] = first_new_edge + edgenumber;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t newedge = first_new_edge + edgenumber + loopedgenumber;
				m_output_edge_points[newedge] = m_corner_points[m_input_edge_points[edge]];
				m_output_clockwise_edges[newedge] = newedge + 1;
				if(m_mesh_arrays.is_companion_affected(edge))
				{
					m_output_edge_points[newedge + 1] = m_edge_midpoints[m_mesh_arrays.companions[edge]];
					m_output_clockwise_edges[newedge + 1] = newedge + 2;
					++loopedgenumber;
				}

				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
				++loopedgenumber;
			}
			m_output_clockwise_edges[first_new_edge + edgenumber + loopedgenumber] = first_new_edge + edgenumber; // close the loop
			edgenumber += loopedgenumber + 1;
		}

		// Set face data
		m_output_face_loop_counts[first_new_face] = m_mesh_arrays.face_loop_counts[Face];
		m_output_face_first_loops[first_new_face] = first_new_loop;
		m_output_face_materials[first_new_face] = m_input_face_materials[Face];
		m_output_face_selection[first_new_face] = m_mesh_arrays.face_selection[Face];
	}

	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::materials_t& m_input_face_materials;
	const k3d::mesh::counts_t& m_face_subface_counts;
	const k3d::mesh::counts_t& m_face_subloop_counts;
	const k3d::mesh::counts_t& m_face_edge_counts;
	const k3d::mesh::indices_t& m_corner_points;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::mesh::indices_t& m_face_centers;
	k3d::mesh::indices_t& m_output_edge_points;
	k3d::mesh::indices_t& m_output_clockwise_edges;
	k3d::mesh::indices_t& m_output_loop_first_edges;
	k3d::mesh::indices_t& m_output_face_first_loops;
	k3d::mesh::counts_t& m_output_face_loop_counts;
	k3d::mesh::materials_t& m_output_face_materials;
	k3d::mesh::selection_t& m_output_face_selection;
};

/// Calculates face centers
class face_center_calculator
{
public:
	/// Constructs a new functor that will calculate the face centers of all selected faces with one loop
	/*
	 * \param FaceCenters The point index of the center for each face (only defined for selected faces with one loop)
	 */
	face_center_calculator(
			const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& OutputFaceFirstLoops,
			const k3d::mesh::indices_t& OutputLoopFirstEdges,
			const k3d::mesh::indices_t& OutputClockwiseEdges,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::counts_t& FaceSubfaceCounts,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints,
			k3d::attribute_array_copier& UniformCopier,
			k3d::attribute_array_copier& FaceVaryingCopier,
			k3d::attribute_array_copier& VertexCopier) :
		m_mesh_arrays(MeshArrays),
		m_input_edge_points(InputEdgePoints),
		m_output_face_first_loops(OutputFaceFirstLoops),
		m_output_loop_first_edges(OutputLoopFirstEdges),
		m_output_clockwise_edges(OutputClockwiseEdges),
		m_face_centers(FaceCenters),
		m_face_subface_counts(FaceSubfaceCounts),
		m_input_points(InputPoints),
		m_output_points(OutputPoints),
		m_uniform_copier(UniformCopier),
		m_face_varying_copier(FaceVaryingCopier),
		m_vertex_copier(VertexCopier),
		position_update_time(0.0),
		varying_and_uniform_data_time(0.0),
		vertex_data_time(0.0)
	{}

	k3d::double_t position_update_time;
	k3d::double_t varying_and_uniform_data_time;
	k3d::double_t vertex_data_time;
			
	void operator()(const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = Face == 0 ? 0 : m_face_subface_counts[Face - 1];
		
		// Copy named arrays for unaffected faces
		if(!m_mesh_arrays.is_affected(Face))
		{
			m_uniform_copier.copy(Face, first_new_face);
			const k3d::uint_t loop_begin = m_mesh_arrays.face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_mesh_arrays.face_loop_counts[Face];
			const k3d::uint_t output_first_loop = m_output_face_first_loops[first_new_face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[loop];
				k3d::uint_t output_edge = m_output_loop_first_edges[output_first_loop + loop - loop_begin];
				for(k3d::uint_t edge = first_edge; ; )
				{
					m_face_varying_copier.copy(edge, output_edge);
					
					output_edge = m_output_clockwise_edges[output_edge];
					edge = m_mesh_arrays.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		else
		{
			const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
			k3d::point3& center = m_output_points[m_face_centers[Face]];
			center = k3d::point3(0,0,0);
			k3d::uint_t count = 0;
			
			k3d::timer t;
			for(k3d::uint_t edge = first_edge; ; )
			{
				center += k3d::to_vector(m_input_points[m_input_edge_points[edge]]);
				++count;
	
				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			center /= count;
			position_update_time += t.elapsed();
			
			//indices for target of the varying data copy
			t.restart();
			k3d::mesh::indices_t edges(count);
			k3d::mesh::indices_t points(count);
			k3d::mesh::weights_t weights(count, 1.0/static_cast<double>(count));
			k3d::uint_t i = 0;
			for(k3d::uint_t edge = first_edge; ; )
			{
				edges[i] = edge;
				points[i] = m_input_edge_points[edge];
				++i;
	
				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			k3d::uint_t output_face = first_new_face;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t output_first_edge = m_output_loop_first_edges[m_output_face_first_loops[output_face]];
				m_face_varying_copier.copy(count, &edges[0], &weights[0], output_first_edge); // varying data for the edge starting at the face center
				m_face_varying_copier.copy(m_mesh_arrays.clockwise_edges[edge], m_output_clockwise_edges[m_output_clockwise_edges[output_first_edge]]); // varying data for the edge starting at the corner
				m_uniform_copier.copy(Face, output_face);
	
				++output_face;
				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			varying_and_uniform_data_time += t.elapsed();
			t.restart();
			m_vertex_copier.copy(count, &points[0], &weights[0], m_face_centers[Face]);
			vertex_data_time += t.elapsed();
		}
	}
	
private:
	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_output_face_first_loops;
	const k3d::mesh::indices_t& m_output_loop_first_edges;
	const k3d::mesh::indices_t& m_output_clockwise_edges;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::counts_t& m_face_subface_counts;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
	k3d::attribute_array_copier& m_uniform_copier;
	k3d::attribute_array_copier& m_face_varying_copier;
	k3d::attribute_array_copier& m_vertex_copier;
};

/// Calculates edge midpoints
class edge_midpoint_calculator
{
public:
	/// Constructs a new functor that will calculate the midpoints of all edges belonging to selected faces with one loop
	/*
	 * \param EdgeMidpoints The point index of the midpoint for each edge (only defined for selected faces with one loop)
	 */
	edge_midpoint_calculator(
			const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& OutputFaceFirstLoops,
			const k3d::mesh::indices_t& OutputLoopFirstEdges,
			const k3d::mesh::indices_t& OutputClockwiseEdges,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::counts_t& FaceSubfaceCounts,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints,
			k3d::attribute_array_copier& FaceVaryingCopier,
			k3d::attribute_array_copier& VertexCopier,
			k3d::attribute_array_copier& VertexMixer) :
		m_mesh_arrays(MeshArrays),
		m_input_edge_points(InputEdgePoints),
		m_output_face_first_loops(OutputFaceFirstLoops),
		m_output_loop_first_edges(OutputLoopFirstEdges),
		m_output_clockwise_edges(OutputClockwiseEdges),
		m_edge_midpoints(EdgeMidpoints),
		m_face_centers(FaceCenters),
		m_face_subface_counts(FaceSubfaceCounts),
		m_input_points(InputPoints),
		m_output_points(OutputPoints),
		m_face_varying_copier(FaceVaryingCopier),
		m_vertex_copier(VertexCopier),
		m_vertex_mixer(VertexMixer)
	{}


	void operator()(const k3d::uint_t Face)
	{
		if(!m_mesh_arrays.is_affected(Face))
			return;
		
		const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
		
		// Get the edge indices of the own face, for named array copying
		for(k3d::uint_t edge = first_edge; ; )
		{
			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		
		const k3d::uint_t first_new_face = Face == 0 ? 0 : m_face_subface_counts[Face - 1];
		k3d::uint_t output_face = first_new_face;
		for(k3d::uint_t edge = first_edge; ; )
		{
			return_if_fail(m_edge_midpoints[edge] != 0);
			k3d::point3& midpoint = m_output_points[m_edge_midpoints[edge]];
			const k3d::uint_t companion = m_mesh_arrays.companions[edge];
			const k3d::uint_t output_first_edge = m_output_loop_first_edges[m_output_face_first_loops[output_face]];
			if(m_mesh_arrays.first_midpoint(edge))
			{
				if(m_mesh_arrays.boundary(edge))
				{
					midpoint = 0.5 * (m_input_points[m_input_edge_points[edge]] + k3d::to_vector(m_input_points[m_input_edge_points[m_mesh_arrays.clockwise_edges[edge]]]));
					const k3d::uint_t indices[] = {m_input_edge_points[edge], m_input_edge_points[m_mesh_arrays.clockwise_edges[edge]]};
					const k3d::double_t weights[] = {0.5, 0.5};
					m_vertex_copier.copy(2, indices, weights, m_edge_midpoints[edge]);
				}
				else
				{
					midpoint = (m_input_points[m_input_edge_points[edge]] + k3d::to_vector(m_input_points[m_input_edge_points[m_mesh_arrays.clockwise_edges[edge]]])); // midpoint
					midpoint += k3d::to_vector(m_output_points[m_face_centers[Face]]); // Add the face center of the current face
					midpoint += k3d::to_vector(m_output_points[m_face_centers[m_mesh_arrays.edge_faces[companion]]]);
					midpoint *= 0.25;
					const k3d::uint_t corner_indices[] = {m_input_edge_points[edge],
												m_input_edge_points[m_mesh_arrays.clockwise_edges[edge]]};
					const k3d::double_t corner_weights[] = {0.5, 0.5};
					m_vertex_copier.copy(2, corner_indices, corner_weights, m_edge_midpoints[edge]); // Copy the corner contribution
					const k3d::double_t face_weights[] = {0.5, 0.25, 0.25};
					const k3d::uint_t face_indices[] = {m_edge_midpoints[edge], m_face_centers[Face],
																	m_face_centers[m_mesh_arrays.edge_faces[companion]]};
					m_vertex_mixer.copy(3, face_indices, face_weights, m_edge_midpoints[edge]); // Mix in the adjacent face values
				}
			}
			// copy varying data
			const k3d::uint_t output_edge1 = m_output_clockwise_edges[m_output_clockwise_edges[m_output_clockwise_edges[output_first_edge]]]; // Edge from clockwise midpoint to center
			const k3d::uint_t next_output_face = m_mesh_arrays.clockwise_edges[edge] == first_edge ? first_new_face : output_face + 1;
			const k3d::uint_t next_output_first_edge = m_output_loop_first_edges[m_output_face_first_loops[next_output_face]];
			const k3d::uint_t output_edge2 = m_output_clockwise_edges[next_output_first_edge]; // Edge from clockwise midpoint to corner
			const k3d::double_t weights[] = {0.5, 0.5};
			const k3d::uint_t varying_indices[] = {m_mesh_arrays.clockwise_edges[edge], m_mesh_arrays.clockwise_edges[m_mesh_arrays.clockwise_edges[edge]]};
			m_face_varying_copier.copy(2, varying_indices, weights, output_edge1);
			m_face_varying_copier.copy(2, varying_indices, weights, output_edge2);

			++output_face;
			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}

private:
	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_output_face_first_loops;
	const k3d::mesh::indices_t& m_output_loop_first_edges;
	const k3d::mesh::indices_t& m_output_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::counts_t& m_face_subface_counts;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
	k3d::attribute_array_copier& m_face_varying_copier;
	k3d::attribute_array_copier& m_vertex_copier;
	k3d::attribute_array_copier& m_vertex_mixer;
};

/// Calculates patch corner positions
class corner_point_calculator
{
public:
	corner_point_calculator(
			const mesh_arrays& MeshArrays,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& CornerPoints,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::indices_t& VertexValences,
			const k3d::mesh::indices_t& PointFirstEdges,
			const k3d::mesh::indices_t& PointEdges,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints,
			k3d::attribute_array_copier& VertexCopier,
			k3d::attribute_array_copier& VertexMixer) :
		m_mesh_arrays(MeshArrays),
		m_input_edge_points(InputEdgePoints),
		m_corner_points(CornerPoints),
		m_edge_midpoints(EdgeMidpoints),
		m_face_centers(FaceCenters),
		m_input_points(InputPoints),
		m_vertex_valences(VertexValences),
		m_point_first_edges(PointFirstEdges),
		m_point_edges(PointEdges),
		m_output_points(OutputPoints),
		m_vertex_copier(VertexCopier),
		m_vertex_mixer(VertexMixer)
	{}


	void operator()(const k3d::uint_t Point)
	{
		// Set initial position
		k3d::point3& output_position = m_output_points[m_corner_points[Point]];
		output_position = m_input_points[Point];
		
		// Get the number of outbound affected and boundary edges
		k3d::uint_t affected_edge_count = 0;
		k3d::uint_t boundary_edge_count = 0;
		const k3d::uint_t valence = m_vertex_valences[Point];
		const k3d::uint_t start_index = m_point_first_edges[Point];
		const k3d::uint_t end_index = start_index + valence;
		for(k3d::uint_t index = start_index; index != end_index; ++index)
		{
			const k3d::uint_t edge = m_point_edges[index];
			const k3d::uint_t face = m_mesh_arrays.edge_faces[edge];
			if(m_mesh_arrays.is_affected(face))
				++affected_edge_count;
			if(m_mesh_arrays.boundary(edge))
				++boundary_edge_count;
		}
		
		if(affected_edge_count == valence && boundary_edge_count == 0) // Interior point of the subdivided surface
		{
			const k3d::double_t own_weight = static_cast<double>(valence - 2.0) / static_cast<double>(valence); // Weight attributed to Point
			const k3d::double_t neighbour_weight = 1.0 / static_cast<double>(valence * valence); // Weight attributed to surrounding corners and face vertices
			output_position *=  own_weight; // adjust initial position
			k3d::vector3 sum(0, 0, 0);
			k3d::mesh::indices_t corner_indices(valence + 1); // indices of neighbor corners, for vertex attribute data
			k3d::mesh::indices_t face_indices(valence + 1); // indices of neighbor faces face vertices, for vertex attribute data
			for(k3d::uint_t index = start_index; index != end_index; ++index)
			{
				const k3d::uint_t edge = m_point_edges[index];
				const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[edge];
				const k3d::uint_t face = m_mesh_arrays.edge_faces[edge];
				const k3d::vector3 next_corner = k3d::to_vector(m_input_points[m_input_edge_points[clockwise]]);
				const k3d::vector3 face_vertex = k3d::to_vector(m_output_points[m_face_centers[face]]);
				corner_indices[index - start_index] = m_input_edge_points[clockwise];
				face_indices[index - start_index] = m_face_centers[face];
				sum += next_corner + face_vertex;
			}
			sum *= neighbour_weight;
			output_position += sum;
			k3d::mesh::weights_t corner_weights(corner_indices.size()+1, neighbour_weight);
			corner_indices.back() = Point; // Append the current point and its weight
			corner_weights.back() = own_weight;
			m_vertex_copier.copy(corner_indices.size(), &corner_indices[0], &corner_weights[0], m_corner_points[Point]); // Contribution of Point and its neighbor corners
			k3d::mesh::weights_t face_weights(face_indices.size()+1, neighbour_weight);
			face_indices.back() = m_corner_points[Point];
			face_weights.back() = 1.0;
			m_vertex_mixer.copy(face_indices.size(), &face_indices[0], &face_weights[0], m_corner_points[Point]); // Contribution of the face vertices
		}
		else if(affected_edge_count != 0) // Boundary of the subdivided surface
		{
			output_position *= 0.5;
			m_vertex_copier.copy(Point, m_corner_points[Point]);
			k3d::double_t boundary_weights[] = {0.5, 0.25};
			k3d::uint_t boundary_indices[] = {m_corner_points[Point], 0};
			for(k3d::uint_t index = start_index; index != end_index; ++index)
			{
				const k3d::uint_t edge = m_point_edges[index];
				// we might also need to account for the counter-clockwise edge, since point_edges only stores outbound edges
				k3d::uint_t counter_clockwise = edge;
				for(; ;)
				{
					const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[counter_clockwise];
					if(clockwise == edge)
						break;
					counter_clockwise = clockwise;
				}
				if(m_mesh_arrays.companions[counter_clockwise] == counter_clockwise && m_mesh_arrays.is_affected(m_mesh_arrays.edge_faces[counter_clockwise]))
				{
					output_position += 0.25 * k3d::to_vector(m_output_points[m_edge_midpoints[counter_clockwise]]);
					boundary_indices[1] = m_edge_midpoints[counter_clockwise];
					m_vertex_mixer.copy(2, &boundary_indices[0], &boundary_weights[0], m_corner_points[Point]);
					boundary_weights[0] = 1.0;
				}
				if(m_mesh_arrays.boundary(edge))
				{
					output_position += 0.25 * k3d::to_vector(m_output_points[m_edge_midpoints[edge]]);
					boundary_indices[1] = m_edge_midpoints[edge];
					m_vertex_mixer.copy(2, &boundary_indices[0], &boundary_weights[0], m_corner_points[Point]);
					boundary_weights[0] = 1.0;
				}
			}
		}
	}

private:
	const mesh_arrays& m_mesh_arrays;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_corner_points;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::counts_t& m_vertex_valences;
	const k3d::mesh::indices_t& m_point_first_edges;
	const k3d::mesh::indices_t& m_point_edges;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
	k3d::attribute_array_copier& m_vertex_copier;
	k3d::attribute_array_copier& m_vertex_mixer;
};

} // namespace detail

class catmull_clark_subdivider::implementation
{
public:
	implementation(const k3d::uint_t Levels) :
		m_levels(Levels),
		m_intermediate_meshes(m_levels),
		m_topology_data(m_levels)
	{
	}
	
	void create_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output, k3d::inode* Node)
	{
		k3d::timer total_timer;
		// If there are no valid polyhedra, we give up
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input));
		if(!polyhedron)
			return;

		// Manually sum timings over different levels
		k3d::timer timer;
		k3d::double_t copy_input_time = 0;
		k3d::double_t calculate_companions_time = 0;
		k3d::double_t calculate_indices_time = 0;
		k3d::double_t allocate_memory_time = 0;
		k3d::double_t subdivide_topology_time = 0;
		k3d::double_t vertex_valences_time = 0;
				
		for(k3d::uint_t level = 0; level != m_levels; ++level)
		{
			topology_data_t& topology_data = m_topology_data[level];
			const k3d::mesh& input = level == 0 ? Input : m_intermediate_meshes[level - 1];
			k3d::mesh& output = m_intermediate_meshes[level];
			const k3d::mesh::selection_t& input_face_selection = level == 0 ? InputFaceSelection : *input.polyhedra->face_selection;
			
			// Shallow copy of the input (no data is copied, only shared pointers are)
			output = input;

			// Make writeable copies of the arrays we intend to modify
			timer.restart();
			k3d::mesh::polyhedra_t& output_polyhedra = output.polyhedra.writable();
			k3d::mesh::polyhedra_t::types_t& output_types = output_polyhedra.types.writable();
			k3d::mesh::indices_t& output_loop_first_edges = output_polyhedra.loop_first_edges.writable();
			k3d::mesh::indices_t& output_edge_points = output_polyhedra.edge_points.writable();
			k3d::mesh::indices_t& output_clockwise_edges = output_polyhedra.clockwise_edges.writable();
			k3d::mesh::selection_t& output_edge_selection = output_polyhedra.edge_selection.writable();

			// Copy the unaffected constant data
			output_polyhedra.constant_data = input.polyhedra->constant_data;

			const k3d::mesh::polyhedra_t& input_polyhedra = *input.polyhedra;
			const k3d::mesh::indices_t& input_first_faces = *(input_polyhedra.first_faces);
			const k3d::mesh::counts_t& input_face_counts = *(input_polyhedra.face_counts);
			const k3d::mesh::indices_t& input_face_first_loops = *(input_polyhedra.face_first_loops);
			const k3d::mesh::indices_t& input_loop_first_edges = *input.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& input_clockwise_edges = *input.polyhedra->clockwise_edges;
			const k3d::mesh::indices_t& input_edge_points = *input.polyhedra->edge_points;
			const k3d::mesh::counts_t& input_face_loop_counts = *(input_polyhedra.face_loop_counts);
			const k3d::mesh::materials_t& input_face_materials = *(input_polyhedra.face_materials);
			const k3d::uint_t input_edge_count = input_clockwise_edges.size();
			const k3d::uint_t input_face_count = input_face_first_loops.size();
			// Face-related arrays can not be appended to because of the possibility of multiple polyhedra,
			// so we will rebuild them from scratch in the new order
			k3d::mesh::indices_t& output_first_faces = output_polyhedra.first_faces.create();
			k3d::mesh::counts_t& output_face_counts = output_polyhedra.face_counts.create();
			k3d::mesh::indices_t& output_face_first_loops = output_polyhedra.face_first_loops.create();
			k3d::mesh::counts_t& output_face_loop_counts = output_polyhedra.face_loop_counts.create();
			k3d::mesh::selection_t& output_face_selection = output_polyhedra.face_selection.create();
			k3d::mesh::materials_t& output_face_materials = output_polyhedra.face_materials.create();

			k3d::mesh::indices_t point_first_faces;
			k3d::mesh::counts_t point_face_counts;
			k3d::mesh::indices_t point_faces;
			
			// store some common arrays
			detail::mesh_arrays mesh_arrays(input_face_selection,
					input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_clockwise_edges,
					topology_data.edge_faces,
					topology_data.companions);
			copy_input_time += timer.elapsed();


			// Get the "companion" edge for each edge
			timer.restart();
			k3d::mesh::bools_t boundary_edges;
			k3d::polyhedron::create_edge_adjacency_lookup(output_edge_points, output_clockwise_edges, boundary_edges, topology_data.companions);
			k3d::polyhedron::create_vertex_face_lookup(input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_edge_points,
					input_clockwise_edges,
					*input.points,
					point_first_faces,
					point_face_counts,
					point_faces);
			calculate_companions_time += timer.elapsed();

			timer.restart();
			// For each edge, get the face it belongs to
			topology_data.edge_faces.resize(input_edge_count);
			k3d::polyhedron::create_edge_face_lookup(input_face_first_loops, input_face_loop_counts, input_loop_first_edges, input_clockwise_edges, topology_data.edge_faces);
			// Count the number of components of the new mesh per old face
			topology_data.face_subface_counts.resize(input_face_count);
			k3d::mesh::indices_t face_subloop_counts(input_face_count);
			k3d::mesh::indices_t face_edge_counts(input_face_count);
			k3d::mesh::indices_t face_point_counts(input_face_count);
			detail::per_face_component_counter per_face_component_counter(mesh_arrays,
						input_edge_points,
						point_first_faces,
						point_face_counts,
						point_faces,
						topology_data.face_subface_counts,
						face_subloop_counts,
						face_edge_counts,
						face_point_counts);
			for(k3d::uint_t face = 0; face != input_face_count; ++face) per_face_component_counter(face);
			// Turn these counts into cumulative sums
			detail::cumulative_sum(topology_data.face_subface_counts);
			detail::cumulative_sum(face_subloop_counts);
			detail::cumulative_sum(face_edge_counts);
			detail::cumulative_sum(face_point_counts);
			// We now have the following relationships between old and new geometry:
			// first new component index = ..._counts[old component index - 1]
			
			topology_data.corner_points.resize(input.points->size(), 0);
			topology_data.edge_midpoints.resize(input_edge_points.size());
			topology_data.face_centers.resize(input_face_first_loops.size());
			detail::point_index_calculator point_index_calculator(mesh_arrays,					
					input_edge_points,
					point_first_faces,
					point_face_counts,
					point_faces,
					face_point_counts,
					topology_data.corner_points,
					topology_data.edge_midpoints,
					topology_data.face_centers);
			for(k3d::uint_t face = 0; face != input_face_count; ++face) point_index_calculator(face);
			calculate_indices_time += timer.elapsed();
					
			// Allocate required memory
			timer.restart();
			k3d::mesh::points_t& output_points = output.points.create(new k3d::mesh::points_t(face_point_counts.back()));
			k3d::mesh::selection_t& output_point_selection = output.point_selection.create(new k3d::mesh::selection_t(output_points.size(), 0.0));
			output_edge_points.resize(face_edge_counts.back(), 0);
			output_clockwise_edges.resize(face_edge_counts.back(), 0);
			output_loop_first_edges.resize(face_subloop_counts.back());
			output_face_first_loops.resize(topology_data.face_subface_counts.back());
			output_face_loop_counts.resize(topology_data.face_subface_counts.back(), 1);
			output_face_selection.resize(topology_data.face_subface_counts.back(), 0.0);
			output_face_materials.resize(topology_data.face_subface_counts.back());
			output_polyhedra.face_varying_data.resize(face_edge_counts.back());
			output_polyhedra.uniform_data.resize(topology_data.face_subface_counts.back());
			allocate_memory_time += timer.elapsed();
			
			timer.restart();
			detail::topology_subdivider topology_subdivider(mesh_arrays,
					*input.polyhedra->edge_points,
					input_face_materials,
					topology_data.face_subface_counts,
					face_subloop_counts,
					face_edge_counts,
					topology_data.corner_points,
					topology_data.edge_midpoints,
					topology_data.face_centers,
					output_edge_points,
					output_clockwise_edges,
					output_loop_first_edges,
					output_face_first_loops,
					output_face_loop_counts,
					output_face_materials,
					output_face_selection);
			
			// Connect face centers to edge midpoints
			for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
			{
				const k3d::uint_t face_start = input_first_faces[polyhedron];
				const k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
				for(k3d::uint_t face = face_start; face != face_end; ++face)
				{
					topology_subdivider(polyhedron, face);
				}
			}
			// Set the per-polyhedron arrays
			output_first_faces.push_back(0);
			for(k3d::uint_t polyhedron = 1; polyhedron != input_first_faces.size(); ++polyhedron)
			{
				output_face_counts.push_back(topology_data.face_subface_counts[input_first_faces[polyhedron] - 1] - output_first_faces.back());
				output_first_faces.push_back(topology_data.face_subface_counts[input_first_faces[polyhedron] - 1]);
			}
			output_face_counts.push_back(topology_data.face_subface_counts.back() - output_first_faces.back());
			subdivide_topology_time += timer.elapsed();
			
			// Update selection arrays
			output_edge_selection.assign(output_edge_points.size(), 0.0);
			
			// Calculate vertex valences, needed for corner point updates.
			timer.restart();
			k3d::polyhedron::create_vertex_valence_lookup(input.points->size(), input_edge_points, topology_data.vertex_valences);
			detail::create_vertex_edge_lookup(input_edge_points, topology_data.vertex_valences, topology_data.point_first_edges, topology_data.point_edges);
			vertex_valences_time += timer.elapsed();
		}
		if(Node)
		{
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Copy input", copy_input_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Calculate companions", calculate_companions_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Calculate indices", calculate_indices_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Allocate memory", allocate_memory_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Subdivide topology", subdivide_topology_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Vertex valences", vertex_valences_time);
		}
		const k3d::double_t total = total_timer.elapsed();
		k3d::log() << debug << "SDS create timings: Total: " << total 
			<< ", copy input: " << copy_input_time << " (" << copy_input_time/total*100 << "%), calculate companions/point to face: "
			<< calculate_companions_time << " (" << calculate_companions_time/total*100 << "%), calculate indices: "
			<< calculate_indices_time << " (" << calculate_indices_time/total*100 << "%), allocate memory: "
			<< allocate_memory_time << " (" << allocate_memory_time/total*100 << "%), subdivide topology: "
			<< subdivide_topology_time << " (" << subdivide_topology_time/total*100 << "%), vertex valences: "
			<< vertex_valences_time << " (" << vertex_valences_time/total*100 << "%)" << std::endl;
		
		Output = m_intermediate_meshes[m_levels - 1];
	}
	
	void update_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output, k3d::inode* Node)
	{
		k3d::timer total_timer;
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input));
		if(!polyhedron)
			return;

		const k3d::mesh::points_t& points = *Input.points;
		
		// Manually keep track of some timing data, in order to get the total time over the loops
		k3d::double_t face_center_time = 0;
		k3d::double_t edge_midpoint_time = 0;
		k3d::double_t point_position_time = 0;
		k3d::double_t face_position_time = 0;
		k3d::double_t face_varying_data_time = 0;
		k3d::double_t face_vertex_data_time = 0;
		k3d::timer timer;
		
		for(k3d::uint_t level = 0; level != m_levels; ++level)
		{
			topology_data_t& topology_data = m_topology_data[level];
			const k3d::mesh& input = level == 0 ? Input : m_intermediate_meshes[level - 1];
			k3d::mesh& output = m_intermediate_meshes[level];
			const k3d::mesh::selection_t& input_face_selection = level == 0 ? InputFaceSelection : *input.polyhedra->face_selection;
		
			const k3d::mesh::points_t& input_points = *input.points;
			const k3d::mesh::indices_t& input_face_first_loops = *input.polyhedra->face_first_loops;
			const k3d::mesh::counts_t& input_face_loop_counts = *input.polyhedra->face_loop_counts;
			const k3d::mesh::indices_t& input_loop_first_edges = *input.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& input_clockwise_edges = *input.polyhedra->clockwise_edges;
			const k3d::mesh::indices_t& input_edge_points = *input.polyhedra->edge_points;
			const k3d::uint_t face_count = input_face_first_loops.size();
			
			// store some common arrays
			detail::mesh_arrays mesh_arrays(input_face_selection,
					input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_clockwise_edges,
					topology_data.edge_faces,
					topology_data.companions
					);
			
			k3d::mesh::polyhedra_t& output_polyhedra = output.polyhedra.writable();
			
			// Create copiers for the uniform and varying data
			output_polyhedra.uniform_data = input.polyhedra->uniform_data.clone_types();
			output_polyhedra.face_varying_data = input.polyhedra->face_varying_data.clone_types();
			output.vertex_data = input.vertex_data.clone_types();
			output_polyhedra.uniform_data.resize(output_polyhedra.face_first_loops->size());
			output_polyhedra.face_varying_data.resize(output_polyhedra.edge_points->size());
			output.vertex_data.resize(output.points->size());
			k3d::attribute_array_copier uniform_data_copier(input.polyhedra->uniform_data, output_polyhedra.uniform_data);
			k3d::attribute_array_copier face_varying_data_copier(input.polyhedra->face_varying_data, output_polyhedra.face_varying_data);
			k3d::attribute_array_copier vertex_data_copier(input.vertex_data, output.vertex_data);
			k3d::attribute_array_copier vertex_data_mixer(output.vertex_data, output.vertex_data);
	
			k3d::mesh::points_t& output_points = output.points.writable();
			output_points.assign(output_points.size(), k3d::point3(0,0,0));
	
			// Calculate face centers
			timer.restart();
			detail::face_center_calculator face_center_calculator(
					mesh_arrays,
					input_edge_points,
					*output.polyhedra->face_first_loops,
					*output.polyhedra->loop_first_edges,
					*output.polyhedra->clockwise_edges,
					topology_data.face_centers,
					topology_data.face_subface_counts,
					input_points,
					output_points,
					uniform_data_copier,
					face_varying_data_copier,
					vertex_data_copier);
			for(k3d::uint_t face = 0; face != face_count; ++face) face_center_calculator(face);
			face_center_time += timer.elapsed();
			face_position_time += face_center_calculator.position_update_time;
			face_varying_data_time += face_center_calculator.varying_and_uniform_data_time;
			face_vertex_data_time += face_center_calculator.vertex_data_time;
	
			// Calculate edge midpoints
			timer.restart();
			detail::edge_midpoint_calculator edge_midpoint_calculator(
					mesh_arrays,
					input_edge_points,
					*output.polyhedra->face_first_loops,
					*output.polyhedra->loop_first_edges,
					*output.polyhedra->clockwise_edges,
					topology_data.edge_midpoints,
					topology_data.face_centers,
					topology_data.face_subface_counts,
					input_points,
					output_points,
					face_varying_data_copier,
					vertex_data_copier,
					vertex_data_mixer);
			for(k3d::uint_t face = 0; face != face_count; ++face) edge_midpoint_calculator(face);
			edge_midpoint_time += timer.elapsed();
			
			// Calculate new point positions
			timer.restart();
			detail::corner_point_calculator corner_point_calculator(
					mesh_arrays,
					input_edge_points,
					topology_data.corner_points,
					topology_data.edge_midpoints,
					topology_data.face_centers,
					topology_data.vertex_valences,
					topology_data.point_first_edges,
					topology_data.point_edges,
					input_points,
					output_points,
					vertex_data_copier,
					vertex_data_mixer);
			for(k3d::uint_t point = 0; point != input_points.size(); ++point) corner_point_calculator(point);
			point_position_time += timer.elapsed();
		}
		if(Node)
		{
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Face centers", face_center_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Edge midpoints", edge_midpoint_time);
			Node->document().pipeline_profiler().add_timing_entry(*Node, "Point positions", point_position_time);
		}
		const k3d::double_t total = total_timer.elapsed();
		k3d::log() << debug << "SDS update timings: Total: " << total << ", face centers: " << face_center_time << " (" << face_center_time/total*100 << "%), edge midpoints: " << edge_midpoint_time << " (" << edge_midpoint_time/total*100 << "%), point positions: " << point_position_time << " (" << point_position_time/total*100 << "%)" << std::endl;
		
		Output = m_intermediate_meshes[m_levels - 1];
	}
	
	void visit_surface(const k3d::uint_t Level, ipatch_surface_visitor& Visitor)
	{
		k3d::timer t;
		k3d::uint_t last_count = 0;
		for(k3d::uint_t face = 0; face != m_topology_data[0].face_subface_counts.size(); ++face)
		{
			const k3d::uint_t face_count = m_topology_data[0].face_subface_counts[face]; 
			if(face_count - last_count > 1)
			{
				Visitor.on_patch(face);
				visit_subfacets(Level, 0, face, Visitor);
			}
			last_count = face_count;
		}
		
		t.restart();
		const k3d::mesh& mesh = m_intermediate_meshes[Level - 1];
		const k3d::mesh::points_t points = *mesh.points;
		const k3d::mesh::normals_t* normals = mesh.vertex_data.lookup<k3d::mesh::normals_t>("sds_normals");
		for(k3d::uint_t point = 0; point != points.size(); ++point)
		{
			Visitor.on_vertex(points[point], normals ? normals->at(point) : k3d::normal3(0,0,1));
		}
	}
	
	void visit_boundary(const k3d::mesh& Mesh, const k3d::uint_t Level, ipatch_boundary_visitor& Visitor)
	{
		k3d::timer t;
		const k3d::uint_t edge_count = m_topology_data[0].edge_midpoints.size();
		const k3d::mesh::indices_t& input_edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& input_clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		for(k3d::uint_t edge = 0; edge != edge_count; ++edge)
		{
			Visitor.on_boundary(edge);
			k3d::uint_t c0 = input_edge_points[edge];
			k3d::uint_t c1 = input_edge_points[input_clockwise_edges[edge]];
			k3d::uint_t first_edge = edge;
			for(k3d::uint_t level = 0; level != Level - 1; ++level)
			{
				c0 = m_topology_data[level].corner_points[c0];
				c1 = m_topology_data[level].corner_points[c1];
				const k3d::uint_t midpoint = m_topology_data[level].edge_midpoints[first_edge];
				const k3d::uint_t point_edge_begin = m_topology_data[level+1].point_first_edges[c0];
				const k3d::uint_t point_edge_end = point_edge_begin + m_topology_data[level+1].vertex_valences[c0];
				const k3d::mesh& mesh = m_intermediate_meshes[level];
				const k3d::mesh::indices_t& edge_points = *mesh.polyhedra->edge_points;
				const k3d::mesh::indices_t& clockwise_edges = *mesh.polyhedra->clockwise_edges;
				for(k3d::uint_t point_edge_index = point_edge_begin; point_edge_index != point_edge_end; ++point_edge_index)
				{
					const k3d::uint_t point_edge = m_topology_data[level+1].point_edges[point_edge_index];
					if(edge_points[clockwise_edges[point_edge]] == midpoint)
					{
						first_edge = point_edge;
						break;
					}
				}
			}
			const k3d::mesh& mesh = m_intermediate_meshes[Level-2];
			const k3d::mesh::indices_t& edge_points = *mesh.polyhedra->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = *mesh.polyhedra->clockwise_edges;
			const k3d::mesh::indices_t& corner_points = m_topology_data[Level-1].corner_points;
			const k3d::mesh::indices_t& edge_midpoints = m_topology_data[Level-1].edge_midpoints;
			const k3d::mesh::indices_t& companions = m_topology_data[Level-1].companions;
			const k3d::mesh::points_t& points = *m_intermediate_meshes[Level-1].points;
			return_if_fail(edge_points[first_edge] == c0);
			for(k3d::uint_t subedge = first_edge; ;)
			{ 
				Visitor.on_point(points[corner_points[edge_points[subedge]]]);
				Visitor.on_point(points[edge_midpoints[subedge]]);
				
				if(edge_points[clockwise_edges[subedge]] == c1)
					break;
				subedge = clockwise_edges[companions[clockwise_edges[subedge]]];
			}
		}
	}
	
	void visit_corners(const k3d::uint_t Level, ipatch_corner_visitor& Visitor)
	{
		const k3d::uint_t point_count = m_topology_data[0].corner_points.size();
		for(k3d::uint_t point = 0; point != point_count; ++point)
		{
			k3d::uint_t corner = point;
			for(k3d::uint_t level = 0; level != Level; ++level)
			{
				corner = m_topology_data[level].corner_points[corner];
			}
			Visitor.on_corner(m_intermediate_meshes[Level-1].points->at(corner));
		}
	}
	
private:
	/// Used to recurse through levels to associate an original face with its subfaces
	void visit_subfacets(const k3d::uint_t MaxLevel, const k3d::uint_t Level, const k3d::uint_t Face, ipatch_surface_visitor& Visitor)
	{
		const k3d::uint_t face_begin = Face == 0 ? 0 : m_topology_data[Level].face_subface_counts[Face - 1];
		const k3d::uint_t face_end = m_topology_data[Level].face_subface_counts[Face];
		if(Level < MaxLevel - 1)
		{
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
				visit_subfacets(MaxLevel, Level + 1, face, Visitor);
		}
		else
		{
			const k3d::mesh& mesh = m_intermediate_meshes[Level];
			const k3d::mesh::indices_t& loop_first_edges = *mesh.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& face_first_loops = *mesh.polyhedra->face_first_loops;
			const k3d::mesh::indices_t& edge_points = *mesh.polyhedra->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = *mesh.polyhedra->clockwise_edges;
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				const k3d::uint_t first_edge = loop_first_edges[face_first_loops[face]];
				for(k3d::uint_t edge = first_edge; ;)
				{
					Visitor.on_edge(edge_points[edge]);
					
					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}
	
	/// Some arrays containing additional topology data
	struct topology_data_t
	{
		k3d::mesh::indices_t corner_points; // New point index, for each original point
		k3d::mesh::indices_t edge_midpoints; // Midpoint index for each edge (if any)
		k3d::mesh::indices_t face_centers; // Face center index, for each face (if any)
		k3d::mesh::indices_t companions; // Companion edges
		k3d::mesh::indices_t edge_faces; // For each original edge, the original owning face
		k3d::mesh::counts_t vertex_valences; // For each original vertex, the valence
		k3d::mesh::indices_t point_first_edges; // The first outgoing edge for each original vertex (index into m_point_edges)
		k3d::mesh::indices_t point_edges; // The outgoing edges for each original vertex
		k3d::mesh::counts_t face_subface_counts; // Cumulative subface count for each input face (needed to copy uniform and face varying data)
	};
	
	const k3d::uint_t m_levels; // The number of SDS levels to create
	meshes_t m_intermediate_meshes;
	std::vector<topology_data_t> m_topology_data;
};

catmull_clark_subdivider::catmull_clark_subdivider(const k3d::uint_t Levels)
{
	m_implementation = new implementation(Levels);
}

catmull_clark_subdivider::~catmull_clark_subdivider()
{
	delete m_implementation;
}

void catmull_clark_subdivider::create_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output, k3d::inode* Node)
{
	m_implementation->create_mesh(Input, InputFaceSelection, Output, Node);
}

void catmull_clark_subdivider::update_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output, k3d::inode* Node)
{
	m_implementation->update_mesh(Input, InputFaceSelection, Output, Node);
}

void catmull_clark_subdivider::visit_surface(const k3d::uint_t Level, ipatch_surface_visitor& Visitor)
{
	m_implementation->visit_surface(Level, Visitor);
}

void catmull_clark_subdivider::visit_boundary(const k3d::mesh& Mesh, const k3d::uint_t Level, ipatch_boundary_visitor& Visitor)
{
	m_implementation->visit_boundary(Mesh, Level, Visitor);
}

void catmull_clark_subdivider::visit_corners(const k3d::uint_t Level, ipatch_corner_visitor& Visitor)
{
	m_implementation->visit_corners(Level, Visitor);
}

} // namespace sds

} // namespace k3d
