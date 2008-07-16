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

#include "euler_python.h"
#include "mesh_python.h"

#include <k3dsdk/euler_operations.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class euler
{
public:
	static k3d::uint_t number(k3d::python::mesh& Mesh, k3d::uint_t Polyhedron)
	{
		return k3d::euler::number(Mesh.wrapped(), Polyhedron);
	}
};

void define_namespace_euler()
{
	scope outer = class_<euler>("euler", no_init)
		.def("number", &euler::number,
			"Computes the Euler number for a polyhedron.")
		.staticmethod("number");
}

} // namespace python

} // namespace k3d

