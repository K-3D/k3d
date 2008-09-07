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
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <set>

namespace module
{

namespace mesh
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

/// For each old face index, count the number of subfaces, loops, edges and distinct points that will be in the new mesh
/**
 * The parameters FaceSubfaceCounts, FaceSubloopCounts, FaceEdgeCounts and FacePointCounts store the result and must
 * have the same length as the number of faces of the input mesh
 */
class per_face_component_counter
{
public:
	per_face_component_counter(const k3d::mesh::selection_t& FaceSelection,
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::counts_t& FaceLoopCounts,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgeFaces,
			const k3d::mesh::indices_t& Companions,
			k3d::mesh::counts_t& FaceSubfaceCounts,
			k3d::mesh::counts_t& FaceSubloopCounts,
			k3d::mesh::counts_t& FaceEdgeCounts,
			k3d::mesh::counts_t& FacePointCounts) :
		m_face_selection(FaceSelection),
		m_face_first_loops(FaceFirstLoops),
		m_face_loop_counts(FaceLoopCounts),
		m_loop_first_edges(LoopFirstEdges),
		m_clockwise_edges(ClockwiseEdges),
		m_edge_faces(EdgeFaces),
		m_companions(Companions),
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
		if(!m_face_selection[Face] || m_face_loop_counts[Face] != 1) // Faces that will not be split
		{
			++subface_count;
			subloop_count += m_face_loop_counts[Face];
			const k3d::uint_t loop_begin = m_face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					const k3d::uint_t companion = m_companions[edge];
					++edge_count; // Count the original edge
					// Note: we assume here that in case of a boundary edge, companion == edge
					if(m_face_selection[m_edge_faces[companion]] && m_face_loop_counts[m_edge_faces[companion]] == 1)
					{
						++edge_count; // Reserve space for an edge that is split because this or the adjacent face is selected
					}
					edge = m_clockwise_edges[edge]; 
					
					// Count the new corner points, respecting face order
					if(m_edge_faces[edge] <= m_edge_faces[companion]) // Note: <= needed because edge == companion on boundary
					{
						++point_count;
					}
					
					if(edge == first_edge)
						break;
				}
			}
		}
		else // Faces to split
		{
			const k3d::uint_t first_edge = m_loop_first_edges[m_face_first_loops[Face]];
			// Count the face point
			++point_count;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t companion = m_companions[edge];
				const k3d::uint_t clockwise = m_clockwise_edges[edge];
				++subface_count;
				++subloop_count;
				edge_count += 4;
				
				if(m_edge_faces[edge] <= m_edge_faces[companion])
				{
					++point_count; // Count the midpoint
					if(m_edge_faces[clockwise] <= m_edge_faces[m_companions[clockwise]])
						++point_count;
				}
				
				edge = clockwise;
				if(edge == first_edge)
					break;
			}
		}
	}
	
private:
	const k3d::mesh::selection_t& m_face_selection;
	const k3d::mesh::indices_t& m_face_first_loops;
	const k3d::mesh::counts_t& m_face_loop_counts;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_faces;
	const k3d::mesh::indices_t& m_companions;
	k3d::mesh::counts_t& m_face_subface_counts;
	k3d::mesh::counts_t& m_face_subloop_counts;
	k3d::mesh::counts_t& m_face_edge_counts;
	k3d::mesh::counts_t& m_face_point_counts;
};

/// Calculates the new indices of corner points, edge midpoints and face centers 
class point_index_calculator
{
public:
	point_index_calculator(const k3d::mesh::selection_t& FaceSelection,
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::counts_t& FaceLoopCounts,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgeFaces,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::counts_t& FacePointCounts,
			k3d::mesh::indices_t& CornerPoints,
			k3d::mesh::indices_t& EdgeMidpoints,
			k3d::mesh::indices_t& FaceCenters
			) :
				m_face_selection(FaceSelection),
				m_face_first_loops(FaceFirstLoops),
				m_face_loop_counts(FaceLoopCounts),
				m_loop_first_edges(LoopFirstEdges),
				m_edge_points(EdgePoints),
				m_clockwise_edges(ClockwiseEdges),
				m_edge_faces(EdgeFaces),
				m_companions(Companions),
				m_face_point_counts(FacePointCounts),
				m_corner_points(CornerPoints),
				m_edge_midpoints(EdgeMidpoints),
				m_face_centers(FaceCenters)
			{}
	
