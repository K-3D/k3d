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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>

#include <iterator>

namespace libk3dprimitives
{

/////////////////////////////////////////////////////////////////////////////
// poly_torus_implementation

class poly_torus_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_torus_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(16) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_majorradius(init_owner(*this) + init_name("majorradius") + init_label(_("Major radius")) + init_description(_("Major Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_minorradius(init_owner(*this) + init_name("minorradius") + init_label(_("Minor radius")) + init_description(_("Minor Radius")) + init_value(2.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Major Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_power(init_owner(*this) + init_name("v_power") + init_label(_("V power")) + init_description(_("Minor Power")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
		m_majorradius.changed_signal().connect(make_reset_mesh_slot());
		m_minorradius.changed_signal().connect(make_reset_mesh_slot());
		m_u_power.changed_signal().connect(make_reset_mesh_slot());
		m_v_power.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double majorradius = m_majorradius.pipeline_value();
		const double minorradius = m_minorradius.pipeline_value();
		const double u_power = m_u_power.pipeline_value();
		const double v_power = m_v_power.pipeline_value();
		const double inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const double inv_v_power = v_power ? 1.0 / v_power : 1.0;
		k3d::imaterial* const material = m_material.pipeline_value();

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		// Create the torus body ...
		const k3d::legacy::grid_results_t grid = k3d::legacy::add_grid(Mesh, polyhedron, m_v_segments.pipeline_value(), m_u_segments.pipeline_value(), true, true, material);
		const boost::multi_array<k3d::legacy::point*, 2>& points = boost::get<0>(grid);

		// Define the shape of the torus ...
		const unsigned long point_v_segments = points.shape()[0];
		const unsigned long point_u_segments = points.shape()[1];

		// Shape the torus points
		for(unsigned long v = 0; v != point_v_segments; ++v)
			{
				const double phi = k3d::pi_times_2() * static_cast<double>(v) / static_cast<double>(point_v_segments);
				double minor_x = cos(phi);
				double minor_y = sin(phi);
				minor_x = k3d::sign(minor_x) * std::pow(std::abs(minor_x), inv_v_power);
				minor_y = k3d::sign(minor_y) * std::pow(std::abs(minor_y), inv_v_power);

				const double radius = majorradius - (minorradius * minor_x);

				for(unsigned long u = 0; u != point_u_segments; ++u)
					{
						const double theta = k3d::pi_times_2() * static_cast<double>(u) / static_cast<double>(point_u_segments);

						double x = -sin(theta);
						double y = -cos(theta);
						double z = minorradius * minor_y;

						x = radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
						y = radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

						points[v][u]->position = k3d::point3(x, y, z);
					}
			}

		assert_warning(k3d::legacy::is_valid(polyhedron));
		assert_warning(k3d::legacy::is_solid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_torus_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x9a5ea45b, 0xebc64e37, 0xa50b287a, 0x89e18b71),
			"PolyTorus",
			_("Generates a polygonal torus"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_majorradius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_minorradius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_power;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_v_power;
};

/////////////////////////////////////////////////////////////////////////////
// poly_torus_factory

k3d::iplugin_factory& poly_torus_factory()
{
	return poly_torus_implementation::get_factory();
}

} // namespace libk3dprimitives


