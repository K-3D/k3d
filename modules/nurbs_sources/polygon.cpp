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
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

namespace source
{

/////////////////////////////////////////////////////////////////////////////
// polygon

class polygon :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	polygon(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_u_segments(init_owner(*this) + init_name("u_segments") + init_label(_("u_segments")) + init_description(_("Sides")) + init_value(4) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_u_segments.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();
		const k3d::int32_t u_segments = m_u_segments.pipeline_value();

		// Compute control points ...
		k3d::mesh::points_t control_points;
		for(k3d::int32_t i = 0; i != u_segments; ++i)
		{
			const k3d::double_t angle = k3d::pi_over_2() + k3d::pi_times_2() / static_cast<k3d::double_t>(u_segments) * static_cast<k3d::double_t>(i);

			control_points.push_back(radius * k3d::point3(std::cos(angle), std::sin(angle), 0));
		}

		// Create curve ...
		Output.points.create();
		Output.point_selection.create();	
		
		boost::scoped_ptr<k3d::nurbs_curve::primitive> primitive(k3d::nurbs_curve::create(Output));

		k3d::nurbs_curve::add_curve(Output, *primitive, 2, control_points, material, 1);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polygon, k3d::interface_list<k3d::imesh_source > > factory(
		  k3d::uuid(0x49796f52, 0x2e29f346, 0xbd47e3c9, 0xfb899a81),
		  "NurbsPolygon",
		  _("Generates a NURBS polygon"),
		  "NURBS",
		  k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_segments;
};

/////////////////////////////////////////////////////////////////////////////
// polygon_factory

k3d::iplugin_factory& polygon_factory()
{
	return polygon::get_factory();
}

} // namespace source

} // namespace nurbs

} // namespace module