	void operator()(const k3d::uint_t Face)
	{
		k3d::uint_t point_count = Face == 0 ? 0 : m_face_point_counts[Face - 1];
		if(!m_face_selection[Face] || m_face_loop_counts[Face] != 1) // Faces that will not be split
		{
			const k3d::uint_t loop_begin = m_face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					const k3d::uint_t companion = m_companions[edge];

					if(m_edge_faces[edge] <= m_edge_faces[companion]) // Note: <= needed because edge == companion on boundary
					{
						m_corner_points[m_edge_points[edge]] = point_count;
						++point_count;
					}
					
					edge = m_clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		else // Faces to split
		{
			const k3d::uint_t first_edge = m_loop_first_edges[m_face_first_loops[Face]];
			m_face_centers[Face] = point_count;
			++point_count;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t companion = m_companions[edge];
				const k3d::uint_t clockwise = m_clockwise_edges[edge];
				
				if(m_edge_faces[edge] <= m_edge_faces[companion])
				{
					m_edge_midpoints[edge] = point_count;
					m_edge_midpoints[companion] = point_count;
					++point_count; // Count the midpoint
					if(m_edge_faces[clockwise] <= m_edge_faces[m_companions[clockwise]])
					{
						m_corner_points[m_edge_points[clockwise]] = point_count;
						++point_count;
					}
				}
				
				edge = clockwise;
				if(edge == first_edge)
					break;
			}
		}
	}
	
private:
	const k3d::mesh::selection_t& m_face_selection;
	const k3d::mesh::indices_t& m_face_first_loops;
	const k3d::mesh::counts_t& m_face_loop_counts;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_faces;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::counts_t& m_face_point_counts;
	k3d::mesh::indices_t& m_corner_points;
	k3d::mesh::indices_t& m_edge_midpoints;
	k3d::mesh::indices_t& m_face_centers;
};

/// Create the subdivided mesh topology
class topology_subdivider
{
public:
	topology_subdivider(const k3d::mesh::indices_t& InputFirstFaces,
			const k3d::mesh::counts_t& InputFaceCounts,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& InputLoopFirstEdges,
			const k3d::mesh::indices_t& InputFaceFirstLoops,
			const k3d::mesh::counts_t& InputFaceLoopCounts,
			const k3d::mesh::materials_t& InputFaceMaterials,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::indices_t& EdgeFaces,
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
			k3d::mesh::selection_t& OutputFaceSelection,
			k3d::attribute_array_copier& UniformCopier,
			k3d::attribute_array_copier& VaryingCopier
			) :
				input_first_faces(InputFirstFaces),
				input_face_counts(InputFaceCounts),
				input_edge_points(InputEdgePoints),
				input_clockwise_edges(InputClockwiseEdges),
				input_loop_first_edges(InputLoopFirstEdges),
				input_face_first_loops(InputFaceFirstLoops),
				input_face_loop_counts(InputFaceLoopCounts),
				input_face_materials(InputFaceMaterials),
				input_face_selection(InputFaceSelection),
				companions(Companions),
				edge_faces(EdgeFaces),
				face_subface_counts(FaceSubfaceCounts),
				face_subloop_counts(FaceSubloopCounts),
				face_edge_counts(FaceEdgeCounts),
				corner_points(CornerPoints),
				edge_midpoints(EdgeMidpoints),
				face_centers(FaceCenters),
				output_edge_points(OutputEdgePoints),
				output_clockwise_edges(OutputClockwiseEdges),
				output_loop_first_edges(OutputLoopFirstEdges),
				output_face_first_loops(OutputFaceFirstLoops),
				output_face_loop_counts(OutputFaceLoopCounts),
				output_face_materials(OutputFaceMaterials),
				output_face_selection(OutputFaceSelection),
				uniform_copier(UniformCopier),
				varying_copier(VaryingCopier)
				{}
	
	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = Face == 0 ? 0 : face_subface_counts[Face - 1];
		const k3d::uint_t first_new_loop = Face == 0 ? 0 : face_subloop_counts[Face - 1];

