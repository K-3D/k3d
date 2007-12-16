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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/persistent.h>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_cylinder

class nurbs_cylinder :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_cylinder(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmax(init_owner(*this) + init_name("zmax") + init_label(_("zmax")) + init_description(_("ZMax")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_zmin(init_owner(*this) + init_name("zmin") + init_label(_("zmin")) + init_description(_("ZMin")) + init_value(-5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("End angle")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Radial Segments")) + init_value(4) + init_constraint(constraint::minimum(3L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_segments(init_owner(*this) + init_name("v_segments") + init_label(_("v_segments")) + init_description(_("Radial Segments")) + init_value(2) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_zmax.changed_signal().connect(make_reset_mesh_slot());
		m_zmin.changed_signal().connect(make_reset_mesh_slot());
		m_thetamax.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_v_segments.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double radius = m_radius.pipeline_value();
		const double zmax = m_zmax.pipeline_value();
		const double zmin = m_zmin.pipeline_value();
		const double thetamax = m_thetamax.pipeline_value();
		const unsigned long u_segments = m_u_segments.pipeline_value();
		const unsigned long v_segments = m_v_segments.pipeline_value();

		k3d::legacy::nupatch* const nupatch = new k3d::legacy::nupatch();
		Mesh.nupatches.push_back(nupatch);
		
		nupatch->material = m_material.pipeline_value();
		nupatch->u_order = 3;
		nupatch->v_order = 2;

		std::vector<double> weights;
		std::vector<k3d::point3> control_points;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, thetamax, u_segments, nupatch->u_knots, weights, control_points);
		return_if_fail(weights.size() == control_points.size());

		nupatch->v_knots.insert(nupatch->v_knots.end(), 2, 0);
		for(unsigned long i = 1; i != v_segments; ++i)
			nupatch->v_knots.insert(nupatch->v_knots.end(), i);
		nupatch->v_knots.insert(nupatch->v_knots.end(), 2, v_segments);

		for(unsigned long i = 0; i <= v_segments; ++i)
		{
			const k3d::point3 offset = k3d::mix(zmin, zmax, static_cast<double>(i) / static_cast<double>(v_segments)) * k3d::point3(0, 0, 1);

			for(unsigned long j = 0; j != control_points.size(); ++j)
			{
				Mesh.points.push_back(new k3d::legacy::point((radius * control_points[j]) + offset));
				nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(Mesh.points.back(), weights[j]));
			}
		}

		assert_warning(is_valid(*nupatch));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_cylinder, k3d::interface_list<k3d::imesh_source > > factory(
		k3d::uuid(0x756958ae, 0x6932432f, 0x854adf09, 0x262c2da2),
			"NurbsCylinder",
			_("Generates a NURBS cylinder"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmax;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_zmin;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_segments;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_cylinder_factory

k3d::iplugin_factory& nurbs_cylinder_factory()
{
	return nurbs_cylinder::get_factory();
}

} // namespace libk3dnurbs

