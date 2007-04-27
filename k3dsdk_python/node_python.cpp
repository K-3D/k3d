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

#include "node_python.h"

#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/iselectable.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

node::node() :
	inode(),
	iproperty_collection(),
	iselectable()
{
}

node::node(k3d::iunknown* Node) :
	inode(dynamic_cast<k3d::inode*>(Node)),
	iproperty_collection(dynamic_cast<k3d::iproperty_collection*>(Node)),
	iselectable(dynamic_cast<k3d::iselectable*>(Node))
{
}

void export_node()
{
	class_<node, bases<inode, iproperty_collection, iselectable> >("node");
}

} // namespace python

} // namespace k3d

