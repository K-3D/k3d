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
	\brief Implements a 3D function plotter able to be animated.
	\author Timothy M. Shead (tshead@k-3d.com) Most of the code used for poly_grid
	\author Joaquín Duo (joaduo@lugmen.org.ar) Adaptation to the function parser
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <iterator>

#include <k3dsdk/log.h>
#include <k3dsdk/module.h>

// { warp function parser
#include "fpconfig.h"
#include "fparser.h"
// } warp function parser


namespace module
{

namespace plot
{

/////////////////////////////////////////////////////////////////////////////
// surface_plot

class surface_plot :
	public k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	surface_plot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_function(init_owner(*this) + init_name("function") + init_label(_("Function")) + init_description(_("Function to be plotted. C-like notation")) + init_value(std::string(_("cos(sqrt(u^2+v^2))")))),
		m_columns(init_owner(*this) + init_name("columns") + init_label(_("Columns")) + init_description(_("Column number")) + init_value(15) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_rows(init_owner(*this) + init_name("rows") + init_label(_("Rows")) + init_description(_("Row number")) + init_value(15) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Grid width")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Grid height")) + init_value(20.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_user_property_changed_signal(*this)
	{
		m_columns.changed_signal().connect(make_topology_changed_slot());
		m_rows.changed_signal().connect(make_topology_changed_slot());
		m_material.changed_signal().connect(make_topology_changed_slot());
		
		m_function.changed_signal().connect(make_full_geometry_changed_slot());
		m_width.changed_signal().connect(make_full_geometry_changed_slot());
		m_height.changed_signal().connect(make_full_geometry_changed_slot());
		m_orientation.changed_signal().connect(make_full_geometry_changed_slot());

		m_user_property_changed_signal.connect(make_full_geometry_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		Mesh = k3d::create_grid(m_rows.pipeline_value(), m_columns.pipeline_value(), m_material.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
		const std::string function = m_function.pipeline_value();

		std::string variables("u,v");
		std::vector<double> values(2, 0.0);

		const k3d::iproperty_collection::properties_t& properties = k3d::node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(!dynamic_cast<k3d::iuser_property*>(*property))
				continue;

			if((**property).property_type() != typeid(double))
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
				continue;
			}

			variables += "," + (**property).property_name();
			values.push_back(k3d::property::pipeline_value<double>(**property));
		}

		//Parse function and check if ok (<= -1)
		FunctionParser function_parser;
		if(function_parser.Parse(function, variables) > -1)
		{
			k3d::log() << error << factory().name() << ": function parsing failed: " << function_parser.ErrorMsg() << std::endl;
			return;
		}

		const unsigned long point_rows = m_rows.pipeline_value() + 1;
		const unsigned long point_columns = m_columns.pipeline_value() + 1;
		const double width = m_width.pipeline_value();
		const double height = m_height.pipeline_value();
		const k3d::signed_axis orientation = m_orientation.pipeline_value();

		k3d::vector3 i, j, k;
		switch(orientation)
		{
			case k3d::PX:
				i = k3d::vector3(0, -1, 0);
				j = k3d::vector3(0, 0, -1);
				k = k3d::vector3(1, 0, 0);
				break;
			case k3d::NX:
				i = k3d::vector3(0, 1, 0);
				j = k3d::vector3(0, 0, -1);
				k = k3d::vector3(-1, 0, 0);
				break;
			case k3d::NY:
				i = k3d::vector3(-1, 0, 0);
				j = k3d::vector3(0, 0, -1);
				k = k3d::vector3(0, -1, 0);
				break;
			case k3d::PY:
				i = k3d::vector3(1, 0, 0);
				j = k3d::vector3(0, 0, -1);
				k = k3d::vector3(0, 1, 0);
				break;
			case k3d::PZ:
				i = k3d::vector3(-1, 0, 0);
				j = k3d::vector3(0, -1, 0);
				k = k3d::vector3(0, 0, 1);
				break;
			case k3d::NZ:
				i = k3d::vector3(1, 0, 0);
				j = k3d::vector3(0, -1, 0);
				k = k3d::vector3(0, 0, -1);
				break;
		}

		k3d::mesh::points_t::iterator point = const_cast<k3d::mesh::points_t&>(*Mesh.points).begin();
		for(unsigned long row = 0; row != point_rows; ++row)
		{
			const double row_percent = static_cast<double>(row) / static_cast<double>(point_rows - 1);
			
			for(unsigned long column = 0; column != point_columns; ++column)
			{
				const double column_percent = static_cast<double>(column) / static_cast<double>(point_columns - 1);

				const double u = k3d::mix(-0.5 * width, 0.5 * width, column_percent);
				const double v = k3d::mix(-0.5 * height, 0.5 * height, row_percent);

				values[0] = u;
				values[1] = v;
				const double w = function_parser.Eval(&values[0]);

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
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_function;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_columns;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_rows;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;

	k3d::user_property_changed_signal m_user_property_changed_signal;
	
	sigc::slot<void, iunknown*> make_full_geometry_changed_slot()
	{
		return sigc::mem_fun(*this, &surface_plot::mesh_full_geometry_changed);
	}
	
	/// Indicate all geometry changed, regardless of selection
	void mesh_full_geometry_changed(iunknown* const Hint)
	{
		k3d::hint::mesh_geometry_changed_t hint;
		hint.full_change = true;
		m_output_mesh.update(&hint);
	}
};

/////////////////////////////////////////////////////////////////////////////
// surface_plot_factory

k3d::iplugin_factory& surface_plot_factory()
{
	return surface_plot::get_factory();
}

} // namespace plot

} // namespace module

