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
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs.h>

namespace module
{

namespace nurbs
{

/////////////////////////////////////////////////////////////////////////////
// hyperboloid

class hyperboloid :
			public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

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
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_x1.changed_signal().connect(make_reset_mesh_slot());
		m_y1.changed_signal().connect(make_reset_mesh_slot());
		m_z1.changed_signal().connect(make_reset_mesh_slot());
		m_x2.changed_signal().connect(make_reset_mesh_slot());
		m_y2.changed_signal().connect(make_reset_mesh_slot());
		m_z2.changed_signal().connect(make_reset_mesh_slot());
		m_thetamax.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double x1 = m_x1.pipeline_value();
		const double y1 = m_y1.pipeline_value();
		const double z1 = m_z1.pipeline_value();
		const double x2 = m_x2.pipeline_value();
		const double y2 = m_y2.pipeline_value();
		const double z2 = m_z2.pipeline_value();
		const double thetamax = m_thetamax.pipeline_value();

		const double thetamin1 = atan2(y1, x1);
		const double thetamin2 = atan2(y2, x2);

		const double radius1 = sqrt(x1 * x1 + y1 * y1);
		const double radius2 = sqrt(x2 * x2 + y2 * y2);

		const k3d::point3 offset1 = z1 * k3d::point3(0, 0, 1);
		const k3d::point3 offset2 = z2 * k3d::point3(0, 0, 1);

		k3d::legacy::nupatch* const nupatch = new k3d::legacy::nupatch();
		Mesh.nupatches.push_back(nupatch);

		nupatch->material = m_material.pipeline_value();
		nupatch->u_order = 3;
		nupatch->v_order = 2;

		std::vector<double> weights1;
		std::vector<k3d::point3> arc_points1;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), thetamin1, thetamin1 + thetamax, 4, nupatch->u_knots, weights1, arc_points1);

		std::vector<double> weights2;
		std::vector<k3d::point3> arc_points2;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), thetamin2, thetamin2 + thetamax, 4, nupatch->u_knots, weights2, arc_points2);

		nupatch->v_knots.insert(nupatch->v_knots.end(), 2, 0);
		nupatch->v_knots.insert(nupatch->v_knots.end(), 2, 1);

		for (unsigned long j = 0; j != arc_points1.size(); ++j)
		{
			Mesh.points.push_back(new k3d::legacy::point(radius1 * arc_points1[j] + offset1));
			nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights1[j]));
		}

		for (unsigned long j = 0; j != arc_points2.size(); ++j)
		{
			Mesh.points.push_back(new k3d::legacy::point(radius2 * arc_points2[j] + offset2));
			nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights2[j]));
		}

		assert_warning(is_valid(*nupatch));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
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
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_x2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_y2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_z2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
};

/////////////////////////////////////////////////////////////////////////////
// hyperboloid_factory

k3d::iplugin_factory& hyperboloid_factory()
{
	return hyperboloid::get_factory();
}

} // namespace nurbs

} // namespace module


