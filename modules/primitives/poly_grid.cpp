// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>

#include <iterator>

namespace libk3dprimitives
{

/////////////////////////////////////////////////////////////////////////////
// poly_grid

class poly_grid :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	poly_grid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values()))
	{
		m_columns.changed_signal().connect(make_topology_changed_slot());
		m_rows.changed_signal().connect(make_topology_changed_slot());
		m_material.changed_signal().connect(make_topology_changed_slot());
		
		m_width.changed_signal().connect(make_geometry_changed_slot());
		m_height.changed_signal().connect(make_geometry_changed_slot());
		m_orientation.changed_signal().connect(make_geometry_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		Mesh = k3d::create_grid(m_rows.pipeline_value(), m_columns.pipeline_value(), m_material.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
		const unsigned long point_rows = m_rows.pipeline_value() + 1;
		const unsigned long point_columns = m_columns.pipeline_value() + 1;
		const double width = m_width.pipeline_value();
		const double height = m_height.pipeline_value();
		const k3d::signed_axis orientation = m_orientation.pipeline_value();

		k3d::vector3 x, y;
		switch(orientation)
		{
			case k3d::PX:
				x = k3d::vector3(0, -width, 0);
				y = k3d::vector3(0, 0, height);
				break;
			case k3d::NX:
				x = k3d::vector3(0, width, 0);
				y = k3d::vector3(0, 0, height);
				break;
			case k3d::NY:
				x = k3d::vector3(-width, 0, 0);
				y = k3d::vector3(0, 0, height);
				break;
			case k3d::PY:
				x = k3d::vector3(width, 0, 0);
				y = k3d::vector3(0, 0, height);
				break;
			case k3d::PZ:
				x = k3d::vector3(-width, 0, 0);
				y = k3d::vector3(0, height, 0);
				break;
			case k3d::NZ:
				x = k3d::vector3(width, 0, 0);
				y = k3d::vector3(0, height, 0);
				break;
		}

		k3d::mesh::points_t::iterator point = const_cast<k3d::mesh::points_t&>(*Mesh.points).begin();
		for(unsigned long row = 0; row != point_rows; ++row)
		{
			const double row_percent = 0.5 - (static_cast<double>(row) / static_cast<double>(point_rows-1));

			for(unsigned long column = 0; column != point_columns; ++column)
			{
				const double column_percent = (static_cast<double>(column) / static_cast<double>(point_columns-1)) - 0.5;

				*point++ = k3d::to_point((column_percent * x) + (row_percent * y));
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_grid, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xacb3b4f8, 0x5cd6471c, 0xaed72686, 0xc576987c),
			"PolyGrid",
			_("Generates a polygonal grid"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
};

/////////////////////////////////////////////////////////////////////////////
// poly_grid_factory

k3d::iplugin_factory& poly_grid_factory()
{
	return poly_grid::get_factory();
}

} // namespace libk3dprimitives

