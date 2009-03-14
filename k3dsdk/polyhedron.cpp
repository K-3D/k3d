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

/** \file Declares structures to add extra "convenience" data to a mesh
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "parallel/blocked_range.h"
#include "parallel/parallel_for.h"
#include "parallel/threads.h"
#include "polyhedron.h"

#include <functional>
#include <iterator>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace k3d
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& FirstFaces,
	const mesh::counts_t& FaceCounts,
//	const typed_array<int32_t>& PolyhedronTypes,
	const mesh::polyhedra_t::types_t& PolyhedronTypes,
	const mesh::indices_t& FaceFirstLoops,
	const mesh::counts_t& FaceLoopCounts,
	const mesh::selection_t& FaceSelections,
	const mesh::materials_t& FaceMaterials,
	const mesh::indices_t& LoopFirstEdges,
	const mesh::indices_t& EdgePoints,
	const mesh::indices_t& ClockwiseEdges,
	const mesh::selection_t& EdgeSelections,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& FaceVaryingData
		) :
	first_faces(FirstFaces),
	face_counts(FaceCounts),
	polyhedron_types(PolyhedronTypes),
	face_first_loops(FaceFirstLoops),
	face_loop_counts(FaceLoopCounts),
	face_selections(FaceSelections),
	face_materials(FaceMaterials),
	loop_first_edges(LoopFirstEdges),
	edge_points(EdgePoints),
	clockwise_edges(ClockwiseEdges),
	edge_selections(EdgeSelections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	face_varying_data(FaceVaryingData)
{
}

const_primitive::const_primitive(const primitive& Primitive) :
	first_faces(Primitive.first_faces),
	face_counts(Primitive.face_counts),
	polyhedron_types(Primitive.polyhedron_types),
	face_first_loops(Primitive.face_first_loops),
	face_loop_counts(Primitive.face_loop_counts),
	face_selections(Primitive.face_selections),
	face_materials(Primitive.face_materials),
	loop_first_edges(Primitive.loop_first_edges),
	edge_points(Primitive.edge_points),
	clockwise_edges(Primitive.clockwise_edges),
	edge_selections(Primitive.edge_selections),
	constant_data(Primitive.constant_data),
	uniform_data(Primitive.uniform_data),
	face_varying_data(Primitive.face_varying_data)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::indices_t& FirstFaces,
	mesh::counts_t& FaceCounts,
//	typed_array<int32_t>& PolyhedronTypes,
	mesh::polyhedra_t::types_t& PolyhedronTypes,
	mesh::indices_t& FaceFirstLoops,
	mesh::counts_t& FaceLoopCounts,
	mesh::selection_t& FaceSelections,
	mesh::materials_t& FaceMaterials,
	mesh::indices_t& LoopFirstEdges,
	mesh::indices_t& EdgePoints,
	mesh::indices_t& ClockwiseEdges,
	mesh::selection_t& EdgeSelections,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& FaceVaryingData
		) :
	first_faces(FirstFaces),
	face_counts(FaceCounts),
	polyhedron_types(PolyhedronTypes),
	face_first_loops(FaceFirstLoops),
	face_loop_counts(FaceLoopCounts),
	face_selections(FaceSelections),
	face_materials(FaceMaterials),
	loop_first_edges(LoopFirstEdges),
	edge_points(EdgePoints),
	clockwise_edges(ClockwiseEdges),
	edge_selections(EdgeSelections),
	constant_data(ConstantData),
	uniform_data(UniformData),
	face_varying_data(FaceVaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::polyhedra_t& polyhedron = Mesh.polyhedra.create();

	primitive* const result = new primitive(
		polyhedron.first_faces.create(),
		polyhedron.face_counts.create(),
		polyhedron.types.create(),
		polyhedron.face_first_loops.create(),
		polyhedron.face_loop_counts.create(),
		polyhedron.face_selection.create(),
		polyhedron.face_materials.create(),
		polyhedron.loop_first_edges.create(),
		polyhedron.edge_points.create(),
		polyhedron.clockwise_edges.create(),
		polyhedron.edge_selection.create(),
		polyhedron.constant_data,
		polyhedron.uniform_data,
		polyhedron.face_varying_data
		);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh, const mesh::points_t& Vertices, const mesh::counts_t& VertexCounts, const mesh::indices_t& VertexIndices, imaterial* const Material)
{
	try
	{
		if(std::count_if(VertexCounts.begin(), VertexCounts.end(), std::bind2nd(std::less<uint_t>(), 3)))
		{
			throw std::runtime_error("each face must have three-or-more vertices.");
		}

		const uint_t expected_indices = std::accumulate(VertexCounts.begin(), VertexCounts.end(), 0);
		if(VertexIndices.size() != expected_indices)
		{
			std::ostringstream buffer;
			buffer << "expected [" << expected_indices << "] vertex indices, received [" << VertexIndices.size() << "]";
			throw std::runtime_error(buffer.str());
		}

		if(std::count_if(VertexIndices.begin(), VertexIndices.end(), std::bind2nd(std::greater_equal<uint_t>(), Vertices.size())))
		{
			throw std::runtime_error("vertex indices out-of-bounds");
		}

		// Append points to the mesh (create the arrays if they don't already exist) ...
		mesh::points_t& points = Mesh.points ? Mesh.points.writable() : Mesh.points.create();
		mesh::selection_t& point_selection = Mesh.point_selection ? Mesh.point_selection.writable() : Mesh.point_selection.create();

		const uint_t point_offset = points.size();
		points.insert(points.end(), Vertices.begin(), Vertices.end());
		point_selection.insert(point_selection.end(), Vertices.size(), 0.0);
		Mesh.vertex_data.resize(points.size());

		// Append a new polyhedron to the mesh ...
		primitive* const polyhedron = create(Mesh);
		polyhedron->first_faces.push_back(0);
		polyhedron->face_counts.push_back(VertexCounts.size());
		polyhedron->polyhedron_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);

		uint_t face_vertex = 0;
		const uint_t face_begin = 0;
		const uint_t face_end = face_begin + VertexCounts.size();
		for(uint_t face = face_begin; face != face_end; ++face)
		{
			polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
			polyhedron->face_loop_counts.push_back(1);
			polyhedron->face_selections.push_back(0.0);
			polyhedron->face_materials.push_back(Material);
			polyhedron->loop_first_edges.push_back(polyhedron->edge_points.size());

			const uint_t vertex_begin = 0;
			const uint_t vertex_end = vertex_begin + VertexCounts[face];
			const uint_t loop_begin = polyhedron->edge_points.size();
			for(uint_t vertex = vertex_begin; vertex != vertex_end; ++vertex, ++face_vertex)
			{
				polyhedron->edge_points.push_back(point_offset + VertexIndices[face_vertex]);
				polyhedron->clockwise_edges.push_back(polyhedron->edge_points.size());
				polyhedron->edge_selections.push_back(0.0);
			}
			polyhedron->clockwise_edges.back() = loop_begin;
		}

		return polyhedron;
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_grid

primitive* create_grid(mesh& Mesh, const uint_t Rows, const uint_t Columns, imaterial* const Material)
{
	try
	{
		if(!Rows)
			throw std::runtime_error("Cannot create grid with zero rows.");

		if(!Columns)
			throw std::runtime_error("Cannot create grid with zero columns.");

		const uint_t rows = Rows;
		const uint_t columns = Columns;
		const uint_t point_rows = rows + 1;
		const uint_t point_columns = columns + 1;

		// Append points to the mesh (create the arrays if they don't already exist) ...
		mesh::points_t& points = Mesh.points ? Mesh.points.writable() : Mesh.points.create();
		mesh::selection_t& point_selection = Mesh.point_selection ? Mesh.point_selection.writable() : Mesh.point_selection.create();

		const uint_t point_offset = points.size();
		points.insert(points.end(), point_rows * point_columns, point3());
		point_selection.insert(point_selection.end(), point_rows * point_columns, 0.0);
		Mesh.vertex_data.resize(point_offset + (point_rows * point_columns));

		// Append a new polyhedron to the mesh ...
		primitive* const polyhedron = create(Mesh);

		polyhedron->first_faces.assign(1, 0);
		polyhedron->face_counts.assign(1, rows * columns);
		polyhedron->polyhedron_types.assign(1, mesh::polyhedra_t::POLYGONS);
		polyhedron->face_first_loops.resize(rows * columns);
		polyhedron->face_loop_counts.assign(rows * columns, 1);
		polyhedron->face_selections.assign(rows * columns, 0.0);
		polyhedron->face_materials.assign(rows * columns, Material);
		polyhedron->loop_first_edges.resize(rows * columns);
		polyhedron->edge_points.resize(4 * rows * columns);
		polyhedron->clockwise_edges.resize(4 * rows * columns);
		polyhedron->edge_selections.assign(4 * rows * columns, 0.0);

		mesh::indices_t::iterator face_first_loop = polyhedron->face_first_loops.begin();
		mesh::indices_t::iterator loop_first_edge = polyhedron->loop_first_edges.begin();
		mesh::indices_t::iterator edge_point = polyhedron->edge_points.begin();
		mesh::indices_t::iterator clockwise_edge = polyhedron->clockwise_edges.begin();

		uint_t face_number = 0;
		for(uint_t row = 0; row != rows; ++row)
		{
			for(uint_t column = 0; column != columns; ++column, ++face_number)
			{
				*face_first_loop++ = face_number;

				*loop_first_edge++ = 4 * face_number;

				*edge_point++ = point_offset + (column + (row * point_columns));
				*edge_point++ = point_offset + (column + (row * point_columns) + 1);
				*edge_point++ = point_offset + (column + ((row + 1) * point_columns) + 1);
				*edge_point++ = point_offset + (column + ((row + 1) * point_columns));

				*clockwise_edge++ = (4 * face_number) + 1;
				*clockwise_edge++ = (4 * face_number) + 2;
				*clockwise_edge++ = (4 * face_number) + 3;
				*clockwise_edge++ = (4 * face_number);
			}
		}

		return polyhedron;
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_cylinder

primitive* create_cylinder(mesh& Mesh, const uint_t Rows, const uint_t Columns, imaterial* const Material)
{
	try
	{
		if(!Rows)
			throw std::runtime_error("Cannot create cylinder with zero rows.");

		if(Columns < 2)
			throw std::runtime_error("Cannot create cylinder with <2 columns.");

		const uint_t rows = Rows;
		const uint_t columns = Columns;
		const uint_t point_rows = rows + 1;
		const uint_t point_columns = columns;

		// Append points to the mesh (create the arrays if they don't already exist) ...
		mesh::points_t& points = Mesh.points ? Mesh.points.writable() : Mesh.points.create();
		mesh::selection_t& point_selection = Mesh.point_selection ? Mesh.point_selection.writable() : Mesh.point_selection.create();

		const uint_t point_offset = points.size();
		points.insert(points.end(), point_rows * point_columns, point3());
		point_selection.insert(point_selection.end(), point_rows * point_columns, 0.0);
		Mesh.vertex_data.resize(point_offset + (point_rows * point_columns));

		// Append a new polyhedron to the mesh ...
		primitive* const polyhedron = create(Mesh);

		polyhedron->first_faces.assign(1, 0);
		polyhedron->face_counts.assign(1, rows * columns);
		polyhedron->polyhedron_types.assign(1, mesh::polyhedra_t::POLYGONS);
		polyhedron->face_first_loops.resize(rows * columns);
		polyhedron->face_loop_counts.assign(rows * columns, 1);
		polyhedron->face_selections.assign(rows * columns, 0.0);
		polyhedron->face_materials.assign(rows * columns, Material);
		polyhedron->loop_first_edges.resize(rows * columns);
		polyhedron->edge_points.resize(4 * rows * columns);
		polyhedron->clockwise_edges.resize(4 * rows * columns);
		polyhedron->edge_selections.assign(4 * rows * columns, 0.0);
		
		mesh::indices_t::iterator face_first_loop = polyhedron->face_first_loops.begin();
		mesh::indices_t::iterator loop_first_edge = polyhedron->loop_first_edges.begin();
		mesh::indices_t::iterator edge_point = polyhedron->edge_points.begin();
		mesh::indices_t::iterator clockwise_edge = polyhedron->clockwise_edges.begin();

		uint_t face_number = 0;
		for(uint_t row = 0; row != rows; ++row)
		{
			for(uint_t column = 0; column != columns; ++column, ++face_number)
			{
				*face_first_loop++ = face_number;

				*loop_first_edge++ = 4 * face_number;

				*edge_point++ = point_offset + (column + (row * point_columns));
				*edge_point++ = point_offset + ((column + (row * point_columns) + 1) % point_columns);
				*edge_point++ = point_offset + ((column + ((row + 1) * point_columns) + 1) % point_columns);
				*edge_point++ = point_offset + (column + ((row + 1) * point_columns));

				*clockwise_edge++ = (4 * face_number) + 1;
				*clockwise_edge++ = (4 * face_number) + 2;
				*clockwise_edge++ = (4 * face_number) + 3;
				*clockwise_edge++ = (4 * face_number);
			}
		}

		return polyhedron;
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const bool_t legacy_validate_polyhedra(const mesh& Mesh)
{
	if(!Mesh.polyhedra)
		return false;

	return_val_if_fail(Mesh.polyhedra->first_faces, false);
	return_val_if_fail(Mesh.polyhedra->face_counts, false);
	return_val_if_fail(Mesh.polyhedra->types, false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops, false);
	return_val_if_fail(Mesh.polyhedra->face_loop_counts, false);
	return_val_if_fail(Mesh.polyhedra->face_selection, false);
	return_val_if_fail(Mesh.polyhedra->face_materials, false);
	return_val_if_fail(Mesh.polyhedra->loop_first_edges, false);
	return_val_if_fail(Mesh.polyhedra->edge_points, false);
	return_val_if_fail(Mesh.polyhedra->clockwise_edges, false);
	return_val_if_fail(Mesh.polyhedra->edge_selection, false);

	return_val_if_fail(Mesh.polyhedra->first_faces->size() == Mesh.polyhedra->face_counts->size(), false);
	return_val_if_fail(Mesh.polyhedra->first_faces->size() == Mesh.polyhedra->types->size(), false);

	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_loop_counts->size(), false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_selection->size(), false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops->size() == Mesh.polyhedra->face_materials->size(), false);

	return_val_if_fail(Mesh.polyhedra->edge_points->size() == Mesh.polyhedra->clockwise_edges->size(), false);
	return_val_if_fail(Mesh.polyhedra->edge_points->size() == Mesh.polyhedra->edge_selection->size(), false);

	// Check for infinite loops
	// TODO:  Catch segmentation faults
	const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	const uint_t loop_begin = 0;
	const uint_t loop_end = loop_begin + loop_first_edges.size();
	for(uint_t loop = loop_begin; loop != loop_end; ++loop)
	{
		const uint_t first_edge = loop_first_edges[loop];
		uint_t edge_slow = first_edge;
		uint_t edge_fast = first_edge;
		uint_t cycle_count = 0;
		while(true)
		{
			edge_slow = clockwise_edges[edge_slow];
			edge_fast = clockwise_edges[clockwise_edges[edge_fast]];

			if(edge_slow == edge_fast)
				++cycle_count;

			if(cycle_count > 2)
			{
				log() << error << "infinite loop at loop index " << loop << std::endl;
				return false;
			}

			if(edge_slow == first_edge)
				break;
		}
	}
	return true;
}

const_primitive* validate(const mesh& Mesh)
{
	if(!legacy_validate_polyhedra(Mesh))
		return 0;

	return new const_primitive(
		*Mesh.polyhedra->first_faces,
		*Mesh.polyhedra->face_counts,
		*Mesh.polyhedra->types,
		*Mesh.polyhedra->face_first_loops,
		*Mesh.polyhedra->face_loop_counts,
		*Mesh.polyhedra->face_selection,
		*Mesh.polyhedra->face_materials,
		*Mesh.polyhedra->loop_first_edges,
		*Mesh.polyhedra->edge_points,
		*Mesh.polyhedra->clockwise_edges,
		*Mesh.polyhedra->edge_selection,
		Mesh.polyhedra->constant_data,
		Mesh.polyhedra->uniform_data,
		Mesh.polyhedra->face_varying_data);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

primitive* validate(mesh& Mesh)
{
	if(!legacy_validate_polyhedra(Mesh))
		return 0;

	mesh::polyhedra_t& polyhedron = Mesh.polyhedra.writable();

	return new primitive(
		polyhedron.first_faces.writable(),
		polyhedron.face_counts.writable(),
		polyhedron.types.writable(),
		polyhedron.face_first_loops.writable(),
		polyhedron.face_loop_counts.writable(),
		polyhedron.face_selection.writable(),
		polyhedron.face_materials.writable(),
		polyhedron.loop_first_edges.writable(),
		polyhedron.edge_points.writable(),
		polyhedron.clockwise_edges.writable(),
		polyhedron.edge_selection.writable(),
		polyhedron.constant_data,
		polyhedron.uniform_data,
		polyhedron.face_varying_data);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// is_triangles

const bool_t is_triangles(const const_primitive& Polyhedron)
{
	const uint_t face_begin = 0;
	const uint_t face_end = face_begin + Polyhedron.face_first_loops.size();
	for(uint_t face = face_begin; face != face_end; ++face)
	{
		uint_t edge_count = 0;
		const uint_t first_edge = Polyhedron.loop_first_edges[Polyhedron.face_first_loops[face]];
		for(uint_t edge = first_edge; ; )
		{
			++edge_count;

			edge = Polyhedron.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}

		if(edge_count != 3)
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// is_solid

const bool_t is_solid(const const_primitive& Polyhedron)
{
	// K-3D uses a split-edge data structure to represent polyhedra.
	// We test for solidity by counting the number of edges that
	// connect each pair of points in the polyhedron.  A polyhedron is 
	// solid if-and-only-if each pair of points is used by two edges.

	mesh::bools_t boundary_edges;
	mesh::indices_t adjacent_edges;
	create_edge_adjacency_lookup(Polyhedron.edge_points, Polyhedron.clockwise_edges, boundary_edges, adjacent_edges);
	return std::find(boundary_edges.begin(), boundary_edges.end(), true) == boundary_edges.end();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// is_sds

const bool_t is_sds(const const_primitive& Polyhedron)
{
	return Polyhedron.polyhedron_types.size() && (Polyhedron.polyhedron_types[0] == mesh::polyhedra_t::CATMULL_CLARK);
}

//////////////////////////////////////////////////////////////////////////////
// center

const point3 center(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex)
{
	point3 result(0, 0, 0);

	uint_t count = 0;
	for(uint_t edge = EdgeIndex; ; )
	{
		result += to_vector(Points[EdgePoints[edge]]);
		++count;

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	if(count)
		result /= static_cast<double>(count);

	return result;
}

//////////////////////////////////////////////////////////////////////////////
// normal

const normal3 normal(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const uint_t EdgeIndex)
{
	// Calculates the normal for an edge loop using the summation method, which is more robust than the three-point methods (handles zero-length edges)
	normal3 result(0, 0, 0);

	for(uint_t edge = EdgeIndex; ; )
	{
		const point3& i = Points[EdgePoints[edge]];
		const point3& j = Points[EdgePoints[ClockwiseEdges[edge]]];

		result[0] += (i[1] + j[1]) * (j[2] - i[2]);
		result[1] += (i[2] + j[2]) * (j[0] - i[0]);
		result[2] += (i[0] + j[0]) * (j[1] - i[1]);

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	return 0.5 * result;
}

namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////////
// find_companion_worker

class find_companion_worker
{
public:
	find_companion_worker(
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const mesh::counts_t& Valences,
		const mesh::indices_t& FirstEdges,
		const mesh::indices_t& PointEdges,
		mesh::bools_t& BoundaryEdges,
		mesh::indices_t& AdjacentEdges) :
			m_edge_points(EdgePoints),
			m_clockwise_edges(ClockwiseEdges),
			m_valences(Valences),
			m_first_edges(FirstEdges),
			m_point_edges(PointEdges),
			m_boundary_edges(BoundaryEdges),
			m_adjacent_edges(AdjacentEdges)
		{
		}
	
	void operator()(const k3d::parallel::blocked_range<k3d::uint_t>& range) const
	{
		const k3d::uint_t edge_begin = range.begin();
		const k3d::uint_t edge_end = range.end();
		for(size_t edge = edge_begin; edge != edge_end; ++edge)
		{
			const uint_t vertex1 = m_edge_points[edge];
			const uint_t vertex2 = m_edge_points[m_clockwise_edges[edge]];

			const uint_t first_index = m_first_edges[vertex2];
			const uint_t last_index = first_index + m_valences[vertex2];
			m_adjacent_edges[edge] = edge;
			for(uint_t i = first_index; i != last_index; ++i)
			{
				const uint_t companion = m_point_edges[i];
				if(m_edge_points[m_clockwise_edges[companion]] == vertex1)
				{
					m_boundary_edges[edge] = false;
					m_adjacent_edges[edge] = companion;
					break;
				}
			}
		}
	}
	
private:
	const mesh::indices_t& m_edge_points;
	const mesh::indices_t& m_clockwise_edges;
	const mesh::counts_t& m_valences;
	const mesh::indices_t& m_first_edges;
	const mesh::indices_t& m_point_edges;
	mesh::bools_t& m_boundary_edges;
	mesh::indices_t& m_adjacent_edges;
};

/// Returns true if a and b are collinear, up to the give threshold
/**
 * The surfaces of the parallelogram formed by the projections of a and b in the
 * XY, XZ and YZ plane all need to be < Threshold for the points to be collinear
 */
