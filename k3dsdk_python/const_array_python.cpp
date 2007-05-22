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

#include "const_array_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/color.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/vectors.h>

namespace k3d
{

namespace python
{

void export_const_arrays()
{
	export_const_array<const k3d::typed_array<bool> >("const_bool_array",
		"Stores an immutable (read-only) collection of boolean values.");
	export_const_array<const k3d::typed_array<double> >("const_double_array",
		"Stores an immutable (read-only) collection of floating-point values.");
	export_const_array<const k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("const_polyhedron_type_array",
		"Stores an immutable (read-only) collection of polyhedron type values.");
	export_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("const_blobby_operator_type_array",
		"Stores an immutable (read-only) collection of blobby operator type values.");
	export_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("const_blobby_primitive_type_array",
		"Stores an immutable (read-only) collection of blobby primitive type values.");
	export_const_array<const k3d::typed_array<k3d::imaterial*> >("const_material_array",
		"Stores an immutable (read-only) collection of L{imaterial} objects.");
	export_const_array<const k3d::typed_array<k3d::color> >("const_color_array",
		"Stores an immutable (read-only) collection of L{color} values.");
	export_const_array<const k3d::typed_array<k3d::matrix4> >("const_matrix4_array",
		"Stores an immutable (read-only) collection of L{matrix4} values.");
	export_const_array<const k3d::typed_array<k3d::normal3> >("const_normal3_array",
		"Stores an immutable (read-only) collection of L{normal3} values.");
	export_const_array<const k3d::typed_array<k3d::point3> >("const_point3_array",
		"Stores an immutable (read-only) collection of L{point3} values.");
	export_const_array<const k3d::typed_array<k3d::point4> >("const_point4_array",
		"Stores an immutable (read-only) collection of L{point4} values.");
	export_const_array<const k3d::typed_array<size_t> >("const_size_t_array",
		"Stores an immutable (read-only) collection of integer values.");
	export_const_array<const k3d::typed_array<k3d::vector3> >("const_vector3_array",
		"Stores an immutable (read-only) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

