// K-3D
// Copyright (c) 2004-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_topology_data.h>
#include <k3dsdk/node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

namespace module
{

namespace mesh
{

namespace detail_sds
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
	
	/// True if the face that Edge belongs to comes before the face its companion belongs to, when considering corner points
	/**
	 * Note that the actual corner point affected by this check is the _endpoint_ of Edge, i.e. edge_points[clockwise_edges[Edge]]
	 * This check does not account for selection, since corner points are added for both affected and unaffected faces
	 */
	k3d::bool_t first_corner(const k3d::uint_t Edge) const
	{
		const k3d::uint_t companion = companions[Edge];
		const k3d::uint_t clockwise = clockwise_edges[Edge];
		return edge_faces[clockwise] <= edge_faces[companions[clockwise]] && edge_faces[Edge] <= edge_faces[companion];
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
			k3d::mesh::counts_t& FaceSubfaceCounts,
			k3d::mesh::counts_t& FaceSubloopCounts,
			k3d::mesh::counts_t& FaceEdgeCounts,
			k3d::mesh::counts_t& FacePointCounts) :
		m_mesh_arrays(MeshArrays),
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
					if(m_mesh_arrays.first_corner(edge))
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
				
				if(m_mesh_arrays.first_corner(edge))
					++point_count;
				
				edge = m_mesh_arrays.clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	
private:
	const mesh_arrays& m_mesh_arrays;
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
			const k3d::mesh::counts_t& FacePointCounts,
			k3d::mesh::indices_t& CornerPoints,
			k3d::mesh::indices_t& EdgeMidpoints,
			k3d::mesh::indices_t& FaceCenters
			) :
				m_mesh_arrays(MeshArrays),
				m_edge_points(EdgePoints),
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
					if(m_mesh_arrays.first_corner(edge))
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
				
				if(m_mesh_arrays.first_corner(edge))
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
		m_vertex_copier(VertexCopier)
	{}


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

		const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
		k3d::point3& center = m_output_points[m_face_centers[Face]];
		center = k3d::point3(0,0,0);
		k3d::uint_t count = 0;
		//indices for target of the varying data copy
		k3d::mesh::indices_t edges;
		k3d::mesh::indices_t points;
		for(k3d::uint_t edge = first_edge; ; )
		{
			center += k3d::to_vector(m_input_points[m_input_edge_points[edge]]);
			++count;
			edges.push_back(edge);
			points.push_back(m_input_edge_points[edge]);

			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}

		center /= count;
		k3d::mesh::weights_t weights(count, 1.0/static_cast<double>(count));

		k3d::uint_t output_face = first_new_face;
		for(k3d::uint_t edge = first_edge; ; )
		{
			const k3d::uint_t output_first_edge = m_output_loop_first_edges[m_output_face_first_loops[output_face]];
			m_face_varying_copier.copy(count, &edges[0], &weights[0], output_first_edge);
			m_face_varying_copier.copy(m_mesh_arrays.clockwise_edges[edge], m_output_clockwise_edges[m_output_clockwise_edges[output_first_edge]]);
			m_uniform_copier.copy(Face, output_face);

			++output_face;
			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		m_vertex_copier.copy(count, &points[0], &weights[0], m_face_centers[Face]);
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
			k3d::attribute_array_copier& VertexCopier) :
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
		m_vertex_copier(VertexCopier)
	{}


	void operator()(const k3d::uint_t Face)
	{
		if(!m_mesh_arrays.is_affected(Face))
			return;
		
		const k3d::uint_t first_edge = m_mesh_arrays.loop_first_edges[m_mesh_arrays.face_first_loops[Face]];
		
		// Get the edge indices of the own face, for named array copying
		k3d::mesh::indices_t face_edges;
		for(k3d::uint_t edge = first_edge; ; )
		{
			face_edges.push_back(edge);
			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		
		const k3d::uint_t first_new_face = Face == 0 ? 0 : m_face_subface_counts[Face - 1];
		k3d::uint_t output_face = first_new_face;
		for(k3d::uint_t edge = first_edge; ; )
		{
			k3d::point3& midpoint = m_output_points[m_edge_midpoints[edge]];
			const k3d::uint_t companion = m_mesh_arrays.companions[edge];
			const k3d::uint_t output_first_edge = m_output_loop_first_edges[m_output_face_first_loops[output_face]];
			const k3d::uint_t output_edge1 = m_output_clockwise_edges[output_first_edge]; // Edge from midpoint to corner
			const k3d::uint_t output_edge2 = m_output_clockwise_edges[m_output_clockwise_edges[output_edge1]]; // Edge from next midpoint to face center
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
					const k3d::uint_t indices[] = {m_input_edge_points[edge],
												m_input_edge_points[m_mesh_arrays.clockwise_edges[edge]],
												m_face_centers[Face],
												m_face_centers[m_mesh_arrays.edge_faces[companion]]};
					const k3d::double_t weights[] = {0.25, 0.25, 0.25, 0.25};
					m_vertex_copier.copy(4, indices, weights, m_edge_midpoints[edge]);
				}
			}
			copy_varying_data(edge, output_edge1, face_edges);
			copy_varying_data(m_mesh_arrays.clockwise_edges[edge], output_edge2, face_edges);

			++output_face;
			edge = m_mesh_arrays.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}

private:
	
