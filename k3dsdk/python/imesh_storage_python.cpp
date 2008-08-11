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

#include <k3dsdk/mesh.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

static k3d::python::mesh reset_mesh(imesh_storage_wrapper& Self)
{
	k3d::mesh* const mesh = new k3d::mesh();
	Self.wrapped().reset_mesh(mesh);

	return k3d::python::mesh(mesh);
}

static void clear_mesh(imesh_storage_wrapper& Self)
{
	Self.wrapped().reset_mesh(0);
}

void define_class_imesh_storage()
{
	class_<imesh_storage_wrapper>("imesh_storage",
		"Abstract interface implemented by nodes that can provide persistent storage of L{mesh} objects.", no_init)
		.def("reset_mesh", &reset_mesh,
			"Creates a new L{mesh} object whose lifetime will be managed by the imesh_storage object.\n\n"
			"@return: Returns a new L{mesh} object.")
		.def("clear_mesh", &clear_mesh,
			"Deletes the stored mesh and resets the storage to a null mesh.");
}

} // namespace python

} // namespace k3d

