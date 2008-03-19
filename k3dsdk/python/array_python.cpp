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

void export_arrays()
{
	export_array<k3d::typed_array<k3d::bool_t> >("k3d_bool_t_array",
		"Stores a mutable (read-write) collection of boolean values.");
	export_array<k3d::typed_array<k3d::double_t> >("k3d_double_t_array",
		"Stores a mutable (read-write) collection of floating-point values.");
	export_array<k3d::typed_array<k3d::int8_t> >("k3d_int8_t_array",
		"Stores a mutable (read-write) collection of 8-bit integer values.");
	export_array<k3d::typed_array<k3d::int16_t> >("k3d_int16_t_array",
		"Stores a mutable (read-write) collection of 16-bit integer values.");
	export_array<k3d::typed_array<k3d::int32_t> >("k3d_int32_t_array",
		"Stores a mutable (read-write) collection of 32-bit integer values.");
	export_array<k3d::typed_array<k3d::int64_t> >("k3d_int64_t_array",
		"Stores a mutable (read-write) collection of 64-bit integer values.");
	export_array<k3d::typed_array<k3d::mesh::polyhedra_t::polyhedron_type> >("k3d_mesh_polyheda_t_polyhedron_type_array",
		"Stores a mutable (read-write) collection of polyhedron type values.");
	export_array<k3d::typed_array<k3d::mesh::blobbies_t::operator_type> >("k3d_mesh_blobbies_t_operator_type_array",
		"Stores a mutable (read-write) collection of blobby operator type values.");
	export_array<k3d::typed_array<k3d::mesh::blobbies_t::primitive_type> >("k3d_mesh_blobbies_t_primitive_type_array",
		"Stores a mutable (read-write) collection of blobby primitive type values.");
	export_array<k3d::typed_array<k3d::imaterial*> >("k3d_imaterial_array",
		"Stores a mutable (read-write) collection of L{imaterial} objects.");
	export_array<k3d::typed_array<k3d::inode*> >("k3d_inode_array",
		"Stores a mutable (read-write) collection of L{inode} objects.");
	export_array<k3d::typed_array<k3d::color> >("k3d_color_array",
		"Stores a mutable (read-write) collection of L{color} values.");
	export_array<k3d::typed_array<k3d::matrix4> >("k3d_matrix4_array",
		"Stores a mutable (read-write) collection of L{matrix4} values.");
	export_array<k3d::typed_array<k3d::normal3> >("k3d_normal3_array",
		"Stores a mutable (read-write) collection of L{normal3} values.");
	export_array<k3d::typed_array<k3d::point2> >("k3d_point2_array",
		"Stores a mutable (read-write) collection of L{point2} values.");
	export_array<k3d::typed_array<k3d::point3> >("k3d_point3_array",
		"Stores a mutable (read-write) collection of L{point3} values.");
	export_array<k3d::typed_array<k3d::point4> >("k3d_point4_array",
		"Stores a mutable (read-write) collection of L{point4} values.");
	export_array<k3d::typed_array<k3d::string_t> >("k3d_string_t_array",
		"Stores a mutable (read-write) collection of string values.");
	export_array<k3d::typed_array<k3d::texture3> >("k3d_texture3_array",
		"Stores a mutable (read-write) collection of L{texture} values.");
	export_array<k3d::typed_array<k3d::uint8_t> >("k3d_uint8_t_array",
		"Stores a mutable (read-write) collection of unsigned 8-bit integer values.");
	export_array<k3d::typed_array<k3d::uint16_t> >("k3d_uint16_t_array",
		"Stores a mutable (read-write) collection of unsigned 16-bit integer values.");
	export_array<k3d::typed_array<k3d::uint32_t> >("k3d_uint32_t_array",
		"Stores a mutable (read-write) collection of unsigned 32-bit integer values.");
	export_array<k3d::typed_array<k3d::uint64_t> >("k3d_uint64_t_array",
		"Stores a mutable (read-write) collection of unsigned 64-bit integer values.");
	export_array<k3d::typed_array<k3d::vector2> >("k3d_vector2_array",
		"Stores a mutable (read-write) collection of L{vector2} values.");
	export_array<k3d::typed_array<k3d::vector3> >("k3d_vector3_array",
		"Stores a mutable (read-write) collection of L{vector3} values.");
}

} // namespace python

} // namespace k3d