		if(!input_face_selection[Face] || input_face_loop_counts[Face] != 1)
		{ // copy unaffected face, splitting edges adjacent to affected faces
			copy_face(Polyhedron, Face);
		}
		else
		{ // subdivide affected faces
			const k3d::uint_t face_center = face_centers[Face];
			const k3d::uint_t first_edge = input_loop_first_edges[input_face_first_loops[Face]];
			k3d::uint_t edgenumber = 0;
			k3d::uint_t first_new_edge = Face == 0 ? 0 : face_edge_counts[Face - 1];
			output_loop_first_edges[first_new_loop] = first_new_edge;
			output_face_first_loops[first_new_face] = first_new_loop;
			output_face_materials[first_new_face] = input_face_materials[Face];
			output_face_selection[first_new_face] = input_face_selection[Face];

			// Store all corner indices for facevarying data copy
//			k3d::mesh::indices_t face_indices;
//			for(k3d::uint_t edge = first_edge; ; )
//			{
//				face_indices.push_back(edge);
//				edge = input_clockwise_edges[edge];
//				if(edge == first_edge)
//					break;
//			}

//			const k3d::uint_t num_edges = face_indices.size();
//			k3d::mesh::weights_t face_weights(num_edges, 1.0/static_cast<k3d::double_t>(num_edges));

			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t clockwise = input_clockwise_edges[edge];
				const k3d::uint_t mid1 = edge_midpoints[edge];
				const k3d::uint_t mid2 = edge_midpoints[clockwise];
				const k3d::uint_t center_to_mid1 = first_new_edge;
				const k3d::uint_t mid1_to_corner = center_to_mid1 + 1;
				const k3d::uint_t corner_to_mid2 = mid1_to_corner + 1;
				const k3d::uint_t mid2_to_center = corner_to_mid2 + 1;
				output_edge_points[mid1_to_corner] = mid1;
				output_edge_points[corner_to_mid2] = corner_points[input_edge_points[clockwise]];
				output_edge_points[mid2_to_center] = mid2;
				output_edge_points[center_to_mid1] = face_center;
				output_clockwise_edges[mid1_to_corner] = corner_to_mid2;
				output_clockwise_edges[corner_to_mid2] = mid2_to_center;
				output_clockwise_edges[mid2_to_center] = center_to_mid1;
				output_clockwise_edges[center_to_mid1] = mid1_to_corner;
				const k3d::double_t weights[] = {0.5, 0.5};
				const k3d::uint_t mid1_to_corner_indices[] = {edge, clockwise};
				// TODO: Restore named array copying varying_copier.copy(2, mid1_to_corner_indices, weights, mid1_to_corner);
				// TODO: Restore named array copying varying_copier.copy(clockwise, corner_to_mid2);
				const k3d::uint_t mid2_to_center_indices[] = {clockwise, input_clockwise_edges[clockwise]};
				// TODO: Restore named array copying varying_copier.copy(2, mid2_to_center_indices, weights, mid2_to_center);
				// TODO: Restore named array copying varying_copier.copy(num_edges, &face_indices[0], &face_weights[0], center_to_mid1);

				// Append face data
				const k3d::uint_t newloop = first_new_loop + edgenumber;
				output_loop_first_edges[newloop] = center_to_mid1; 
				output_face_first_loops[first_new_face + edgenumber] = newloop;
				output_face_materials[first_new_face + edgenumber] = input_face_materials[Face];
				output_face_selection[first_new_face + edgenumber] = input_face_selection[Face];
				// TODO: Restore named array copying uniform_copier.copy(Face, first_new_face + edgenumber);

				first_new_edge += 4;

				edge = input_clockwise_edges[edge];
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
		const k3d::uint_t first_new_edge = Face == 0 ? 0 : face_edge_counts[Face - 1];
		const k3d::uint_t first_new_face = Face == 0 ? 0 : face_subface_counts[Face - 1];
		const k3d::uint_t first_new_loop = Face == 0 ? 0 : face_subloop_counts[Face - 1];

		const k3d::uint_t loop_begin = input_face_first_loops[Face];
		const k3d::uint_t loop_end = loop_begin + input_face_loop_counts[Face];
		k3d::uint_t edgenumber = 0;
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = input_loop_first_edges[loop];
			k3d::uint_t loopedgenumber = 0;
			output_loop_first_edges[first_new_loop + loop - loop_begin] = first_new_edge + edgenumber;
			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t companion = companions[edge];
				const k3d::uint_t newedge = first_new_edge + edgenumber + loopedgenumber;
				output_edge_points[newedge] = corner_points[input_edge_points[edge]];
				// Copy varying data.
				// TODO: Restore named array copying varying_copier.copy(edge, newedge);
				output_clockwise_edges[newedge] = newedge + 1;
				if(input_face_selection[edge_faces[companion]] && input_face_loop_counts[edge_faces[companion]] == 1)
				{
					const k3d::double_t weights[] = {0.5, 0.5};
					const k3d::uint_t indices[] = {edge, input_clockwise_edges[edge]};
					// TODO: Restore named array copying varying_copier.copy(2, indices, weights, newedge + 1);
					output_edge_points[newedge + 1] = edge_midpoints[companions[edge]];
					output_clockwise_edges[newedge + 1] = newedge + 2;
					++loopedgenumber;
				}

				edge = input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
				++loopedgenumber;
			}
			output_clockwise_edges[first_new_edge + edgenumber + loopedgenumber] = first_new_edge + edgenumber; // close the loop
			edgenumber += loopedgenumber + 1;
		}

		// Set face data
		output_face_loop_counts[first_new_face] = input_face_loop_counts[Face];
		output_face_first_loops[first_new_face] = first_new_loop;
		output_face_materials[first_new_face] = input_face_materials[Face];
		output_face_selection[first_new_face] = input_face_selection[Face];

		// Copy uniform data.
		// TODO: Restore named array copying uniform_copier.copy(Face, first_new_face);
	}

	const k3d::mesh::indices_t& input_first_faces;
	const k3d::mesh::counts_t& input_face_counts;
	const k3d::mesh::indices_t& input_edge_points;
	const k3d::mesh::indices_t& input_clockwise_edges;
	const k3d::mesh::indices_t& input_loop_first_edges;
	const k3d::mesh::indices_t& input_face_first_loops;
	const k3d::mesh::counts_t& input_face_loop_counts;
	const k3d::mesh::materials_t& input_face_materials;
	const k3d::mesh::selection_t& input_face_selection;
	const k3d::mesh::indices_t& companions;
	const k3d::mesh::indices_t& edge_faces;
	const k3d::mesh::counts_t& face_subface_counts;
	const k3d::mesh::counts_t& face_subloop_counts;
	const k3d::mesh::counts_t& face_edge_counts;
	const k3d::mesh::indices_t& corner_points;
	const k3d::mesh::indices_t& edge_midpoints;
	const k3d::mesh::indices_t& face_centers;
	k3d::mesh::indices_t& output_edge_points;
	k3d::mesh::indices_t& output_clockwise_edges;
	k3d::mesh::indices_t& output_loop_first_edges;
	k3d::mesh::indices_t& output_face_first_loops;
	k3d::mesh::counts_t& output_face_loop_counts;
	k3d::mesh::materials_t& output_face_materials;
	k3d::mesh::selection_t& output_face_selection;
	k3d::attribute_array_copier& uniform_copier;
	k3d::attribute_array_copier& varying_copier;
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
			const k3d::mesh::indices_t& InputFaceFirstLoops,
			const k3d::mesh::counts_t& InputFaceLoopCounts,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& InputLoopFirstEdges,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints) :
		m_input_face_first_loops(InputFaceFirstLoops),
		m_input_face_loop_counts(InputFaceLoopCounts),
		m_input_face_selection(InputFaceSelection),
		m_input_loop_first_edges(InputLoopFirstEdges),
		m_input_clockwise_edges(InputClockwiseEdges),
		m_input_edge_points(InputEdgePoints),
		m_face_centers(FaceCenters),
		m_input_points(InputPoints),
		m_output_points(OutputPoints)
	{}


	void operator()(const k3d::uint_t Face)
	{
		if(!m_input_face_selection[Face] || m_input_face_loop_counts[Face] != 1)
			return;
		
		const k3d::uint_t first_edge = m_input_loop_first_edges[m_input_face_first_loops[Face]];
		k3d::point3& center = m_output_points[m_face_centers[Face]];
		center = k3d::point3(0,0,0);
		k3d::uint_t count = 0;
		for(k3d::uint_t edge = first_edge; ; )
		{
			center += k3d::to_vector(m_input_points[m_input_edge_points[edge]]);
			++count;

			edge = m_input_clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}

		center /= count;
	}

