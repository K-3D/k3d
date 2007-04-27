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

#include "mesh_utility.h"

namespace k3d
{

namespace dev
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

} // namespace dev

} // namespace k3d

