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

#include "inode_python.h"

#include <k3dsdk/inode.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

inode::inode() :
	base()
{
}

inode::inode(k3d::inode* Node) :
	base(Node)
{
}

object inode::document()
{
	return object(idocument(wrapped().document()));
}

object inode::factory()
{
	return object(iplugin_factory(wrapped().factory()));
}

uint64_t inode::hash()
{
	return reinterpret_cast<uint64_t>(wrapped_ptr());
}

void inode::define_class()
{
	class_<inode>("inode", no_init)
		.def("document", &inode::document)
		.def("factory", &inode::factory)
		.def("__hash__", &inode::hash);
}

} // namespace python

} // namespace k3d

