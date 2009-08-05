#ifndef K3DSDK_PRIMITIVE_VALIDATION_H
#define K3DSDK_PRIMITIVE_VALIDATION_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/mesh.h>

#include <sstream>
#include <stdexcept>

namespace k3d
{

//@{

/// Helper methods for use with in mesh primitive validate() functions only!

/// Performs automated validation of the structure of a mesh primitive:
///
/// * Validates that "constant" tables have length 1.
/// * Validates that the mesh has point data if the primitive contains point indices.
/// * Validates that primitive point indices are within bounds.
/// * Validates that every array in a given table is the same length.
/// * Validates that matching structure and attribute tables contain the same number of rows.
void require_valid_primitive(const mesh& Mesh, const mesh::primitive& Primitive);

/// Tests a primitive to verify that it contains the named structure table, throws an exception otherwise.
const mesh::table_t& require_structure(const mesh::primitive& Primitive, const string_t& Name);
/// Tests a primitive to verify that it contains the named structure table, throws an exception otherwise.
mesh::table_t& require_structure(mesh::primitive& Primitive, const string_t& Name);
/// Tests a primitive to verify that it contains the named attribute table, throws an exception otherwise.
const mesh::table_t& require_attributes(const mesh::primitive& Primitive, const string_t& Name);
/// Tests a primitive to verify that it contains the named attribute table, throws an exception otherwise.
mesh::table_t& require_attributes(mesh::primitive& Primitive, const string_t& Name);

/// Tests a table to verify that it matches the given row count, throws an exception otherwise.
void require_table_row_count(const mesh::primitive& Primitive, const table& Table, const string_t& TableName, const uint_t RowCount);

/// Tests a table to verify that it contains an array with given name and type, throws an exception otherwise.
template<typename ArrayT>
const ArrayT& require_array(const mesh::primitive& Primitive, const mesh::table_t& Table, const string_t& Name)
{
	const ArrayT* const array = Table.lookup<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing array [" + Name + "]");

	return *array;
}

/// Tests a table to verify that it contains an array with given name and type, throws an exception otherwise.
template<typename ArrayT>
ArrayT& require_array(mesh::primitive& Primitive, mesh::table_t& Table, const string_t& Name)
{
	ArrayT* const array = Table.writable<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing array [" + Name + "]");

	return *array;
}

/// Tests an array to verify that it has metadata with the given name and value, throws an exception otherwise.
void require_metadata(const mesh::primitive& Primitive, const array& Array, const string_t& ArrayName, const string_t& MetadataName, const string_t& MetadataValue);

//@}

} // namespace k3d

#endif // !K3DSDK_PRIMITIVE_VALIDATION_H