private:
	const k3d::mesh::indices_t& m_input_face_first_loops;
	const k3d::mesh::counts_t& m_input_face_loop_counts;
	const k3d::mesh::selection_t& m_input_face_selection;
	const k3d::mesh::indices_t& m_input_loop_first_edges;
	const k3d::mesh::indices_t& m_input_clockwise_edges;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
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
			const k3d::mesh::indices_t& InputFaceFirstLoops,
			const k3d::mesh::counts_t& InputFaceLoopCounts,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& InputLoopFirstEdges,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::indices_t& EdgeFaces,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints) :
		m_input_face_first_loops(InputFaceFirstLoops),
		m_input_face_loop_counts(InputFaceLoopCounts),
		m_input_face_selection(InputFaceSelection),
		m_input_loop_first_edges(InputLoopFirstEdges),
		m_input_clockwise_edges(InputClockwiseEdges),
		m_input_edge_points(InputEdgePoints),
		m_companions(Companions),
		m_edge_faces(EdgeFaces),
		m_edge_midpoints(EdgeMidpoints),
		m_face_centers(FaceCenters),
		m_input_points(InputPoints),
		m_output_points(OutputPoints)
	{}


	void operator()(const k3d::uint_t Face)
	{
		if(!m_input_face_selection[Face] || m_input_face_loop_counts[Face] != 1)
			return;
		
		const k3d::uint_t first_edge = m_input_loop_first_edges[m_input_face_first_loops[Face]];
		for(k3d::uint_t edge = first_edge; ; )
		{
			k3d::point3& midpoint = m_output_points[m_edge_midpoints[edge]];
			const k3d::uint_t companion = m_companions[edge];
			if(Face <= m_edge_faces[companion] || !m_input_face_selection[m_edge_faces[companion]])
			{
				if(companion == edge || !m_input_face_selection[m_edge_faces[companion]]) // boundary edge or boundary of selection
				{
					midpoint = 0.5 * (m_input_points[m_input_edge_points[edge]] + k3d::to_vector(m_input_points[m_input_edge_points[m_input_clockwise_edges[edge]]]));
				}
				else
				{
					midpoint = (m_input_points[m_input_edge_points[edge]] + k3d::to_vector(m_input_points[m_input_edge_points[m_input_clockwise_edges[edge]]])); // midpoint
					midpoint += k3d::to_vector(m_output_points[m_face_centers[Face]]); // Add the face center of the current face
					// If needed, add the face center of the neighboring face
					if(companion != edge && m_input_face_selection[m_edge_faces[companion]])
						midpoint += k3d::to_vector(m_output_points[m_face_centers[m_edge_faces[companion]]]);
					midpoint *= 0.25;
				}
			}

			edge = m_input_clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}

private:
	const k3d::mesh::indices_t& m_input_face_first_loops;
	const k3d::mesh::counts_t& m_input_face_loop_counts;
	const k3d::mesh::selection_t& m_input_face_selection;
	const k3d::mesh::indices_t& m_input_loop_first_edges;
	const k3d::mesh::indices_t& m_input_clockwise_edges;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::indices_t& m_edge_faces;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
};

