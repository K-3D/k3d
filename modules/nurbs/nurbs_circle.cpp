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
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/persistent.h>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_circle

class nurbs_circle :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_circle(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("End angle")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Segments")) + init_value(4) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_thetamax.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double radius = m_radius.pipeline_value();
		const double thetamax = m_thetamax.pipeline_value();
		const unsigned long u_segments = m_u_segments.pipeline_value();

		k3d::legacy::nucurve_group* const nucurve_group = new k3d::legacy::nucurve_group();
		k3d::legacy::nucurve* const nucurve = new k3d::legacy::nucurve();
		Mesh.nucurve_groups.push_back(nucurve_group);
		nucurve_group->curves.push_back(nucurve);
		nucurve_group->material = m_material.pipeline_value();
		nucurve->order = 3;
		
		std::vector<double> weights;
		std::vector<k3d::point3> control_points;
		k3d::nurbs::circular_arc(k3d::point3(1, 0, 0), k3d::point3(0, 1, 0), 0, thetamax, u_segments, nucurve->knots, weights, control_points);
		return_if_fail(weights.size() == control_points.size());
		
		for(unsigned long i = 0; i != control_points.size(); ++i)
		{
			Mesh.points.push_back(new k3d::legacy::point(radius * control_points[i]));
			nucurve->control_points.push_back(k3d::legacy::nucurve::control_point(Mesh.points.back(), weights[i]));
		}

		assert_warning(k3d::legacy::is_valid(*nucurve));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_circle, k3d::interface_list<k3d::imesh_source > > factory(
		k3d::uuid(0x82c3a0ef, 0x4e074f5c, 0xb3926e7f, 0xb725b571),
			"NurbsCircle",
			_("Generates a NURBS circle"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thetamax;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_circle_factory

k3d::iplugin_factory& nurbs_circle_factory()
{
	return nurbs_circle::get_factory();
}

} // namespace libk3dnurbs

