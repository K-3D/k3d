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
#include <k3dsdk/normal3.h>
#include <k3dsdk/point2.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/point4.h>
#include <k3dsdk/texture3.h>
#include <k3dsdk/vector2.h>
#include <k3dsdk/vector3.h>

namespace k3d
{

namespace python
{

void export_const_arrays()
{
	export_const_array<const k3d::typed_array<k3d::bool_t> >("const_k3d_bool_t_array",
		"Stores an immutable (read-only) collection of boolean values.");
	export_const_array<const k3d::typed_array<k3d::double_t> >("const_k3d_double_t_array",
		"Stores an immutable (read-only) collection of floating-point values.");
	export_const_array<const k3d::typed_array<k3d::int8_t> >("const_k3d_int8_t_array",
		"Stores an immutable (read-only) collection of 8-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::int16_t> >("const_k3d_int16_t_array",
		"Stores an immutable (read-only) collection of 16-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::int32_t> >("const_k3d_int32_t_array",
		"Stores an immutable (read-only) collection of 32-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::int64_t> >("const_k3d_int64_t_array",
		"Stores an immutable (read-only) collection of 64-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("const_k3d_mesh_polyhedra_t_polyhedron_type_array",
		"Stores an immutable (read-only) collection of polyhedron type values.");
	export_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("const_k3d_mesh_blobbies_t_operator_type_array",
		"Stores an immutable (read-only) collection of blobby operator type values.");
	export_const_array<const k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("const_k3d_mesh_blobbies_t_primitive_type_array",
		"Stores an immutable (read-only) collection of blobby primitive type values.");
	export_const_array<const k3d::typed_array<k3d::imaterial*> >("const_k3d_imaterial_array",
		"Stores an immutable (read-only) collection of L{imaterial} objects.");
	export_const_array<const k3d::typed_array<k3d::inode*> >("const_k3d_inode_array",
		"Stores an immutable (read-only) collection of L{inode} objects.");
	export_const_array<const k3d::typed_array<k3d::color> >("const_k3d_color_array",
		"Stores an immutable (read-only) collection of L{color} values.");
	export_const_array<const k3d::typed_array<k3d::matrix4> >("const_k3d_matrix4_array",
		"Stores an immutable (read-only) collection of L{matrix4} values.");
	export_const_array<const k3d::typed_array<k3d::normal3> >("const_k3d_normal3_array",
		"Stores an immutable (read-only) collection of L{normal3} values.");
	export_const_array<const k3d::typed_array<k3d::point2> >("const_k3d_point2_array",
		"Stores an immutable (read-only) collection of L{point2} values.");
	export_const_array<const k3d::typed_array<k3d::point3> >("const_k3d_point3_array",
		"Stores an immutable (read-only) collection of L{point3} values.");
	export_const_array<const k3d::typed_array<k3d::point4> >("const_k3d_point4_array",
		"Stores an immutable (read-only) collection of L{point4} values.");
	export_const_array<const k3d::typed_array<k3d::string_t> >("const_k3d_string_t_array",
		"Stores an immutable (read-only) collection of string values.");
	export_const_array<const k3d::typed_array<k3d::texture3> >("const_k3d_texture3_array",
		"Stores an immutable (read-only) collection of L{texture3} values.");
	export_const_array<const k3d::typed_array<k3d::uint8_t> >("const_k3d_uint8_t_array",
		"Stores an immutable (read-only) collection of unsigned 8-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::uint16_t> >("const_k3d_uint16_t_array",
		"Stores an immutable (read-only) collection of unsigned 16-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::uint32_t> >("const_k3d_uint32_t_array",
		"Stores an immutable (read-only) collection of unsigned 32-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::uint64_t> >("const_k3d_uint64_t_array",
		"Stores an immutable (read-only) collection of unsigned 64-bit integer values.");
	export_const_array<const k3d::typed_array<k3d::vector2> >("const_k3d_vector2_array",
		"Stores an immutable (read-only) collection of L{vector2} values.");
	export_const_array<const k3d::typed_array<k3d::vector3> >("const_k3d_vector3_array",
		"Stores an immutable (read-only) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

