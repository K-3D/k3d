#ifndef NGUI_MODIFIERS_H
#define NGUI_MODIFIERS_H

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

#include <k3dsdk/plugins.h>
#include <string>
#include <vector>

namespace k3d { class idocument; }
namespace k3d { class inode; }
namespace k3d { class imesh_sink; }
namespace k3d { class imesh_source; }
namespace k3d { class iplugin_factory; }
namespace k3d { class itransform_sink; }
namespace k3d { class itransform_source; }
namespace k3d { class uuid; }

namespace libk3dngui
{

class document_state;
	
/// Returns upstream transform modifier (0 if none)
k3d::inode* upstream_transform_modifier(k3d::inode& Object);
/// Returns upstream mesh modifier (0 if none)
k3d::inode* upstream_mesh_modifier(k3d::inode& Object);

typedef std::vector<k3d::iplugin_factory*> factories_t;
/// Returns a collection of transform modifier plugin factories, sorted by name
const factories_t& transform_modifiers();
/// Returns a collection of mesh modifier plugin factories, sorted by name
const factories_t& mesh_modifiers();

/// Modify transformation 
k3d::inode* modify_transformation(k3d::idocument& Document, k3d::inode& Object, k3d::iplugin_factory* Modifier);
/// Modify mesh object
k3d::inode* modify_mesh(document_state& DocumentState, k3d::inode& Object, k3d::iplugin_factory* Modifier);

struct transform_modifier;
const transform_modifier create_transform_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName);
k3d::inode* insert_transform_modifier(k3d::inode& Object, const k3d::uuid& ModifierType, const std::string& ModifierName);

struct mesh_modifier;
const mesh_modifier create_mesh_modifier(k3d::idocument& Document, const k3d::uuid& ModifierType, const std::string& ModifierName);
k3d::inode* insert_mesh_modifier(k3d::inode& Object, const k3d::uuid& ModifierType, const std::string& ModifierName);

struct transform_modifier
{
	transform_modifier() :
		node(0),
		sink(0),
		source(0)
	{
	}

	transform_modifier(k3d::inode& Object, k3d::itransform_sink& Sink, k3d::itransform_source& Source) :
		node(&Object),
		sink(&Sink),
		source(&Source)
	{
	}

	operator bool() const
	{
		return node && sink && source;
	}

	bool empty() const
	{
		return !(node && sink && source);
	}

	k3d::inode* node;
	k3d::itransform_sink* sink;
	k3d::itransform_source* source;
};

struct mesh_modifier
{
	mesh_modifier() :
		node(0),
		sink(0),
		source(0)
	{
	}

	mesh_modifier(k3d::inode& Object, k3d::imesh_sink& Sink, k3d::imesh_source& Source) :
		node(&Object),
		sink(&Sink),
		source(&Source)
	{
	}

	operator bool() const
	{
		return node && sink && source;
	}

	bool empty() const
	{
		return !(node && sink && source);
	}

	k3d::inode* node;
	k3d::imesh_sink* sink;
	k3d::imesh_source* source;
};

} // namespace libk3dngui

#endif // !NGUI_MODIFIERS_H


