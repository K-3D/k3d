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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

namespace sources
{

/////////////////////////////////////////////////////////////////////////////
// poly_cone_implementation

class poly_cone_implementation :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	poly_cone_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("U segments")) + init_description(_("Columns")) + init_value(32) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("V segments")) + init_description(_("Rows")) + init_value(5) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_bottom(init_owner(*this) + init_name("bottom") + init_label(_("Bottom")) + init_description(_("Cap cone bottom")) + init_value(true)),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Bottom radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Z Max (RenderMan convention)")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_power(init_owner(*this) + init_name("u_power") + init_label(_("U power")) + init_description(_("Radial power")) + init_value(1.0) + init_step_increment(0.1)),
		m_v_power(init_owner(*this) + init_name("v_power") + init_label(_("V power")) + init_description(_("Length power")) + init_value(1.0) + init_step_increment(0.1))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_v_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_bottom.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));

		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_height.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_u_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_v_power.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		const k3d::bool_t bottom = m_bottom.pipeline_value();

		// Create the cone topology ...
		boost::scoped_ptr<k3d::polyhedron::primitive> primitive(k3d::polyhedron::create_cylinder(Output, v_segments, u_segments, material));

		// Ensure that the top of the cone is "closed" topologically ...
		primitive->shell_face_counts[0] += 1;
		primitive->face_first_loops.push_back(primitive->loop_first_edges.size());
		primitive->face_loop_counts.push_back(1);
		primitive->face_selections.push_back(0);
		primitive->face_materials.push_back(material);
		primitive->loop_first_edges.push_back(primitive->edge_points.size());

		for(k3d::int32_t u = u_segments; u != 0; --u)
		{
			primitive->edge_points.push_back(u % u_segments);
			primitive->edge_selections.push_back(0);
			primitive->clockwise_edges.push_back(primitive->edge_points.size());
		}
		primitive->clockwise_edges.back() = primitive->loop_first_edges.back();

		// Optionally close the bottom of the cone ...
		if(bottom)
		{
			primitive->shell_face_counts[0] += 1;
			primitive->face_first_loops.push_back(primitive->loop_first_edges.size());
			primitive->face_loop_counts.push_back(1);
			primitive->face_selections.push_back(0);
			primitive->face_materials.push_back(material);
			primitive->loop_first_edges.push_back(primitive->edge_points.size());

			const k3d::uint_t offset = v_segments * u_segments;
			for(k3d::int32_t u = 0; u != u_segments; ++u)
			{
				primitive->edge_points.push_back(offset + u);
				primitive->edge_selections.push_back(0);
				primitive->clockwise_edges.push_back(primitive->edge_points.size());
			}
			primitive->clockwise_edges.back() = primitive->loop_first_edges.back();
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();
		const k3d::double_t height = m_height.pipeline_value();
		const k3d::double_t u_power = m_u_power.pipeline_value();
		const k3d::double_t v_power = m_v_power.pipeline_value();
		const k3d::double_t inv_u_power = u_power ? 1.0 / u_power : 1.0;

		const k3d::int32_t point_u_segments = u_segments;
		const k3d::int32_t point_v_segments = v_segments + 1;

		k3d::mesh::points_t& points = Output.points.writable();

		k3d::uint_t point = 0;
		for(k3d::int32_t v = 0; v != point_v_segments; ++v)
		{
			const k3d::double_t percent = static_cast<k3d::double_t>(v) / static_cast<k3d::double_t>(point_v_segments - 1);
			const k3d::double_t varying_radius = k3d::mix(0.001 * radius, radius, percent);

			for(k3d::int32_t u = 0; u != point_u_segments; ++u, ++point)
			{
				const k3d::double_t theta = k3d::pi_times_2() * static_cast<k3d::double_t>(u) / static_cast<k3d::double_t>(point_u_segments);

				k3d::double_t x = cos(theta);
				k3d::double_t y = -sin(theta);
				k3d::double_t z = k3d::mix(height, 0.0, std::pow(percent, v_power));

				x = varying_radius * k3d::sign(x) * std::pow(std::abs(x), inv_u_power);
				y = varying_radius * k3d::sign(y) * std::pow(std::abs(y), inv_u_power);

				points[point] = k3d::point3(x, y, z);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cone_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x7c41b43f, 0x385143ab, 0x86969795, 0x368d6199),
			"PolyCone",
			_("Generates a polygonal cone with optional endcap"),
			"Polygon",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_bottom;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_u_power;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v_power;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cone_factory

k3d::iplugin_factory& poly_cone_factory()
{
	return poly_cone_implementation::get_factory();
}

} // namespace sources

} // namespace polyhedron

} // namespace module

