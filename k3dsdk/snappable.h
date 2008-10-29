#ifndef K3DSDK_SNAPPABLE_H
#define K3DSDK_SNAPPABLE_H

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

#include "snappable_detail.h"

namespace k3d
{

/// Provides a boilerplate implementation of k3d::isnappable for objects that can be interactively "snapped"
template<typename base_t>
class snappable :
	public base_t,
	public snappable_detail
{
public:
	snappable(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document)
	{
	}

	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		snappable_detail::save(Element, Context);
		base_t::save(Element, Context);
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		snappable_detail::load(Element, Context);
		base_t::load(Element, Context);
	}
};

} // namespace k3d

#endif // !K3DSDK_SNAPPABLE_H

