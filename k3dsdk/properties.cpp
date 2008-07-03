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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "iattribute_property_ri.h"
#include "idocument.h"
#include "inode.h"
#include "ioption_property_ri.h"
#include "iuser_property.h"
#include "metadata.h"
#include "properties.h"
#include "property_types.h"
#include "property_types_ri.h"
#include "result.h"
#include "tokens.h"
#include "type_registry.h"

#include <boost/mpl/for_each.hpp>


namespace k3d
{

namespace property
{

namespace detail
{

//////////////////////////////////////////////////////////////////////////////////////////
// user_property

/// Wraps a k3d_data object to mark it as a user property
template<typename property_t>
class user_property :
	public property_t,
	public iuser_property,
	public metadata
{
public:
	template<typename init_t>
	user_property(const init_t& Init) :
		property_t(Init)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////////////////
// null_property_collection

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

	sigc::connection connect_properties_changed_signal(const sigc::slot<void, ihint*>& Slot)
	{
		return sigc::connection();
	}
};

/////////////////////////////////////////////////////////////////////////////
// user_serialization

/// Serialization policy for generic user properties
template<typename value_t, class property_policy_t>
class user_serialization :
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
			xml::attribute("user_property", "generic"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		string_t value = Element.text;
		property_policy_t::set_value(from_string(value, property_policy_t::internal_value()), 0);
	}

protected:
	template<typename init_t>
	user_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_serialization

/// Serialization policy for generic user properties that store k3d::mesh
template<typename value_t, class property_policy_t>
class user_mesh_serialization :
	public property_policy_t,
	public ipersistent
{
	// This policy only works for data stored by-pointer
	BOOST_STATIC_ASSERT((boost::is_pointer<value_t>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		// User mesh properties should always be empty ...
		assert_error(0 == property_policy_t::internal_value());

		xml::element xml_storage(
			"property",
			string_cast(property_policy_t::internal_value()),
			xml::attribute("name", property_policy_t::name()),
			xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
			xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
			xml::attribute("type", type_string<value_t>()),
			xml::attribute("user_property", "generic"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		// User mesh properties should always be empty ...
	}

protected:
	template<typename init_t>
	user_mesh_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// user_node_serialization

/// Serialization policy for data containers that store a document node and can be serialized as XML
template<typename value_t, class property_policy_t>
class user_node_serialization :
	public property_policy_t,
	public ipersistent
{
	// If this assertion fires, it means that you're trying to use node_serialization with a non-node data type
	BOOST_STATIC_ASSERT((boost::is_base_and_derived<iunknown, typename boost::remove_pointer<value_t>::type>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		if(property_policy_t::internal_node())
		{
			Element.append(
				xml::element(
					"property",
					string_cast(Context.lookup.lookup_id(property_policy_t::internal_node())),
					xml::attribute("name", property_policy_t::name()),
					xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
					xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
					xml::attribute("type", type_string<value_t>()),
					xml::attribute("user_property", "generic")));
		}
		else
		{
			Element.append(
				xml::element(
					"property",
					"0",
					xml::attribute("name", property_policy_t::name()),
					xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
					xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
					xml::attribute("type", type_string<value_t>()),
					xml::attribute("user_property", "generic")));
		}
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		string_t value = Element.text;
		property_policy_t::set_value(dynamic_cast<value_t>(Context.lookup.lookup_object(from_string(value, static_cast<ipersistent_lookup::id_type>(0)))), 0);
	}

protected:
	template<typename init_t>
	user_node_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}

private:
	ipersistent_lookup::id_type m_node_id;
};

/////////////////////////////////////////////////////////////////////////////
// property_factory

/// Helper class for instantiating user property instances
class property_factory
{
public:
	property_factory(inode& Owner, iproperty_collection& PropertyCollection, ipersistent_collection& PersistentCollection, const std::type_info& Type, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value, iproperty*& Result) :
		owner(Owner),
		property_collection(PropertyCollection),
		persistent_collection(PersistentCollection),
		type(Type),
		name(Name),
		label(Label),
		description(Description),
		value(Value),
		result(Result)
	{
	}

	template<typename T, typename PropertyT>
	void create_property(const T& DefaultValue) const
	{
		if(result)
			return;

		if(typeid(T) != type)
			return;

		T initial_value = DefaultValue;
		if(!value.empty())
		{
			try
			{
				initial_value = boost::any_cast<T>(value);
			}
			catch(...)
			{
				log() << error << "error extracting default value for user property [" << name << "] with type [" << demangle(type) << "]" << std::endl;
				return;
			}
		}

		null_property_collection temp_property_collection;

		result = new PropertyT(
			init_owner(owner.document(), temp_property_collection, persistent_collection, &owner)
			+ init_name(make_token(name.c_str()))
			+ init_label(make_token(label.c_str()))
			+ init_description(make_token(description.c_str()))
			+ init_value(initial_value)
			);

		property_collection.register_property(*result);
	}

	template<typename T, typename PropertyT>
	void create_path_property(const T& DefaultValue) const
	{
		if(result)
			return;

		if(typeid(T) != type)
			return;

		T initial_value = DefaultValue;
		if(!value.empty())
		{
			try
			{
				initial_value = boost::any_cast<T>(value);
			}
			catch(...)
			{
				log() << error << "error extracting default value for user property [" << name << "] with type [" << demangle(type) << "]" << std::endl;
				return;
			}
		}

		null_property_collection temp_property_collection;

		result = new PropertyT(
			init_owner(owner.document(), temp_property_collection, persistent_collection, &owner)
			+ init_name(make_token(name.c_str()))
			+ init_label(make_token(label.c_str()))
			+ init_description(make_token(description.c_str()))
			+ init_value(initial_value)
			+ init_path_mode(k3d::ipath_property::WRITE)
			+ init_path_type("any")
			);

		property_collection.register_property(*result);
	}

	template<typename T>
	void operator()(T DefaultValue) const
	{
		create_property<T, user_property<k3d_data(T, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, user_serialization)> >(DefaultValue);
	}

	void operator()(filesystem::path DefaultValue) const
	{
		create_path_property<filesystem::path, user_property<k3d_data(filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization)> >(DefaultValue);
	}

	void operator()(mesh* DefaultValue) const
	{
		create_property<mesh*, user_property<k3d_data(mesh*, immutable_name, change_signal, with_undo, local_storage, no_constraint, read_only_property, user_mesh_serialization)> >(DefaultValue);
	}

	void operator()(legacy::mesh* DefaultValue) const
	{
		create_property<legacy::mesh*, user_property<k3d_data(legacy::mesh*, immutable_name, change_signal, with_undo, local_storage, no_constraint, read_only_property, user_mesh_serialization)> >(DefaultValue);
	}

	void operator()(imaterial* DefaultValue) const
	{
		create_property<imaterial*, user_property<k3d_data(imaterial*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, user_node_serialization)> >(DefaultValue);
	}

	void operator()(gl::imesh_painter* DefaultValue) const
	{
		create_property<gl::imesh_painter*, user_property<k3d_data(gl::imesh_painter*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, user_node_serialization)> >(DefaultValue);
	}

	void operator()(k3d::ri::imesh_painter* DefaultValue) const
	{
		create_property<k3d::ri::imesh_painter*, user_property<k3d_data(k3d::ri::imesh_painter*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, user_node_serialization)> >(DefaultValue);
	}

	void operator()(k3d::ri::itexture* DefaultValue) const
	{
		create_property<k3d::ri::itexture*, user_property<k3d_data(k3d::ri::itexture*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, user_node_serialization)> >(DefaultValue);
	}

	void operator()(k3d::inode* DefaultValue) const
	{
		create_property<k3d::inode*, user_property<k3d_data(k3d::inode*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, user_node_serialization)> >(DefaultValue);
	}

private:
	inode& owner;
	iproperty_collection& property_collection;
	ipersistent_collection& persistent_collection;
	const std::type_info& type;
	const string_t& name;
	const string_t& label;
	const string_t& description;
	const boost::any& value;
	iproperty*& result;
};

//////////////////////////////////////////////////////////////////////////////////////////
// renderman_parameter_list_name_t

/// Helper class used to set a renderman property's parameter list name
class renderman_parameter_list_name_t
{
public:
	explicit renderman_parameter_list_name_t(const char* const Name) :
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

inline const initializer_t<renderman_parameter_list_name_t> init_parameter_list_name(const char* const Name)
{
	return initializer_t<renderman_parameter_list_name_t>(renderman_parameter_list_name_t(Name));
}

/////////////////////////////////////////////////////////////////////////////
// renderman_attribute_property

/// Property policy that exposes a data container as a K-3D renderman property
template<typename value_t, class name_policy_t>
class renderman_attribute_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public k3d::ri::iattribute_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return boost::any_cast<value_t>(source->property_internal_value());

		return name_policy_t::internal_value();
	}

	const string_t property_name()
	{
		return name_policy_t::name();
	}

	const string_t property_label()
	{
		return m_label;
	}

	const string_t property_description()
	{
		return m_description;
	}

	const std::type_info& property_type()
	{
		return typeid(value_t);
	}

	const boost::any property_internal_value()
	{
		return name_policy_t::internal_value();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
	}

	const string_t property_attribute_name()
	{
		return m_attribute_name;
	}

	inode* property_node()
	{
		return m_node;
	}

	typename name_policy_t::changed_signal_t& property_changed_signal()
	{
		return name_policy_t::changed_signal();
	}

	deleted_signal_t& property_deleted_signal()
	{
		return m_deleted_signal;
	}

	iproperty* property_dependency()
	{
		return m_dependency;
	}

	void property_set_dependency(iproperty* Dependency)
	{
		m_dependency = Dependency;
	}

	bool property_set_value(const boost::any Value, ihint* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

protected:
	template<typename init_t>
	renderman_attribute_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_attribute_name(Init.parameter_list_name()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~renderman_attribute_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const char* const m_attribute_name;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// renderman_option_property

/// Property policy that exposes a data container as a K-3D renderman property
template<typename value_t, class name_policy_t>
class renderman_option_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public k3d::ri::ioption_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return boost::any_cast<value_t>(source->property_internal_value());

		return name_policy_t::internal_value();
	}

	const string_t property_name()
	{
		return name_policy_t::name();
	}

	const string_t property_label()
	{
		return m_label;
	}

	const string_t property_description()
	{
		return m_description;
	}

	const std::type_info& property_type()
	{
		return typeid(value_t);
	}

	const boost::any property_internal_value()
	{
		return name_policy_t::internal_value();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
	}

	const string_t property_option_name()
	{
		return m_option_name;
	}

	inode* property_node()
	{
		return m_node;
	}

	typename name_policy_t::changed_signal_t& property_changed_signal()
	{
		return name_policy_t::changed_signal();
	}

	deleted_signal_t& property_deleted_signal()
	{
		return m_deleted_signal;
	}

	iproperty* property_dependency()
	{
		return m_dependency;
	}

	void property_set_dependency(iproperty* Dependency)
	{
		m_dependency = Dependency;
	}

	bool property_set_value(const boost::any Value, ihint* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

protected:
	template<typename init_t>
	renderman_option_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_option_name(Init.parameter_list_name()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~renderman_option_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const char* const m_option_name;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// renderman_attribute_serialization

/// Serialization policy for data containers that can be serialized as XML
template<typename value_t, class property_policy_t>
class renderman_attribute_serialization :
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
			xml::attribute("parameter_type", "attribute"),
			xml::attribute("parameter_name", property_policy_t::property_attribute_name()),
			xml::attribute("user_property", "renderman"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		string_t value = Element.text;
		property_policy_t::set_value(from_string(value, property_policy_t::internal_value()));
	}

protected:
	template<typename init_t>
	renderman_attribute_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// renderman_option_serialization

/// Serialization policy for data containers that can be serialized as XML
template<typename value_t, class property_policy_t>
class renderman_option_serialization :
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
			xml::attribute("parameter_type", "option"),
			xml::attribute("parameter_name", property_policy_t::property_option_name()),
			xml::attribute("user_property", "renderman"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		string_t value = Element.text;
		property_policy_t::set_value(from_string(value, property_policy_t::internal_value()));
	}

protected:
	template<typename init_t>
	renderman_option_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// renderman_attribute_factory

/// Helper class for instantiating user property instances
class renderman_attribute_factory
{
public:
	renderman_attribute_factory(inode& Owner, iproperty_collection& PropertyCollection, ipersistent_collection& PersistentCollection, const std::type_info& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value, iproperty*& Result) :
		owner(Owner),
		property_collection(PropertyCollection),
		persistent_collection(PersistentCollection),
		type(Type),
		attribute_name(AttributeName),
		name(Name),
		label(Label),
		description(Description),
		value(Value),
		result(Result)
	{
	}

	template<typename T, typename PropertyT>
	void create_property() const
	{
		if(result)
			return;

		if(typeid(T) != type)
			return;

		T initial_value;
		if(!value.empty())
		try
		{
			initial_value = boost::any_cast<T>(value);
		}
		catch(...)
		{
			log() << error << "error extracting default value for attribute [" << name << "] with type [" << demangle(type) << "]" << std::endl;
			return;
		}

		null_property_collection temp_property_collection;

		result = new PropertyT(
			init_owner(owner.document(), temp_property_collection, persistent_collection, &owner)
			+ init_parameter_list_name(make_token(attribute_name.c_str()))
			+ init_name(make_token(name.c_str()))
			+ init_label(make_token(label.c_str()))
			+ init_description(make_token(description.c_str()))
			+ init_value(initial_value)
			);

		property_collection.register_property(*result);
	}

	template<typename T>
	void operator()(T) const
	{
		create_property<T, k3d::property::detail::user_property<k3d_data(T, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_attribute_property, renderman_attribute_serialization)> >();
	}

private:
	inode& owner;
	iproperty_collection& property_collection;
	ipersistent_collection& persistent_collection;
	const std::type_info& type;
	const string_t& attribute_name;
	const string_t& name;
	const string_t& label;
	const string_t& description;
	const boost::any& value;
	iproperty*& result;
};

/////////////////////////////////////////////////////////////////////////////
// renderman_option_factory

/// Helper class for instantiating user property instances
class renderman_option_factory
{
public:
	renderman_option_factory(inode& Owner, iproperty_collection& PropertyCollection, ipersistent_collection& PersistentCollection, const std::type_info& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value, iproperty*& Result) :
		owner(Owner),
		property_collection(PropertyCollection),
		persistent_collection(PersistentCollection),
		type(Type),
		option_name(OptionName),
		name(Name),
		label(Label),
		description(Description),
		value(Value),
		result(Result)
	{
	}

	template<typename T, typename PropertyT>
	void create_property() const
	{
		if(result)
			return;

		if(typeid(T) != type)
			return;

		T initial_value;
		if(!value.empty())
		try
		{
			initial_value = boost::any_cast<T>(value);
		}
		catch(...)
		{
			log() << error << "error extracting default value for option [" << name << "] with type [" << demangle(type) << "]" << std::endl;
			return;
		}

		null_property_collection temp_property_collection;

		result = new PropertyT(
			init_owner(owner.document(), temp_property_collection, persistent_collection, &owner)
			+ init_parameter_list_name(make_token(option_name.c_str()))
			+ init_name(make_token(name.c_str()))
			+ init_label(make_token(label.c_str()))
			+ init_description(make_token(description.c_str()))
			+ init_value(initial_value)
			);

		property_collection.register_property(*result);
	}

	template<typename T>
	void operator()(T) const
	{
		create_property<T, k3d::property::detail::user_property<k3d_data(T, immutable_name, change_signal, with_undo, local_storage, no_constraint, renderman_option_property, renderman_option_serialization)> >();
	}

private:
	inode& owner;
	iproperty_collection& property_collection;
	ipersistent_collection& persistent_collection;
	const std::type_info& type;
	const string_t& option_name;
	const string_t& name;
	const string_t& label;
	const string_t& description;
	const boost::any& value;
	iproperty*& result;
};

} // namespace detail

iproperty* get(iunknown& Object, const string_t& Name)
{
	// If the object doesn't have properties, we're done ...
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object);
	if(!property_collection)
		return 0;

	// Get the object's properties ...
	const iproperty_collection::properties_t properties(property_collection->properties());

	// Look for a property with a matching name ...
	for(iproperty_collection::properties_t::const_iterator property_iterator = properties.begin(); property_iterator != properties.end(); ++property_iterator)
	{
		// if the name matches, we're done ...
		if((*property_iterator)->property_name() == Name)
			return *property_iterator;
	}

	// No property by that name!
	return 0;
}

const boost::any internal_value(iproperty& Property)
{
	return Property.property_internal_value();
}

const boost::any internal_value(iunknown& Object, const string_t& Name)
{
	// Look for the property by name ...
	if(iproperty* const property = get(Object, Name))
		return internal_value(*property);

	return boost::any();
}

const boost::any pipeline_value(iproperty& Property)
{
	return Property.property_pipeline_value();
}

const boost::any pipeline_value(iunknown& Object, const string_t& Name)
{
	if(iproperty* const property = get(Object, Name))
		return pipeline_value(*property);

	return boost::any();
}

bool set_internal_value(iunknown& Object, const string_t Name, const boost::any& Value)
{
	// If the object isn't a property collection, we're done ...
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object);
	if(!property_collection)
	{
		log() << error << k3d_file_reference << " : object has no property collection!" << std::endl;
		return false;
	}

	// Look for a property with a matching name ...
	const iproperty_collection::properties_t properties(property_collection->properties());
	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		// Name doesn't match, so keep going ...
		if((*property)->property_name() != Name)
			continue;

		// Success ...
		return set_internal_value(**property, Value);
	}

	// Couldn't find a property by that name!
	log() << error << k3d_file_reference << " : could not find property [" << Name << "]" << std::endl;
	return false;
}

bool set_internal_value(iproperty& Property, const boost::any& Value)
{
	// If the property type doesn't match, we're done ...
	if(Value.type() != Property.property_type())
	{
		log() << error << "Value type [" << demangle(Value.type()) << "] doesn't match property [" << Property.property_name() << "] type [" << demangle(Property.property_type()) << "]" << std::endl;
		return false;
	}

	// If the property is read-only, we're done ...
	iwritable_property* const writable_property = dynamic_cast<iwritable_property*>(&Property);
	if(!writable_property)
	{
		log() << error << "Property [" << Property.property_name() << "] is not writable!" << std::endl;
		return false;
	}

	return writable_property->property_set_value(Value);
}

////////////////////////////////////////////////////////////////////////////////////////////
// user_properties

const std::vector<iproperty*> user_properties(iunknown& Object)
{
	std::vector<iproperty*> results;

	if(iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Object))
	{
		const iproperty_collection::properties_t& properties = property_collection->properties();
		for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(dynamic_cast<iuser_property*>(*property))
				results.push_back(*property);
		}
	}

	return results;
}

/////////////////////////////////////////////////////////////////////////////
// create

iproperty* create(inode& Owner, const std::type_info& Type, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Owner);
	return_val_if_fail(property_collection, 0);

	ipersistent_collection* const persistent_collection = dynamic_cast<ipersistent_collection*>(&Owner);
	return_val_if_fail(persistent_collection, 0);

	iproperty* result = 0;
	boost::mpl::for_each<k3d::property::types>(detail::property_factory(Owner, *property_collection, *persistent_collection, Type, Name, Label, Description, Value, result));

	return result;
}

/////////////////////////////////////////////////////////////////////////////
// create

iproperty* create(inode& Owner, const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	const std::type_info* const type_info = type_id(Type);
	return_val_if_fail(type_info, 0);

	return create(Owner, *type_info, Name, Label, Description, Value);
}

namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// create_attribute

iproperty* create_attribute(inode& Owner, const std::type_info& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Owner);
	return_val_if_fail(property_collection, 0);

