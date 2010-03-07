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

#include <k3d-i18n-config.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/property.h>

#include <algorithm>
#include <iostream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// node

node::node(iplugin_factory& Factory, idocument& Document) :
	property_collection(),
	m_factory(Factory),
	m_document(Document),
	m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Assign a human-readable name to identify this node.")) + init_value<std::string>(""))
{
	m_deleted_signal.connect(sigc::mem_fun(*this, &node::on_deleted));
	m_name.changed_signal().connect(sigc::hide(m_name_changed_signal.make_slot()));
}

node::~node()
{
}

void node::set_name(const std::string Name)
{
	m_name.set_value(Name);
}

const std::string node::name()
{
	return m_name.internal_value();
}

void node::on_deleted()
{
	// Signal that our properties are going away ...
	const property_collection::properties_t props = properties();
	for(property_collection::properties_t::const_iterator property = props.begin(); property != props.end(); ++property)
		(**property).property_deleted_signal().emit();
}

iplugin_factory& node::factory()
{
	return m_factory;
}

idocument& node::document()
{
	return m_document;
}

inode::deleted_signal_t& node::deleted_signal()
{
	return m_deleted_signal;
}

inode::name_changed_signal_t& node::name_changed_signal()
{
	return m_name_changed_signal;
}

void node::save(xml::element& Element, const ipersistent::save_context& Context)
{
	persistent_property_collection::save(Element, Context);
}

void node::load(xml::element& Element, const ipersistent::load_context& Context)
{
	// Load object name ...
	set_name(xml::attribute_text(Element, "name"));

	// Load Variables
	persistent_property_collection::load(Element, Context);
}

const std::vector<inode*> node::lookup(idocument& Document)
{
	return Document.nodes().collection();
}

const std::vector<inode*> node::lookup(idocument& Document, const uuid FactoryID)
{
	std::vector<inode*> result;

	const std::vector<inode*>::const_iterator end = Document.nodes().collection().end();
	for(std::vector<inode*>::const_iterator node = Document.nodes().collection().begin(); node != end; ++node)
	{
		if((**node).factory().factory_id() == FactoryID)
			result.push_back(*node);
	}

	return result;
}

const std::vector<inode*> node::lookup(idocument& Document, const string_t& NodeName)
{
	std::vector<inode*> result;

	const std::vector<inode*>::const_iterator end = Document.nodes().collection().end();
	for(std::vector<inode*>::const_iterator node = Document.nodes().collection().begin(); node != end; ++node)
	{
		if((**node).name() == NodeName)
			result.push_back(*node);
	}

	return result;
}

const std::vector<inode*> node::lookup(idocument& Document, const string_t& MetaName, const string_t& MetaValue)
{
	std::vector<inode*> result;

	const std::vector<inode*>::const_iterator end = Document.nodes().collection().end();
	for(std::vector<inode*>::const_iterator node = Document.nodes().collection().begin(); node != end; ++node)
	{
		if(imetadata* const metadata = dynamic_cast<imetadata*>(*node))
		{
			imetadata::metadata_t pairs = metadata->get_metadata();
			imetadata::metadata_t::iterator pair = pairs.find(MetaName);
			if(pair != pairs.end() && pair->second == MetaValue)
				result.push_back(*node);
		}
	}
	
	return result;
}

inode* node::lookup_one(idocument& Document, const string_t& NodeName)
{
	std::vector<inode*> nodes = lookup(Document, NodeName);
	return nodes.size() == 1 ? nodes[0] : 0;
}

void node::show(idocument& Document, inode& Node)
{
	show(Document, std::vector<inode*>(1, &Node));
}

void node::show(idocument& Document, const std::vector<inode*>& Nodes)
{
	const std::vector<inode_collection_sink*> sinks = lookup<inode_collection_sink>(Document);
	for(uint_t i = 0; i != sinks.size(); ++i)
	{
		const inode_collection_sink::properties_t properties = sinks[i]->node_collection_properties();
		for(inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(inode_collection_property* const node_collection_property = dynamic_cast<inode_collection_property*>(*property))
			{
				inode_collection_property::nodes_t nodes = property::internal_value<inode_collection_property::nodes_t>(**property);
				for(uint_t i = 0; i != Nodes.size(); ++i)
				{
					if(node_collection_property->property_allow(*Nodes[i]))
					{
						nodes.erase(std::remove(nodes.begin(), nodes.end(), Nodes[i]), nodes.end());
						nodes.push_back(Nodes[i]);
					}
				}
				property::set_internal_value(**property, nodes);
			}
		}
	}
}

void node::hide(idocument& Document, inode& Node)
{
	hide(Document, std::vector<inode*>(1, &Node));
}

void node::hide(idocument& Document, const std::vector<inode*>& Nodes)
{
	const std::vector<inode_collection_sink*> sinks = lookup<inode_collection_sink>(Document);
	for(uint_t i = 0; i != sinks.size(); ++i)
	{
		const inode_collection_sink::properties_t properties = sinks[i]->node_collection_properties();
		for(inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(inode_collection_property* const node_collection_property = dynamic_cast<inode_collection_property*>(*property))
			{
				inode_collection_property::nodes_t nodes = property::internal_value<inode_collection_property::nodes_t>(**property);
				for(uint_t i = 0; i != Nodes.size(); ++i)
				{
					nodes.erase(std::remove(nodes.begin(), nodes.end(), Nodes[i]), nodes.end());
				}
				property::set_internal_value(**property, nodes);
			}
		}
	}
}

} // namespace k3d