	/// Copy varying data from InputEdge to OutputEdge
	void copy_varying_data(const k3d::uint_t InputEdge, const k3d::uint_t OutputEdge, const k3d::mesh::indices_t& face_edges)
	{
		const k3d::uint_t edge = InputEdge;
		const k3d::uint_t companion = m_mesh_arrays.companions[edge];
		
		if(m_mesh_arrays.boundary(edge))
		{
			k3d::double_t weights[] = {0.5, 0.5};
			k3d::uint_t indices[] = {edge, m_mesh_arrays.clockwise_edges[edge]};
			m_face_varying_copier.copy(2, indices, weights, OutputEdge);
		}
		else
		{
			k3d::mesh::indices_t indices;
			indices.push_back(edge);
			indices.push_back(m_mesh_arrays.clockwise_edges[edge]);
			indices.insert(indices.end(), face_edges.begin(), face_edges.end());
			for(k3d::uint_t edge2 = companion; ;)
			{
				indices.push_back(edge2);
				edge2 = m_mesh_arrays.clockwise_edges[edge2];
				if(edge2 == companion)
					break;
			}
			k3d::mesh::weights_t weights;
			weights.push_back(0.25);
			weights.push_back(0.25);
			const k3d::double_t corner_weight = 0.25 / static_cast<k3d::double_t>(face_edges.size());
			for(k3d::uint_t i = 0; i != face_edges.size(); ++i)
				weights.push_back(corner_weight);
			const k3d::uint_t companion_face_edge_count = indices.size() - 2 - face_edges.size();
			const k3d::double_t companion_corner_weight = 0.25 / static_cast<k3d::double_t>(companion_face_edge_count);
			for(k3d::uint_t i = 0; i != companion_face_edge_count; ++i)
				weights.push_back(companion_corner_weight);
			m_face_varying_copier.copy(indices.size(), &indices[0], &weights[0], OutputEdge);
		}
	}
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
			k3d::attribute_array_copier& VertexCopier) :
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
		m_vertex_copier(VertexCopier)
	{}


	void operator()(const k3d::uint_t Point)
	{
		// Set initial position
		k3d::point3& output_position = m_output_points[m_corner_points[Point]];
		output_position = m_input_points[Point];
		m_vertex_copier.copy(Point, m_corner_points[Point]);
		
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
			output_position *= static_cast<double>(valence - 2.0) / static_cast<double>(valence); // adjust initial position
			k3d::vector3 sum(0, 0, 0);
			for(k3d::uint_t index = start_index; index != end_index; ++index)
			{
				const k3d::uint_t edge = m_point_edges[index];
				const k3d::uint_t clockwise = m_mesh_arrays.clockwise_edges[edge];
				const k3d::uint_t face = m_mesh_arrays.edge_faces[edge];
				const k3d::vector3 next_corner = k3d::to_vector(m_input_points[m_input_edge_points[clockwise]]);
				const k3d::vector3 face_vertex = k3d::to_vector(m_output_points[m_face_centers[face]]);
				sum += next_corner + face_vertex;
			}
			sum *= 1.0 / static_cast<double>(valence * valence);
			output_position += sum;
		}
		else if(affected_edge_count != 0) // Boundary of the subdivided surface
		{
			output_position *= 0.5;
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
				}
				if(!m_mesh_arrays.boundary(edge))
					continue;
				if(m_mesh_arrays.companions[edge] != edge || m_mesh_arrays.is_affected(m_mesh_arrays.edge_faces[edge]))
				{
					output_position += 0.25 * k3d::to_vector(m_output_points[m_edge_midpoints[edge]]);
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
};

/// Subdivide multiple levels
class catmull_clark_subdivider
{
	typedef std::vector<k3d::mesh> meshes_t;
public:
	/// Node is passed to enable the pipeline profiler
	catmull_clark_subdivider(const k3d::uint_t Levels, k3d::inode& Node) :
		m_levels(Levels),
		m_intermediate_meshes(m_levels - 1),
		m_topology_data(m_levels),
		m_node(Node)
	{}
	
