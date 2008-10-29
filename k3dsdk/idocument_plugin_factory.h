#ifndef K3DSDK_IDOCUMENT_PLUGIN_FACTORY_H
#define K3DSDK_IDOCUMENT_PLUGIN_FACTORY_H

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
		\brief Declares idocument_plugin_factory, an abstract factory interface for document-context plugin objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

class idocument;
class inode;
class iplugin_factory;

/// Abstract factory interface for plugin objects
class idocument_plugin_factory :
	public virtual iunknown
{
public:
	/// Creates a new instance of a plugin object
	virtual inode* create_plugin(iplugin_factory& Factory, idocument& Document) = 0;

protected:
	idocument_plugin_factory() {}
	idocument_plugin_factory(const idocument_plugin_factory&) {}
	idocument_plugin_factory& operator=(const idocument_plugin_factory&) { return *this; }
	virtual ~idocument_plugin_factory() {}
};

} // namespace k3d

#endif // !K3DSDK_IDOCUMENT_PLUGIN_FACTORY_H

