// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <iterator>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_grid

class nurbs_grid :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_grid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_order(init_owner(*this) + init_name("u_order") + init_label(_("u_order")) + init_description(_("U Order")) + init_value(4) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_order(init_owner(*this) + init_name("v_order") + init_label(_("v_order")) + init_description(_("V Order")) + init_value(4) + init_constraint(constraint::minimum(2L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Columns")) + init_value(1) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("v_segments")) + init_description(_("Rows")) + init_value(1) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("width")) + init_description(_("Width")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("height")) + init_description(_("Height")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_order.changed_signal().connect(make_reset_mesh_slot());
		m_v_order.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
		m_width.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::nupatch* const patch = new k3d::legacy::nupatch();
		Mesh.nupatches.push_back(patch);

		patch->material = m_material.value();

		const unsigned long u_order = m_u_order.value();
		const unsigned long v_order = m_v_order.value();
		const unsigned long u_segments = m_u_segments.value();
		const unsigned long v_segments = m_v_segments.value();
		const double width = m_width.value();
		const double height = m_height.value();

		const unsigned long u_degree = u_order - 1;
		const unsigned long v_degree = v_order - 1;
		const unsigned long u_points = (u_segments * u_degree) + 1;
		const unsigned long v_points = (v_segments * v_degree) + 1;
		//const unsigned long u_knots = u_order + u_points;
		//const unsigned long v_knots = v_order + v_points;

		patch->u_order = u_order;
		patch->v_order = v_order;

		patch->u_knots.insert(patch->u_knots.end(), u_order, 0);
		for(unsigned long i = 1; i != u_segments; ++i)
			patch->u_knots.insert(patch->u_knots.end(), u_order-1, i);
		patch->u_knots.insert(patch->u_knots.end(), u_order, u_segments);

		patch->v_knots.insert(patch->v_knots.end(), v_order, 0);
		for(unsigned long i = 1; i != v_segments; ++i)
			patch->v_knots.insert(patch->v_knots.end(), v_order-1, i);
		patch->v_knots.insert(patch->v_knots.end(), v_order, v_segments);

		for(unsigned long row = 0; row != v_points; ++row)
		{
			const double row_percent = 0.5 - (static_cast<double>(row) / static_cast<double>(v_points-1));

			for(unsigned long column = 0; column != u_points; ++column)
			{
				const double column_percent = (static_cast<double>(column) / static_cast<double>(u_points-1)) - 0.5;

				k3d::legacy::point* point = new k3d::legacy::point(width * column_percent, height * row_percent, 0);
				Mesh.points.push_back(point);
				patch->control_points.push_back(k3d::legacy::nupatch::control_point(point, 1));
			}
		}

		assert_warning(k3d::legacy::is_valid(*patch));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_grid, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x5aac4e72, 0xf9b04b61, 0xf8b1bdbc, 0x851cf62e),
			"NurbsGrid",
			_("Generates a NURBS grid"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_order;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_order;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_grid_factory

k3d::iplugin_factory& nurbs_grid_factory()
{
	return nurbs_grid::get_factory();
}

} // namespace libk3dnurbs

