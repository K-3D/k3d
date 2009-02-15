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

#include "imesh_storage_python.h"
#include "mesh_python.h"
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

static k3d::python::mesh create_mesh(iunknown_wrapper& Self)
{
	k3d::mesh* const mesh = new k3d::mesh();
	Self.wrapped<k3d::imesh_storage>().reset_mesh(mesh);

	return k3d::python::mesh(mesh);
}

static k3d::python::mesh set_mesh(iunknown_wrapper& Self, mesh& Mesh)
{
	k3d::mesh* const mesh = new k3d::mesh(Mesh.wrapped());
	Self.wrapped<k3d::imesh_storage>().reset_mesh(mesh);

	return k3d::python::mesh(mesh);
}

static void clear_mesh(iunknown_wrapper& Self)
{
	Self.wrapped<k3d::imesh_storage>().reset_mesh(0);
}

void define_methods_imesh_storage(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::imesh_storage*>(&Interface))
		return;

	utility::add_method(utility::make_function(&create_mesh,
		"Creates a new L{mesh} object whose lifetime will be managed by the imesh_storage object.\n\n"
		"@return: Returns a new L{mesh} object."), "create_mesh", Instance);
	utility::add_method(utility::make_function(&set_mesh,
		"Assigns a shallow-copy of an existing L{mesh} object whose lifetime will be managed by the imesh_storage object.\n\n"
		"@return: Returns the copied L{mesh} object."), "set_mesh", Instance);
	utility::add_method(utility::make_function(&clear_mesh,
		"Deletes the stored mesh and resets the storage to a null mesh."), "clear_mesh", Instance);
}

} // namespace python

} // namespace k3d

