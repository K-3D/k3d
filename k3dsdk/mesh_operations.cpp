// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "mesh_operations.h"

namespace k3d
{

const mesh create_grid(const size_t Rows, const size_t Columns, imaterial* const Material)
{
	return_val_if_fail(Rows, mesh());
	return_val_if_fail(Columns, mesh());

	mesh result;

	const unsigned long rows = Rows;
	const unsigned long columns = Columns;
	const unsigned long point_rows = rows + 1;
	const unsigned long point_columns = columns + 1;

	boost::shared_ptr<mesh::polyhedra_t> polyhedra(new mesh::polyhedra_t());
	boost::shared_ptr<mesh::indices_t> first_faces(new mesh::indices_t(1, 0));
	boost::shared_ptr<mesh::counts_t> face_counts(new mesh::counts_t(1, rows * columns));
	boost::shared_ptr<mesh::polyhedra_t::types_t> types(new mesh::polyhedra_t::types_t(1, mesh::polyhedra_t::POLYGONS));
	boost::shared_ptr<mesh::indices_t> face_first_loops(new mesh::indices_t(rows * columns));
	boost::shared_ptr<mesh::counts_t> face_loop_counts(new mesh::counts_t(rows * columns, 1));
	boost::shared_ptr<mesh::selection_t> face_selection(new mesh::selection_t(rows * columns, 0.0));
	boost::shared_ptr<mesh::materials_t> face_materials(new mesh::materials_t(rows * columns, Material));
	boost::shared_ptr<mesh::indices_t> loop_first_edges(new mesh::indices_t(rows * columns));
	boost::shared_ptr<mesh::indices_t> edge_points(new mesh::indices_t(4 * rows * columns));
	boost::shared_ptr<mesh::indices_t> clockwise_edges(new mesh::indices_t(4 * rows * columns));
	boost::shared_ptr<mesh::selection_t> edge_selection(new mesh::selection_t(4 * rows * columns, 0.0));
	boost::shared_ptr<mesh::points_t> points(new mesh::points_t(point_rows * point_columns));
	boost::shared_ptr<mesh::selection_t> point_selection(new mesh::selection_t(point_rows * point_columns, 0.0));

	mesh::indices_t::iterator face_first_loop = face_first_loops->begin();
	mesh::indices_t::iterator loop_first_edge = loop_first_edges->begin();
	mesh::indices_t::iterator edge_point = edge_points->begin();
	mesh::indices_t::iterator clockwise_edge = clockwise_edges->begin();
	
	size_t face_number = 0;

	for(unsigned long row = 0; row != rows; ++row)
	{
		for(unsigned long column = 0; column != columns; ++column)
		{
			*face_first_loop++ = face_number;

			*loop_first_edge++ = 4 * face_number;

			*edge_point++ = column + (row * point_columns);
			*edge_point++ = column + (row * point_columns) + 1;
			*edge_point++ = column + ((row + 1) * point_columns) + 1;
			*edge_point++ = column + ((row + 1) * point_columns);

			*clockwise_edge++ = (4 * face_number) + 1;
			*clockwise_edge++ = (4 * face_number) + 2;
			*clockwise_edge++ = (4 * face_number) + 3;
			*clockwise_edge++ = (4 * face_number);

			++face_number;
		}
	}

	polyhedra->first_faces = first_faces;
	polyhedra->face_counts = face_counts;
	polyhedra->types = types;
	polyhedra->face_first_loops = face_first_loops;
	polyhedra->face_loop_counts = face_loop_counts;
	polyhedra->face_selection = face_selection;
	polyhedra->face_materials = face_materials;
	polyhedra->loop_first_edges = loop_first_edges;
	polyhedra->edge_points = edge_points;
	polyhedra->clockwise_edges = clockwise_edges;
	polyhedra->edge_selection = edge_selection;

	result.polyhedra = polyhedra;
	result.points = points;
	result.point_selection = point_selection;

	return result;
}

const mesh create_cylinder(const size_t Rows, const size_t Columns, imaterial* const Material)
{
	return_val_if_fail(Rows, mesh());
	return_val_if_fail(Columns > 1, mesh());

	mesh result;

	const unsigned long rows = Rows;
	const unsigned long columns = Columns;
	const unsigned long point_rows = rows + 1;
	const unsigned long point_columns = columns;

	boost::shared_ptr<mesh::polyhedra_t> polyhedra(new mesh::polyhedra_t());
	boost::shared_ptr<mesh::indices_t> first_faces(new mesh::indices_t(1, 0));
	boost::shared_ptr<mesh::counts_t> face_counts(new mesh::counts_t(1, rows * columns));
	boost::shared_ptr<mesh::polyhedra_t::types_t> types(new mesh::polyhedra_t::types_t(1, mesh::polyhedra_t::POLYGONS));
	boost::shared_ptr<mesh::indices_t> face_first_loops(new mesh::indices_t(rows * columns));
	boost::shared_ptr<mesh::counts_t> face_loop_counts(new mesh::counts_t(rows * columns, 1));
	boost::shared_ptr<mesh::selection_t> face_selection(new mesh::selection_t(rows * columns, 0.0));
	boost::shared_ptr<mesh::materials_t> face_materials(new mesh::materials_t(rows * columns, Material));
	boost::shared_ptr<mesh::indices_t> loop_first_edges(new mesh::indices_t(rows * columns));
	boost::shared_ptr<mesh::indices_t> edge_points(new mesh::indices_t(4 * rows * columns));
	boost::shared_ptr<mesh::indices_t> clockwise_edges(new mesh::indices_t(4 * rows * columns));
	boost::shared_ptr<mesh::selection_t> edge_selection(new mesh::selection_t(4 * rows * columns, 0.0));
	boost::shared_ptr<mesh::points_t> points(new mesh::points_t(point_rows * point_columns));
	boost::shared_ptr<mesh::selection_t> point_selection(new mesh::selection_t(point_rows * point_columns, 0.0));

	mesh::indices_t::iterator face_first_loop = face_first_loops->begin();
	mesh::indices_t::iterator loop_first_edge = loop_first_edges->begin();
	mesh::indices_t::iterator edge_point = edge_points->begin();
	mesh::indices_t::iterator clockwise_edge = clockwise_edges->begin();
	
	size_t face_number = 0;

	for(unsigned long row = 0; row != rows; ++row)
	{
		for(unsigned long column = 0; column != columns; ++column)
		{
			*face_first_loop++ = face_number;

			*loop_first_edge++ = 4 * face_number;

			*edge_point++ = column + (row * point_columns);
			*edge_point++ = (column + (row * point_columns) + 1) % point_columns;
			*edge_point++ = (column + ((row + 1) * point_columns) + 1) % point_columns;
			*edge_point++ = column + ((row + 1) * point_columns);

			*clockwise_edge++ = (4 * face_number) + 1;
			*clockwise_edge++ = (4 * face_number) + 2;
			*clockwise_edge++ = (4 * face_number) + 3;
			*clockwise_edge++ = (4 * face_number);

			++face_number;
		}
	}

	polyhedra->first_faces = first_faces;
	polyhedra->face_counts = face_counts;
	polyhedra->types = types;
	polyhedra->face_first_loops = face_first_loops;
	polyhedra->face_loop_counts = face_loop_counts;
	polyhedra->face_selection = face_selection;
	polyhedra->face_materials = face_materials;
	polyhedra->loop_first_edges = loop_first_edges;
	polyhedra->edge_points = edge_points;
	polyhedra->clockwise_edges = clockwise_edges;
	polyhedra->edge_selection = edge_selection;

	result.polyhedra = polyhedra;
	result.points = points;
	result.point_selection = point_selection;

	return result;
}

const bool is_solid(const mesh& Mesh)
{
	// K-3D uses a split-edge data structure to represent polyhedra.
	// We test for solidity by counting the number of edges that
	// connect each pair of points in the polyhedra.  The polyhedra are
	// solid if-and-only-if each pair of points is used by two edges.
	if(!validate_polyhedra(Mesh))
		return true;

	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	typedef std::map<std::pair<size_t, size_t>, size_t> adjacent_edges_t;
	adjacent_edges_t adjacent_edges;

	const size_t edge_begin = 0;
	const size_t edge_end = edge_begin + edge_points.size();
	for(size_t edge = edge_begin; edge != edge_end; ++edge)
	{
		const size_t vertex1 = std::min(edge_points[edge], edge_points[clockwise_edges[edge]]);
		const size_t vertex2 = std::max(edge_points[edge], edge_points[clockwise_edges[edge]]);
		adjacent_edges[std::make_pair(vertex1, vertex2)] += 1;
	}

	for(adjacent_edges_t::iterator edges = adjacent_edges.begin(); edges != adjacent_edges.end(); ++edges)
	{
		if(edges->second != 2)
			return false;
	}

	return true;
}

const bool is_triangles(const mesh& Mesh)
{
	if(!validate_polyhedra(Mesh))
		return true;
	
	const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

	const size_t face_begin = 0;
	const size_t face_end = face_begin + face_first_loops.size();
	for(size_t face = face_begin; face != face_end; ++face)
	{
		size_t edge_count = 0;
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		for(size_t edge = first_edge; ; )
		{
			++edge_count;

			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}

		if(edge_count != 3)
			return false;
	}

	return true;
}

} // namespace k3d

