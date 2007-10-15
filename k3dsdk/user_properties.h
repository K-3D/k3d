#ifndef K3DSDK_USER_PROPERTIES_H
#define K3DSDK_USER_PROPERTIES_H

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
		\brief Declares objects that may be instantiated and attached to document plugin instances as "user properties"
		\author Tim Shead (tshead@k-3d.com)
*/

#include "algebra.h"
#include "color.h"
#include "data.h"
#include "ideletable.h"
#include "imesh_painter_gl.h"
#include "imesh_painter_ri.h"
#include "itexture_ri.h"
#include "iuser_property.h"
#include "legacy_mesh.h"
#include "mesh.h"
#include "path.h"
#include "tokens.h"
#include "type_registry.h"
#include "types_ri.h"

namespace k3d
{

/// Wraps a k3d_data object to mark it as a user property
template<typename property_t>
class user_property :
	public property_t,
	public iuser_property,
	public ideletable
{
public:
	template<typename init_t>
	user_property(const init_t& Init) :
		property_t(Init)
	{
	}
};

namespace user
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// with_serialization

/// Serialization policy for user data containers that can be serialized as XML
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
			xml::attribute("user_property", "vanilla"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		std::string value = Element.text;
		property_policy_t::set_value(from_string(value, property_policy_t::internal_value()), 0);
	}

protected:
	template<typename init_t>
	with_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_serialization

/// Serialization policy for user data containers that store mesh pointers and can be serialized as XML
template<typename value_t, class property_policy_t>
class mesh_serialization :
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
			xml::attribute("user_property", "vanilla"));

		Element.append(xml_storage);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		// User mesh properties should always be empty ...
	}

protected:
	template<typename init_t>
	mesh_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// node_serialization

/// Serialization policy for data containers that store a document node and can be serialized as XML
template<typename value_t, class property_policy_t>
class node_serialization :
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
					xml::attribute("user_property", "vanilla")));
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
					xml::attribute("user_property", "vanilla")));
		}
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		std::string value = Element.text;
		property_policy_t::set_value(dynamic_cast<value_t>(Context.lookup.lookup_object(from_string(value, static_cast<ipersistent_lookup::id_type>(0)))), 0);
	}

protected:
	template<typename init_t>
	node_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}

private:
	ipersistent_lookup::id_type m_node_id;
};

} // namespace detail

typedef user_property<k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> bool_property;
/*
typedef user_property<k3d_data(long, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> long_property;
typedef user_property<k3d_data(unsigned long, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> unsigned_long_property;
*/
typedef user_property<k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> double_property;
typedef user_property<k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> string_property;
typedef user_property<k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> point3_property;
typedef user_property<k3d_data(k3d::vector3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> vector3_property;
typedef user_property<k3d_data(k3d::normal3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> normal3_property;
typedef user_property<k3d_data(k3d::point4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> point4_property;
typedef user_property<k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> matrix4_property;
typedef user_property<k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, detail::with_serialization)> color_property;
typedef user_property<k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, detail::mesh_serialization)> legacy_mesh_property;
typedef user_property<k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, detail::mesh_serialization)> mesh_property;
typedef user_property<k3d_data(k3d::gl::imesh_painter*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, detail::node_serialization)> gl_mesh_painter_property;
typedef user_property<k3d_data(k3d::ri::imesh_painter*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, detail::node_serialization)> ri_mesh_painter_property;
typedef user_property<k3d_data(k3d::ri::itexture*, immutable_name, change_signal, with_undo, node_storage, no_constraint, k3d::data::node_property, detail::node_serialization)> ri_texture_property;
typedef user_property<k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization)> path_property;

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

/// Helper function for creating user properties
template<typename PropertyT, typename ValueT>
PropertyT* create_property(const std::string& Name, const std::string& Label, const std::string& Description, idocument& Document, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode* const Object, const ValueT& Value)
{
	null_property_collection property_collection;

	PropertyT* const property =
		new PropertyT(
			init_owner(Document, property_collection, PersistentContainer, Object)
			+ init_name(make_token(Name.c_str()))
			+ init_label(make_token(Label.c_str()))
			+ init_description(make_token(Description.c_str()))
			+ init_value(Value));

	PropertyCollection.register_property(*property);

	return property;
}

/// Specialization for path properties
template<>
inline path_property* create_property(const std::string& Name, const std::string& Label, const std::string& Description, idocument& Document, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode* const Object, const k3d::filesystem::path& Value)
{
	null_property_collection property_collection;

	path_property* const property =
			new path_property(
				init_owner(Document, property_collection, PersistentContainer, Object)
				+ init_name(make_token(Name.c_str()))
				+ init_label(make_token(Label.c_str()))
				+ init_description(make_token(Description.c_str()))
				+ init_value(Value)
				+ init_path_mode(k3d::ipath_property::WRITE)
				+ init_path_type("any"));

		PropertyCollection.register_property(*property);

		return property;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// property_container

/// istate_container implementation that handles undo/redo for the set of user properties within a property collection
class property_container :
	public istate_container
{
public:
	property_container(iunknown& Owner);
	void restore_state();

private:
	iproperty_collection* const m_property_collection;
	ipersistent_container* const m_persistent_container;
	iproperty_collection::properties_t m_user_properties;
	ipersistent_container::named_children_t m_persistent_properties;
};

} // namespace user

} // namespace k3d

#endif // K3DSDK_USER_PROPERTIES_H

