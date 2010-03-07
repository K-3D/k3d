// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com) Most of the code used for poly_grid
	\author Joaquín Duo (joaduo@lugmen.org.ar) Adaptation to the function parser
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/expression/parser.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/property.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace plot
{

/////////////////////////////////////////////////////////////////////////////
// surface_plot

class surface_plot :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	surface_plot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_function(init_owner(*this) + init_name("function") + init_label(_("Function")) + init_description(_("Function to be plotted, in terms of u, v, and any scalar user properties added to the node.")) + init_value(k3d::string_t(_("cos(sqrt(u^2+v^2))")))),
		m_u_samples(init_owner(*this) + init_name("u_samples") + init_label(_("U Samples")) + init_description(_("Number of samples along the u dimension.")) + init_value(15) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_v_samples(init_owner(*this) + init_name("v_samples") + init_label(_("V Samples")) + init_description(_("Number of samples along the v dimension.")) + init_value(15) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_u_size(init_owner(*this) + init_name("u_size") + init_label(_("U Size")) + init_description(_("Plot size along the u dimension.")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_v_size(init_owner(*this) + init_name("v_size") + init_label(_("V Size")) + init_description(_("Plot size along the v dimension.")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Plot orientation (positive or negative along the X, Y or Z axis).")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_user_property_changed_signal(*this)
	{
		m_u_samples.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_v_samples.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_update_mesh_slot()));
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		
		m_function.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_u_size.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_v_size.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
		m_orientation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));

		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
		polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);
		k3d::polyhedron::add_grid(Output, *polyhedron, 0, m_v_samples.pipeline_value(), m_u_samples.pipeline_value(), m_material.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		const k3d::string_t function = m_function.pipeline_value();

		k3d::string_t variables("u,v");
		std::vector<k3d::double_t> values(2, 0.0);

		const k3d::iproperty_collection::properties_t& properties = k3d::node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(!dynamic_cast<k3d::iuser_property*>(*property))
				continue;

			if((**property).property_type() != typeid(k3d::double_t))
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
				continue;
			}

			variables += "," + (**property).property_name();
			values.push_back(k3d::property::pipeline_value<k3d::double_t>(**property));
		}

		k3d::expression::parser parser;
		if(!parser.parse(function, variables))
		{
			k3d::log() << error << factory().name() << ": function parsing failed: " << parser.last_parse_error() << std::endl;
			return;
		}

		const k3d::int32_t point_v_samples = m_v_samples.pipeline_value() + 1;
		const k3d::int32_t point_u_samples = m_u_samples.pipeline_value() + 1;
		const k3d::double_t u_size = m_u_size.pipeline_value();
		const k3d::double_t v_size = m_v_size.pipeline_value();
		const k3d::signed_axis orientation = m_orientation.pipeline_value();

		k3d::vector3 i, j, k;
		switch(orientation)
		{
			case k3d::PX:
				i = k3d::vector3(0, 1, 0);
				j = k3d::vector3(0, 0, 1);
				k = k3d::vector3(1, 0, 0);
				break;
			case k3d::NX:
				i = k3d::vector3(0, 1, 0);
				j = k3d::vector3(0, 0, -1);
				k = k3d::vector3(-1, 0, 0);
				break;
			case k3d::PY:
				i = k3d::vector3(0, 0, 1);
				j = k3d::vector3(1, 0, 0);
				k = k3d::vector3(0, 1, 0);
				break;
			case k3d::NY:
				i = k3d::vector3(0, 0, 1);
				j = k3d::vector3(-1, 0, 0);
				k = k3d::vector3(0, -1, 0);
				break;			
			case k3d::PZ:
				i = k3d::vector3(1, 0, 0);
				j = k3d::vector3(0, 1, 0);
				k = k3d::vector3(0, 0, 1);
				break;
			case k3d::NZ:
				i = k3d::vector3(1, 0, 0);
				j = k3d::vector3(0, -1, 0);
				k = k3d::vector3(0, 0, -1);
				break;
		}

		k3d::mesh::points_t::iterator point = const_cast<k3d::mesh::points_t&>(*Output.points).begin();
		for(k3d::int32_t row = 0; row != point_v_samples; ++row)
		{
			const k3d::double_t row_percent = k3d::ratio(row, point_v_samples - 1);
			
			for(k3d::int32_t column = 0; column != point_u_samples; ++column)
			{
				const k3d::double_t column_percent = k3d::ratio(column, point_u_samples - 1);

				const k3d::double_t u = k3d::mix(-0.5 * u_size, 0.5 * u_size, column_percent);
				const k3d::double_t v = k3d::mix(-0.5 * v_size, 0.5 * v_size, row_percent);

				values[0] = u;
				values[1] = v;
				const k3d::double_t w = parser.evaluate(&values[0]);

				*point++ = k3d::to_point((u * i) + (v * j) + (w * k));
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<surface_plot, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xf2434d00, 0xd54a4482, 0xaea14eb6, 0x40a967d3),
			"SurfacePlot",
			_("Creates a 3D surface plot."),
			"Math",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_function;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_u_samples;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_v_samples;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_u_size;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_v_size;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;

	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// surface_plot_factory

k3d::iplugin_factory& surface_plot_factory()
{
	return surface_plot::get_factory();
}

} // namespace plot

} // namespace module

