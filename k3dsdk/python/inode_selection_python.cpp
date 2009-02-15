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
#include "inode_selection_python.h"
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/inode.h>
#include <k3dsdk/inode_selection.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void select(iunknown_wrapper& Self, iunknown_wrapper& Node, const k3d::double_t Weight)
{
	Self.wrapped<k3d::inode_selection>().select(Node.wrapped<k3d::inode>(), Weight);
}

static double selection_weight(iunknown_wrapper& Self, iunknown_wrapper& Node)
{
	return Self.wrapped<k3d::inode_selection>().selection_weight(Node.wrapped<k3d::inode&>());
}

static list selected_nodes(iunknown_wrapper& Self)
{
	list results;
	
	const k3d::inode_selection::selected_nodes_t selected_nodes = Self.wrapped<k3d::inode_selection>().selected_nodes();
	for(k3d::inode_selection::selected_nodes_t::const_iterator n = selected_nodes.begin(); n != selected_nodes.end(); ++n)
		results.append(wrap_unknown(*n));
	
	return results;
}

static void deselect_all(iunknown_wrapper& Self)
{
	Self.wrapped<k3d::inode_selection>().deselect_all();
}

void define_methods_inode_selection(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::inode_selection*>(&Interface))
		return;

	utility::add_method(utility::make_function(&select,
		"Select the supplied node with the given selection weight\n\n"), "select", Instance);
	utility::add_method(utility::make_function(&selection_weight,
		"Returns the selection weight of the supplied node.\n\n"
		"@rtype: double\n"), "selection_weight", Instance);
	utility::add_method(utility::make_function(&selected_nodes,
		"Returns a list of selected nodes\n\n"
		"@return: A list of L{inode} objects.\n\n"), "selected_nodes", Instance);
	utility::add_method(utility::make_function(&deselect_all,
		"Clears the stored selection."), "deselect_all", Instance);
}

} // namespace python

} // namespace k3d
