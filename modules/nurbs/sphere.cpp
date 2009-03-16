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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

/////////////////////////////////////////////////////////////////////////////
// sphere

class sphere :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	sphere(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmax(init_owner(*this) + init_name("zmax") + init_label(_("zmax")) + init_description(_("ZMax")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmin(init_owner(*this) + init_name("zmin") + init_label(_("zmin")) + init_description(_("ZMin")) + init_value(-1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("thetamax")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Radial Segments")) + init_value(4) + init_constraint(constraint::minimum<k3d::int32_t>(3)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("v_segments")) + init_description(_("Radial Segments")) + init_value(2) + init_constraint(constraint::minimum<k3d::int32_t>(2)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_zmax.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_zmin.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_thetamax.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_v_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();
		const k3d::double_t zmax = m_zmax.pipeline_value();
		const k3d::double_t zmin = m_zmin.pipeline_value();
		const k3d::double_t thetamax = m_thetamax.pipeline_value();
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();
		const k3d::int32_t v_segments = m_v_segments.pipeline_value();

		const k3d::double_t phimin = (zmin > -1) ? asin(zmin) : -k3d::pi_over_2();
		const k3d::double_t phimax = (zmax < 1) ? asin(zmax) : k3d::pi_over_2();

		// Compute NURBS control points ...
		k3d::mesh::knots_t v_knots;
		k3d::mesh::weights_t v_weights;
		k3d::mesh::points_t v_control_points;
		k3d::nurbs_curve::circular_arc(k3d::vector3(0, 1, 0), k3d::vector3(0, 0, 1), phimin, phimax, v_segments, v_knots, v_weights, v_control_points);

		k3d::mesh::knots_t u_knots;
		k3d::mesh::weights_t u_weights;
		k3d::mesh::points_t u_control_points;
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), 0, thetamax, u_segments, u_knots, u_weights, u_control_points);

		// Create patch ...
		k3d::mesh::points_t& points = Output.points.create();
		k3d::mesh::selection_t& point_selection = Output.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_patch::primitive> primitive(k3d::nurbs_patch::create(Output));

		primitive->patch_first_points.push_back(primitive->patch_points.size());
		primitive->patch_u_point_counts.push_back(u_control_points.size());
		primitive->patch_v_point_counts.push_back(v_control_points.size());
		primitive->patch_u_orders.push_back(3);
		primitive->patch_v_orders.push_back(3);
		primitive->patch_u_first_knots.push_back(primitive->patch_u_knots.size());
		primitive->patch_v_first_knots.push_back(primitive->patch_v_knots.size());
		primitive->patch_selections.push_back(0);
		primitive->patch_materials.push_back(material);

		for(k3d::uint_t v = 0; v != v_control_points.size(); ++v)
		{
			const k3d::point3 offset = radius * v_control_points[v][2] * k3d::point3(0, 0, 1);
			const k3d::double_t radius2 = radius * v_control_points[v][1];
			const k3d::double_t v_weight = v_weights[v];

			for(k3d::uint_t u = 0; u != u_control_points.size(); ++u)
			{
				primitive->patch_points.push_back(points.size());
				primitive->patch_point_weights.push_back(v_weight * u_weights[u]);

				points.push_back((radius2 * u_control_points[u]) + offset);
				point_selection.push_back(0);
			}
		}

		primitive->patch_u_knots.assign(u_knots.begin(), u_knots.end());
		primitive->patch_v_knots.assign(v_knots.begin(), v_knots.end());

		primitive->patch_trim_loop_counts.push_back(0);
		primitive->patch_first_trim_loops.push_back(0);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sphere, k3d::interface_list<k3d::imesh_source > > factory(
		  k3d::uuid(0x082eeef0, 0x837f4277, 0xa7c21ff1, 0x839b286e),
		  "NurbsSphere",
		  _("Generates a NURBS sphere"),
		  "NURBS",
		  k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmax;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmin;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
};

/////////////////////////////////////////////////////////////////////////////
// sphere_factory

k3d::iplugin_factory& sphere_factory()
{
	return sphere::get_factory();
}

} // namespace nurbs

} // namespace module



