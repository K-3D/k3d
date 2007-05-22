// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "array_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/color.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/vectors.h>

namespace k3d
{

namespace python
{

void export_arrays()
{
	export_array<k3d::typed_array<bool> >("bool_array",
		"Stores a mutable (read-write) collection of boolean values.");
	export_array<k3d::typed_array<double> >("double_array",
		"Stores a mutable (read-write) collection of floating-point values.");
	export_array<k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("polyhedron_type_array",
		"Stores a mutable (read-write) collection of polyhedron type values.");
	export_array<k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("blobby_operator_type_array",
		"Stores a mutable (read-write) collection of blobby operator type values.");
	export_array<k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("blobby_primitive_type_array",
		"Stores a mutable (read-write) collection of blobby primitive type values.");
	export_array<k3d::typed_array<k3d::imaterial*> >("material_array",
		"Stores a mutable (read-write) collection of L{imaterial} objects.");
	export_array<k3d::typed_array<k3d::color> >("color_array",
		"Stores a mutable (read-write) collection of L{color} values.");
	export_array<k3d::typed_array<k3d::matrix4> >("matrix4_array",
		"Stores a mutable (read-write) collection of L{matrix4} values.");
	export_array<k3d::typed_array<k3d::normal3> >("normal3_array",
		"Stores a mutable (read-write) collection of L{normal3} values.");
	export_array<k3d::typed_array<k3d::point3> >("point3_array",
		"Stores a mutable (read-write) collection of L{point3} values.");
	export_array<k3d::typed_array<k3d::point4> >("point4_array",
		"Stores a mutable (read-write) collection of L{point4} values.");
	export_array<k3d::typed_array<size_t> >("size_t_array",
		"Stores a mutable (read-write) collection of integer values.");
	export_array<k3d::typed_array<k3d::vector3> >("vector3_array",
		"Stores a mutable (read-write) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