/// Calculates patch corner positions
class corner_point_calculator
{
public:
	corner_point_calculator(
			const k3d::mesh::indices_t& InputFaceFirstLoops,
			const k3d::mesh::counts_t& InputFaceLoopCounts,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& InputLoopFirstEdges,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::indices_t& EdgeFaces,
			const k3d::mesh::indices_t& CornerPoints,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::indices_t& VertexValences,
			const k3d::mesh::points_t& InputPoints,
			k3d::mesh::points_t& OutputPoints) :
		m_input_face_first_loops(InputFaceFirstLoops),
		m_input_face_loop_counts(InputFaceLoopCounts),
		m_input_face_selection(InputFaceSelection),
		m_input_loop_first_edges(InputLoopFirstEdges),
		m_input_clockwise_edges(InputClockwiseEdges),
		m_input_edge_points(InputEdgePoints),
		m_companions(Companions),
		m_edge_faces(EdgeFaces),
		m_corner_points(CornerPoints),
		m_edge_midpoints(EdgeMidpoints),
		m_face_centers(FaceCenters),
		m_input_points(InputPoints),
		m_vertex_valences(VertexValences),
		m_output_points(OutputPoints)
	{}


	void operator()(const k3d::uint_t Face)
	{
		if(!m_input_face_selection[Face] || m_input_face_loop_counts[Face] != 1)
		{
			const k3d::uint_t loop_begin = m_input_face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + m_input_face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_input_loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					m_output_points[m_corner_points[m_input_edge_points[edge]]] = m_input_points[m_input_edge_points[edge]];
					edge = m_input_clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
			return;
		}
		
		const k3d::uint_t first_edge = m_input_loop_first_edges[m_input_face_first_loops[Face]];
		for(k3d::uint_t edge = first_edge; ; )
		{
			const k3d::uint_t clockwise = m_input_clockwise_edges[edge];
			const k3d::vector3 next_corner = k3d::to_vector(m_input_points[m_input_edge_points[clockwise]]);
			const k3d::vector3 face_vertex = k3d::to_vector(m_output_points[m_face_centers[Face]]);
			const k3d::double_t n = static_cast<k3d::double_t>(m_vertex_valences[m_input_edge_points[edge]]); // valence of the updated vertex
			const k3d::double_t nninv = 1.0 / (n*n); // inverse of valence^2
			const k3d::uint_t companion = m_companions[edge];
			
			k3d::point3& corner = m_output_points[m_corner_points[m_input_edge_points[edge]]];
			if(companion == edge || !m_input_face_selection[m_edge_faces[companion]]) // boundary edge or boundary of selection
			{
				corner += 0.5 * k3d::to_vector(m_input_points[m_input_edge_points[edge]]) + 0.25 * k3d::to_vector(m_output_points[m_edge_midpoints[edge]]);
				m_output_points[m_corner_points[m_input_edge_points[clockwise]]] +=
					0.25 * k3d::to_vector(m_output_points[m_edge_midpoints[edge]]);
			}
			else
			{
				// If this is the "first" time we see this point, add the contribution of the original position
				if(m_edge_faces[edge] <= m_edge_faces[companion] && m_edge_faces[clockwise] <= m_edge_faces[m_companions[clockwise]])
				{
					const k3d::double_t valence = static_cast<k3d::double_t>(m_vertex_valences[m_input_edge_points[clockwise]]);
					m_output_points[m_corner_points[m_input_edge_points[clockwise]]] += (valence - 2.0) / valence * k3d::to_vector(m_input_points[m_input_edge_points[clockwise]]);
				}
				if(m_input_clockwise_edges[companion] != m_companions[m_input_clockwise_edges[companion]]
							         && m_input_face_selection[m_edge_faces[m_companions[m_input_clockwise_edges[companion]]]])
					corner += nninv * (next_corner + face_vertex);
			}

			edge = clockwise;
			if(edge == first_edge)
				break;
		}
	}

private:
	const k3d::mesh::indices_t& m_input_face_first_loops;
	const k3d::mesh::counts_t& m_input_face_loop_counts;
	const k3d::mesh::selection_t& m_input_face_selection;
	const k3d::mesh::indices_t& m_input_loop_first_edges;
	const k3d::mesh::indices_t& m_input_clockwise_edges;
	const k3d::mesh::indices_t& m_input_edge_points;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::indices_t& m_edge_faces;
	const k3d::mesh::indices_t& m_corner_points;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::mesh::indices_t& m_face_centers;
	const k3d::mesh::counts_t& m_vertex_valences;
	const k3d::mesh::points_t& m_input_points;
	k3d::mesh::points_t& m_output_points;
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
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Shallow copy of the input (no data is copied, only shared pointers are)
		document().pipeline_profiler().start_execution(*this, "Merge selection");
		Output = Input;
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh
		document().pipeline_profiler().finish_execution(*this, "Merge selection");