const bool_t is_collinear(const vector3& a, const vector3& b, const double_t Threshold)
{
	if(std::abs(a[0]*b[1] - a[1]*b[0]) > Threshold)
		return false;
	if(std::abs(a[0]*b[2] - a[2]*b[0]) > Threshold)
		return false;
	if(std::abs(a[1]*b[2] - a[2]*b[1]) > Threshold)
		return false;
	return true;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////////////////
// create_edge_adjacency_lookup

void create_edge_adjacency_lookup(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, mesh::bools_t& BoundaryEdges, mesh::indices_t& AdjacentEdges)
{
	mesh::counts_t valences;
	create_vertex_valence_lookup(0, EdgePoints, valences);
	mesh::counts_t found_edges(valences.size(), 0);
	mesh::indices_t first_edges(valences.size(), 0); // first edge in point_edges for each point
	mesh::indices_t point_edges(EdgePoints.size(), 0);
	uint_t count = 0;
	for(uint_t point = 0; point != valences.size(); ++point)
	{
		first_edges[point] = count;
		count += valences[point];
	}
	BoundaryEdges.assign(EdgePoints.size(), true);
	AdjacentEdges.assign(EdgePoints.size(), 0);

	const uint_t edge_begin = 0;
	const uint_t edge_end = edge_begin + EdgePoints.size();
	for(uint_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const uint_t point = EdgePoints[edge];
		point_edges[first_edges[point] + found_edges[point]] = edge;
		++found_edges[point];
	}
	
	// Making this parallel decreases running time by 20 % on a Pentium D. 
	k3d::parallel::parallel_for(
				k3d::parallel::blocked_range<k3d::uint_t>(edge_begin, edge_end, k3d::parallel::grain_size()),
				detail::find_companion_worker(EdgePoints, ClockwiseEdges, valences, first_edges, point_edges, BoundaryEdges, AdjacentEdges));
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_edge_face_lookup

void create_edge_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, mesh::indices_t& EdgeFaces)
{
	EdgeFaces.assign(ClockwiseEdges.size(), 0);

	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				EdgeFaces[edge] = face;

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_vertex_face_lookup

void create_vertex_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, mesh::indices_t& PointFirstFaces, mesh::counts_t& PointFaceCounts, mesh::indices_t& PointFaces)
{
	std::vector<std::vector<size_t> > adjacency_list(Points.size());

	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				adjacency_list[EdgePoints[edge]].push_back(face);

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	PointFirstFaces.assign(Points.size(), 0);
	PointFaceCounts.assign(Points.size(), 0);
	PointFaces.clear();

	const size_t point_begin = 0;
	const size_t point_end = point_begin + Points.size();
	for(size_t point = point_begin; point != point_end; ++point)
	{
		PointFirstFaces[point] = PointFaces.size();
		PointFaceCounts[point] = adjacency_list[point].size();
		PointFaces.insert(PointFaces.end(), adjacency_list[point].begin(), adjacency_list[point].end());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_vertex_valence_lookup

void create_vertex_valence_lookup(const uint_t PointCount, const mesh::indices_t& EdgePoints, mesh::counts_t& Valences)
{
	Valences.assign(PointCount, 0);
	
	// Add 1 for each edge that starts at a point
	uint_t edge_count = EdgePoints.size();
	for (uint_t edge = 0; edge != edge_count; ++edge)
	{
		const uint_t edge_point = EdgePoints[edge];
		if(edge_point >= Valences.size()) // In case PointCount was not known to the caller
			Valences.resize(edge_point + 1, 0);
		++Valences[edge_point];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create_boundary_face_lookup

void create_boundary_face_lookup(const mesh::indices_t& FaceFirstLoops, const mesh::indices_t& FaceLoopCounts, const mesh::indices_t& LoopFirstEdges, const mesh::indices_t& ClockwiseEdges, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, mesh::bools_t& BoundaryFaces)
{
	BoundaryFaces.clear();
	BoundaryFaces.resize(FaceFirstLoops.size());
	
	const size_t face_begin = 0;
	const size_t face_end = face_begin + FaceFirstLoops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		const size_t loop_begin = FaceFirstLoops[face];
		const size_t loop_end = loop_begin + FaceLoopCounts[face];
		for(size_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const size_t first_edge = LoopFirstEdges[loop];
			for(size_t edge = first_edge; ;)
			{
				if (BoundaryEdges[edge])
				{
					BoundaryFaces[face] = true;
					break;
				}

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// mark_collinear_edges

void mark_collinear_edges(mesh::indices_t& RedundantEdges, const mesh::selection_t& EdgeSelection, const mesh::points_t& Points, const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::counts_t& VertexValences, const mesh::bools_t& BoundaryEdges, const mesh::indices_t& AdjacentEdges, const double_t Threshold)
{
	for(uint_t edge = 0; edge != EdgePoints.size(); ++edge)
	{
		if(!EdgeSelection[edge])
			continue;
		const uint_t clockwise = ClockwiseEdges[edge];
		const uint_t p1_index = EdgePoints[edge];
		const uint_t p2_index = EdgePoints[clockwise];
		const uint_t p3_index = EdgePoints[ClockwiseEdges[clockwise]];
		// valence must be 2 for internal edges or 1 for boundary edges
		if(!((!BoundaryEdges[clockwise] && VertexValences[p2_index] == 2) || (BoundaryEdges[clockwise] && VertexValences[p2_index] == 1)))
			continue;
		
		const point3& p1 = Points[p1_index];
		const point3& p2 = Points[p2_index];
		const point3& p3 = Points[p3_index];
		
		if(detail::is_collinear(p1-p2, p1-p3, Threshold))
		{
			RedundantEdges.push_back(clockwise);
		}
	}
}

void mark_coplanar_edges(const mesh::indices_t& Companions,
		const mesh::bools_t& BoundaryEdges,
		const mesh::normals_t& Normals,
		const mesh::indices_t& EdgeFaces,
		const mesh::selection_t& FaceSelection,
		mesh::indices_t& RedundantEdges,
		const double_t Threshold)
{
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
		
		if((!Normals[face].length()) || (std::abs((Normals[face] * Normals[companion_face]) - 1) < Threshold))
			RedundantEdges.push_back(edge);
	}
}

} // namespace polyhedron

} // namespace k3d

