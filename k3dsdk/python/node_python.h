#ifndef K3DSDK_NODE_PYTHON_H
#define K3DSDK_NODE_PYTHON_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "interface_wrapper_python.h"

#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty_collection.h>

namespace k3d
{

class iunknown;

namespace python
{

class node :
	public interface_wrapper<k3d::inode>,
	public interface_wrapper<k3d::iproperty_collection>
{
public:
	node();
	node(k3d::iunknown* Node);

	const double get_selection_weight() const;
	void set_selection_weight(const double Weight);

	static void define_class();
};

} // namespace python

} // namespace k3d

#endif // !K3DSDK_NODE_PYTHON_H

