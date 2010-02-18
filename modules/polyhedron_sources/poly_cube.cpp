// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@polytechnic.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_cube_implementation

class poly_cube_implementation :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	poly_cube_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number (X axis)")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number (Y axis)")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_slices(init_owner(*this) + init_name("slices") + init_label(_("Slices")) + init_description(_("Slice number (Z axis)")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Cube width")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Cube height")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_depth(init_owner(*this) + init_name("depth") + init_label(_("Depth")) + init_description(_("Cube depth")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_rows.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_columns.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_slices.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_width.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_height.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_depth.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::int32_t rows = m_rows.pipeline_value();
		const k3d::int32_t columns = m_columns.pipeline_value();
		const k3d::int32_t slices = m_slices.pipeline_value();

		const k3d::int32_t point_columns = columns + 1;
		const k3d::int32_t point_slices = slices + 1;

		const k3d::int32_t circumference = 2 * (rows + columns);
		const k3d::int32_t cylinder_point_count = point_slices * circumference;
		const k3d::int32_t cap_point_count = (rows - 1) * (columns - 1);

		// We model a cube as a cylinder ...
		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));

		polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);
		k3d::polyhedron::add_cylinder(Output, *polyhedron, 0, slices, circumference, material);

		k3d::mesh::points_t& points = Output.points.writable();
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

		// Create the cube "top" points ...
		points.insert(points.end(), cap_point_count, k3d::point3());
		point_selection.insert(point_selection.end(), cap_point_count, 0);

		// Create a lookup table to simplify creating the "top" faces ...
		k3d::mesh::indices_t top_map;
		for(k3d::int32_t column = 0; column != point_columns; ++column)
			top_map.push_back(columns - column);

		for(k3d::int32_t row = 1; row != rows; ++row)
		{
			top_map.push_back(columns + row);

			for(k3d::int32_t column = 1; column != columns; ++column)
				top_map.push_back(cylinder_point_count + ((row - 1) * (columns - 1)) + (column - 1));

			top_map.push_back(circumference - row);
		}

		for(k3d::int32_t column = 0; column != point_columns; ++column)
			top_map.push_back(column + columns + rows);

		// Create the "top" faces ...
		for(k3d::int32_t row = 0; row != rows; ++row)
		{
			for(k3d::int32_t column = 0; column != columns; ++column)
			{
				k3d::polyhedron::add_quadrilateral(
					Output,
					*polyhedron,
					0,
					top_map[(row + 0) * point_columns + (column + 0)],
					top_map[(row + 0) * point_columns + (column + 1)],
					top_map[(row + 1) * point_columns + (column + 1)],
					top_map[(row + 1) * point_columns + (column + 0)],
					material);
			}
		}

		// Create the cube "bottom" points ...
		points.insert(points.end(), cap_point_count, k3d::point3());
		point_selection.insert(point_selection.end(), cap_point_count, 0);

		// Create a lookup table to simplify creating the "bottom" faces ...
		k3d::mesh::indices_t bottom_map;
		for(k3d::int32_t column = 0; column != point_columns; ++column)
			bottom_map.push_back((slices * circumference) + column);

		for(k3d::int32_t row = 1; row != rows; ++row)
		{
			bottom_map.push_back((slices * circumference) + circumference - row);

			for(k3d::int32_t column = 1; column != columns; ++column)
				bottom_map.push_back(cylinder_point_count + cap_point_count + ((row - 1) * (columns - 1)) + (column - 1));

			bottom_map.push_back((slices * circumference) + columns + row);
		}

		for(k3d::int32_t column = 0; column != point_columns; ++column)
			bottom_map.push_back((slices * circumference) + columns + rows + columns - column);

		// Create the "bottom" faces ...
		for(k3d::int32_t row = 0; row != rows; ++row)
		{
			for(k3d::int32_t column = 0; column != columns; ++column)
			{
				k3d::polyhedron::add_quadrilateral(
					Output,
					*polyhedron,
					0,
					bottom_map[(row + 0) * point_columns + (column + 0)],
					bottom_map[(row + 0) * point_columns + (column + 1)],
					bottom_map[(row + 1) * point_columns + (column + 1)],
					bottom_map[(row + 1) * point_columns + (column + 0)],
					material);
			}
		}

	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::int32_t rows = m_rows.pipeline_value();
		const k3d::int32_t columns = m_columns.pipeline_value();
		const k3d::int32_t slices = m_slices.pipeline_value();
		const k3d::double_t width = m_width.pipeline_value();
		const k3d::double_t height = m_height.pipeline_value();
		const k3d::double_t depth = m_depth.pipeline_value();

		const k3d::int32_t circumference = 2 * (rows + columns);
		const k3d::int32_t cylinder_point_count = (slices + 1) * circumference;
		const k3d::int32_t cap_point_count = (rows - 1) * (columns - 1);

		k3d::mesh::points_t& points = Output.points.writable();

		// Create a mapping from an index around the "circumference" of the cube to point coordinates
		k3d::mesh::points_t square_map;
		for(k3d::int32_t i = 0; i != columns; ++i)
		{
			square_map.push_back(
				k3d::point3(k3d::mix(-width/2, width/2, k3d::ratio(i, columns)), height/2, 0));
		}
		for(k3d::int32_t i = 0; i != rows; ++i)
		{
			square_map.push_back(
				k3d::point3(width/2, k3d::mix(height/2, -height/2, k3d::ratio(i, rows)), 0));
		}
		for(k3d::int32_t i = 0; i != columns; ++i)
		{
			square_map.push_back(
				k3d::point3(k3d::mix(width/2, -width/2, k3d::ratio(i, columns)), -height/2, 0));
		}
		for(k3d::int32_t i = 0; i != rows; ++i)
		{
			square_map.push_back(
				k3d::point3(-width/2, k3d::mix(-height/2, height/2, k3d::ratio(i, rows)), 0));
		}

		// Assign coordinates to the "body" of the cube ...
		for(k3d::int32_t i = 0; i <= slices; ++i)
		{
			const k3d::vector3 offset(0, 0, k3d::mix(depth/2, -depth/2, k3d::ratio(i, slices)));
			for(k3d::int32_t j = 0; j != circumference; ++j)
			{
				points[(i * circumference) + j] = offset + square_map[j];
			}
		}

		// Assign coordinates to the top of the cube ...
		for(k3d::int32_t row = 1; row < rows; ++row)
		{
			for(k3d::int32_t column = 1; column < columns; ++column)
			{
				points[cylinder_point_count + ((row - 1) * (columns - 1)) + (column - 1)] =
					k3d::point3(
						k3d::mix(width/2, -width/2, k3d::ratio(column, columns)),
						k3d::mix(height/2, -height/2, k3d::ratio(row, rows)),
						depth/2);
			}
		}

		// Assign coordinates to the bottom of the cube ...
		for(k3d::int32_t row = 1; row < rows; ++row)
		{
			for(k3d::int32_t column = 1; column < columns; ++column)
			{
				points[cylinder_point_count + cap_point_count + ((row - 1) * (columns - 1)) + (column - 1)] =
					k3d::point3(
						k3d::mix(-width/2, width/2, k3d::ratio(column, columns)),
						k3d::mix(height/2, -height/2, k3d::ratio(row, rows)),
						-depth/2);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cube_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x7ec966d1, 0xd78a445c, 0x8806b8a3, 0x5593f241),
			"PolyCube",
			_("Generates a polygonal cube"),
			"Polyhedron",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_slices;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_depth;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cube_factory

k3d::iplugin_factory& poly_cube_factory()
{
	return poly_cube_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module


