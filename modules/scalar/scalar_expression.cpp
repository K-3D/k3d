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
	\author Anders Dahnielson (anders@dahnielson.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/expression/parser.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace libk3dcore
{

class scalar_expression :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	scalar_expression(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_expression(init_owner(*this) + init_name("expression") + init_label(_("Expression")) + init_description(_("Expression to be evaluated.")) + init_value(std::string(_("cos(pi/2)")))),
		m_output(init_owner(*this) + init_name("output") + init_label(_("Output")) + init_description(_("Output value.")) + init_slot(sigc::mem_fun(*this, &scalar_expression::get_value))),
		m_user_property_changed_signal(*this)
	{
		m_expression.changed_signal().connect(m_output.make_reset_slot());
		m_user_property_changed_signal.connect(m_output.make_reset_slot());
	}

	double get_value()
	{
		const std::string expression = m_expression.pipeline_value();

		std::string variables;
		std::vector<k3d::double_t> values;

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

			if(values.size())
				variables += ",";
			variables += (**property).property_name();
			values.push_back(k3d::property::pipeline_value<k3d::double_t>(**property));
		}

		k3d::expression::parser parser;
		if(!parser.parse(expression, variables))
		{
			k3d::log() << error << factory().name() << ": expression parsing failed: " << parser.last_parse_error() << std::endl;
			return 0.0;
		}

		return parser.evaluate(&values[0]);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_expression > factory(
			k3d::uuid(0x53de65d7, 0xee4a01e2, 0x259127b2, 0x676834f8),
			"ScalarExpression",
			_("Calculates a C-style expression, returning a scalar value as output"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_expression;
	k3d_data(double, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;

	k3d::user_property_changed_signal m_user_property_changed_signal;
};

k3d::iplugin_factory& scalar_expression_factory()
{
	return scalar_expression::get_factory();
}

} //namespace libk3dcore

