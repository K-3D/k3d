#ifndef K3DSDK_IPERSISTENT_H
#define K3DSDK_IPERSISTENT_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares ipersistent, an abstract interface implemented by all objects that can be serialized to/from a K-3D document
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "path.h"

/**

The methods in ipersistent will be called in a specific order at various stages of an object's lifetime:

When the object is saved as part of a document:

1. save()

When the object is reloaded from a document:

1. load() - called only after all objects have been instantiated

Important note:

Although all document objects exist before load() is called, their load() methods are not called in any
particular order.  Thus, you should not access any member methods of an external object in your load()
implementation.

*/

namespace k3d
{

class idependencies;
class ipersistent_lookup;

namespace xml { class element; }

/// Abstract interface implemented by objects that can be serialized to/from a K-3D document
class ipersistent :
	public virtual iunknown
{
public:
	/// Placeholder for arguments passed at save time
	struct save_context
	{
		save_context(const filesystem::path& RootPath, idependencies& Dependencies, ipersistent_lookup& Lookup) :
			root_path(RootPath),
			dependencies(Dependencies),
			lookup(Lookup)
		{
		}

		const filesystem::path& root_path;
		idependencies& dependencies;
		ipersistent_lookup& lookup;
	};
	/// Called once during document save
	virtual void save(xml::element& Element, const save_context& Context) = 0;

	/// Placeholder for arguments passed at load time
	struct load_context
	{
		load_context(const filesystem::path& RootPath, ipersistent_lookup& Lookup) :
			root_path(RootPath),
			lookup(Lookup)
		{
		}

		const filesystem::path& root_path;
		ipersistent_lookup& lookup;
	};
	/// Called once during document loading
	virtual void load(xml::element& Element, const load_context& Context) = 0;

protected:
	ipersistent() {}
	ipersistent(const ipersistent&) {}
	ipersistent& operator=(const ipersistent&) { return *this; }
	virtual ~ipersistent() {}
};

} // namespace k3d

#endif // K3DSDK_IPERSISTENT_H

