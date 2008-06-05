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
	interface_wrapper<k3d::inode>(),
	interface_wrapper<k3d::iproperty_collection>()
{
}

node::node(k3d::iunknown* Node) :
	interface_wrapper<k3d::inode>(dynamic_cast<k3d::inode*>(Node)),
	interface_wrapper<k3d::iproperty_collection>(dynamic_cast<k3d::iproperty_collection*>(Node))
{
}

const double node::get_selection_weight() const
{
	if(k3d::iselectable* const selectable = dynamic_cast<k3d::iselectable*>(interface_wrapper<k3d::inode>::wrapped_ptr()))
		return selectable->get_selection_weight();

	throw std::runtime_error("internal error: node does not implement k3d::iselectable");
}

void node::set_selection_weight(const double Weight)
{
	if(k3d::iselectable* const selectable = dynamic_cast<k3d::iselectable*>(interface_wrapper<k3d::inode>::wrapped_ptr()))
	{
		selectable->set_selection_weight(Weight);
		return;
	}

	throw std::runtime_error("internal error: node does not implement k3d::iselectable");
}

void node::define_class()
{
	class_<node, bases<interface_wrapper<k3d::inode>, interface_wrapper<k3d::iproperty_collection> > >("node")
		.add_property("selection_weight", &node::get_selection_weight, &node::set_selection_weight);
}

} // namespace python

} // namespace k3d

