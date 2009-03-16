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
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

/////////////////////////////////////////////////////////////////////////////
// hyperboloid

class hyperboloid :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	hyperboloid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_x1(init_owner(*this) + init_name("x1") + init_label(_("x1")) + init_description(_("x1")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y1(init_owner(*this) + init_name("y1") + init_label(_("y1")) + init_description(_("y1")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z1(init_owner(*this) + init_name("z1") + init_label(_("z1")) + init_description(_("z1")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_x2(init_owner(*this) + init_name("x2") + init_label(_("x2")) + init_description(_("x2")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_y2(init_owner(*this) + init_name("y2") + init_label(_("y2")) + init_description(_("y2")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_z2(init_owner(*this) + init_name("z2") + init_label(_("z2")) + init_description(_("z2")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("thetamax")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_x1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_y1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_z1.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_x2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_y2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_z2.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_thetamax.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::double_t x1 = m_x1.pipeline_value();
		const k3d::double_t y1 = m_y1.pipeline_value();
		const k3d::double_t z1 = m_z1.pipeline_value();
		const k3d::double_t x2 = m_x2.pipeline_value();
		const k3d::double_t y2 = m_y2.pipeline_value();
		const k3d::double_t z2 = m_z2.pipeline_value();
		const k3d::double_t thetamax = m_thetamax.pipeline_value();

		const k3d::double_t thetamin1 = atan2(y1, x1);
		const k3d::double_t thetamin2 = atan2(y2, x2);

		const k3d::double_t radius1 = sqrt(x1 * x1 + y1 * y1);
		const k3d::double_t radius2 = sqrt(x2 * x2 + y2 * y2);

		const k3d::vector3 offset1 = z1 * k3d::vector3(0, 0, 1);
		const k3d::vector3 offset2 = z2 * k3d::vector3(0, 0, 1);

		// Compute NURBS control points ...
		k3d::mesh::knots_t knots1;
		k3d::mesh::weights_t weights1;
		k3d::mesh::points_t control_points1;
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), thetamin1, thetamin1 + thetamax, 4, knots1, weights1, control_points1);

		k3d::mesh::knots_t knots2;
		k3d::mesh::weights_t weights2;
		k3d::mesh::points_t control_points2;
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), thetamin2, thetamin2 + thetamax, 4, knots2, weights2, control_points2);

		// Create patch ...
		k3d::mesh::points_t& points = Output.points.create();
		k3d::mesh::selection_t& point_selection = Output.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_patch::primitive> primitive(k3d::nurbs_patch::create(Output));

		primitive->patch_first_points.push_back(primitive->patch_points.size());
		primitive->patch_u_point_counts.push_back(control_points1.size());
		primitive->patch_v_point_counts.push_back(2);
		primitive->patch_u_orders.push_back(3);
		primitive->patch_v_orders.push_back(2);
		primitive->patch_u_first_knots.push_back(primitive->patch_u_knots.size());
		primitive->patch_v_first_knots.push_back(primitive->patch_v_knots.size());
		primitive->patch_selections.push_back(0);
		primitive->patch_materials.push_back(material);

		for(k3d::uint_t j = 0; j != control_points1.size(); ++j)
		{
			primitive->patch_points.push_back(points.size());
			primitive->patch_point_weights.push_back(weights1[j]);

			points.push_back((radius1 * control_points1[j]) + offset1);
			point_selection.push_back(0);
		}

		for(k3d::uint_t j = 0; j != control_points2.size(); ++j)
		{
			primitive->patch_points.push_back(points.size());
			primitive->patch_point_weights.push_back(weights2[j]);

			points.push_back((radius2 * control_points2[j]) + offset2);
			point_selection.push_back(0);
		}

		primitive->patch_u_knots.assign(knots1.begin(), knots1.end());

		primitive->patch_v_knots.push_back(0);
		primitive->patch_v_knots.push_back(0);
		primitive->patch_v_knots.push_back(1);
		primitive->patch_v_knots.push_back(1);

		primitive->patch_trim_loop_counts.push_back(0);
		primitive->patch_first_trim_loops.push_back(0);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<hyperboloid, k3d::interface_list<k3d::imesh_source > > factory(
		  k3d::uuid(0xb653277c, 0x7bba4505, 0xbabcd65d, 0x591ecedf),
		  "NurbsHyperboloid",
		  _("Generates a NURBS hyperboloid"),
		  "NURBS",
		  k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x1;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y1;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z1;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x2;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y2;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z2;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
};

/////////////////////////////////////////////////////////////////////////////
// hyperboloid_factory

k3d::iplugin_factory& hyperboloid_factory()
{
	return hyperboloid::get_factory();
}

} // namespace nurbs

} // namespace module