	/// Create the SDS topology in Output
	void create_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output)
	{
		// If there are no valid polyhedra, we give up
		m_node.document().pipeline_profiler().start_execution(m_node, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			m_node.document().pipeline_profiler().finish_execution(m_node, "Validate input");
			return;
		}
		m_node.document().pipeline_profiler().finish_execution(m_node, "Validate input");
		
		for(k3d::uint_t level = 0; level != m_levels; ++level)
		{
			topology_data_t& topology_data = m_topology_data[level];
			const k3d::mesh& input = level == 0 ? Input : m_intermediate_meshes[level - 1];
			k3d::mesh& output = level == m_levels-1 ? Output : m_intermediate_meshes[level];
			const k3d::mesh::selection_t& input_face_selection = level == 0 ? InputFaceSelection : *input.polyhedra->face_selection;
			
			// Shallow copy of the input (no data is copied, only shared pointers are)
			output = input;

			// Make writeable copies of the arrays we intend to modify
			m_node.document().pipeline_profiler().start_execution(m_node, "Copy input");
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

			// store some common arrays
			detail_sds::mesh_arrays mesh_arrays(input_face_selection,
					input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_clockwise_edges,
					topology_data.edge_faces,
					topology_data.companions);
			
			m_node.document().pipeline_profiler().finish_execution(m_node, "Copy input");


			// Get the "companion" edge for each edge
			m_node.document().pipeline_profiler().start_execution(m_node, "Calculate companions");
			k3d::mesh::bools_t boundary_edges;
			k3d::create_edge_adjacency_lookup(output_edge_points, output_clockwise_edges, boundary_edges, topology_data.companions);
			m_node.document().pipeline_profiler().finish_execution(m_node, "Calculate companions");

			m_node.document().pipeline_profiler().start_execution(m_node, "Calculate indices");
			// For each edge, get the face it belongs to
			topology_data.edge_faces.resize(input_edge_count);
			k3d::create_edge_face_lookup(input_face_first_loops, input_face_loop_counts, input_loop_first_edges, input_clockwise_edges, topology_data.edge_faces);
			// Count the number of components of the new mesh per old face
			topology_data.face_subface_counts.resize(input_face_count);
			k3d::mesh::indices_t face_subloop_counts(input_face_count);
			k3d::mesh::indices_t face_edge_counts(input_face_count);
			k3d::mesh::indices_t face_point_counts(input_face_count);
			detail_sds::per_face_component_counter per_face_component_counter(mesh_arrays,
						topology_data.face_subface_counts,
						face_subloop_counts,
						face_edge_counts,
						face_point_counts);
			for(k3d::uint_t face = 0; face != input_face_count; ++face) per_face_component_counter(face);
			// Turn these counts into cumulative sums
			detail_sds::cumulative_sum(topology_data.face_subface_counts);
			detail_sds::cumulative_sum(face_subloop_counts);
			detail_sds::cumulative_sum(face_edge_counts);
			detail_sds::cumulative_sum(face_point_counts);
			// We now have the following relationships between old and new geometry:
			// first new component index = ..._counts[old component index - 1]
			
			topology_data.corner_points.resize(input.points->size());
			topology_data.edge_midpoints.resize(input_edge_points.size());
			topology_data.face_centers.resize(input_face_first_loops.size());
			detail_sds::point_index_calculator point_index_calculator(mesh_arrays,					
					input_edge_points,
					face_point_counts,
					topology_data.corner_points,
					topology_data.edge_midpoints,
					topology_data.face_centers);
			for(k3d::uint_t face = 0; face != input_face_count; ++face) point_index_calculator(face);
					
			// Allocate required memory
			m_node.document().pipeline_profiler().start_execution(m_node, "Allocate memory");
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
			m_node.document().pipeline_profiler().finish_execution(m_node, "Allocate memory");
			
			m_node.document().pipeline_profiler().start_execution(m_node, "Subdivide topology");
			detail_sds::topology_subdivider topology_subdivider(mesh_arrays,
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
				output_first_faces.push_back(topology_data.face_subface_counts[input_first_faces[polyhedron] - 1]);
				output_face_counts.push_back(topology_data.face_subface_counts[input_first_faces[polyhedron]]);
			}
			output_face_counts.push_back(topology_data.face_subface_counts.back());
			m_node.document().pipeline_profiler().finish_execution(m_node, "Subdivide topology");
			
			// Update selection arrays
			output_edge_selection.assign(output_edge_points.size(), 0.0);
			
			// Calculate vertex valences, needed for corner point updates.
			m_node.document().pipeline_profiler().start_execution(m_node, "Vertex valence calculation");
			k3d::create_vertex_valence_lookup(input.points->size(), input_edge_points, topology_data.vertex_valences);
			detail_sds::create_vertex_edge_lookup(input_edge_points, topology_data.vertex_valences, topology_data.point_first_edges, topology_data.point_edges);
			m_node.document().pipeline_profiler().finish_execution(m_node, "Vertex valence calculation");
		}
	}
	
	void update_mesh(const k3d::mesh& Input, const k3d::mesh::selection_t& InputFaceSelection, k3d::mesh& Output)
	{
		// If there are no valid polyhedra, we give up
		m_node.document().pipeline_profiler().start_execution(m_node, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			m_node.document().pipeline_profiler().finish_execution(m_node, "Validate input");
			return;
		}
		m_node.document().pipeline_profiler().finish_execution(m_node, "Validate input");
		
		for(k3d::uint_t level = 0; level != m_levels; ++level)
		{
			topology_data_t& topology_data = m_topology_data[level];
			const k3d::mesh& input = level == 0 ? Input : m_intermediate_meshes[level - 1];
			k3d::mesh& output = level == m_levels-1 ? Output : m_intermediate_meshes[level];
			const k3d::mesh::selection_t& input_face_selection = level == 0 ? InputFaceSelection : *input.polyhedra->face_selection;
		
			const k3d::mesh::points_t& input_points = *input.points;
			const k3d::mesh::indices_t& input_face_first_loops = *input.polyhedra->face_first_loops;
			const k3d::mesh::counts_t& input_face_loop_counts = *input.polyhedra->face_loop_counts;
			const k3d::mesh::indices_t& input_loop_first_edges = *input.polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& input_clockwise_edges = *input.polyhedra->clockwise_edges;
			const k3d::mesh::indices_t& input_edge_points = *input.polyhedra->edge_points;
			const k3d::uint_t face_count = input_face_first_loops.size();
			
			// store some common arrays
			detail_sds::mesh_arrays mesh_arrays(input_face_selection,
					input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_clockwise_edges,
					topology_data.edge_faces,
					topology_data.companions);
			
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
	
			k3d::mesh::points_t& output_points = output.points.writable();
			output_points.assign(output_points.size(), k3d::point3(0,0,0));
	
			// Calculate face centers
			m_node.document().pipeline_profiler().start_execution(m_node, "Face centers");
			detail_sds::face_center_calculator face_center_calculator(
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
			m_node.document().pipeline_profiler().finish_execution(m_node, "Face centers");
	
			// Calculate edge midpoints
			m_node.document().pipeline_profiler().start_execution(m_node, "Edge midpoints");
			detail_sds::edge_midpoint_calculator edge_midpoint_calculator(
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
					vertex_data_copier);
			for(k3d::uint_t face = 0; face != face_count; ++face) edge_midpoint_calculator(face);
			m_node.document().pipeline_profiler().finish_execution(m_node, "Edge midpoints");
			
			// Calculate new point positions
			m_node.document().pipeline_profiler().start_execution(m_node, "Point positions");
			detail_sds::corner_point_calculator corner_point_calculator(
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
					vertex_data_copier);
			for(k3d::uint_t point = 0; point != input_points.size(); ++point) corner_point_calculator(point);
			m_node.document().pipeline_profiler().finish_execution(m_node, "Point positions");
		}
	}
private:
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
	k3d::inode& m_node;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivider

class catmull_clark_subdivider :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	catmull_clark_subdivider(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_level(init_owner(*this) + init_name("level") + init_label(_("Level")) + init_description(_("Subdivision level")) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_value(1) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_subdivider(0)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_level.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(m_subdivider)
		{
			delete m_subdivider;
			m_subdivider = 0;
		}
		m_subdivider = new detail_sds::catmull_clark_subdivider(m_level.pipeline_value(), *this);
		k3d::mesh::selection_t input_face_selection = *Input.polyhedra->face_selection; // copy, so we can merge selection
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::merge_selection(mesh_selection.faces, input_face_selection);
		m_subdivider->create_mesh(Input, input_face_selection, Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::mesh::selection_t input_face_selection = *Input.polyhedra->face_selection; // copy, so we can merge selection
		k3d::merge_selection(mesh_selection.faces, input_face_selection);
		m_subdivider->update_mesh(Input, input_face_selection, Output);
	}


	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<catmull_clark_subdivider,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xdc73f6e3, 0x744b4b51, 0xac535307, 0xc82d261c),
				"CatmullClark",
				_("Generates a Catmull-Clark subdivision mesh"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_level;
	detail_sds::catmull_clark_subdivider* m_subdivider;
};

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivision_factory

k3d::iplugin_factory& catmull_clark_subdivision_factory()
{
	return module::mesh::catmull_clark_subdivider::get_factory();
}

} // namespace mesh

} // namespace module

