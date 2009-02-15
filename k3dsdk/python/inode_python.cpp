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
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/inode.h>
#include <k3dsdk/iplugin_factory.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

static object document(iunknown_wrapper& Self)
{
	return wrap(Self.wrapped<k3d::inode>().document());
}

static object factory(iunknown_wrapper& Self)
{
	return wrap_unknown(Self.wrapped<k3d::inode>().factory());
}

static boost::uint64_t hash(iunknown_wrapper& Self)
{
	return reinterpret_cast<boost::uint64_t>(Self.wrapped_ptr());
}

void define_methods_inode(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::inode*>(&Interface))
		return;

	utility::add_method(utility::make_function(&document, "Returns the L{idocument} that owns this node."), "document", Instance);
	utility::add_method(utility::make_function(&factory, "Returns the plugin factory used to create this node type."), "factory", Instance);
	utility::add_method(utility::make_function(&hash, "Returns a hash value that can be used as a key when storing inode objects in a map."), "__hash__", Instance);
}

} // namespace python

} // namespace k3d

