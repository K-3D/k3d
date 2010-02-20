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
		m_x_function(init_owner(*this) + init_name("x_function") + init_label(_("X Function")) + init_description(_("Output X coordinate function, in terms of x, y, z, and any user-defined scalars.")) + init_value(k3d::string_t(_("x")))),
		m_y_function(init_owner(*this) + init_name("y_function") + init_label(_("Y Function")) + init_description(_("Output Y coordinate function, in terms of x, y, z, and any user-defined scalars.")) + init_value(k3d::string_t(_("y")))),
		m_z_function(init_owner(*this) + init_name("z_function") + init_label(_("Z Function")) + init_description(_("Output Z coordinate function, in terms of x, y, z, and any user-defined scalars.")) + init_value(k3d::string_t(_("z + sin(x)")))),
		m_user_property_changed_signal(*this)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_x_function.changed_signal().connect(make_update_mesh_slot());
		m_y_function.changed_signal().connect(make_update_mesh_slot());
		m_z_function.changed_signal().connect(make_update_mesh_slot());
		m_user_property_changed_signal.connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		OutputPoints = InputPoints;

		// Setup default variables for x, y, and z coordinates ...
		std::string variables("x,y,z");
		std::vector<k3d::double_t> values(3, 0.0);

		// Collect additional, user-defined variables ...		
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
			
		k3d::expression::parser parser_x_component;
		if(!parser_x_component.parse(m_x_function.pipeline_value(), variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for x component failed: " << parser_x_component.last_parse_error() << std::endl;
			return;
		}
		k3d::expression::parser parser_y_component;
		if(!parser_y_component.parse(m_y_function.pipeline_value(), variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for y component failed: " << parser_y_component.last_parse_error() << std::endl;
			return;
		}
		k3d::expression::parser parser_z_component;
		if(!parser_z_component.parse(m_z_function.pipeline_value(), variables))
		{
			k3d::log() << error << factory().name() << ": function parsing for z component failed: " << parser_z_component.last_parse_error() << std::endl;
			return;
		}				
		
		//Evaluate functions on each point.
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + OutputPoints.size();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
		{
			if(!PointSelection[point])
				continue;

			values[0] = InputPoints[point].n[0];
			values[1] = InputPoints[point].n[1];
			values[2] = InputPoints[point].n[2];

			OutputPoints[point] = k3d::point3(
				parser_x_component.evaluate(&values[0]),
				parser_y_component.evaluate(&values[0]),
				parser_z_component.evaluate(&values[0])
				);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<deformation_expression,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x82777f90, 0xb74d7201, 0xcc5bed8a, 0x35275e00),
				"DeformationExpression",
				_("Displace a mesh using functional expressions in x, y, and z."),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
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

