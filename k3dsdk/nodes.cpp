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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "ideletable.h"
#include "idocument.h"
#include "iplugin_factory.h"
#include "iproperty_collection.h"
#include "istate_recorder.h"
#include "nodes.h"
#include "string_cast.h"
#include "string_modifiers.h"
#include "utility.h"

namespace k3d
{

namespace detail
{

template<typename functor_t>
struct class_id_filter_t
{
	explicit class_id_filter_t(const uuid ClassID, functor_t Functor) : class_id(ClassID), functor(Functor) {}

	void operator()(k3d::inode* Object) { if(Object->factory().class_id() == class_id) functor(Object); }

	const uuid class_id;
	functor_t functor;
};

template<typename functor_t>
class_id_filter_t<functor_t> class_id_filter(const uuid ID, functor_t Functor)
{
	return class_id_filter_t<functor_t>(ID, Functor);
}

template<typename functor_t>
struct name_filter_t
{
	explicit name_filter_t(const std::string Name, functor_t Functor) : name(Name), functor(Functor) {}

	void operator()(k3d::inode* Object)
	{
		if(Object->name() == name)
		{
			functor(Object);
		}
	}

	const std::string name;
	functor_t functor;
};

template<typename functor_t>
name_filter_t<functor_t> name_filter(const std::string Name, functor_t Functor)
{
	return name_filter_t<functor_t>(Name, Functor);
}

} // namespace detail

inode* find_node(inode_collection& Nodes, const std::string& ObjectName)
{
	nodes_t nodes = find_nodes(Nodes, ObjectName);
	if(1 == nodes.size())
		return *nodes.begin();

	return 0;
}

inode* find_node(inode_collection& Nodes, iproperty& Property)
{
	const nodes_t::const_iterator end = Nodes.collection().end();
	for(nodes_t::const_iterator node = Nodes.collection().begin(); node != end; ++node)
	{
		iproperty_collection* const property_collection = dynamic_cast<iproperty_collection*>(*node);
		if(!property_collection)
			continue;

		const iproperty_collection::properties_t& properties = property_collection->properties();
		if(std::find(properties.begin(), properties.end(), &Property) != properties.end())
			return *node;
	}

	return 0;
}

const nodes_t find_nodes(inode_collection& Nodes, const uuid ClassID)
{
	nodes_t results;
	std::for_each(Nodes.collection().begin(), Nodes.collection().end(), detail::class_id_filter(ClassID, inserter(results)));

	return results;
}

const nodes_t find_nodes(inode_collection& Nodes, const std::string& ObjectName)
{
	nodes_t results;
	std::for_each(Nodes.collection().begin(), Nodes.collection().end(), detail::name_filter(ObjectName, inserter(results)));

	return results;
}

const std::string unique_name(inode_collection& Nodes, const std::string& Name)
{
	// For each node in the collection ...
	for(k3d::inode_collection::nodes_t::const_iterator handle = Nodes.collection().begin(); handle != Nodes.collection().end(); handle++)
	{
		inode* const node = *handle;

		// Name doesn't match, so keep going ...
		if(Name != node->name())
			continue;

		// Got a duplicate name, so recursively try something else ...
		std::string base(k3d::trim(Name));
		unsigned int copy = 1;

		//parse(Name.c_str(), lexeme_d[*(anychar_p - space_p)][assign(base)] >> !(int_p[assign(copy)]), space_p);

		// Find trailing space followed by a number and increment ('k3d 5' -> 'k3d 6', 'k3d3' -> 'k3d3 2')
		std::string::iterator c = base.end();
		while(--c != base.begin() &&
			*c >= '0' && *c <= '9');

		if(*c == ' ')
		{
			// Get trailing number
			std::string number(c + 1, base.end());
			copy = k3d::from_string<unsigned long>(number, 1);

			// Cut trailing space + number
			base = std::string(base.begin(), c);
		}

		return unique_name(Nodes, base + ' ' + k3d::string_cast(copy+1));
	}

	return Name;
}

void delete_nodes(idocument& Document, const nodes_t& Nodes)
{
	// Let the nodes know that they're about to be deleted ...
	for(nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		(*node)->deleted_signal().emit();

	// Remove them from the document node collection ...
	Document.nodes().remove_nodes(Nodes);

	// Make sure the node gets cleaned-up properly after a redo ...
	for(nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		k3d::undoable_delete(dynamic_cast<k3d::ideletable*>(*node), Document);
}

} // namespace k3d

