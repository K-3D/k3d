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
		\author Romain Behar (romainbehar@yahoo.com)
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
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh
{

namespace detail
{

/// Calculate the indices of edge midpoints, and compile a list of unique edges to split
class midpoint_index_calculator
{
public:
	/// Constructor for the function object
	/**
	 * \param Polyhedra The Polyhedra structure to be investigated
	 * \param Companions The companion edge for each edge, if any
	 * \param BoundaryEdges True for all edges that have no companion
	 * \param StoreFaceCenters True if face centers are to be stored
	 * \param AffectedEdges The edges that belong to selected faces, without their companions
	 * \param EdgeMidpoints The midpoint index for each midpoint
	 * \param HasMidpoint True for each affected edge that has a midpoint
	 */
	midpoint_index_calculator(const k3d::mesh::polyhedra_t& Polyhedra,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::bools_t& BoundaryEdges,
			const k3d::bool_t StoreMidpoints,
			const k3d::bool_t StoreFaceCenters,
			k3d::mesh::indices_t& AffectedEdges,
			k3d::mesh::indices_t& EdgeMidpoints,
			k3d::mesh::bools_t& HasMidpoint,
			k3d::mesh::indices_t& FaceCenters,
			const k3d::uint_t FirstMidpoint) :
		m_polyhedra(Polyhedra),
		m_input_face_selection(InputFaceSelection),
		m_companions(Companions),
		m_boundary_edges(BoundaryEdges),
		m_store_midpoints(StoreMidpoints),
		m_store_face_centers(StoreFaceCenters),
		m_affected_edges(AffectedEdges),
		m_edge_midpoints(EdgeMidpoints),
		m_has_midpoint(HasMidpoint),
		m_face_centers(FaceCenters),
		m_midpoint(FirstMidpoint)
	{}

	void operator()(const k3d::uint_t Face)
	{
		const k3d::mesh::indices_t& face_first_loops = *m_polyhedra.face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *m_polyhedra.face_loop_counts;
		const k3d::mesh::selection_t& face_selection = m_input_face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *m_polyhedra.loop_first_edges;
		const k3d::mesh::indices_t& clockwise_edges = *m_polyhedra.clockwise_edges;

		// Skip unselected faces and faces with holes
		if(!face_selection[Face] || face_loop_counts[Face] != 1)
			return;

		const k3d::uint_t first_edge = loop_first_edges[face_first_loops[Face]];
		if(m_store_midpoints)
		{
			for(k3d::uint_t edge = first_edge; ; )
			{
				m_has_midpoint[edge] = true;
				// Skip edges that were marked already
				if(m_boundary_edges[edge] || !m_has_midpoint[m_companions[edge]])
				{
					m_edge_midpoints[edge] = m_midpoint;
					m_affected_edges.push_back(edge);
					++m_midpoint;
				}
				if(!m_boundary_edges[edge])
					m_edge_midpoints[m_companions[edge]] = m_edge_midpoints[edge];

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
		if(m_store_face_centers)
		{
			m_face_centers[Face] = m_midpoint;
			++m_midpoint;
		}
	}

private:
	const k3d::mesh::polyhedra_t& m_polyhedra;
	const k3d::mesh::selection_t& m_input_face_selection;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::bools_t& m_boundary_edges;
	const k3d::bool_t m_store_midpoints;
	const k3d::bool_t m_store_face_centers;
	k3d::mesh::indices_t& m_affected_edges;
	k3d::mesh::indices_t& m_edge_midpoints;
	k3d::mesh::bools_t& m_has_midpoint;
	k3d::mesh::indices_t& m_face_centers;
	k3d::uint_t m_midpoint;
};

/// Count selected faces and their edges. Must be run sequentially.
class face_edge_counter
{
public:
	/// Constructor for the function object
	/**
	 * \param Polyhedra The Polyhedra structure to be investigated
	 * \param HasMidpoint True for edges belonging to a selected face that have a midpoint
	 * \param ExtraFaceCount The number of extra faces for each subdivided faces (in addision to the number of edges)
	 * \param NewEdgeCount The number of edges for each new face created
	 * \param ReplacementFaceKeepsEdgeCount True if the face of a subdivided face that replaces the original face has the same edge count as the original face. If false, NewEdgeCount is used instead
	 * \param AffectedFaces The indices for all selected faces without holes
	 * \param FirstNewEdges For each old face, the index of the first new edge in the output
	 * \param FirstNewFaces For each old face, the corresponding index in the new array or the first subface in the new array
	 * \param FirstNewLoops The first new loop index for each face
	 */
	face_edge_counter(const k3d::mesh::polyhedra_t& Polyhedra,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::bools_t& BoundaryEdges,
			const k3d::mesh::bools_t& HasMidpoint,
			const k3d::mesh::counts_t& LoopsPerPolyhedra,
			const k3d::uint_t ExtraFaceCount,
			const k3d::uint_t NewEdgeCount,
			const k3d::bool_t ReplacementFaceKeepsEdgeCount,
			k3d::mesh::indices_t& AffectedFaces,
			k3d::mesh::indices_t& FirstNewEdges,
			k3d::mesh::indices_t& FirstNewFaces,
			k3d::mesh::indices_t& FirstNewLoops,
			k3d::mesh::counts_t& FaceEdgeCounts,
			k3d::mesh::indices_t& FaceStarts,
			k3d::mesh::counts_t& FaceCounts) :
		edge_count(0),
		face_count(0),
		loop_count(0),
		m_polyhedra(Polyhedra),
		m_input_face_selection(InputFaceSelection),
		m_companions(Companions),
		m_boundary_edges(BoundaryEdges),
		m_has_midpoint(HasMidpoint),
		m_loops_per_polyhedra(LoopsPerPolyhedra),
		m_extra_face_count(ExtraFaceCount),
		m_new_edge_count(NewEdgeCount),
		m_replacement_face_keeps_edge_count(ReplacementFaceKeepsEdgeCount),
		m_affected_faces(AffectedFaces),
		m_first_new_edges(FirstNewEdges),
		m_first_new_faces(FirstNewFaces),
		m_first_new_loops(FirstNewLoops),
		m_face_edge_counts(FaceEdgeCounts),
		m_face_starts(FaceStarts),
		m_face_counts(FaceCounts),
		m_last_polyhedron(0),
		m_new_face_count(0),
		m_running_loop_counter(0),
		m_running_edge_counter(0)
	{
		m_first_new_faces.resize(face_count);
		m_first_new_loops.resize(face_count);
		m_face_starts.push_back(0);
		m_face_counts.push_back(0);
	}

	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		if(Polyhedron != m_last_polyhedron)
		{
			m_running_loop_counter += m_new_face_count;
			m_running_edge_counter += m_new_face_count * (m_new_edge_count);
			m_new_face_count = 0;
			m_face_starts.push_back(m_face_counts.back() + m_face_starts.back());
			m_face_counts.push_back(0);
		}
		m_last_polyhedron = Polyhedron;

		const k3d::mesh::counts_t& face_counts = *m_polyhedra.face_counts;
		const k3d::mesh::indices_t& first_faces = *m_polyhedra.first_faces;
		const k3d::mesh::indices_t& face_first_loops = *m_polyhedra.face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *m_polyhedra.face_loop_counts;
		const k3d::mesh::selection_t& face_selection = m_input_face_selection;
		const k3d::mesh::indices_t& loop_first_edges = *m_polyhedra.loop_first_edges;
		const k3d::mesh::indices_t& clockwise_edges = *m_polyhedra.clockwise_edges;

		m_first_new_edges[Face] = m_running_edge_counter;
		m_first_new_loops[Face] = m_running_loop_counter;

		m_running_loop_counter += face_loop_counts[Face];

		if(!face_selection[Face] || face_loop_counts[Face] != 1) // Faces that will not be split
		{
			++(m_face_counts.back());
			++face_count;
			loop_count += face_loop_counts[Face];
			const k3d::uint_t loop_begin = face_first_loops[Face];
			const k3d::uint_t loop_end = loop_begin + face_loop_counts[Face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					++edge_count;
					++m_running_edge_counter;
					if(!m_boundary_edges[edge] && m_has_midpoint[m_companions[edge]])
					{
						++edge_count; // Reserve space for edge split because of adjacent face subdivision
						++m_running_edge_counter;
						++(m_face_edge_counts[Face]);
					}
					edge = clockwise_edges[edge];
					++(m_face_edge_counts[Face]);
					if(edge == first_edge)
						break;
				}
			}
		}
		else // Faces to split
		{
			m_first_new_faces[Face] = m_new_face_count + face_counts[Polyhedron] + m_face_starts.back();
			m_affected_faces.push_back(Face);
			const k3d::uint_t first_edge = loop_first_edges[face_first_loops[Face]];
			for(k3d::uint_t edge = first_edge; ; )
			{
				++(m_face_counts.back());
				++face_count;
				++loop_count;
				edge_count += m_new_edge_count;

				if(m_replacement_face_keeps_edge_count)
				{
					++(m_face_edge_counts[Face]);
					++m_running_edge_counter;
					++edge_count;
				}

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
				++m_new_face_count; // Counts number of edges - 1, so the original face stays in place
			}
			if(!m_replacement_face_keeps_edge_count)
			{
				m_face_edge_counts[Face] += m_new_edge_count;
				m_running_edge_counter += m_new_edge_count;
			}
			face_count += m_extra_face_count;
			m_new_face_count += m_extra_face_count;
			m_face_counts.back() += m_extra_face_count;
			loop_count += m_extra_face_count;
		}
	}

	k3d::uint_t edge_count;
	k3d::uint_t face_count;
	k3d::uint_t loop_count;

private:
	const k3d::mesh::polyhedra_t& m_polyhedra;
	const k3d::mesh::selection_t& m_input_face_selection;
	const k3d::mesh::indices_t& m_companions;
	const k3d::mesh::bools_t& m_boundary_edges;
	const k3d::mesh::bools_t& m_has_midpoint;
	const k3d::mesh::counts_t& m_loops_per_polyhedra;
	const k3d::uint_t m_extra_face_count;
	const k3d::uint_t m_new_edge_count;
	const k3d::bool_t m_replacement_face_keeps_edge_count;
	k3d::mesh::indices_t& m_affected_faces;
	k3d::mesh::indices_t& m_first_new_edges;
	k3d::mesh::indices_t& m_first_new_faces;
	k3d::mesh::indices_t& m_first_new_loops;
	k3d::mesh::counts_t& m_face_edge_counts;
	k3d::mesh::indices_t& m_face_starts;
	k3d::mesh::counts_t& m_face_counts;
	k3d::uint_t m_last_polyhedron;
	k3d::uint_t m_new_face_count;
	k3d::uint_t m_running_loop_counter;
	k3d::uint_t m_running_edge_counter;
};

/// Calculates and stores edge midpoints
class edge_midpoint_calculator
{
public:
	/// Calculate edge midpoints, storing them in Points starting at index StartIndex
	/*
	 * \param EdgeList Indices of the edges (into EdgePoints) of all edges that need to be split. If both an edge and its companion are included, both will have a separate midpoint
	 * \param EdgeMidpoints For each edge, the index of its midpoint (only defined for edges in EdgeList)
	 * \param Offset Offset (in distance units) from the midpoint of the edge to the splitpoint
	*/
	edge_midpoint_calculator(k3d::mesh::points_t& Points,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgeList,
			const k3d::mesh::indices_t& EdgeMidpoints,
			const k3d::double_t Offset = 0.0) :
				m_points(Points),
				m_edge_points(EdgePoints),
				m_clockwise_edges(ClockwiseEdges),
				m_edge_list(EdgeList),
				m_edge_midpoints(EdgeMidpoints),
				m_offset(Offset) {}

	void operator()(const k3d::uint_t EdgeIndex)
	{
		const k3d::uint_t edge = m_edge_list[EdgeIndex];
		const k3d::uint_t clockwise_edge = m_clockwise_edges[edge];
		const k3d::point3& start_point = m_points[m_edge_points[edge]];
		const k3d::point3& end_point = m_points[m_edge_points[clockwise_edge]];
		k3d::point3& mid_point = m_points[m_edge_midpoints[EdgeIndex]];
		if(m_offset)
		{ // If an offset is given, mix the two points using linear interpolation
			double edge_length = k3d::length(start_point - end_point);
			mid_point = k3d::mix(start_point, end_point, 0.5 + m_offset / edge_length);
		}
		else
		{ // Otherwise calculate the real midpoint
			mid_point = (start_point + end_point) * 0.5;
		}
	}

private:
	k3d::mesh::points_t& m_points;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_list;
	const k3d::mesh::indices_t& m_edge_midpoints;
	const k3d::double_t m_offset;
};

/// Calculates face centers
class face_center_calculator
{
public:
	/// Constructs a new functor that will calculate the face centers of all faces listed in FaceList and store them in Points starting at StartIndex
	/*
	 * \param FaceCenters The point index of the center for each face (only defined for faces listed in FaceList)
	 */
	face_center_calculator(
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& EdgePoints,
			const k3d::mesh::indices_t& FaceList,
			const k3d::mesh::indices_t& FaceCenters,
			k3d::mesh::points_t& Points) :
		m_face_first_loops(FaceFirstLoops),
		m_loop_first_edges(LoopFirstEdges),
		m_clockwise_edges(ClockwiseEdges),
		m_edge_points(EdgePoints),
		m_face_list(FaceList),
		m_face_centers(FaceCenters),
		m_points(Points)
	{}


	void operator()(const k3d::uint_t FaceIndex)
	{
		const k3d::uint_t first_edge = m_loop_first_edges[m_face_first_loops[m_face_list[FaceIndex]]];
		k3d::point3& center = m_points[m_face_centers[FaceIndex]];
		k3d::uint_t count = 0;
		for(k3d::uint_t edge = first_edge; ; )
		{
			center += k3d::to_vector(m_points[m_edge_points[edge]]);
			++count;

			edge = m_clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}

		center /= count;
	}

private:
	const k3d::mesh::indices_t& m_face_first_loops;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_edge_points;
	const k3d::mesh::indices_t& m_face_list;
	const k3d::mesh::indices_t& m_face_centers;
	k3d::mesh::points_t& m_points;
};

/// Store in- and output mesh data
struct mesh_arrays
{
	mesh_arrays(const k3d::mesh::indices_t& InputFirstFaces,
			const k3d::mesh::counts_t& InputFaceCounts,
			const k3d::mesh::indices_t& InputEdgePoints,
			const k3d::mesh::indices_t& InputClockwiseEdges,
			const k3d::mesh::indices_t& InputLoopFirstEdges,
			const k3d::mesh::indices_t& InputFaceFirstLoops,
			const k3d::mesh::counts_t& InputFaceLoopCounts,
			const k3d::mesh::materials_t& InputFaceMaterials,
			const k3d::mesh::selection_t& InputFaceSelection,
			const k3d::mesh::indices_t& FirstNewEdges,
			const k3d::mesh::indices_t& FirstNewFaces,
			const k3d::mesh::indices_t& FirstNewLoops,
			const k3d::mesh::indices_t& EdgeMidPoints,
			const k3d::mesh::bools_t& HasMidpoints,
			const k3d::mesh::indices_t& FaceCenters,
			const k3d::mesh::indices_t& Companions,
			const k3d::mesh::bools_t& BoundaryEdges,
			const k3d::mesh::indices_t& OutputFirstFaces,
			const k3d::mesh::counts_t& LoopsPerPolyhedra,
			const k3d::mesh::counts_t& FaceEdgeCounts,
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
				first_new_edges(FirstNewEdges),
				first_new_faces(FirstNewFaces),
				first_new_loops(FirstNewLoops),
				edge_midpoints(EdgeMidPoints),
				has_midpoints(HasMidpoints),
				face_centers(FaceCenters),
				companions(Companions),
				boundary_edges(BoundaryEdges),
				output_first_faces(OutputFirstFaces),
				loops_per_polyhedra(LoopsPerPolyhedra),
				face_edge_counts(FaceEdgeCounts),
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

	/// Copies the face to the correct location in the output
	void copy_face(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_edge = first_new_edges[Face];
		const k3d::uint_t first_new_face = Face + output_first_faces[Polyhedron] - input_first_faces[Polyhedron];
		const k3d::uint_t first_new_loop = first_new_loops[Face];

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
				const k3d::uint_t newedge = first_new_edge + edgenumber + loopedgenumber;
				output_edge_points[newedge] = input_edge_points[edge];
				// Copy varying data.
				varying_copier.copy(edge, newedge);
				output_clockwise_edges[newedge] = newedge + 1;
				if(!boundary_edges[edge] && has_midpoints[companions[edge]])
				{
					const k3d::double_t weights[] = {0.5, 0.5};
					const k3d::uint_t indices[] = {edge, input_clockwise_edges[edge]};
					varying_copier.copy(2, indices, weights, newedge + 1);
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
		uniform_copier.copy(Face, first_new_face);
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
	const k3d::mesh::indices_t& first_new_edges;
	const k3d::mesh::indices_t& first_new_faces;
	const k3d::mesh::indices_t& first_new_loops;
	const k3d::mesh::indices_t& edge_midpoints;
	const k3d::mesh::bools_t& has_midpoints;
	const k3d::mesh::indices_t& face_centers;
	const k3d::mesh::indices_t& companions;
	const k3d::mesh::bools_t& boundary_edges;
	const k3d::mesh::indices_t& output_first_faces;
	const k3d::mesh::counts_t& loops_per_polyhedra;
	const k3d::mesh::counts_t& face_edge_counts;
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

/// Subdivide faces by connecting their centers to the midpoints of the edges.
class mid_to_center_subdivider
{
public:
	mid_to_center_subdivider(mesh_arrays& MeshArrays) : m_mesh_arrays(MeshArrays) {}

	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = m_mesh_arrays.first_new_faces[Face];
		const k3d::uint_t first_new_loop = m_mesh_arrays.first_new_loops[Face];

		if(!m_mesh_arrays.input_face_selection[Face] || m_mesh_arrays.input_face_loop_counts[Face] != 1)
		{ // copy unaffected face, splitting edges adjacent to affected faces
			m_mesh_arrays.copy_face(Polyhedron, Face);
		}
		else
		{ // subdivide affected faces
			// New index of the face replacing the original
			k3d::uint_t newface = Face - m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.output_first_faces[Polyhedron];
			// Keeps track of how many new faces were added
			const k3d::uint_t new_face_number = first_new_face - m_mesh_arrays.output_first_faces[Polyhedron] - m_mesh_arrays.input_face_counts[Polyhedron];
			// Index of the last old face of this polyhedron
			const k3d::uint_t last_old_face = m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.input_face_counts[Polyhedron] - 1;
			const k3d::uint_t face_center = m_mesh_arrays.face_centers[Face];
			const k3d::uint_t first_edge = m_mesh_arrays.input_loop_first_edges[m_mesh_arrays.input_face_first_loops[Face]];
			k3d::uint_t edgenumber = 0;
			k3d::uint_t first_new_edge = m_mesh_arrays.first_new_edges[Face];
			m_mesh_arrays.output_loop_first_edges[first_new_loop] = first_new_edge;
			m_mesh_arrays.output_face_first_loops[newface] = first_new_loop;
			m_mesh_arrays.output_face_materials[newface] = m_mesh_arrays.input_face_materials[Face];
			m_mesh_arrays.output_face_selection[newface] = m_mesh_arrays.input_face_selection[Face];

			// Store all corner indices for facevarying data copy
			k3d::mesh::indices_t face_indices;
			for(k3d::uint_t edge = first_edge; ; )
			{
				face_indices.push_back(edge);
				edge = m_mesh_arrays.input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}

			const k3d::uint_t num_edges = face_indices.size();
			k3d::mesh::weights_t face_weights(num_edges, 1.0/static_cast<k3d::double_t>(num_edges));

			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t clockwise = m_mesh_arrays.input_clockwise_edges[edge];
				const k3d::uint_t mid1 = m_mesh_arrays.edge_midpoints[edge];
				const k3d::uint_t mid2 = m_mesh_arrays.edge_midpoints[clockwise];
				const k3d::uint_t mid1_to_corner = first_new_edge;
				const k3d::uint_t corner_to_mid2 = mid1_to_corner + 1;
				const k3d::uint_t mid2_to_center = corner_to_mid2 + 1;
				const k3d::uint_t center_to_mid1 = mid2_to_center + 1;
				m_mesh_arrays.output_edge_points[mid1_to_corner] = mid1;
				m_mesh_arrays.output_edge_points[corner_to_mid2] = m_mesh_arrays.input_edge_points[clockwise];
				m_mesh_arrays.output_edge_points[mid2_to_center] = mid2;
				m_mesh_arrays.output_edge_points[center_to_mid1] = face_center;
				m_mesh_arrays.output_clockwise_edges[mid1_to_corner] = corner_to_mid2;
				m_mesh_arrays.output_clockwise_edges[corner_to_mid2] = mid2_to_center;
				m_mesh_arrays.output_clockwise_edges[mid2_to_center] = center_to_mid1;
				m_mesh_arrays.output_clockwise_edges[center_to_mid1] = mid1_to_corner;
				const k3d::double_t weights[] = {0.5, 0.5};
				const k3d::uint_t mid1_to_corner_indices[] = {edge, clockwise};
				m_mesh_arrays.varying_copier.copy(2, mid1_to_corner_indices, weights, mid1_to_corner);
				m_mesh_arrays.varying_copier.copy(clockwise, corner_to_mid2);
				const k3d::uint_t mid2_to_center_indices[] = {clockwise, m_mesh_arrays.input_clockwise_edges[clockwise]};
				m_mesh_arrays.varying_copier.copy(2, mid2_to_center_indices, weights, mid2_to_center);
				m_mesh_arrays.varying_copier.copy(num_edges, &face_indices[0], &face_weights[0], center_to_mid1);


				// Append face data
				if(edge != first_edge)
				{
					const k3d::uint_t newloop = m_mesh_arrays.first_new_loops[m_mesh_arrays.input_first_faces[Polyhedron]] + m_mesh_arrays.loops_per_polyhedra[Polyhedron] + new_face_number + edgenumber - 1;
					m_mesh_arrays.output_loop_first_edges[newloop] = mid1_to_corner;
					m_mesh_arrays.output_face_first_loops[first_new_face + edgenumber - 1] = newloop;
					m_mesh_arrays.output_face_materials[first_new_face + edgenumber - 1] = m_mesh_arrays.input_face_materials[Face];
					m_mesh_arrays.output_face_selection[first_new_face + edgenumber - 1] = m_mesh_arrays.input_face_selection[Face];
				}
				m_mesh_arrays.uniform_copier.copy(Face, first_new_face + edgenumber - 1);

				first_new_edge = m_mesh_arrays.first_new_edges[last_old_face] + m_mesh_arrays.face_edge_counts[last_old_face] + 4 * (edgenumber + new_face_number);

				edge = m_mesh_arrays.input_clockwise_edges[edge];
				++edgenumber;
				if(edge == first_edge)
					break;
			}
		}
	}

private:
	mesh_arrays& m_mesh_arrays;
};

/// Subdivide faces by connecting the midpoints of their edges contiguously
class mid_contiguous_subdivider
{
public:
	mid_contiguous_subdivider(mesh_arrays& MeshArrays) : m_mesh_arrays(MeshArrays) {}

	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = m_mesh_arrays.first_new_faces[Face];
		const k3d::uint_t first_new_loop = m_mesh_arrays.first_new_loops[Face];

		if(!m_mesh_arrays.input_face_selection[Face] || m_mesh_arrays.input_face_loop_counts[Face] != 1)
		{ // copy unaffected face, splitting edges adjacent to affected faces
			m_mesh_arrays.copy_face(Polyhedron, Face);
		}
		else
		{ // subdivide affected faces
			// New index of the face replacing the original
			k3d::uint_t newface = Face - m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.output_first_faces[Polyhedron];
			// Keeps track of how many new faces were added
			const k3d::uint_t new_face_number = first_new_face - m_mesh_arrays.output_first_faces[Polyhedron] - m_mesh_arrays.input_face_counts[Polyhedron];
			// Index of the last old face of this polyhedron
			const k3d::uint_t last_old_face = m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.input_face_counts[Polyhedron] - 1;
			const k3d::uint_t first_edge = m_mesh_arrays.input_loop_first_edges[m_mesh_arrays.input_face_first_loops[Face]];
			k3d::uint_t edgenumber = 0;
			k3d::uint_t first_new_edge = m_mesh_arrays.first_new_edges[Face];
			const k3d::uint_t center_first_edge = first_new_edge; // First edge of the center face
			m_mesh_arrays.output_loop_first_edges[first_new_loop] = center_first_edge;
			m_mesh_arrays.output_face_first_loops[newface] = first_new_loop;
			m_mesh_arrays.output_face_materials[newface] = m_mesh_arrays.input_face_materials[Face];
			m_mesh_arrays.output_face_selection[newface] = m_mesh_arrays.input_face_selection[Face];
			m_mesh_arrays.uniform_copier.copy(Face, newface);
			for(k3d::uint_t edge = first_edge; ; )
			{
				first_new_edge = m_mesh_arrays.first_new_edges[last_old_face] + m_mesh_arrays.face_edge_counts[last_old_face] + 3 * (edgenumber + new_face_number);
				const k3d::uint_t clockwise = m_mesh_arrays.input_clockwise_edges[edge];
				const k3d::uint_t mid1 = m_mesh_arrays.edge_midpoints[edge];
				const k3d::uint_t mid2 = m_mesh_arrays.edge_midpoints[clockwise];
				const k3d::uint_t mid1_to_corner = first_new_edge;
				const k3d::uint_t corner_to_mid2 = mid1_to_corner + 1;
				const k3d::uint_t mid2_to_mid1 = corner_to_mid2 + 1;
				m_mesh_arrays.output_edge_points[center_first_edge + edgenumber] = mid1;
				m_mesh_arrays.output_edge_points[mid1_to_corner] = mid1;
				m_mesh_arrays.output_edge_points[corner_to_mid2] = m_mesh_arrays.input_edge_points[clockwise];
				m_mesh_arrays.output_edge_points[mid2_to_mid1] = mid2;
				m_mesh_arrays.output_clockwise_edges[center_first_edge + edgenumber] = center_first_edge + edgenumber + 1;
				m_mesh_arrays.output_clockwise_edges[mid1_to_corner] = corner_to_mid2;
				m_mesh_arrays.output_clockwise_edges[corner_to_mid2] = mid2_to_mid1;
				m_mesh_arrays.output_clockwise_edges[mid2_to_mid1] = mid1_to_corner;

				const k3d::double_t weights[] = {0.5, 0.5};
				const k3d::uint_t mid1_indices[] = {edge, clockwise};
				const k3d::uint_t mid2_indices[] = {clockwise, m_mesh_arrays.input_clockwise_edges[clockwise]};
				m_mesh_arrays.varying_copier.copy(2, mid1_indices, weights, center_first_edge + edgenumber);
				m_mesh_arrays.varying_copier.copy(2, mid1_indices, weights, mid1_to_corner);
				m_mesh_arrays.varying_copier.copy(clockwise, corner_to_mid2);
				m_mesh_arrays.varying_copier.copy(2, mid2_indices, weights, mid2_to_mid1);

				// Append face data
				const k3d::uint_t newloop = m_mesh_arrays.first_new_loops[m_mesh_arrays.input_first_faces[Polyhedron]] + m_mesh_arrays.loops_per_polyhedra[Polyhedron] + new_face_number + edgenumber;
				m_mesh_arrays.output_loop_first_edges[newloop] = mid1_to_corner;
				m_mesh_arrays.output_face_first_loops[first_new_face + edgenumber] = newloop;
				m_mesh_arrays.output_face_materials[first_new_face + edgenumber] = m_mesh_arrays.input_face_materials[Face];
				m_mesh_arrays.output_face_selection[first_new_face + edgenumber] = m_mesh_arrays.input_face_selection[Face];
				m_mesh_arrays.uniform_copier.copy(Face, first_new_face + edgenumber);

				edge = m_mesh_arrays.input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
				++edgenumber;
			}
			// Close central loop
			m_mesh_arrays.output_clockwise_edges[center_first_edge + edgenumber] = center_first_edge;
		}
	}

private:
	mesh_arrays& m_mesh_arrays;
};

/// Subdivide faces by connecting their centers to the face corners
class center_to_corner_subdivider
{
public:
	center_to_corner_subdivider(mesh_arrays& MeshArrays) : m_mesh_arrays(MeshArrays) {}

	void operator()(const k3d::uint_t Polyhedron, const k3d::uint_t Face)
	{
		const k3d::uint_t first_new_face = m_mesh_arrays.first_new_faces[Face];
		const k3d::uint_t first_new_loop = m_mesh_arrays.first_new_loops[Face];

		if(!m_mesh_arrays.input_face_selection[Face] || m_mesh_arrays.input_face_loop_counts[Face] != 1)
		{ // copy unaffected face, splitting edges adjacent to affected faces
			m_mesh_arrays.copy_face(Polyhedron, Face);
		}
		else
		{ // subdivide affected faces
			// New index of the face replacing the original
			k3d::uint_t newface = Face - m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.output_first_faces[Polyhedron];
			// Keeps track of how many new faces were added
			const k3d::uint_t new_face_number = first_new_face - m_mesh_arrays.output_first_faces[Polyhedron] - m_mesh_arrays.input_face_counts[Polyhedron];
			// Index of the last old face of this polyhedron
			const k3d::uint_t last_old_face = m_mesh_arrays.input_first_faces[Polyhedron] + m_mesh_arrays.input_face_counts[Polyhedron] - 1;
			const k3d::uint_t face_center = m_mesh_arrays.face_centers[Face];
			const k3d::uint_t first_edge = m_mesh_arrays.input_loop_first_edges[m_mesh_arrays.input_face_first_loops[Face]];
			k3d::uint_t edgenumber = 0;
			k3d::uint_t first_new_edge = m_mesh_arrays.first_new_edges[Face];
			m_mesh_arrays.output_loop_first_edges[first_new_loop] = first_new_edge;
			m_mesh_arrays.output_face_first_loops[newface] = first_new_loop;
			m_mesh_arrays.output_face_materials[newface] = m_mesh_arrays.input_face_materials[Face];
			m_mesh_arrays.output_face_selection[newface] = m_mesh_arrays.input_face_selection[Face];

			// Store all corner indices for facevarying data copy
			k3d::mesh::indices_t face_indices;
			for(k3d::uint_t edge = first_edge; ; )
			{
				face_indices.push_back(edge);
				edge = m_mesh_arrays.input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}

			const k3d::uint_t num_edges = face_indices.size();
			k3d::mesh::weights_t face_weights(num_edges, 1.0/static_cast<k3d::double_t>(num_edges));

			for(k3d::uint_t edge = first_edge; ; )
			{
				const k3d::uint_t clockwise = m_mesh_arrays.input_clockwise_edges[edge];
				const k3d::uint_t corner1_to_corner2 = first_new_edge;
				const k3d::uint_t corner2_to_center = corner1_to_corner2 + 1;
				const k3d::uint_t center_to_corner1 = corner2_to_center + 1;
				m_mesh_arrays.output_edge_points[corner1_to_corner2] = m_mesh_arrays.input_edge_points[edge];
				m_mesh_arrays.output_edge_points[corner2_to_center] = m_mesh_arrays.input_edge_points[clockwise];
				m_mesh_arrays.output_edge_points[center_to_corner1] = face_center;
				m_mesh_arrays.output_clockwise_edges[corner1_to_corner2] = corner2_to_center;
				m_mesh_arrays.output_clockwise_edges[corner2_to_center] = center_to_corner1;
				m_mesh_arrays.output_clockwise_edges[center_to_corner1] = corner1_to_corner2;

				m_mesh_arrays.varying_copier.copy(edge, corner1_to_corner2);
				m_mesh_arrays.varying_copier.copy(clockwise, corner2_to_center);
				m_mesh_arrays.varying_copier.copy(num_edges, &face_indices[0], &face_weights[0], center_to_corner1);

				// Append face data
				if(edge != first_edge)
				{
					const k3d::uint_t newloop = m_mesh_arrays.first_new_loops[m_mesh_arrays.input_first_faces[Polyhedron]] + m_mesh_arrays.loops_per_polyhedra[Polyhedron] + new_face_number + edgenumber - 1;
					m_mesh_arrays.output_loop_first_edges[newloop] = corner1_to_corner2;
					m_mesh_arrays.output_face_first_loops[first_new_face + edgenumber - 1] = newloop;
					m_mesh_arrays.output_face_materials[first_new_face + edgenumber - 1] = m_mesh_arrays.input_face_materials[Face];
					m_mesh_arrays.output_face_selection[first_new_face + edgenumber - 1] = m_mesh_arrays.input_face_selection[Face];
				}
				m_mesh_arrays.uniform_copier.copy(Face, first_new_face + edgenumber - 1);

				first_new_edge = m_mesh_arrays.first_new_edges[last_old_face] + m_mesh_arrays.face_edge_counts[last_old_face] + 3 * (edgenumber + new_face_number);

				edge = m_mesh_arrays.input_clockwise_edges[edge];
				++edgenumber;
				if(edge == first_edge)
					break;
			}
		}
	}

private:
	mesh_arrays& m_mesh_arrays;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces

class subdivide_faces :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	subdivide_faces(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivision_type(init_owner(*this) + init_name("subdivision_type") + init_label(_("Subdivision type")) + init_description(_("Join : center to midpoints, midpoints, center to points")) + init_value(CENTERTOMIDPOINTS) + init_enumeration(subdivision_values())),
		m_middle_offset(init_owner(*this) + init_name("middle_offset") + init_label(_("Middle offset")) + init_description(_("Middle offset, for midpoints only")) + init_value(0.0) + init_step_increment(0.05) + init_units(typeid(k3d::measurement::distance)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_subdivision_type.changed_signal().connect(make_reset_mesh_slot());

		m_middle_offset.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input));
		if(!input_polyhedron)
			return;

		subdivision_t subdivision_type = m_subdivision_type.pipeline_value();
		// Shallow copy of the input (no data is copied, only shared pointers are)
		document().pipeline_profiler().start_execution(*this, "Merge selection");
		Output = Input;
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh
		const k3d::mesh::selection_t input_face_selection = *Output.polyhedra->face_selection; // copy this, so we can keep using it
		document().pipeline_profiler().finish_execution(*this, "Merge selection");

		// Make writeable copies of the arrays we intend to modify
		document().pipeline_profiler().start_execution(*this, "Copy input");
		k3d::mesh::points_t& output_points = Output.points.writable();
		k3d::mesh::selection_t& output_point_selection = Output.point_selection.writable();
		k3d::mesh::polyhedra_t& output_polyhedra = Output.polyhedra.writable();
		k3d::mesh::polyhedra_t::types_t& output_types = output_polyhedra.types.writable();
		k3d::mesh::indices_t& output_loop_first_edges = output_polyhedra.loop_first_edges.writable();
		k3d::mesh::indices_t& output_edge_points = output_polyhedra.edge_points.writable();
		k3d::mesh::indices_t& output_clockwise_edges = output_polyhedra.clockwise_edges.writable();
		k3d::mesh::selection_t& output_edge_selection = output_polyhedra.edge_selection.writable();

		// Copy the unaffected constant data
		output_polyhedra.constant_data = Input.polyhedra->constant_data;
		// Create copiers for the uniform and varying data
		output_polyhedra.uniform_data = Input.polyhedra->uniform_data.clone_types();
		k3d::attribute_array_copier uniform_data_copier(Input.polyhedra->uniform_data, output_polyhedra.uniform_data);
		output_polyhedra.face_varying_data = Input.polyhedra->face_varying_data.clone_types();
		k3d::attribute_array_copier face_varying_data_copier(Input.polyhedra->face_varying_data, output_polyhedra.face_varying_data);

		// Face-related arrays can not be appended to because of the possibility of multiple polyhedra,
		// so we will rebuild them from scratch in the new order
		const k3d::mesh::indices_t& input_first_faces = *Input.polyhedra->first_faces;
		const k3d::mesh::counts_t& input_face_counts = *Input.polyhedra->face_counts;
		const k3d::mesh::indices_t& input_face_first_loops = *Input.polyhedra->face_first_loops;
		const k3d::mesh::indices_t& input_loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& input_clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::mesh::counts_t& input_face_loop_counts = *Input.polyhedra->face_loop_counts;
		const k3d::mesh::materials_t& input_face_materials = *Input.polyhedra->face_materials;
		k3d::mesh::indices_t& output_first_faces = output_polyhedra.first_faces.create();
		k3d::mesh::counts_t& output_face_counts = output_polyhedra.face_counts.create();
		k3d::mesh::indices_t& output_face_first_loops = output_polyhedra.face_first_loops.create();
		k3d::mesh::counts_t& output_face_loop_counts = output_polyhedra.face_loop_counts.create();
		k3d::mesh::selection_t& output_face_selection = output_polyhedra.face_selection.create();
		k3d::mesh::materials_t& output_face_materials = output_polyhedra.face_materials.create();

		m_affected_edges.clear();
		m_edge_midpoints.clear();
		m_affected_faces.clear();
		m_face_centers.clear();
		document().pipeline_profiler().finish_execution(*this, "Copy input");


		// Get the "companion" edge for each edge
		document().pipeline_profiler().start_execution(*this, "Calculate companions");
		k3d::mesh::indices_t companions;
		k3d::mesh::bools_t boundary_edges;
		if(subdivision_type == CENTERTOMIDPOINTS || subdivision_type == CONTIGUOUSMIDPOINTS)
		{
			k3d::polyhedron::create_edge_adjacency_lookup(output_edge_points, output_clockwise_edges, boundary_edges, companions);
		}
		else
		{ // We don't need companions if no midpoints are being calculated
			companions.resize(output_edge_points.size());
			boundary_edges.resize(output_edge_points.size(), true);
		}
		document().pipeline_profiler().finish_execution(*this, "Calculate companions");

		document().pipeline_profiler().start_execution(*this, "Count components");
		k3d::mesh::indices_t edge_midpoints(output_edge_points.size());
		k3d::mesh::bools_t has_midpoint(output_edge_points.size(), false);
		k3d::mesh::indices_t face_centers(input_face_first_loops.size());
		// Get the indices of the midpoints, the number of midpoints to calculate and the indices of face centers, if they are needed
		detail::midpoint_index_calculator midpoint_index_calculator(*Input.polyhedra,
				input_face_selection,
				companions,
				boundary_edges,
				subdivision_type != CENTERTOPOINTS,
				subdivision_type != CONTIGUOUSMIDPOINTS,
				m_affected_edges,
				edge_midpoints,
				has_midpoint,
				face_centers,
				output_points.size());
		for (k3d::uint_t face = 0; face != input_face_first_loops.size(); ++face)
		{
			midpoint_index_calculator(face);
		}

		const k3d::uint_t extra_face_count = subdivision_type == CONTIGUOUSMIDPOINTS ? 1 : 0;
		k3d::uint_t new_edge_count = 3;
		if(subdivision_type == CENTERTOMIDPOINTS)
			new_edge_count = 4;
		k3d::mesh::indices_t first_new_edges(input_face_first_loops.size());
		k3d::mesh::indices_t first_new_faces(input_face_first_loops.size());
		k3d::mesh::indices_t first_new_loops(input_face_first_loops.size(), 0); // We rely on this being inited to 0 in face_edge_counter!
		k3d::mesh::counts_t face_edge_counts(input_face_first_loops.size(), 0);
		// Count loops per polyhedra first
		k3d::mesh::counts_t loops_per_polyhedra(input_first_faces.size(), 0);
		for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
		{
			k3d::uint_t face_start = input_first_faces[polyhedron];
			k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
			for (k3d::uint_t face = face_start; face != face_end; ++face)
			{
				loops_per_polyhedra[polyhedron] += input_face_loop_counts[face];
			}
		}
		// Count the rest of the geometry
		detail::face_edge_counter face_edge_counter(
				*Input.polyhedra,
				input_face_selection,
				companions,
				boundary_edges,
				has_midpoint,
				loops_per_polyhedra,
				extra_face_count,
				new_edge_count,
				subdivision_type == CONTIGUOUSMIDPOINTS,
				m_affected_faces,
				first_new_edges,
				first_new_faces,
				first_new_loops,
				face_edge_counts,
				output_first_faces,
				output_face_counts);
		for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
		{
			k3d::uint_t face_start = input_first_faces[polyhedron];
			k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
			for (k3d::uint_t face = face_start; face != face_end; ++face)
			{
				face_edge_counter(polyhedron, face);
			}
		}

		// Store midpoint and face center indices, so they are accessible in the update step
		if(subdivision_type != CENTERTOPOINTS)
		{
			m_edge_midpoints.reserve(m_affected_edges.size());
			for(k3d::uint_t edge_index = 0; edge_index != m_affected_edges.size(); ++edge_index)
				m_edge_midpoints.push_back(edge_midpoints[m_affected_edges[edge_index]]);
		}
		if(subdivision_type != CONTIGUOUSMIDPOINTS)
		{
			m_face_centers.reserve(m_affected_faces.size());
			for(k3d::uint_t face_index = 0; face_index != m_affected_faces.size(); ++face_index)
				m_face_centers.push_back(face_centers[m_affected_faces[face_index]]);
		}
		document().pipeline_profiler().finish_execution(*this, "Count components");

		// Allocate required memory
		document().pipeline_profiler().start_execution(*this, "Allocate memory");
		const k3d::uint_t center_count = subdivision_type == CONTIGUOUSMIDPOINTS ? 0 : m_affected_faces.size();
		const k3d::uint_t midpoint_count = subdivision_type == CENTERTOPOINTS ? 0 : m_affected_edges.size();
		output_points.resize(output_points.size() + center_count + midpoint_count, k3d::point3(0,0,0));
		output_edge_points.resize(face_edge_counter.edge_count, 0);
		output_clockwise_edges.resize(face_edge_counter.edge_count, 0);
		output_loop_first_edges.resize(face_edge_counter.loop_count);
		output_face_first_loops.resize(face_edge_counter.face_count);
		output_face_loop_counts.resize(face_edge_counter.face_count, 1);
		output_face_selection.resize(face_edge_counter.face_count, 0.0);
		output_face_materials.resize(face_edge_counter.face_count);
		output_polyhedra.face_varying_data.resize(face_edge_counter.edge_count);
		output_polyhedra.uniform_data.resize(face_edge_counter.face_count);
		document().pipeline_profiler().finish_execution(*this, "Allocate memory");

		detail::mesh_arrays mesh_arrays(
						input_first_faces,
						input_face_counts,
						*Input.polyhedra->edge_points,
						*Input.polyhedra->clockwise_edges,
						*Input.polyhedra->loop_first_edges,
						input_face_first_loops,
						*Input.polyhedra->face_loop_counts,
						input_face_materials,
						input_face_selection,
						first_new_edges,
						first_new_faces,
						first_new_loops,
						edge_midpoints,
						has_midpoint,
						face_centers,
						companions,
						boundary_edges,
						output_first_faces,
						loops_per_polyhedra,
						face_edge_counts,
						output_edge_points,
						output_clockwise_edges,
						output_loop_first_edges,
						output_face_first_loops,
						output_face_loop_counts,
						output_face_materials,
						output_face_selection,
						uniform_data_copier,
						face_varying_data_copier);

		if(subdivision_type == CENTERTOMIDPOINTS)
		{
			// Connect face centers to edge midpoints
			for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
			{
				const k3d::uint_t face_start = input_first_faces[polyhedron];
				const k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
				detail::mid_to_center_subdivider subdivider(mesh_arrays);
				for(k3d::uint_t face = face_start; face != face_end; ++face)
				{
					subdivider(polyhedron, face);
				}
			}
		}

		if(subdivision_type == CONTIGUOUSMIDPOINTS)
		{
			// Connect face centers to edge midpoints
			for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
			{
				const k3d::uint_t face_start = input_first_faces[polyhedron];
				const k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
				detail::mid_contiguous_subdivider subdivider(mesh_arrays);
				for(k3d::uint_t face = face_start; face != face_end; ++face)
				{
					subdivider(polyhedron, face);
				}
			}
		}

		if(subdivision_type == CENTERTOPOINTS)
		{
			// Connect face centers to edge midpoints
			for(k3d::uint_t polyhedron = 0; polyhedron != input_first_faces.size(); ++polyhedron)
			{
				const k3d::uint_t face_start = input_first_faces[polyhedron];
				const k3d::uint_t face_end = face_start + input_face_counts[polyhedron];
				detail::center_to_corner_subdivider subdivider(mesh_arrays);
				for(k3d::uint_t face = face_start; face != face_end; ++face)
				{
					subdivider(polyhedron, face);
				}
			}
		}

		// Update selection arrays
		output_edge_selection.assign(output_edge_points.size(), 0.0);
		output_point_selection.resize(output_points.size(), 0.0);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input));
		if(!input_polyhedron)
			return;

		subdivision_t subdivision_type = m_subdivision_type.pipeline_value();

		const k3d::mesh::indices_t& input_face_first_loops = *Input.polyhedra->face_first_loops;
		const k3d::mesh::indices_t& input_loop_first_edges = *Input.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& input_clockwise_edges = *Input.polyhedra->clockwise_edges;
		const k3d::mesh::indices_t& input_edge_points = *Input.polyhedra->edge_points;

		k3d::mesh::points_t& output_points = Output.points.writable();

		if(subdivision_type != CONTIGUOUSMIDPOINTS)
		{
			return_if_fail(m_affected_faces.size() == m_face_centers.size());
			// Calculate face centers
			document().pipeline_profiler().start_execution(*this, "Face centers");
			detail::face_center_calculator face_center_functor(input_face_first_loops,
					input_loop_first_edges,
					input_clockwise_edges,
					input_edge_points,
					m_affected_faces,
					m_face_centers,
					output_points);
			for(k3d::uint_t face_index = 0; face_index != m_affected_faces.size(); ++face_index) face_center_functor(face_index);
			document().pipeline_profiler().finish_execution(*this, "Face centers");
		}

		if(subdivision_type != CENTERTOPOINTS)
		{
			return_if_fail(m_affected_edges.size() == m_edge_midpoints.size());
			// Calculate edge midpoints
			document().pipeline_profiler().start_execution(*this, "Edge midpoints");
			detail::edge_midpoint_calculator midpoint_functor(output_points,
					input_edge_points,
					input_clockwise_edges,
					m_affected_edges,
					m_edge_midpoints,
					m_middle_offset.pipeline_value());
			for(k3d::uint_t edge_index = 0; edge_index != m_affected_edges.size(); ++edge_index) midpoint_functor(edge_index);
			document().pipeline_profiler().finish_execution(*this, "Edge midpoints");
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<subdivide_faces,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xb9d4d815, 0x241c473d, 0xa22d3523, 0x406fa390),
				"SubdivideFaces",
				"Subdivide faces by joining face center with vertices or edge midpoints, or contiguous midpoints",
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		CENTERTOPOINTS,
		CENTERTOMIDPOINTS,
		CONTIGUOUSMIDPOINTS,
	} subdivision_t;

	friend std::ostream& operator << (std::ostream& Stream, const subdivision_t& Value)
	{
		switch(Value)
		{
			case CENTERTOPOINTS:
				Stream << "center";
				break;
			case CENTERTOMIDPOINTS:
				Stream << "centermidpoints";
				break;
			case CONTIGUOUSMIDPOINTS:
				Stream << "midpoints";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, subdivision_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "center")
			Value = CENTERTOPOINTS;
		else if(text == "centermidpoints")
			Value = CENTERTOMIDPOINTS;
		else if(text == "midpoints")
			Value = CONTIGUOUSMIDPOINTS;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& subdivision_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Center", "center", "Joins each face vertex to face center; fun fact: produces triangles only"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("CenterMidpoints", "centermidpoints", "Joins face center to each edge middle; innovative info: produces quads only"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Midpoints", "midpoints", "Joins contiguous edge midpoints; amusing anecdote: produces one triangle per original vertex, plus a polygon in the middle with as many edges as the original"));
		}

		return values;
	}

private:
	k3d_data(subdivision_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_subdivision_type;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_middle_offset;

	// Indices of the edges for which to calculate the midpoints, and the corresponding midpoint indices
	k3d::mesh::indices_t m_affected_edges;
	k3d::mesh::indices_t m_edge_midpoints;
	// Indices of the faces for which to calculate centers, and the corresponding face center point indices
	k3d::mesh::indices_t m_affected_faces;
	k3d::mesh::indices_t m_face_centers;
};

/////////////////////////////////////////////////////////////////////////////
// subdivide_faces_factory

k3d::iplugin_factory& subdivide_faces_factory()
{
	return subdivide_faces::get_factory();
}

} // namespace mesh

} // namespace module

