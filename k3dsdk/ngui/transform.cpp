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

#include "modifiers.h"
#include "transform.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iparentable.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/transform.h>

#include <algorithm>

namespace libk3dngui
{

const transform_history_t parent_to_node_history(k3d::inode& Object)
{
	k3d::iparentable* const parentable = dynamic_cast<k3d::iparentable*>(&Object);
	k3d::inode* const parent = parentable ? boost::any_cast<k3d::inode*>(parentable->parent().property_internal_value()) : 0;

	transform_history_t results;
	for(k3d::inode* object = &Object; object; )
	{
		results.push_back(object);

		k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(object);
		if(!transform_sink)
			break;

		k3d::iproperty* const dependency = Object.document().pipeline().dependency(transform_sink->transform_sink_input());
		if(!dependency)
			break;

		object = dependency->property_node();

		if(object == parent)
			break;
	}

	std::reverse(results.begin(), results.end());

	return results;
}

std::ostream& operator<<(std::ostream& Stream, const show_history& RHS)
{
	for(transform_history_t::const_iterator object = RHS.history.begin(); object != RHS.history.end(); ++object)
	{
		if(object != RHS.history.begin())
			k3d::log() << " -> ";
		Stream << (*object)->name();
	}
	return Stream;
}

void unparent(k3d::inode& Node)
{
	k3d::iparentable* const parentable = dynamic_cast<k3d::iparentable*>(&Node);
	return_if_fail(parentable);

	k3d::inode* const parent = boost::any_cast<k3d::inode*>(parentable->parent().property_internal_value());
	if(!parent)
		return;

	const k3d::matrix4 unparent_compensation = k3d::node_to_world_matrix(*parent);

	const transform_history_t history = parent_to_node_history(Node);
	if(!history.empty())
	{
		if(k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(history.front()))
		{
			const transform_modifier modifier = create_transform_modifier(Node.document(), k3d::classes::FrozenTransformation(), "Unparent Compensation");
			if(modifier)
			{
				k3d::property::set_internal_value(*modifier.node, "matrix", unparent_compensation);

				k3d::ipipeline::dependencies_t dependencies;
				dependencies.insert(std::make_pair(&transform_sink->transform_sink_input(), &modifier.source->transform_source_output()));
				Node.document().pipeline().set_dependencies(dependencies);
			}
		}
	}

	if(k3d::iwritable_property* const writable_parent = dynamic_cast<k3d::iwritable_property*>(&parentable->parent()))
		writable_parent->property_set_value(static_cast<k3d::inode*>(0));
}

} // namespace libk3dngui

