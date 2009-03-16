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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

/////////////////////////////////////////////////////////////////////////////
// circle

class circle :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	circle(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_thetamax(init_owner(*this) + init_name("thetamax") + init_label(_("thetamax")) + init_description(_("End angle")) + init_value(k3d::radians(360.0)) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Segments")) + init_value(4) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_thetamax.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();
		const k3d::double_t thetamax = m_thetamax.pipeline_value();
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();

		// Compute NURBS control points ...
		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;
		k3d::mesh::points_t control_points;
		k3d::nurbs_curve::circular_arc(k3d::vector3(radius, 0, 0), k3d::vector3(0, radius, 0), 0, thetamax, u_segments, knots, weights, control_points);

		// Create a NURBS curve primitive, and add a single curve ...
		Output.points.create();
		Output.point_selection.create();

		boost::scoped_ptr<k3d::nurbs_curve::primitive> primitive(k3d::nurbs_curve::create(Output));

		primitive->first_curves.push_back(primitive->curve_first_points.size());
		primitive->curve_counts.push_back(1);
		primitive->materials.push_back(material);

		k3d::nurbs_curve::add_curve(Output, *primitive, 3, control_points, weights, knots);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<circle, k3d::interface_list<k3d::imesh_source > > factory(
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
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
};

/////////////////////////////////////////////////////////////////////////////
// circle_factory

k3d::iplugin_factory& circle_factory()
{
	return circle::get_factory();
}

} // namespace nurbs

} // namespace module


