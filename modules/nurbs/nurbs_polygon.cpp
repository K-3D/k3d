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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>

namespace libk3dnurbs
{

/////////////////////////////////////////////////////////////////////////////
// nurbs_polygon

class nurbs_polygon :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	nurbs_polygon(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Sides")) + init_value(4) + init_constraint(constraint::minimum(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_u_segments.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::nucurve_group* const nucurve_group = new k3d::legacy::nucurve_group();
		k3d::legacy::nucurve* const nucurve = new k3d::legacy::nucurve();

		Mesh.nucurve_groups.push_back(nucurve_group);
		nucurve_group->curves.push_back(nucurve);
		nucurve_group->material = m_material.pipeline_value();

		// Create a NURBS polygon ...
		nucurve->order = 2;

		// Get side number ...
		const unsigned long u_segments = m_u_segments.pipeline_value();

		// Build knot vector ...
		nucurve->knots.push_back(0);
		for(unsigned long n = 0; n <= u_segments; ++n)
			nucurve->knots.push_back(n);
		nucurve->knots.push_back(u_segments);

		// Build control vertices ...
		const double radius = m_radius.pipeline_value();

		for(unsigned long n = 0; n != u_segments; ++n)
		{
			const double angle = k3d::pi_over_2() + k3d::pi_times_2() / static_cast<double>(u_segments) * static_cast<double>(n);

			k3d::legacy::point* const new_point = new k3d::legacy::point(radius * cos(angle), radius * sin(angle), 0.0);
			Mesh.points.push_back(new_point);
			nucurve->control_points.push_back(k3d::legacy::nucurve::control_point(new_point));
		}
		nucurve->control_points.push_back(k3d::legacy::nucurve::control_point(Mesh.points.front()));

		assert_warning(k3d::legacy::is_valid(*nucurve));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<nurbs_polygon, k3d::interface_list<k3d::imesh_source > > factory(
		k3d::uuid(0x49796f52, 0x2e29f346, 0xbd47e3c9, 0xfb899a81),
			"NurbsPolygon",
			_("Generates a NURBS polygon"),
			"NURBS",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// nurbs_polygon_factory

k3d::iplugin_factory& nurbs_polygon_factory()
{
	return nurbs_polygon::get_factory();
}

} // namespace libk3dnurbs