	ipersistent_collection* const persistent_collection = dynamic_cast<ipersistent_collection*>(&Owner);
	return_val_if_fail(persistent_collection, 0);

	iproperty* result = 0;
	boost::mpl::for_each<k3d::property::ri::attribute_types>(k3d::property::detail::renderman_attribute_factory(Owner, *property_collection, *persistent_collection, Type, AttributeName, Name, Label, Description, Value, result));

	return result;
}

/////////////////////////////////////////////////////////////////////////////
// create_attribute

iproperty* create_attribute(inode& Owner, const string_t& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	const std::type_info* const type_info = type_id(Type);
	return_val_if_fail(type_info, 0);

	return create_attribute(Owner, *type_info, AttributeName, Name, Label, Description, Value);
}

/////////////////////////////////////////////////////////////////////////////
// create_option

iproperty* create_option(inode& Owner, const std::type_info& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(&Owner);
	return_val_if_fail(property_collection, 0);

	ipersistent_collection* const persistent_collection = dynamic_cast<ipersistent_collection*>(&Owner);
	return_val_if_fail(persistent_collection, 0);

	iproperty* result = 0;
	boost::mpl::for_each<k3d::property::ri::option_types>(k3d::property::detail::renderman_option_factory(Owner, *property_collection, *persistent_collection, Type, OptionName, Name, Label, Description, Value, result));

	return result;
}

/////////////////////////////////////////////////////////////////////////////
// create_option

iproperty* create_option(inode& Owner, const string_t& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description, const boost::any& Value)
{
	const std::type_info* const type_info = type_id(Type);
	return_val_if_fail(type_info, 0);

	return create_option(Owner, *type_info, OptionName, Name, Label, Description, Value);
}

} // namespace ri

} // namespace property

} // namespace k3d

