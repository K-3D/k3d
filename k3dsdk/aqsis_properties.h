#ifndef K3DSDK_AQSIS_PROPERTIES_H
#define K3DSDK_AQSIS_PROPERTIES_H

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
		\brief Declares objects that may be instantiated and attached to document plugin instances as "aqsis properties"
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "iaqsis.h"
#include "user_properties.h"

namespace k3d
{

namespace aqsis
{

/// Helper class used to set a data container's document
class document_initializer_t
{
public:
	explicit document_initializer_t(idocument& Document) :
		m_document(Document)
	{
	}

	idocument& document() const
	{
		return m_document;
	}

private:
	idocument& m_document;
};

inline const initializer_t<document_initializer_t> init_document(idocument& Document)
{
	return initializer_t<document_initializer_t>(document_initializer_t(Document));
}
/////////////////////////////////////////////////////////////////////////////
// layer_connection_property

/// Property policy that exposes a data container as an Aqsis layer connection property 
template<typename value_t, class name_policy_t>
class layer_connection_property :
	public data::node_property<value_t, name_policy_t>,
	public ilayer_connection_property
{
public:
	const std::string get_source_variable()
	{
		return m_source_variable.value();
	}

	void set_source_variable(const std::string& Variable)
	{
		m_source_variable.set_value(Variable);
	}

	source_variable_changed_signal_t& source_variable_changed_signal()
	{
		return m_source_variable.changed_signal();
	}

	const std::string get_target_variable()
	{
		return m_target_variable.value();
	}

	void set_target_variable(const std::string& Variable)
	{
		m_target_variable.set_value(Variable);
	}

	target_variable_changed_signal_t& target_variable_changed_signal()
	{
		return m_target_variable.changed_signal();
	}

protected:
	template<typename init_t>
	layer_connection_property(const init_t& Init) :
		data::node_property<value_t, name_policy_t>(Init),
		m_source_variable(init_document(Init.document()) + init_value<std::string>("Cout")),
		m_target_variable(init_document(Init.document()) + init_value<std::string>("Cin"))
	{
	}

private:
	k3d_data(std::string, no_name, change_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) m_source_variable;
	k3d_data(std::string, no_name, change_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) m_target_variable;
};

/////////////////////////////////////////////////////////////////////////////
// layer_connection_serialization

/// Serialization policy for data containers that store an aqsis layer connection and can be serialized as XML
template<typename value_t, class property_policy_t>
class layer_connection_serialization :
	public property_policy_t,
	public ipersistent
{
	// If this assertion fires, it means that you're trying to use layer_connection_serialization with a non-node data type
	BOOST_STATIC_ASSERT((boost::is_base_and_derived<iunknown, typename boost::remove_pointer<value_t>::type>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		std::string type;
		if(typeid(value_t) == typeid(idisplacement_layer*))
			type = "displacement_layer_connection";
		else if(typeid(value_t) == typeid(isurface_layer*))
			type = "surface_layer_connection";
		
		xml::element xml_storage(
			"property",
			property_policy_t::internal_node() ? string_cast(Context.lookup.lookup_id(property_policy_t::internal_node())) : "",
			xml::attribute("name", property_policy_t::name()),
			xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
			xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
			xml::attribute("type", type),
			xml::attribute("source", property_policy_t::get_source_variable()),
			xml::attribute("target", property_policy_t::get_target_variable()),
			xml::attribute("user_property", "aqsis"));
		
		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		std::string value = Element.text;
		property_policy_t::set_value(dynamic_cast<value_t>(Context.lookup.lookup_object(from_string(value, static_cast<ipersistent_lookup::id_type>(0)))));
		property_policy_t::set_source_variable(xml::attribute_text(Element, "source"));
		property_policy_t::set_target_variable(xml::attribute_text(Element, "target"));
	}

protected:
	template<typename init_t>
	layer_connection_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}
};

typedef user_property<k3d_data(idisplacement_layer*, immutable_name, change_signal, with_undo, node_storage, no_constraint, layer_connection_property, layer_connection_serialization)> displacement_layer_connection_property;
typedef user_property<k3d_data(isurface_layer*, immutable_name, change_signal, with_undo, node_storage, no_constraint, layer_connection_property, layer_connection_serialization)> surface_layer_connection_property;

} // namespace aqsis

} // namespace k3d

#endif // K3DSDK_AQSIS_PROPERTIES_H