		// If there are no valid polyhedra, we give up
		document().pipeline_profiler().start_execution(*this, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			document().pipeline_profiler().finish_execution(*this, "Validate input");
			return;
		}
		document().pipeline_profiler().finish_execution(*this, "Validate input");


		// Make writeable copies of the arrays we intend to modify
		document().pipeline_profiler().start_execution(*this, "Copy input");
		k3d::mesh::polyhedra_t& output_polyhedra = *k3d::make_unique(Output.polyhedra);
		k3d::mesh::polyhedra_t::types_t& output_types = *k3d::make_unique(output_polyhedra.types);
		k3d::mesh::indices_t& output_loop_first_edges = *k3d::make_unique(output_polyhedra.loop_first_edges);
		k3d::mesh::indices_t& output_edge_points = *k3d::make_unique(output_polyhedra.edge_points);
		k3d::mesh::indices_t& output_clockwise_edges = *k3d::make_unique(output_polyhedra.clockwise_edges);
		k3d::mesh::selection_t& output_edge_selection = *k3d::make_unique(output_polyhedra.edge_selection);

		// Copy the unaffected constant data
		output_polyhedra.constant_data = Input.polyhedra->constant_data;
		// Create copiers for the uniform and varying data
		output_polyhedra.uniform_data = Input.polyhedra->uniform_data.clone_types();
		k3d::attribute_array_copier uniform_data_copier(Input.polyhedra->uniform_data, output_polyhedra.uniform_data);
		output_polyhedra.face_varying_data = Input.polyhedra->face_varying_data.clone_types();
		k3d::attribute_array_copier face_varying_data_copier(Input.polyhedra->face_varying_data, output_polyhedra.face_varying_data);

