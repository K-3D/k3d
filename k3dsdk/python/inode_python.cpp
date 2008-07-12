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
*/

#include "idocument_python.h"
#include "inode_python.h"
#include "interface_wrapper_python.h"

#include <k3dsdk/inode.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

typedef interface_wrapper<k3d::inode> inode_wrapper;

static object document(inode_wrapper& Self)
{
	return wrap(Self.wrapped().document());
}

static object factory(inode_wrapper& Self)
{
	return wrap(Self.wrapped().factory());
}

static boost::uint64_t hash(inode_wrapper& Self)
{
	return reinterpret_cast<boost::uint64_t>(Self.wrapped_ptr());
}

void define_class_inode()
{
	class_<inode_wrapper>("inode",
		"Abstract interface implemented by all document nodes.\n\n"
		"Use L{dynamic_cast} to test whether an inode object implements a specific interface / "
		"convert an inode object to a specific interface type.", no_init)
		.def("document", &document,
			"Returns the L{idocument} that owns this node.")
		.def("factory", &factory,
			"Returns the L{iplugin_factory} used to create this node type.")
		.def("__hash__", &hash,
			"Returns a hash value that can be used as a key when storing inode objects in a map.");
}

} // namespace python

} // namespace k3d

