#ifndef K3DSDK_PROPERTIES_RI_H
#define K3DSDK_PROPERTIES_RI_H

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
		\brief Declares objects that may be instantiated and attached to document plugin instances as "renderman properties"
		\author Tim Shead (tshead@k-3d.com)
*/

#include "color.h"
#include "data.h"
#include "irenderman_property.h"
#include "tokens.h"
#include "types.h"
#include "user_properties.h"

namespace k3d
{

std::ostream& operator<<(std::ostream&, const irenderman_property::parameter_type_t);
std::istream& operator>>(std::istream&, irenderman_property::parameter_type_t);

namespace ri
{

/// Helper class used to set a renderman property's parameter list type
class parameter_list_type_t
{
public:
	explicit parameter_list_type_t(const irenderman_property::parameter_type_t Type) :
		m_type(Type)
	{
	}

	const irenderman_property::parameter_type_t parameter_list_type() const
	{
		return m_type;
	}

private:
	const irenderman_property::parameter_type_t m_type;
};

inline const initializer_t<parameter_list_type_t> init_parameter_list_type(const irenderman_property::parameter_type_t Type)
{
	return initializer_t<parameter_list_type_t>(parameter_list_type_t(Type));
}

/// Helper class used to set a renderman property's parameter list name
class parameter_list_name_t
{
public:
	explicit parameter_list_name_t(const char* const Name) :
		m_name(Name)
	{
	}

	const char* const parameter_list_name() const
	{
		return m_name;
	}

private:
	const char* const m_name;
};

inline const initializer_t<parameter_list_name_t> init_parameter_list_name(const char* const Name)
{
	return initializer_t<parameter_list_name_t>(parameter_list_name_t(Name));
}

/////////////////////////////////////////////////////////////////////////////
// renderman_property

/// Property policy that exposes a data container as a K-3D renderman property
template<typename value_t, class name_policy_t>
class renderman_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public irenderman_property
{
public:
	const value_t value()
	{
		iproperty* source = this;
		for(iproperty* dependency = m_dag.dependency(*source); dependency; dependency = m_dag.dependency(*dependency))
			source = dependency;

		if(source != this)
			return boost::any_cast<value_t>(source->property_value());

		return name_policy_t::internal_value();
	}

	const std::string property_name()
	{
		return name_policy_t::name();
	}

	const std::string property_label()
	{
		return m_label;
	}

	const std::string property_description()
	{
		return m_description;
	}

	const std::type_info& property_type()
	{
		return typeid(value_t);
	}

	const boost::any property_value()
	{
		return name_policy_t::internal_value();
	}

	const irenderman_property::parameter_type_t property_parameter_list_type()
	{
		return m_parameter_list_type;
	}

	const std::string property_parameter_list_name()
	{
		return m_parameter_list_name;
	}

	inode* property_node()
	{
		return m_node;
	}

	changed_signal_t& property_changed_signal()
	{
		return name_policy_t::changed_signal();
	}

	deleted_signal_t& property_deleted_signal()
	{
		return m_deleted_signal;
	}

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

protected:
	template<typename init_t>
	renderman_property(const init_t& Init) :
		name_policy_t(Init),
		m_dag(Init.document().dag()),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_parameter_list_type(Init.parameter_list_type()),
		m_parameter_list_name(Init.parameter_list_name())
	{
		Init.property_collection().register_property(*this);
	}

	~renderman_property()
	{
		m_deleted_signal.emit();
	}

private:
	idag& m_dag;
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const irenderman_property::parameter_type_t m_parameter_list_type;
	const char* const m_parameter_list_name;
	deleted_signal_t m_deleted_signal;
};

/////////////////////////////////////////////////////////////////////////////
// with_serialization

/// Serialization policy for data containers that can be serialized as XML
template<typename value_t, class property_policy_t>
class with_serialization :
	public property_policy_t,
	public ipersistent
{
	// This policy only works for data stored by-value
	BOOST_STATIC_ASSERT((!boost::is_pointer<value_t>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		xml::element xml_storage(
			"property",
			string_cast(property_policy_t::internal_value()),
			xml::attribute("name", property_policy_t::name()),
			xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
			xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
			xml::attribute("type", type_string<value_t>()),
			xml::attribute("parameter_type", string_cast(property_policy_t::property_parameter_list_type())),
			xml::attribute("parameter_name", property_policy_t::property_parameter_list_name()),
			xml::attribute("user_property", "renderman"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		std::string value = Element.text;
		property_policy_t::set_value(from_string(value, property_policy_t::internal_value()));
	}

protected:
	template<typename init_t>
	with_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}
};

typedef user_property<k3d_data(ri::integer, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> integer_property;
typedef user_property<k3d_data(ri::real, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> real_property;
typedef user_property<k3d_data(ri::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> string_property;
typedef user_property<k3d_data(ri::point, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> point_property;
typedef user_property<k3d_data(ri::vector, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> vector_property;
typedef user_property<k3d_data(ri::normal, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> normal_property;
typedef user_property<k3d_data(ri::hpoint, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> hpoint_property;
typedef user_property<k3d_data(ri::matrix, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> matrix_property;
typedef user_property<k3d_data(ri::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_property, with_serialization)> color_property;

template<typename PropertyT, typename ValueT>
PropertyT* create_property(const irenderman_property::parameter_type_t ParameterType, const std::string& ParameterName, const std::string& Name, const std::string& Label, const std::string& Description, idocument& Document, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode* const Object, const ValueT& Value)
{
	/// This is an ugly hack, but it allows user properties to be fully constructed before they're added to their property collection
	struct null_property_collection :
		public iproperty_collection
	{
		void register_property(iproperty& Property)
		{
		}

		void register_properties(const properties_t& Properties)
		{
		}

		void unregister_property(iproperty& Property)
		{
		}

		void unregister_properties(const properties_t& Properties)
		{
		}

		const properties_t& properties()
		{
			static properties_t properties;
			return properties;
		}

		sigc::connection connect_properties_changed_signal(const sigc::slot<void, iunknown*>& Slot)
		{
			return sigc::connection();
		}
	};

	null_property_collection property_collection;

	PropertyT* const property =
		new PropertyT(
			init_owner(Document, property_collection, PersistentContainer, Object)
			+ ri::init_parameter_list_type(ParameterType)
			+ ri::init_parameter_list_name(make_token(ParameterName.c_str()))
			+ init_name(make_token(Name.c_str()))
			+ init_label(make_token(Label.c_str()))
			+ init_description(make_token(Description.c_str()))
			+ init_value(Value));

	PropertyCollection.register_property(*property);

	return property;
}

} // namespace ri

} // namespace k3d

#endif // K3DSDK_PROPERTIES_RI_H