		const k3d::mesh::polyhedra_t& input_polyhedra = *Input.polyhedra;
		const k3d::mesh::indices_t& input_first_faces = *(input_polyhedra.first_faces);
		const k3d::mesh::counts_t& input_face_counts = *(input_polyhedra.face_counts);
		const k3d::mesh::indices_t& input_face_first_loops = *(input_polyhedra.face_first_loops);
		const k3d::mesh::indices_t& input_loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& input_clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::mesh::indices_t& input_edge_points = *Input.polyhedra->edge_points;
		const k3d::mesh::counts_t& input_face_loop_counts = *(input_polyhedra.face_loop_counts);
		const k3d::mesh::selection_t& input_face_selection = *(output_polyhedra.face_selection);
		const k3d::mesh::materials_t& input_face_materials = *(input_polyhedra.face_materials);
		const k3d::uint_t input_edge_count = input_clockwise_edges.size();
		const k3d::uint_t input_face_count = input_face_first_loops.size();
		// Face-related arrays can not be appended to because of the possibility of multiple polyhedra,
		// so we will rebuild them from scratch in the new order
		boost::shared_ptr<k3d::mesh::indices_t> output_first_faces(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::counts_t> output_face_counts(new k3d::mesh::counts_t());
		boost::shared_ptr<k3d::mesh::indices_t> output_face_first_loops(new k3d::mesh::indices_t());
		boost::shared_ptr<k3d::mesh::counts_t> output_face_loop_counts(new k3d::mesh::counts_t());
		boost::shared_ptr<k3d::mesh::selection_t> output_face_selection(new k3d::mesh::selection_t());
		boost::shared_ptr<k3d::mesh::materials_t> output_face_materials(new k3d::mesh::materials_t());

		document().pipeline_profiler().finish_execution(*this, "Copy input");


		// Get the "companion" edge for each edge
		document().pipeline_profiler().start_execution(*this, "Calculate companions");
		k3d::mesh::bools_t boundary_edges;
		k3d::create_edge_adjacency_lookup(output_edge_points, output_clockwise_edges, boundary_edges, m_companions);
		document().pipeline_profiler().finish_execution(*this, "Calculate companions");

		document().pipeline_profiler().start_execution(*this, "Calculate indices");
		// For each edge, get the face it belongs to
		m_edge_faces.resize(input_edge_count);
		k3d::create_edge_face_lookup(input_face_first_loops, input_face_loop_counts, input_loop_first_edges, input_clockwise_edges, m_edge_faces);
		// Count the number of components of the new mesh per old face
		k3d::mesh::indices_t face_subface_counts(input_face_count);
		k3d::mesh::indices_t face_subloop_counts(input_face_count);
		k3d::mesh::indices_t face_edge_counts(input_face_count);
		k3d::mesh::indices_t face_point_counts(input_face_count);
		detail::per_face_component_counter per_face_component_counter(input_face_selection,
					input_face_first_loops,
					input_face_loop_counts,
					input_loop_first_edges,
					input_clockwise_edges,
					m_edge_faces,
					m_companions,
					face_subface_counts,
					face_subloop_counts,
					face_edge_counts,
					face_point_counts);
		for(k3d::uint_t face = 0; face != input_face_count; ++face) per_face_component_counter(face);
		// Turn these counts into cumulative sums
		detail::cumulative_sum(face_subface_counts);
		detail::cumulative_sum(face_subloop_counts);
		detail::cumulative_sum(face_edge_counts);
		detail::cumulative_sum(face_point_counts);
		// We now have the following relationships between old and new geometry:
		// first new component index = ...counts[old component index - 1]
//		k3d::log() << debug << "face_subface_counts:";
//		for(k3d::uint_t face = 0; face != input_face_count; ++face)
//		{
//			k3d::log() << debug << " " << face_subface_counts[face];
//		}
//		k3d::log() << debug << std::endl;
		
		m_corner_points.resize(Input.points->size());
		m_edge_midpoints.resize(input_edge_points.size());
		m_face_centers.resize(input_face_first_loops.size());
		detail::point_index_calculator point_index_calculator(input_face_selection,
				input_face_first_loops,
				input_face_loop_counts,
				input_loop_first_edges,					
				input_edge_points,
				input_clockwise_edges,
				m_edge_faces,
				m_companions,
				face_point_counts,
				m_corner_points,
				m_edge_midpoints,
				m_face_centers);
		for(k3d::uint_t face = 0; face != input_face_count; ++face) point_index_calculator(face);
//		k3d::log() << debug << "corner_points:";
//		for(k3d::uint_t point = 0; point != corner_points.size(); ++point)
//		{
//			k3d::log() << debug << " " << corner_points[point];
//		}
//		k3d::log() << debug << std::endl;
				
		// Allocate required memory
		document().pipeline_profiler().start_execution(*this, "Allocate memory");
		boost::shared_ptr<k3d::mesh::points_t> output_points(new k3d::mesh::points_t(face_point_counts.back()));
		boost::shared_ptr<k3d::mesh::selection_t> output_point_selection(new k3d::mesh::selection_t(output_points->size(), 0.0));
		output_edge_points.resize(face_edge_counts.back(), 0);
		output_clockwise_edges.resize(face_edge_counts.back(), 0);
		output_loop_first_edges.resize(face_subloop_counts.back());
		output_face_first_loops->resize(face_subface_counts.back());
		output_face_loop_counts->resize(face_subface_counts.back(), 1);
		output_face_selection->resize(face_subface_counts.back(), 0.0);
		output_face_materials->resize(face_subface_counts.back());
		output_polyhedra.face_varying_data.resize(face_edge_counts.back());
		output_polyhedra.uniform_data.resize(face_subface_counts.back());
		document().pipeline_profiler().finish_execution(*this, "Allocate memory");
		
		document().pipeline_profiler().start_execution(*this, "Subdivide topology");
		detail::topology_subdivider topology_subdivider(input_first_faces,
				input_face_counts,
				*Input.polyhedra->edge_points,
				*Input.polyhedra->clockwise_edges,
				*Input.polyhedra->loop_first_edges,
				input_face_first_loops,
				*Input.polyhedra->face_loop_counts,
				input_face_materials,
				input_face_selection,
				m_companions,
				m_edge_faces,
				face_subface_counts,
				face_subloop_counts,
				face_edge_counts,
				m_corner_points,
				m_edge_midpoints,
				m_face_centers,
				output_edge_points,
				output_clockwise_edges,
				output_loop_first_edges,
				*output_face_first_loops,
				*output_face_loop_counts,
				*output_face_materials,
				*output_face_selection,
				uniform_data_copier,
				face_varying_data_copier);
		
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
		output_first_faces->push_back(0);
		for(k3d::uint_t polyhedron = 1; polyhedron != input_first_faces.size(); ++polyhedron)
		{
			output_first_faces->push_back(face_subface_counts[input_first_faces[polyhedron] - 1]);
			output_face_counts->push_back(face_subface_counts[input_first_faces[polyhedron]]);
		}
		output_face_counts->push_back(face_subface_counts.back());
		document().pipeline_profiler().finish_execution(*this, "Subdivide topology");
		
		// Update selection arrays
		output_edge_selection.assign(output_edge_points.size(), 0.0);

		// Set the arrays that were rebuilt
		output_polyhedra.first_faces = output_first_faces;
		output_polyhedra.face_counts = output_face_counts;
		output_polyhedra.face_first_loops = output_face_first_loops;
		output_polyhedra.face_loop_counts = output_face_loop_counts;
		output_polyhedra.face_selection = output_face_selection;
		output_polyhedra.face_materials = output_face_materials;
		Output.points = output_points;
		Output.point_selection = output_point_selection;
		
		// Calculate vertex valences, needed for corner point updates.
		document().pipeline_profiler().start_execution(*this, "Vertex valence calculation");
		k3d::create_vertex_valence_lookup(Input.points->size(), input_edge_points, m_vertex_valences);
		document().pipeline_profiler().finish_execution(*this, "Vertex valence calculation");
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// If there are no valid polyhedra, we give up
		document().pipeline_profiler().start_execution(*this, "Validate input");
		if(!k3d::validate_polyhedra(Input))
		{
			document().pipeline_profiler().finish_execution(*this, "Validate input");
			return;
		}
		document().pipeline_profiler().finish_execution(*this, "Validate input");
		
		const k3d::mesh::points_t& input_points = *Input.points;
		const k3d::mesh::indices_t& input_face_first_loops = *Input.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& input_face_loop_counts = *Input.polyhedra->face_loop_counts;
		const k3d::mesh::indices_t& input_loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& input_clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::mesh::indices_t& input_edge_points = *Input.polyhedra->edge_points;
		const k3d::uint_t face_count = input_face_first_loops.size();
		k3d::mesh::selection_t input_face_selection(face_count);
		const k3d::mesh_selection mesh_selection = m_mesh_selection.pipeline_value();
		k3d::merge_selection(mesh_selection.faces, input_face_selection);

		k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);
		output_points.assign(output_points.size(), k3d::point3(0,0,0));

