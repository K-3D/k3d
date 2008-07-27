// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "any_python.h"
#include "interface_wrapper_python.h"
#include "inode_selection_python.h"
#include "node_python.h"

#include <k3dsdk/inode_selection.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

typedef interface_wrapper<k3d::inode_selection> inode_selection_wrapper;

static void select(inode_selection_wrapper& Self, interface_wrapper<k3d::inode>& Node, const k3d::double_t Weight)
{
	Self.wrapped().select(Node.wrapped(), Weight);
}

static double selection_weight(inode_selection_wrapper& Self, interface_wrapper<k3d::inode>& Node)
{
	return Self.wrapped().selection_weight(Node.wrapped());
}

static list selected_nodes(inode_selection_wrapper& Self)
{
	list results;
	
	const k3d::inode_selection::selected_nodes_t selected_nodes = Self.wrapped().selected_nodes();
	for(k3d::inode_selection::selected_nodes_t::const_iterator n = selected_nodes.begin(); n != selected_nodes.end(); ++n)
	{
		results.append(node(*n));
	}
	
	return results;
}

static void deselect_all(inode_selection_wrapper& Self)
{
	Self.wrapped().deselect_all();
}

void define_class_inode_selection()
{
	class_<inode_selection_wrapper>("inode_selection",
		"Abstract interface for storage of node selections", no_init)
		.def("select", &select,
			"Select the supplied node with the given selection weight\n\n")
		.def("selection_weight", &selection_weight,
			"Returns the selection weight of the supplied node.\n\n"
			"@rtype: double\n")
		.def("selected_nodes", &selected_nodes,
			"Returns a list of selected nodes\n\n"
			"@return: A list of L{inode} objects.\n\n")
		.def("deselect_all", &deselect_all,
			"Clears the stored selection.");
}

} // namespace python

} // namespace k3d
