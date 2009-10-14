#ifndef K3DSDK_GEOMETRY_H
#define K3DSDK_GEOMETRY_H

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

namespace k3d
{

namespace legacy { class mesh; }
namespace selection { class set; }
namespace selection { class storage; }

namespace geometry
{

namespace point_selection
{

/// Gathers the member arrays of a point selection into a convenient package.
class const_storage
{
public:
	const_storage(
		const mesh::indices_t& IndexBegin,
		const mesh::indices_t& IndexEnd,
		const mesh::selection_t& Weight
		);

	const mesh::indices_t& index_begin;
	const mesh::indices_t& index_end;
	const mesh::selection_t& weight;
};

/// Gathers the member arrays of a mutable point selection into a convenient package.
class storage
{
public:
	storage(
		mesh::indices_t& IndexBegin,
		mesh::indices_t& IndexEnd,
		mesh::selection_t& Weight
		);

	mesh::indices_t& index_begin;
	mesh::indices_t& index_end;
	mesh::selection_t& weight;
};

/// Creates an empty point selection, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
storage* create(k3d::selection::set& Set);
/// Creates a point selection that assigns a uniform weight to all points, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
storage* create(k3d::selection::set& Set, const double_t Weight);

/// Tests the given storage to see if it is a valid point selection, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_storage* validate(const k3d::selection::storage& GenericStorage);

/// Tests the given storage to see if it is a valid point selection, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
storage* validate(k3d::selection::storage& GenericStorage);

/// Appends a uniform weight to all points.  Useful for "select all" and "deselect all".
void append(storage& Storage, const double_t Weight);

/// Appends a weight to a range of points.
void append(storage& Storage, const uint_t Begin, const uint_t End, const double_t Weight);

/// Merges a point selection with the points in a mesh.
void merge(const_storage& Storage, mesh& Mesh);

} // namespace point_selection

namespace primitive_selection
{

/// Gathers the member arrays of a primitive selection into a convenient package.
class const_storage
{
public:
	const_storage(
		const mesh::indices_t& PrimitiveBegin,
		const mesh::indices_t& PrimitiveEnd,
		const typed_array<k3d::int32_t>& PrimitiveSelectionType,
		const mesh::indices_t& PrimitiveFirstRange,
		const mesh::counts_t& PrimitiveRangeCount,
		const mesh::indices_t& IndexBegin,
		const mesh::indices_t& IndexEnd,
		const mesh::selection_t& Weight
		);

	const mesh::indices_t& primitive_begin;
	const mesh::indices_t& primitive_end;
	const typed_array<k3d::int32_t>& primitive_selection_type;
	const mesh::indices_t& primitive_first_range;
	const mesh::counts_t& primitive_range_count;
	const mesh::indices_t& index_begin;
	const mesh::indices_t& index_end;
	const mesh::selection_t& weight;
};

/// Gathers the member arrays of a mutable primitive selection into a convenient package.
class storage
{
public:
	storage(
		mesh::indices_t& PrimitiveBegin,
		mesh::indices_t& PrimitiveEnd,
		typed_array<k3d::int32_t>& PrimitiveSelectionType,
		mesh::indices_t& PrimitiveFirstRange,
		mesh::counts_t& PrimitiveRangeCount,
		mesh::indices_t& IndexBegin,
		mesh::indices_t& IndexEnd,
		mesh::selection_t& Weight
		);

	mesh::indices_t& primitive_begin;
	mesh::indices_t& primitive_end;
	typed_array<k3d::int32_t>& primitive_selection_type;
	mesh::indices_t& primitive_first_range;
	mesh::counts_t& primitive_range_count;
	mesh::indices_t& index_begin;
	mesh::indices_t& index_end;
	mesh::selection_t& weight;
};

/// Creates an empty primitive selection, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
storage* create(k3d::selection::set& Set);
/// Creates a primitive selection that will apply to all primitives, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
storage* create(k3d::selection::set& Set, const int32_t SelectionType);
/// Creates a primitive selection that will apply to a range of primitives, returning references to its member arrays.
/// The caller is responsible for the lifetime of the returned object.
storage* create(k3d::selection::set& Set, const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const int32_t SelectionType);

/// Tests the given storage to see if it is a valid primitive selection, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
const_storage* validate(const k3d::selection::storage& GenericStorage);

/// Tests the given storage to see if it is a valid primitive selection, returning references to its member arrays, or NULL.
/// The caller is responsible for the lifetime of the returned object.
storage* validate(k3d::selection::storage& GenericStorage);

/// Appends a selection weight to a range of components within a range of primitives.
void append(storage& Storage, const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const int32_t SelectionType, const uint_t Begin, const uint_t End, const double_t Weight);
/// Appends a selection weight to a range of components across all primitives.
void append(storage& Storage, const int32_t SelectionType, const uint_t Begin, const uint_t End, const double_t Weight);
/// Appends a selection weight to all components across all primitives.
void append(storage& Storage, const int32_t SelectionType, const double_t Weight);
/// Appends a selection weight to a range of components.
void append(storage& Storage, const uint_t Begin, const uint_t End, const double_t Weight);
/// Appends a selection weight to all components.
void append(storage& Storage, const double_t Weight);

/// Merges a primitive selection with the primitives in a mesh.
void merge(const_storage& Storage, mesh& Mesh);

} // namespace primitive_selection

namespace selection
{

/// Returns a selection set that applies a uniform weight to every component.
/// Useful for "select all" and "deselect all".
k3d::selection::set create(const double_t Weight);
/// Merges a selection set with the selections in a mesh.
void merge(const k3d::selection::set& Set, mesh& Mesh);
/// Merges a selection set with the selections in a mesh.
void merge(const k3d::selection::set& Set, legacy::mesh& Mesh);

} // namespace selection

} // namespace geometry

} // namespace k3d

#endif // !K3DSDK_GEOMETRY_H

