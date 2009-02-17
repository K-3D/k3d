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
	\author Joaquin Duo (joaduo at lugmen org ar)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/expression/parser.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// deformation_expression

class deformation_expression :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	deformation_expression(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_xyz_rename(init_owner(*this) + init_name("xyz_rename") + init_label(_("Rename local x y z")) + init_description(_("Renames the local x,y,z variables to suit the variables from your function. Could be u,v,w for example")) + init_value(k3d::string_t(_("x,y,z")))),
		m_x_function(init_owner(*this) + init_name("x_function") + init_label(_("Local X component function")) + init_description(_("Function for the X component of the vector field.")) + init_value(k3d::string_t(_("x + x/sqrt(x^2+y^2+z^2) * exp(-(x^2+y^2+z^2))")))),
		m_y_function(init_owner(*this) + init_name("y_function") + init_label(_("Local Y component function")) + init_description(_("Function for the Y component of the vector field.")) + init_value(k3d::string_t(_("y + y/sqrt(x^2+y^2+z^2) * exp(-(x^2+y^2+z^2))")))),
		m_z_function(init_owner(*this) + init_name("z_function") + init_label(_("Local Z component function")) + init_description(_("Function for the Z component of the vector field.")) + init_value(k3d::string_t(_("z + z/sqrt(x^2+y^2+z^2) * exp(-(x^2+y^2+z^2))")))),
		m_user_property_changed_signal(*this)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_xyz_rename.changed_signal().connect(make_update_mesh_slot());
		m_x_function.changed_signal().connect(make_update_mesh_slot());
		m_y_function.changed_signal().connect(make_update_mesh_slot());
		m_z_function.changed_signal().connect(make_update_mesh_slot());
		m_user_property_changed_signal.connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const std::string xyz_rename = m_xyz_rename.pipeline_value();
		std::string variables(xyz_rename);
		std::vector<k3d::double_t> values(3, 0.0);
		
		//Collect additional variables
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
			values.push_back(k3d::property::pipeline_value<double>(**property));
		}
			
		const std::string x_function = m_x_function.pipeline_value();
		const std::string y_function = m_y_function.pipeline_value();
		const std::string z_function = m_z_function.pipeline_value();
		
		//Create all the three function parsers
		k3d::expression::parser parser_x_component;
		if(!parser_x_component.parse(x_function, variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for x component failed: " << parser_x_component.last_parse_error() << std::endl;
			return;
		}
		k3d::expression::parser parser_y_component;
		if(!parser_y_component.parse(y_function, variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for y component failed: " << parser_y_component.last_parse_error() << std::endl;
			return;
		}
		k3d::expression::parser parser_z_component;
		if(!parser_z_component.parse(z_function, variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for z component failed: " << parser_z_component.last_parse_error() << std::endl;
			return;
		}				
		
		OutputPoints = InputPoints;
		//Evaluate functions on each point.
		k3d::point3 output_point(0,0,0);
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + OutputPoints.size();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(PointSelection[point])
			{
				values[0] = InputPoints[point].n[0];
				values[1] = InputPoints[point].n[1];
				values[2] = InputPoints[point].n[2];
				output_point.n[0] = parser_x_component.evaluate(&values[0]);
				output_point.n[1] = parser_y_component.evaluate(&values[0]);
				output_point.n[2] = parser_z_component.evaluate(&values[0]);
				OutputPoints[point] = output_point;
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<deformation_expression,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x82777f90, 0xb74d7201, 0xcc5bed8a, 0x35275e00),
				"DeformationExpression",
				_("Displace a mesh according a expression on x,y and z points components"),
				"Deformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_xyz_rename;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_function;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_function;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_function;

	k3d::user_property_changed_signal m_user_property_changed_signal;
	
};

/////////////////////////////////////////////////////////////////////////////
// deformation_expression_factory

k3d::iplugin_factory& deformation_expression_factory()
{
	return deformation_expression::get_factory();
}

} // namespace deformation

} // namespace module

