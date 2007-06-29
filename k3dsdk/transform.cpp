// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "classes.h"
#include "create_plugins.h"
#include "k3d-i18n-config.h"
#include "idag.h"
#include "idocument.h"
#include "inode.h"
#include "iparentable.h"
#include "itransform_sink.h"
#include "itransform_source.h"
#include "property.h"
#include "result.h"
#include "transform.h"

#include <iostream>

namespace k3d
{

namespace detail
{

const matrix4 upstream_matrix(inode& Node)
{
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Node);
	return_val_if_fail(downstream_sink, k3d::identity3D());

	iproperty& downstream_input = downstream_sink->transform_sink_input();
	iproperty* const upstream_output = Node.document().dag().dependency(downstream_input);
	if(upstream_output)
		return boost::any_cast<k3d::matrix4>(upstream_output->property_value());

	return k3d::identity3D();
}

inode* upstream_frozen_transformation(inode& Node)
{
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Node);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->transform_sink_input();
	iproperty* const upstream_output = Node.document().dag().dependency(downstream_input);

	if(upstream_output && upstream_output->property_node() && upstream_output->property_node()->factory().class_id() == classes::FrozenTransformation())
		return upstream_output->property_node();

	return 0;
}

inode* insert_transform_modifier(inode& Node)
{
	itransform_sink* const downstream_sink = dynamic_cast<itransform_sink*>(&Node);
	return_val_if_fail(downstream_sink, 0);

	iproperty& downstream_input = downstream_sink->transform_sink_input();
	iproperty* const upstream_output = Node.document().dag().dependency(downstream_input);

	inode* const modifier = create_plugin<inode>(classes::FrozenTransformation(), Node.document(), _("Transformation"));
	return_val_if_fail(modifier, 0);
	itransform_sink* const modifier_sink = dynamic_cast<itransform_sink*>(modifier);
	return_val_if_fail(modifier_sink, 0);
	itransform_source* const modifier_source = dynamic_cast<itransform_source*>(modifier);

	idag::dependencies_t dependencies;
	dependencies.insert(std::make_pair(&modifier_sink->transform_sink_input(), upstream_output));
	dependencies.insert(std::make_pair(&downstream_input, &modifier_source->transform_source_output()));
	Node.document().dag().set_dependencies(dependencies);

	return modifier;
}

} // namespace detail

const point3 world_position(iunknown& Node)
{
	return node_to_world_matrix(Node) * point3(0, 0, 0);
}

const matrix4 node_to_world_matrix(iunknown& Node)
{
	iproperty* const property = get_typed_property<matrix4>(Node, "output_matrix");
	if(property)
		return boost::any_cast<matrix4>(property->property_value());

	return identity3D();
}

const matrix4 parent_to_world_matrix(iunknown& Node)
{
	iparentable* const parentable = dynamic_cast<iparentable*>(&Node);
	inode* const parent = parentable ? boost::any_cast<inode*>(parentable->parent().property_value()) : 0;
	if(parent)
		return node_to_world_matrix(*parent);

	return identity3D();
}

k3d::inode* set_matrix(iunknown& Node, const matrix4& Matrix)
{
	inode* const node = dynamic_cast<inode*>(&Node);
	return_val_if_fail(node, 0);

	// Check for an upstream FrozenTransformation modifier
	if(inode* const modifier = detail::upstream_frozen_transformation(*node))
	{
		const k3d::matrix4 upstream_matrix = detail::upstream_matrix(*modifier);
		set_value(*modifier, "matrix", Matrix * inverse(upstream_matrix));
		return 0;
	}

	// Create a new one
	const k3d::matrix4 upstream_matrix = detail::upstream_matrix(*node);

	inode* const modifier = detail::insert_transform_modifier(*node);
	return_val_if_fail(modifier, 0);

	set_value(*modifier, "matrix", Matrix * inverse(upstream_matrix));

	return modifier;
}

} // namespace k3d

