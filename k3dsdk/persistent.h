#ifndef K3DSDK_PERSISTENT_H
#define K3DSDK_PERSISTENT_H

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
		\brief Declares k3d::persistence::container and related classes used to automate serialization of data objects
		\author Tim Shead (tshead@k-3d.com)
*/

#include "persistent_container.h"
#include "xml.h"

namespace k3d
{

class idocument;
class iplugin_factory;

/// Provides a boilerplate implementation of an object that can be serialized, with automatic serialization of data objects
template<typename base_t>
class persistent :
	public base_t,
	public ipersistent,
	public persistent_container
{
public:
	persistent(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document)
	{
	}

	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		persistent_container::save(Element, Context);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		// Load object name ...
		base_t::set_name(xml::attribute_text(Element, "name"));

		// Load Variables
		persistent_container::load(Element, Context);
	}
};

} // namespace k3d

#endif // !K3DSDK_PERSISTENT_H