		// Calculate face centers
		document().pipeline_profiler().start_execution(*this, "Face centers");
		detail::face_center_calculator face_center_calculator(
				input_face_first_loops,
				input_face_loop_counts,
				input_face_selection,
				input_loop_first_edges,
				input_clockwise_edges,
				input_edge_points,
				m_face_centers,
				input_points,
				output_points);
		for(k3d::uint_t face = 0; face != face_count; ++face) face_center_calculator(face);
		document().pipeline_profiler().finish_execution(*this, "Face centers");

		// Calculate edge midpoints
		document().pipeline_profiler().start_execution(*this, "Edge midpoints");
		detail::edge_midpoint_calculator edge_midpoint_calculator(
				input_face_first_loops,
				input_face_loop_counts,
				input_face_selection,
				input_loop_first_edges,
				input_clockwise_edges,
				input_edge_points,
				m_companions,
				m_edge_faces,
				m_edge_midpoints,
				m_face_centers,
				input_points,
				output_points);
		for(k3d::uint_t face = 0; face != face_count; ++face) edge_midpoint_calculator(face);
		document().pipeline_profiler().finish_execution(*this, "Edge midpoints");
		
		// Calculate new point positions
		document().pipeline_profiler().start_execution(*this, "Point positions");
		detail::corner_point_calculator corner_point_calculator(
				input_face_first_loops,
				input_face_loop_counts,
				input_face_selection,
				input_loop_first_edges,
				input_clockwise_edges,
				input_edge_points,
				m_companions,
				m_edge_faces,
				m_corner_points,
				m_edge_midpoints,
				m_face_centers,
				m_vertex_valences,
				input_points,
				output_points);
		for(k3d::uint_t face = 0; face != face_count; ++face) corner_point_calculator(face);
		document().pipeline_profiler().finish_execution(*this, "Point positions");
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
	//k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_levels;

	k3d::mesh::indices_t m_corner_points; // New point index, for each original point
	k3d::mesh::indices_t m_edge_midpoints; // Midpoint index for each edge (if any)
	k3d::mesh::indices_t m_face_centers; // Face center index, for each face (if any)
	k3d::mesh::indices_t m_companions; // Companion edges
	k3d::mesh::indices_t m_edge_faces; // For each original edge, the original owning face
	k3d::mesh::counts_t m_vertex_valences; // For each original vertex, the valence
};

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivision_factory

k3d::iplugin_factory& catmull_clark_subdivision_factory()
{
	return module::mesh::catmull_clark_subdivider::get_factory();
}

} // namespace mesh

} // namespace module

