#ifndef K3DSDK_DATA_H
#define K3DSDK_DATA_H

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

#include "idocument.h"
#include "ienumeration_property.h"
#include "ilist_property.h"
#include "imeasurement_property.h"
#include "inode.h"
#include "inode_change_signal.h"
#include "inode_collection.h"
#include "inode_collection_property.h"
#include "inode_property.h"
#include "ipath_property.h"
#include "ipersistent.h"
#include "ipersistent_container.h"
#include "ipersistent_lookup.h"
#include "iplugin_factory.h"
#include "iproperty.h"
#include "iproperty_collection.h"
#include "iscript_property.h"
#include "istate_container.h"
#include "istate_recorder.h"
#include "iwritable_property.h"
#include "nodes.h"
#include "result.h"
#include "state_change_set.h"
#include "xml.h"

#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/version.hpp>

#include <cassert>
#include <string>
#include <typeinfo>

namespace k3d
{

namespace data
{

/////////////////////////////////////////////////////////////////////////////
// container

/// Host class for storing data using serialization, property, name, constraint, undo, storage, and signal policies
template<typename value_t, class serialization_policy_t>
class container :
	public serialization_policy_t
{
public:
	template<typename init_t>
	container(const init_t& Init) :
		serialization_policy_t(Init)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// no_serialization

/// Serialization policy for data containers that do not need to be serialized
template<typename value_t, class property_policy_t>
class no_serialization :
	public property_policy_t
{
protected:
	template<typename init_t>
	no_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
	}
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
		Element.append(xml::element("property", string_cast(property_policy_t::internal_value()), xml::attribute("name", property_policy_t::name())));
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

/////////////////////////////////////////////////////////////////////////////
// path_serialization

void save_external_resource(xml::element& Element, const ipersistent::save_context& Context, const std::string& Name, const ipath_property::reference_t Reference, const filesystem::path& Value);
void load_external_resource(xml::element& Element, const ipersistent::load_context& Context, ipath_property::reference_t& Reference, filesystem::path& Value);

/// Serialization policy for filesystem path data that handles external filesystem resources
template<typename value_t, class property_policy_t>
class path_serialization :
	public property_policy_t,
	public ipersistent
{
	// This policy only works for data stored by-value
	BOOST_STATIC_ASSERT((!boost::is_pointer<value_t>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		save_external_resource(Element, Context, property_policy_t::name(), property_policy_t::property_path_reference(), property_policy_t::internal_value());
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		ipath_property::reference_t reference;
		filesystem::path value;

		load_external_resource(Element, Context, reference, value);

		property_policy_t::set_property_path_reference(reference);
		property_policy_t::set_value(value);
	}

protected:
	template<typename init_t>
	path_serialization(const init_t& Init) :
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
			Element.append(xml::element("property", string_cast(Context.lookup.lookup_id(property_policy_t::internal_node())), xml::attribute("name", property_policy_t::name())));
		else
			Element.append(xml::element("property", "0", xml::attribute("name", property_policy_t::name())));
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		std::string value = Element.text;
		property_policy_t::set_value(dynamic_cast<value_t>(Context.lookup.lookup_object(from_string(value, static_cast<ipersistent_lookup::id_type>(0)))));
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

/////////////////////////////////////////////////////////////////////////////
// node_collection_serialization

/// Serialization policy for data containers that can be serialized as XML
template<typename value_t, class property_policy_t>
class node_collection_serialization :
	public property_policy_t,
	public ipersistent
{
public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		std::stringstream buffer;

		const inode_collection_property::nodes_t& nodes = property_policy_t::internal_value();
		for(inode_collection_property::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if(*node)
				buffer << " " << string_cast(Context.lookup.lookup_id(*node));
			else
				buffer << " 0";
		}

		Element.append(xml::element("property", buffer.str(), xml::attribute("name", property_policy_t::name())));
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		inode_collection_property::nodes_t nodes;

		std::stringstream buffer(Element.text);
		std::string node;
		while(buffer >> node)
			nodes.push_back(dynamic_cast<inode*>(Context.lookup.lookup_object(from_string(node, static_cast<ipersistent_lookup::id_type>(0)))));
		nodes.erase(std::remove(nodes.begin(), nodes.end(), static_cast<inode*>(0)), nodes.end());

		property_policy_t::set_value(nodes);
	}

protected:
	template<typename init_t>
	node_collection_serialization(const init_t& Init) :
		property_policy_t(Init)
	{
		Init.persistent_container().enable_serialization(Init.name(), *this);
	}
};

/////////////////////////////////////////////////////////////////////////////
// property_lookup

/** \brief Encapsulates the lookup process for connected properties
	\note In the case of circular dependencies, returns the same value as the input property
*/
iproperty* property_lookup(iproperty* const Source);

/////////////////////////////////////////////////////////////////////////////
// no_property

/// Property policy for data containers that do not expose their values using the K-3D property mechanism
template<typename value_t, class name_policy_t>
class no_property :
	public name_policy_t
{
public:

protected:
	template<typename init_t>
	no_property(const init_t& Init) :
		name_policy_t(Init)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// read_only_property

/// Property policy that exposes a data container as a read-only K-3D property
template<typename value_t, class name_policy_t>
class read_only_property :
	public name_policy_t,
	public iproperty
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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

	const boost::any property_internal_value()
	{
		return name_policy_t::internal_value();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

protected:
	template<typename init_t>
	read_only_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~read_only_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// writable_property

/// Property policy that exposes a data container as a writable K-3D property
template<typename value_t, class name_policy_t>
class writable_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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

	const boost::any property_internal_value()
	{
		return name_policy_t::internal_value();
	}
	
	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

	iproperty* property_dependency()
	{
		return m_dependency;
	}

	void property_set_dependency(iproperty* Dependency)
	{
		m_dependency = Dependency;
	}

protected:
	template<typename init_t>
	writable_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~writable_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// string_property

/// Property policy that exposes a data container as a writable string property
template<typename value_t, class name_policy_t>
class string_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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
		return typeid(std::string);
	}

	const boost::any property_internal_value()
	{
		try
		{
			return boost::any(boost::lexical_cast<std::string>(name_policy_t::internal_value()));
		}
		catch(...)
		{
		}

		return boost::any();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const std::string* const new_value = boost::any_cast<std::string>(&Value);
		if(!new_value)
			return false;

		try
		{
			set_value(boost::lexical_cast<value_t>(*new_value), Hint);
			return true;
		}
		catch(...)
		{
		}

		return false;
	}

protected:
	template<typename init_t>
	string_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~string_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// path_property

/// Property policy that exposes a data container as a writable K-3D path property
template<typename value_t, class name_policy_t>
class path_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public ipath_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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

	const boost::any property_internal_value()
	{
		return name_policy_t::internal_value();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;
		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

	const ipath_property::mode_t property_path_mode()
	{
		return m_mode;
	}

	const std::string property_path_type()
	{
		return m_type;
	}

	const ipath_property::reference_t property_path_reference()
	{
		return m_reference;
	}

	void set_property_path_reference(const ipath_property::reference_t Reference)
	{
		if(Reference != m_reference)
		{
			m_reference = Reference;
			m_reference_changed_signal.emit();
		}
	}

	ipath_property::path_reference_changed_signal_t& property_path_reference_changed_signal()

	{
		return m_reference_changed_signal;
	}

	const ipath_property::pattern_filters_t pattern_filters()
	{
		return m_pattern_filters;
	}

	void add_pattern_filter(const ipath_property::pattern_filter& PatternFilter)
	{
		m_pattern_filters.push_back(PatternFilter);
	}

protected:
	template<typename init_t>
	path_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_mode(Init.path_mode()),
		m_type(Init.path_type()),
		m_reference(RELATIVE_REFERENCE),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~path_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	const ipath_property::mode_t m_mode;
	const std::string m_type;
	ipath_property::reference_t m_reference;
	ipath_property::path_reference_changed_signal_t m_reference_changed_signal;
	ipath_property::pattern_filters_t m_pattern_filters;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// script_property

/// Property policy that exposes a data container as a writable K-3D script property
template<typename value_t, class name_policy_t>
class script_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public iscript_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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
		return typeid(std::string);
	}

	const boost::any property_internal_value()
	{
		try
		{
			return boost::any(boost::lexical_cast<std::string>(name_policy_t::internal_value()));
		}
		catch(...)
		{
		}

		return boost::any();
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const std::string* const new_value = boost::any_cast<std::string>(&Value);
		if(!new_value)
			return false;

		try
		{
			set_value(boost::lexical_cast<value_t>(*new_value), Hint);
			return true;
		}
		catch(...)
		{
		}

		return false;
	}

protected:
	template<typename init_t>
	script_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~script_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// enumeration_property

/// Property policy that exposes a data container as a K-3D enumeration property
template<typename value_t, class name_policy_t>
class enumeration_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public ienumeration_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
		{
			try
			{
				// Because enumeration properties all have type "string", we have to convert the source property into our internal type (which
				// could fail) before applying any constraints.
				return name_policy_t::constrain_value(boost::lexical_cast<value_t>(boost::any_cast<string_t>(source->property_internal_value())));
			}
			catch(...)
			{
				return name_policy_t::internal_value();
			}
		}

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
		return typeid(string_t);
	}

	const boost::any property_internal_value()
	{
		try
		{
			return boost::any(boost::lexical_cast<string_t>(name_policy_t::internal_value()));
		}
		catch(...)
		{
		}

		return boost::any();
	}

	const boost::any property_pipeline_value()
	{
		try
		{
			return boost::any(boost::lexical_cast<string_t>(pipeline_value()));
		}
		catch(...)
		{
		}

		return boost::any();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const string_t* const new_value = boost::any_cast<string_t>(&Value);
		if(!new_value)
			return false;

		try
		{
			set_value(boost::lexical_cast<value_t>(*new_value), Hint);
			return true;
		}
		catch(...)
		{
		}

		return false;
	}

	enumeration_values_t enumeration_values()
	{
		return m_values;
	}

	sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot)
	{
		return m_values_changed_signal.connect(Slot);
	}

	void notify_enumeration_values_changed()
	{
		m_values_changed_signal.emit();
	}

protected:
	template<typename init_t>
	enumeration_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_values(Init.values()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~enumeration_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const ienumeration_property::enumeration_values_t& m_values;
	sigc::signal<void> m_values_changed_signal;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// list_property

/// Property policy that exposes a data container as a K-3D list property
template<typename value_t, class name_policy_t>
class list_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public ilist_property<value_t>
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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

	const boost::any property_internal_value()
	{
		return boost::any(name_policy_t::internal_value());
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		set_value(*new_value, Hint);
		return true;
	}

	typename ilist_property<value_t>::values_t property_values()
	{
		return m_values;
	}

protected:
	template<typename init_t>
	list_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_values(Init.values()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~list_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const typename ilist_property<value_t>::values_t& m_values;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// node_property

/// Property policy that exposes a data container as a K-3D node property
template<typename value_t, class name_policy_t>
class node_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public inode_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(dynamic_cast<value_t>(boost::any_cast<inode*>(source->property_internal_value())));

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
		return typeid(inode*);
	}

	const boost::any property_internal_value()
	{
		return boost::any(name_policy_t::internal_node());
	}

	const boost::any property_pipeline_value()
	{
		return dynamic_cast<inode*>(pipeline_value());
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		inode* const * new_value = boost::any_cast<inode*>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(dynamic_cast<value_t>(*new_value), Hint);
		return true;
	}

	bool property_allow_none()
	{
		return name_policy_t::allow_none();
	}

	bool property_allow(iplugin_factory& Factory)
	{
		return name_policy_t::allow(Factory);
	}

	bool property_allow(inode& Object)
	{
		return name_policy_t::allow(Object);
	}

protected:
	template<typename init_t>
	node_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~node_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// measurement_property

/// Property policy that exposes a data container as a K-3D measurement property
template<typename value_t, class name_policy_t>
class measurement_property :
	public name_policy_t,
	public iproperty,
	public iwritable_property,
	public imeasurement_property
{
public:
	const value_t pipeline_value()
	{
		iproperty* const source = property_lookup(this);
		if(source != this)
			return name_policy_t::constrain_value(boost::any_cast<value_t>(source->property_internal_value()));

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

	const boost::any property_internal_value()
	{
		return boost::any(name_policy_t::internal_value());
	}

	const boost::any property_pipeline_value()
	{
		return pipeline_value();
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

	bool property_set_value(const boost::any Value, iunknown* const Hint)
	{
		const value_t* const new_value = boost::any_cast<value_t>(&Value);
		if(!new_value)
			return false;

		name_policy_t::set_value(*new_value, Hint);
		return true;
	}

	const double property_step_increment()
	{
		return m_step_increment;
	}

	const std::type_info& property_units()
	{
		return m_units;
	}

protected:
	template<typename init_t>
	measurement_property(const init_t& Init) :
		name_policy_t(Init),
		m_node(Init.node()),
		m_label(Init.label()),
		m_description(Init.description()),
		m_step_increment(Init.step_increment()),
		m_units(Init.units()),
		m_dependency(0)
	{
		Init.property_collection().register_property(*this);
	}

	~measurement_property()
	{
		m_deleted_signal.emit();
	}

private:
	inode* const m_node;
	const char* const m_label;
	const char* const m_description;
	const double m_step_increment;
	const std::type_info& m_units;
	deleted_signal_t m_deleted_signal;
	iproperty* m_dependency;
};

/////////////////////////////////////////////////////////////////////////////
// no_name

/// Name policy for unnamed data containers
template<class constraint_policy_t>
class no_name :
	public constraint_policy_t
{
protected:
	template<typename init_t>
	no_name(const init_t& Init) :
		constraint_policy_t(Init)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// immutable_name

/// Name policy for data containers whose name cannot be changed
template<class constraint_policy_t>
class immutable_name :
	public constraint_policy_t
{
public:
	/// Returns the container name
	const std::string name() const
	{
		return m_name;
	}

protected:
	template<typename init_t>
	immutable_name(const init_t& Init) :
		constraint_policy_t(Init),
		m_name(Init.name())
	{
	}

private:
	/// Storage for the container name
	const char* const m_name;
};

/////////////////////////////////////////////////////////////////////////////
// no_constraint

/// Constraint policy for data containers whose values are unconstrained
template<typename value_t, class undo_policy_t>
class no_constraint :
	public undo_policy_t
{
public:
	void set_value(const value_t& Value, iunknown* const Hint = 0)
	{
		if(Value != undo_policy_t::internal_value())
			undo_policy_t::set_value(Value, Hint);
	}

	const value_t constrain_value(const value_t Value)
	{
		return Value;
	}

protected:
	template<typename init_t>
	no_constraint(const init_t& Init) :
		undo_policy_t(Init)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// iconstraint

/// Abstract interface for a chain-of-responsibility constraint node
template<typename value_t>
class iconstraint
{
public:
	virtual ~iconstraint() {}

	void constrain(value_t& Value)
	{
		on_constrain(Value);

		if(m_next_constraint.get())
			m_next_constraint->constrain(Value);
	}

protected:
	explicit iconstraint(iconstraint<value_t>* NextConstraint) :
		m_next_constraint(NextConstraint)
	{
	}

private:
	/// Implement this method in a derived class to modify / constrain the input Value
	virtual void on_constrain(value_t& Value) = 0;

	/// Storage for the (optional) next constraint to apply in the chain
	const std::auto_ptr<iconstraint<value_t> > m_next_constraint;
};

namespace constraint
{

/////////////////////////////////////////////////////////////////////////////
// minimum_t

/// Constraint node that enforces a minimum value (using the std::max concept)
template<typename value_t>
class minimum_t :
	public iconstraint<value_t>
{
	// We require that the minimum constraint be applied only to signed types, to prevent
	// overflow problems associated with unsigned types (for example, if you had a minimum
	// constraint of 0 applied to an unsigned long with current value 0, and subtract 1, the
	// result will 0xffffffff, which compares as larger than 0)
	BOOST_STATIC_ASSERT((std::numeric_limits<value_t>::is_signed));

public:
	minimum_t(const value_t MinimumValue, iconstraint<value_t>* NextConstraint = 0) :
		iconstraint<value_t>(NextConstraint),
		m_minimum_value(MinimumValue)
	{
	}

private:
	void on_constrain(value_t& Value)
	{
		Value = std::max(Value, m_minimum_value);
	}

	const value_t m_minimum_value;
};

/////////////////////////////////////////////////////////////////////////////
// minimum

/// Convenience factory function for creating minimum_t constraint nodes
template<typename value_t>
iconstraint<value_t>* minimum(const value_t MinimumValue, iconstraint<value_t>* NextConstraint = 0)
{
	return new minimum_t<value_t>(MinimumValue, NextConstraint);
}

/////////////////////////////////////////////////////////////////////////////
// maximum_t

/// Constraint node that enforces a maximum value (using the std::min concept)
template<typename value_t>
class maximum_t :
	public iconstraint<value_t>
{
public:
	maximum_t(const value_t MaximumValue, iconstraint<value_t>* NextConstraint = 0) :
		iconstraint<value_t>(NextConstraint),
		m_maximum_value(MaximumValue)
	{
	}

private:
	void on_constrain(value_t& Value)
	{
		Value = std::min(Value, m_maximum_value);
	}

	const value_t m_maximum_value;
};

/////////////////////////////////////////////////////////////////////////////
// maximum

/// Convenience factory function for creating maximum_t constraint nodes
template<typename value_t>
iconstraint<value_t>* maximum(const value_t MaximumValue, iconstraint<value_t>* NextConstraint = 0)
{
	return new maximum_t<value_t>(MaximumValue, NextConstraint);
}

} // namespace constraint

/////////////////////////////////////////////////////////////////////////////
// with_constraint

/// Policy for data nodes whose values are constrained by a chain of one-to-many iconstraint nodes
template<typename value_t, class undo_policy_t>
class with_constraint :
	public undo_policy_t
{
public:
	void set_value(value_t Value, iunknown* const Hint = 0)
	{
		m_constraint->constrain(Value);

		if(Value != undo_policy_t::internal_value())
			undo_policy_t::set_value(Value, Hint);
	}

	const value_t constrain_value(value_t Value)
	{
		m_constraint->constrain(Value);
		return Value;
	}

protected:
	template<typename init_t>
	with_constraint(const init_t& Init) :
		undo_policy_t(Init),
		m_constraint(Init.constraint())
	{
		assert(m_constraint.get());
	}

private:
	/// Stores the (mandatory) chain of constraint nodes to apply to incoming values
	const std::auto_ptr<iconstraint<value_t> > m_constraint;
};

/////////////////////////////////////////////////////////////////////////////
// no_undo

/// Undo policy for data containers that do not store state changes for undo/redo
template<typename value_t, class storage_policy_t>
class no_undo :
	public storage_policy_t
{
protected:
	template<typename init_t>
	no_undo(const init_t& Init) :
		storage_policy_t(Init)
	{
	}

	/// This little bit of magic makes it possible for base classes (such as node_storage) to update their own values while observing the correct undo policy
	void internal_set_value(const value_t& Value, iunknown* const Hint)
	{
		set_value(Value, Hint);
	}
};

/////////////////////////////////////////////////////////////////////////////
// with_undo

/// Undo policy for data containers that stores state changes for undo/redo
template <typename value_t, class storage_policy_t>
class with_undo :
	public storage_policy_t,
	public virtual sigc::trackable
{
public:
	/// Returns true iff the next write to the underlying storage will generate undo/redo data (useful if you need to perform additional undo/redo related operations, such as creating a signal connection)
	bool ready_to_record()
	{
		return !m_changes && m_state_recorder.current_change_set();
	}

	/// Returns the state recorder for storing undo/redo data
	istate_recorder& state_recorder()
	{
		return m_state_recorder;
	}

protected:
	template<typename init_t>
	with_undo(const init_t& Init) :
		storage_policy_t(Init),
		m_state_recorder(Init.document().state_recorder()),
		m_changes(false)
	{
	}

	typedef with_undo<value_t, storage_policy_t> this_t;

	/// This little bit of magic makes it possible for base classes (such as node_storage) to update their own values while observing the correct undo policy
	void internal_set_value(const value_t& Value, iunknown* const Hint)
	{
		set_value(Value, Hint);
	}

	void set_value(const value_t& Value, iunknown* const Hint = 0)
	{
		if(ready_to_record())
		{
			m_changes = true;
			/// Note ... we don't save the connection here because connections are automatically disconnected when the signal is emitted
			m_state_recorder.connect_recording_done_signal(sigc::mem_fun(*this, &this_t::on_recording_done));
			storage_policy_t::start_recording(m_state_recorder);
		}

		storage_policy_t::set_value(Value, Hint);
	}

private:
	/// Called by the signal system once undo/redo recording is complete, so we can record our final value for redo purposes
	void on_recording_done()
	{
		// Sanity check ...
		assert(m_changes);
		assert(m_state_recorder.current_change_set());

		m_changes = false;
		storage_policy_t::finish_recording(m_state_recorder);
	}

	/// Central location where undo/redo data is stored
	istate_recorder& m_state_recorder;
	/// Set to true iff the underlying data has changed during undo/redo recording
	bool m_changes;
};

/////////////////////////////////////////////////////////////////////////////
// local_storage

/// Storage policy for data containers that store their state by value
template<typename value_t, class signal_policy_t>
class local_storage :
	public signal_policy_t
{
public:
	/// Writable access to the underlying data - this is handy for working with STL containers, but be careful - writing data in this way will bypass signal and undo policies
	value_t& internal_value()
	{
		return m_value;
	}

	/// Read-only access to the underlying data
	const value_t& internal_value() const
	{
		return m_value;
	}

protected:
	template<typename init_t>
	local_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_value(Init.value())
	{
	}

	/// Optionally called to store the original state of the data prior to modification
	void start_recording(istate_recorder& StateRecorder)
	{
		signal_policy_t::start_recording(StateRecorder);
		StateRecorder.current_change_set()->record_old_state(new value_container(m_value));
	}

	/// Sets a new value for the data
	void set_value(const value_t& Value, iunknown* const Hint = 0)
	{
		m_value = Value;
		signal_policy_t::set_value(Hint);
	}

	/// Optionally called to store the new state of the data after one-or-more modifications
	void finish_recording(istate_recorder& StateRecorder)
	{
		StateRecorder.current_change_set()->record_new_state(new value_container(m_value));
		signal_policy_t::finish_recording(StateRecorder);
	}

private:
	/// Local storage for the data stored by this policy
	value_t m_value;

	/// Provides an implementation of istate_container for storing nodes by value (ValueType must have a copy constructor and assignment operator)
	class value_container :
		public istate_container
	{
	public:
		value_container(value_t& Instance) :
			m_instance(Instance),
			m_value(Instance)
		{
		}

		void restore_state()
		{
			m_instance = m_value;
		}

	private:
		value_t& m_instance;
		const value_t m_value;
	};
};

/////////////////////////////////////////////////////////////////////////////
// node_storage

/// Storage policy for data containers that store a pointer to a document node that could be deleted by the user at any time - note that value_t should be the interface type used to access the node
template<typename value_t, class signal_policy_t>
class node_storage :
	public signal_policy_t,
	public virtual sigc::trackable
{
public:
	/// Returns an interface pointer to the node (could be NULL)
	value_t internal_value()
	{
		return dynamic_cast<value_t>(m_node);
	}

	/// Returns the underlying node (could be NULL)
	inode* internal_node()
	{
		return m_node;
	}

	/// Returns true iff the value is allowed to be NULL
	bool allow_none()
	{
		return true;
	}

	/// Returns true iff the given factory creates an node that could be stored by this container
	bool allow(iplugin_factory& Factory)
	{
		return Factory.implements(typeid(typename boost::remove_pointer<value_t>::type));
	}

	/// Returns true iff the given node coult be stored by this container
	bool allow(inode& Object)
	{
		return dynamic_cast<value_t>(&Object) ? true : false;
	}

protected:
	template<typename init_t>
	node_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_node(dynamic_cast<inode*>(Init.value()))
	{
		if(m_node)
		{
			m_node_deleted_connection = m_node->deleted_signal().connect(sigc::mem_fun(*this, &node_storage::on_node_deleted));
			if(inode_change_signal* const node_change_signal = dynamic_cast<inode_change_signal*>(m_node))
				m_node_changed_connection = node_change_signal->node_changed_signal().connect(signal_policy_t::changed_signal().make_slot());
		}
	}

	virtual ~node_storage()
	{
	}

	void on_node_deleted()
	{
		internal_set_value(0, 0);
	}

	/// Optionally called to store the original state of the data prior to modification
	void start_recording(istate_recorder& StateRecorder)
	{
		signal_policy_t::start_recording(StateRecorder);
		StateRecorder.current_change_set()->record_old_state(new value_container(m_node));
	}

	/// Sets a new value for the data
	void set_value(value_t Value, iunknown* const Hint)
	{
		if(m_node)
		{
			m_node_deleted_connection.disconnect();
			m_node_changed_connection.disconnect();
		}

		m_node = dynamic_cast<inode*>(Value);

		if(m_node)
		{
			m_node_deleted_connection = m_node->deleted_signal().connect(sigc::mem_fun(*this, &node_storage::on_node_deleted));
			if(inode_change_signal* const node_change_signal = dynamic_cast<inode_change_signal*>(m_node))
				m_node_changed_connection = node_change_signal->node_changed_signal().connect(signal_policy_t::changed_signal().make_slot());
		}

		signal_policy_t::set_value(Hint);
	}

	/// Optionally called to store the new state of the data after one-or-more modifications
	void finish_recording(istate_recorder& StateRecorder)
	{
		StateRecorder.current_change_set()->record_new_state(new value_container(m_node));
		signal_policy_t::finish_recording(StateRecorder);
	}

private:
	/// This little bit of magic allows us to "call" the derived undo policy when our state is modified internally, e.g. if the node is deleted
	virtual void internal_set_value(const value_t& Value, iunknown* const Hint)
	{
	}

	/// Local storage for the node stored by this policy
	inode* m_node;
	sigc::connection m_node_deleted_connection;
	sigc::connection m_node_changed_connection;

	/// Provides an implementation of istate_container for storing nodes by value (ValueType must have a copy constructor and assignment operator)
	class value_container :
		public istate_container
	{
	public:
		value_container(inode*& Instance) :
			m_instance(Instance),
			m_value(Instance)
		{
		}

		void restore_state()
		{
			m_instance = m_value;
		}

	private:
		inode*& m_instance;
		inode* m_value;
	};
};

/////////////////////////////////////////////////////////////////////////////
// demand_storage

/// Read-only storage policy that stores a value by pointer, created on-demand
/** \deprecated You should prefer to use pointer_storage, instead */
template<typename value_t, typename signal_policy_t>
class demand_storage :
	public signal_policy_t
{
public:
	// This policy only works for data stored by-pointer
	BOOST_STATIC_ASSERT((boost::is_pointer<value_t>::value));

	/// Returns a slot that will invoke the reset() method
	sigc::slot<void, iunknown*> make_reset_slot()
	{
		return sigc::mem_fun(*this, &demand_storage<value_t, signal_policy_t>::internal_reset);
	}

	/// Resets the underlying data so it will be created-again next read
	void reset(value_t NewValue = 0, iunknown* const Hint = 0)
	{
		// Ensure that our value doesn't go out-of-scope while it's getting initialized ...
		if(m_executing)
			return;

		m_cache.reset(NewValue);
		signal_policy_t::set_value(Hint);
	}

	/// Accesses the underlying data, creating it if it doesn't already exist
	value_t internal_value()
	{
		if(!m_cache.get())
		{
			m_executing = true;

			// Note: we create the value and update its state in two steps
			// because m_data_slot() may cause this method to be executed in a loop
			m_cache.reset(new typename boost::remove_pointer<value_t>::type());
			m_data_slot(*m_cache);

			m_executing = false;
		}

		return m_cache.get();
	}

protected:
	template<typename init_t>
	demand_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_data_slot(Init.slot()),
		m_executing(false)
	{
	}

private:
	/// Resets the underlying data so it will be created-again next read
	void internal_reset(iunknown* const Hint = 0)
	{
		// Ensure that our value doesn't go out-of-scope while it's getting initialized ...
		if(m_executing)
			return;

		m_cache.reset();
		signal_policy_t::set_value(Hint);
	}

	/// Storage for this policy's value
	std::auto_ptr<typename boost::remove_pointer<value_t>::type> m_cache;
	/// Stores a slot that will be executed to initialize this policy's value
	sigc::slot<void, typename boost::remove_pointer<value_t>::type&> m_data_slot;
	/// Set to true during initialization of the policy's value, to prevent problems with recursion
	bool m_executing;
};

/////////////////////////////////////////////////////////////////////////////
// pointer_storage

/// Read-only storage policy that stores a value by pointer, created on-demand
template<typename pointer_t, typename signal_policy_t>
class pointer_storage :
	public signal_policy_t
{
	// This policy only works for data stored by-pointer
	BOOST_STATIC_ASSERT((boost::is_pointer<pointer_t>::value));

public:
	typedef typename boost::remove_pointer<pointer_t>::type non_pointer_t;
	typedef pointer_storage<pointer_t, signal_policy_t> this_t;

	/// Set a slot that will be called to initialize the value when first created
	void set_initialize_slot(const sigc::slot<void, non_pointer_t&>& Slot)
	{
		m_initialize_slot = Slot;
		reset();
	}

	/// Set the slot that will be called to update the value whenever it changes
	void set_update_slot(const sigc::slot<void, non_pointer_t&>& Slot)
	{
		m_update_slot = Slot;
		update();
	}

	/// Returns a slot that will invoke the reset() method
	sigc::slot<void, iunknown*> make_reset_slot()
	{
		return sigc::bind<0>(sigc::mem_fun(*this, &this_t::reset), static_cast<pointer_t>(0));
	}

	/// Returns a slot that will invoke the update() method
	sigc::slot<void, iunknown*> make_update_slot()
	{
		return sigc::mem_fun(*this, &this_t::update);
	}

	/// Store an object as the new value, taking control of its lifetime
	void reset(pointer_t NewValue = 0, iunknown* const Hint = 0)
	{
		// Ensure that our value doesn't go out-of-scope while it's being modified
		if(m_executing)
			return;

		m_value.reset(NewValue);
		signal_policy_t::set_value(Hint);
	}

	/// Schedule an update for the value the next time it's read
	void update(iunknown* const Hint = 0)
	{
		// Ensure that our value doesn't go out-of-scope while it's being modified
		if(m_executing)
			return;

		m_update = true;
		signal_policy_t::set_value(Hint);
	}

	/// Accesses the underlying value, creating it if it doesn't already exist
	pointer_t internal_value()
	{
		if(!m_value.get())
		{
			m_executing = true;

			// We can cancel pending updates since we're creating the value from scratch
			m_update = false;

			// Note: we create the value and update its state in two steps
			// because m_data_slot() may cause this method to be executed in a loop
			m_value.reset(new non_pointer_t());
			m_initialize_slot(*m_value);

			m_executing = false;
		}
		
		if(m_update)
		{
			m_executing = true;

			m_update = false;
			m_update_slot(*m_value);

			m_executing = false;
		}

		return m_value.get();
	}

protected:
	template<typename init_t>
	pointer_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_update(false),
		m_executing(false)
	{
	}

private:
	/// Storage for this policy's value
	std::auto_ptr<non_pointer_t> m_value;
	/// Set to true if this policy's value is stale and needs to be updated
	bool m_update;
	/// Stores a slot that will be executed to initialize this policy's value
	sigc::slot<void, non_pointer_t&> m_initialize_slot;
	/// Stores a slot that will be executed to update this policy's value
	sigc::slot<void, non_pointer_t&> m_update_slot;
	/// Set to true during initialization / update of the policy value, to prevent problems with recursion
	bool m_executing;
};

/////////////////////////////////////////////////////////////////////////////
// computed_storage

/// Read-only storage policy that returns a computed value, storing nothing
template<typename value_t, typename signal_policy_t>
class computed_storage :
	public signal_policy_t
{
public:
	// This policy only works for data stored by-value
	BOOST_STATIC_ASSERT((!boost::is_pointer<value_t>::value));

	/// Returns a slot that will invoke the reset() method
	sigc::slot<void, iunknown*> make_reset_slot()
	{
		return sigc::mem_fun(*this, &computed_storage<value_t, signal_policy_t>::reset);
	}

	/// Resets the underlying data so it will be recalculated the next time it's read
	void reset(iunknown* const Hint = 0)
	{
		if(m_executing)
			return;

		m_cache.reset();
		signal_policy_t::set_value(0);
	}

	/// Accesses the underlying data, creating it if it doesn't already exist
	value_t internal_value()
	{
		if(!m_cache.get())
		{
			m_executing = true;

			// Note: we create the cache and set its value in two steps
			// because m_data_slot() may cause this method to be executed in a loop
			m_cache.reset(new value_t());
			*m_cache = m_data_slot();

			m_executing = false;
		}

		return *m_cache;
	}

protected:
	template<typename init_t>
	computed_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_data_slot(Init.slot()),
		m_executing(false)
	{
	}

private:
	/// Storage for this policy's value
	std::auto_ptr<value_t> m_cache;
	/// Stores a slot that will be executed to initialize this policy's value
	sigc::slot<value_t> m_data_slot;
	/// Set to true during initialization of the policy value, to prevent problems with recursion
	bool m_executing;
};

/////////////////////////////////////////////////////////////////////////////
// no_signal

/// Signal policy for data containers that do not notify observers when their value changes
template<typename value_t>
class no_signal
{
protected:
	template<typename init_t>
	no_signal(const init_t& Init)
	{
	}

	void start_recording(istate_recorder& StateRecorder)
	{
	}

	void set_value(iunknown* const Hint)
	{
	}

	void finish_recording(istate_recorder& StateRecorder)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
// change_signal

/// Signal policy for data containers that notify observers when their value changes, including through undo/redo.
template<typename value_t>
class change_signal
{
public:
	/// Defines a signal emitted when the underlying data changes.  The signal includes an optional "hint" that describes the nature of the change.
	typedef sigc::signal<void, iunknown*> changed_signal_t;

	/// Returns a reference to the signal that is emitted whenever the underlying data changes
	changed_signal_t& changed_signal()
	{
		return m_changed_signal;
	}

protected:
	template<typename init_t>
	change_signal(const init_t& Init)
	{
	}

	void start_recording(istate_recorder& StateRecorder)
	{
	}

	void set_value(iunknown* const Hint)
	{
		m_changed_signal.emit(Hint);
	}

	void finish_recording(istate_recorder& StateRecorder)
	{
		StateRecorder.current_change_set()->connect_undo_signal(sigc::bind(m_changed_signal.make_slot(), static_cast<iunknown*>(0)));
		StateRecorder.current_change_set()->connect_redo_signal(sigc::bind(m_changed_signal.make_slot(), static_cast<iunknown*>(0)));
	}

private:
	changed_signal_t m_changed_signal;
};

/// Signal policy for data containers that notify observers when their value changes.  Observers can choose whether to be notified every time the value changes (including undo/redo), or only when the value is changed explicitly by a caller.
template<typename value_t>
class explicit_change_signal
{
public:
	/// Defines a signal emitted when the underlying data changes.  The signal includes an optional "hint" that describes the nature of the change.
	typedef sigc::signal<void, k3d::iunknown*> changed_signal_t;

	/// Returns a reference to the signal that is emitted whenever the underlying data changes.
	changed_signal_t& changed_signal()
	{
		return m_changed_signal;
	}

	/// Connects a slot to a signal that is emitted whenever the underlying data is modified explicitly through set_value, never by undo/redo
	const sigc::connection connect_explicit_change_signal(const sigc::slot<void, k3d::iunknown*>& Slot)
	{
		return m_explicit_change_signal.connect(Slot);
	}

protected:
	template<typename init_t>
	explicit_change_signal(const init_t& Init)
	{
	}

	void start_recording(k3d::istate_recorder& StateRecorder)
	{
	}

	void set_value(k3d::iunknown* const Hint)
	{
		m_changed_signal.emit(Hint);
		m_explicit_change_signal.emit(Hint);
	}

	void finish_recording(k3d::istate_recorder& StateRecorder)
	{
		StateRecorder.current_change_set()->connect_undo_signal(sigc::bind(m_changed_signal.make_slot(), static_cast<k3d::iunknown*>(0)));
		StateRecorder.current_change_set()->connect_redo_signal(sigc::bind(m_changed_signal.make_slot(), static_cast<k3d::iunknown*>(0)));
	}

private:
	changed_signal_t m_changed_signal;
	changed_signal_t m_explicit_change_signal;
};

/// Convenience macro for declaring k3d::data::container nodes
#define k3d_data(value_type, name_policy, signal_policy, undo_policy, storage_policy, constraint_policy, property_policy, serialization_policy) \
	k3d::data::container<value_type, serialization_policy<value_type, property_policy<value_type, name_policy<constraint_policy<value_type, undo_policy<value_type, storage_policy<value_type, signal_policy<value_type> > > > > > > >

/// Allows an arbitrary set of initializers to be consolidated into a single node
template<class init_t>
class initializer_t :
	public init_t
{
public:
	explicit initializer_t(const init_t& Init) :
		init_t(Init)
	{
	};
};

/// Helper class used to consolidate two initializers into a single node
template<typename lhs_t, typename rhs_t>
class composition_t :
	public lhs_t,
	public rhs_t
{
public:
	explicit composition_t(const lhs_t& LHS, const rhs_t& RHS) :
		lhs_t(LHS),
		rhs_t(RHS)
	{
	}
};

/// Consolidates two initializers into a single node, which can be consolidated in-turn
template<typename lhs_t, typename rhs_t>
inline const initializer_t<composition_t<lhs_t, rhs_t> > operator+(const initializer_t<lhs_t>& LHS, const initializer_t<rhs_t>& RHS)
{
	return initializer_t<composition_t<lhs_t, rhs_t> >(composition_t<lhs_t, rhs_t>(LHS, RHS));
}

/// Helper class used to set a data container's initial value
template<typename value_t>
class value_initializer_t
{
public:
	explicit value_initializer_t(const value_t& Value) :
		m_value(Value)
	{
	}

	const value_t& value() const
	{
		return m_value;
	}

private:
	const value_t m_value;
};

template<typename value_t>
inline const initializer_t<value_initializer_t<value_t> > init_value(const value_t& Value)
{
	return initializer_t<value_initializer_t<value_t> >(value_initializer_t<value_t>(Value));
}

/// Helper class used to set a data container's name
class name_t
{
public:
	explicit name_t(const char* const Name) :
		m_name(Name)
	{
	}

	const char* const name() const
	{
		return m_name;
	}

private:
	const char* const m_name;
};

inline const initializer_t<name_t> init_name(const char* const Name)
{
	return initializer_t<name_t>(name_t(Name));
}

/// Helper class used to initialize a data container's label
class label_t
{
public:
	explicit label_t(const char* const Label) :
		m_label(Label)
	{
	}

	const char* const label() const
	{
		return m_label;
	}

private:
	const char* const m_label;
};

inline const initializer_t<label_t> init_label(const char* const Label)
{
	return initializer_t<label_t>(label_t(Label));
}

/// Helper class used to initialize a data container's description
class description_t
{
public:
	explicit description_t(const char* const Description) :
		m_description(Description)
	{
	}

	const char* const description() const
	{
		return m_description;
	}

private:
	const char* const m_description;
};

inline const initializer_t<description_t> init_description(const char* const Description)
{
	return initializer_t<description_t>(description_t(Description));
}

/// Helper class used to initialize a data container's constraints
template<typename data_t>
class constraint_t
{
public:
	explicit constraint_t(iconstraint<data_t>* Constraint) :
		m_constraint(Constraint)
	{
	}

	iconstraint<data_t>* constraint() const
	{
		return m_constraint;
	}

private:
	iconstraint<data_t>* const m_constraint;
};

template<typename data_t>
inline const initializer_t<constraint_t<data_t> > init_constraint(iconstraint<data_t>* Constraint)
{
	return initializer_t<constraint_t<data_t> >(constraint_t<data_t>(Constraint));
}

/// Helper class used to initialize the mode for path property nodes
class path_mode_t
{
public:
	explicit path_mode_t(const ipath_property::mode_t Mode) :
		m_mode(Mode)
	{
	}

	const ipath_property::mode_t path_mode() const
	{
		return m_mode;
	}

private:
	ipath_property::mode_t m_mode;
};

inline const initializer_t<path_mode_t> init_path_mode(const ipath_property::mode_t Mode)
{
	return initializer_t<path_mode_t>(path_mode_t(Mode));
}

/// Helper class used to initialize the type for path property nodes
class path_type_t
{
public:
	explicit path_type_t(const std::string& Type) :
		m_type(Type)
	{
	}

	const std::string& path_type() const
	{
		return m_type;
	}

private:
	const std::string m_type;
};

inline const initializer_t<path_type_t> init_path_type(const std::string& Type)
{
	return initializer_t<path_type_t>(path_type_t(Type));
}

/// Helper class used to initialize enumeration property types
class enumeration_t
{
public:
	explicit enumeration_t(const ienumeration_property::enumeration_values_t& Values) :
		m_values(Values)
	{
	}

	const ienumeration_property::enumeration_values_t& values() const
	{
		return m_values;
	}

private:
	const ienumeration_property::enumeration_values_t& m_values;
};

inline const initializer_t<enumeration_t> init_enumeration(const ienumeration_property::enumeration_values_t& Values)
{
	return initializer_t<enumeration_t>(enumeration_t(Values));
}

/// Helper class used to initialize list properties
template<typename data_t>
class values_t
{
public:
	explicit values_t(const data_t& Values) :
		m_values(Values)
	{
	}

	const data_t& values() const
	{
		return m_values;
	}

private:
	const data_t& m_values;
};

template<typename data_t>
inline const initializer_t<values_t<data_t> > init_values(const data_t& Values)
{
	return initializer_t<values_t<data_t> >(values_t<data_t>(Values));
}

/// Helper class used to initialize measurement properties
class step_increment_t
{
public:
	explicit step_increment_t(const double StepIncrement) :
		m_step_increment(StepIncrement)
	{
	}

	const double step_increment() const
	{
		return m_step_increment;
	}

private:
	const double m_step_increment;
};

inline const initializer_t<step_increment_t> init_step_increment(const double StepIncrement)
{
	return initializer_t<step_increment_t>(step_increment_t(StepIncrement));
}

/// Helper class used to initialize measurement properties
class units_t
{
public:
	explicit units_t(const std::type_info& Units) :
		m_units(Units)
	{
	}

	const std::type_info& units() const
	{
		return m_units;
	}

private:
	const std::type_info& m_units;
};

inline const initializer_t<units_t> init_units(const std::type_info& Units)
{
	return initializer_t<units_t>(units_t(Units));
}

/// Helper class used to initialize data containers
template<typename owner_t>
class owner_initializer_t
{
public:
	explicit owner_initializer_t(owner_t& Owner) :
		m_owner(Owner)
	{
	}

	owner_t& owner() const
	{
		return m_owner;
	}

	idocument& document() const
	{
		return m_owner.document();
	}

	iproperty_collection& property_collection() const
	{
		return m_owner;
	}

	ipersistent_container& persistent_container() const
	{
		return m_owner;
	}

	inode* node() const
	{
		return dynamic_cast<inode*>(&m_owner);
	}

private:
	owner_t& m_owner;
};

template<typename owner_t>
inline const initializer_t<owner_initializer_t<owner_t> > init_owner(owner_t& Owner)
{
	return initializer_t<owner_initializer_t<owner_t> >(owner_initializer_t<owner_t>(Owner));
}

/// Helper class used to initialize data containers
class external_owner_initializer_t
{
public:
	explicit external_owner_initializer_t(idocument& Document, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode* const Node) :
		m_document(Document),
		m_property_collection(PropertyCollection),
		m_persistent_container(PersistentContainer),
		m_node(Node)
	{
	}

	idocument& document() const
	{
		return m_document;
	}

	iproperty_collection& property_collection() const
	{
		return m_property_collection;
	}

	ipersistent_container& persistent_container() const
	{
		return m_persistent_container;
	}

	inode* node() const
	{
		return m_node;
	}

private:
	idocument& m_document;
	iproperty_collection& m_property_collection;
	ipersistent_container& m_persistent_container;
	inode* const m_node;
};

inline const initializer_t<external_owner_initializer_t> init_owner(idocument& Document, iproperty_collection& PropertyCollection, ipersistent_container& PersistentContainer, inode* const Object)
{
	return initializer_t<external_owner_initializer_t>(external_owner_initializer_t(Document, PropertyCollection, PersistentContainer, Object));
}

/// Helper class used to initialize slots
template<typename slot_t>
class slot_initializer_t
{
public:
	explicit slot_initializer_t(const slot_t& Slot) :
		m_slot(Slot)
	{
	}

	const slot_t& slot() const
	{
		return m_slot;
	}

private:
	const slot_t& m_slot;
};

template<typename slot_t>
inline const initializer_t<slot_initializer_t<slot_t> > init_slot(const slot_t& Slot)
{
	return initializer_t<slot_initializer_t<slot_t> >(slot_initializer_t<slot_t>(Slot));
}

} // namespace data

namespace detail
{

template<typename instance_t>
class instance_container :
	public istate_container,
	public sigc::trackable
{
public:
	instance_container(instance_t* const Instance, const bool Owned) :
		m_instance(Instance),
		m_owned(Owned)
	{
	}

	virtual ~instance_container()
	{
		if(m_owned)
			delete m_instance;
	}

	void restore_state()
	{
	}

	void on_owned(bool Owned)
	{
		m_owned = Owned;
	}

private:
	instance_t* const m_instance;
	bool m_owned;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// undoable_new

/// To make creation of an object undoable / redoable, pass it to this function after calling operator new
template<typename object_t>
void undoable_new(object_t* const Object, idocument& Document)
{
	// If we aren't recording undos, we're done...
	state_change_set* const changeset = Document.state_recorder().current_change_set();
	if(!changeset)
		return;

	// Create an instance container that only deletes the object if it has been undone
	typedef detail::instance_container<object_t> container_t;
	container_t* const container = new container_t(Object, false);
	changeset->connect_undo_signal(sigc::bind(sigc::mem_fun(*container, &container_t::on_owned), true));
	changeset->connect_redo_signal(sigc::bind(sigc::mem_fun(*container, &container_t::on_owned), false));

	changeset->record_old_state(container);
}

/////////////////////////////////////////////////////////////////////////////
// undoable_delete

/// To make deletion of an object undoable / redoable, pass it to this function instead of calling operator delete
template<typename object_t>
void undoable_delete(object_t* const Object, idocument& Document)
{
	// If we aren't recording undos, delete it the old-fashioned way ...
	state_change_set* const changeset = Document.state_recorder().current_change_set();
	if(!changeset)
	{
		delete Object;
		return;
	}

	// Create an instance container that only deletes the object if it hasn't been undone
	typedef detail::instance_container<object_t> container_t;
	container_t* const container = new container_t(Object, true);
	changeset->connect_undo_signal(sigc::bind(sigc::mem_fun(*container, &container_t::on_owned), false));
	changeset->connect_redo_signal(sigc::bind(sigc::mem_fun(*container, &container_t::on_owned), true));

	changeset->record_old_state(container);
}

} // namespace k3d

using namespace k3d::data;

#endif // !K3DSDK_DATA_H

