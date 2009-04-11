// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include "scalar_source.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/expression/parser.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace scalar
{

class scalar_expression :
	public scalar_source
{
	typedef scalar_source base;
public:
	scalar_expression(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, _("Output value.")),
		m_expression(init_owner(*this) + init_name("expression") + init_label(_("Expression")) + init_description(_("Expression to be evaluated.")) + init_value(std::string(_("cos(pi/2)")))),
		m_user_property_changed_signal(*this)
	{
		m_expression.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_value_slot()));
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_value_slot()));
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
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_expression;
	k3d::user_property_changed_signal m_user_property_changed_signal;

	void on_update_value(k3d::double_t& Output)
	{
		const k3d::string_t expression = m_expression.pipeline_value();

		k3d::string_t variables;
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
			Output = 0.0;
			return;
		}

		Output = parser.evaluate(&values[0]);
	}

};

k3d::iplugin_factory& scalar_expression_factory()
{
	return scalar_expression::get_factory();
}

} //namespace scalar

} // namespace module

