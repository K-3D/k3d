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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <iterator>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_torus_implementation

class poly_torus_implementation :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

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
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_v_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_majorradius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_minorradius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_u_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_v_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));

		polyhedron->shell_first_faces.push_back(0);
		polyhedron->shell_face_counts.push_back(0);
		polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

		k3d::polyhedron::add_torus(Output, *polyhedron, v_segments, u_segments, material);;
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		const k3d::double_t majorradius = m_majorradius.pipeline_value();
		const k3d::double_t minorradius = m_minorradius.pipeline_value();
		const k3d::double_t u_power = m_u_power.pipeline_value();
		const k3d::double_t v_power = m_v_power.pipeline_value();
		const k3d::double_t inv_u_power = u_power ? 1.0 / u_power : 1.0;
		const k3d::double_t inv_v_power = v_power ? 1.0 / v_power : 1.0;

		k3d::mesh::points_t& points = Output.points.writable();

		// Shape the torus points
		for(k3d::int32_t v = 0; v != v_segments; ++v)
		{
			const k3d::double_t phi = k3d::pi_times_2() * k3d::ratio(v, v_segments);
			k3d::double_t minor_x = cos(phi);
			k3d::double_t minor_y = sin(phi);
			minor_x = k3d::sign(minor_x) * std::pow(std::abs(minor_x), inv_v_power);
			minor_y = k3d::sign(minor_y) * std::pow(std::abs(minor_y), inv_v_power);

			const k3d::double_t radius = majorradius - (minorradius * minor_x);

			for(k3d::int32_t u = 0; u != u_segments; ++u)
			{
				const k3d::double_t theta = k3d::pi_times_2() * k3d::ratio(u, u_segments);

				k3d::double_t x = -sin(theta);
				k3d::double_t y = -cos(theta);
				k3d::double_t z = minorradius * minor_y;

				x = radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
				y = radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

				points[v * u_segments + u] = k3d::point3(x, y, z);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_torus_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x9a5ea45b, 0xebc64e37, 0xa50b287a, 0x89e18b71),
			"PolyTorus",
			_("Generates a polygonal torus"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_majorradius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_minorradius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_power;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_v_power;
};

/////////////////////////////////////////////////////////////////////////////
// poly_torus_factory

k3d::iplugin_factory& poly_torus_factory()
{
	return poly_torus_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module


