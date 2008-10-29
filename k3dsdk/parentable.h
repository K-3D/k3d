#ifndef K3DSDK_PARENTABLE_H
#define K3DSDK_PARENTABLE_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "k3d-i18n-config.h"
#include "iparentable.h"

namespace k3d
{

class idocument;
class iplugin_factory;
	
/// Provides a boilerplate implementation of an object that can be parented to another
template<typename base_t>
class parentable :
	public base_t,
	public iparentable
{
public:
	parentable(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_parent(init_owner(*this) + init_name("parent") + init_label(_("Parent")) + init_description(_("Parent")) + init_value<inode*>(0))
	{
	}

	iproperty& parent()
	{
		return m_parent;
	}

private:
	k3d_data(inode*, data::immutable_name, data::change_signal, data::with_undo, data::node_storage, data::no_constraint, data::node_property, data::node_serialization) m_parent;
};

} // namespace k3d

#endif // !K3DSDK_PARENTABLE_H

