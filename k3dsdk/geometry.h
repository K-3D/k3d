#ifndef K3DSDK_GEOMETRY_H
#define K3DSDK_GEOMETRY_H

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

#include "mesh.h"

namespace k3d
{

namespace legacy { class mesh; }
namespace selection { class set; }
namespace selection { class storage; }

namespace geometry
{

namespace point_selection
{

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

storage* create(k3d::selection::set& Set);
void reset(storage& Storage, const double_t Weight);
const_storage* validate(const k3d::selection::storage& GenericStorage);
storage* validate(k3d::selection::storage& GenericStorage);
void merge(const_storage& Storage, mesh& Mesh);

} // namespace point_selection

namespace primitive_selection
{

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

storage* create(k3d::selection::set& Set);
void reset(storage& Storage, const double_t Weight);
void reset(storage& Storage, const int32_t SelectionType, const double_t Weight);
const_storage* validate(const k3d::selection::storage& GenericStorage);
storage* validate(k3d::selection::storage& GenericStorage);
void merge(const_storage& Storage, mesh& Mesh);

} // namespace primitive_selection

void reset_selection(k3d::selection::set& Set, const double_t Weight);
void merge_selection(const k3d::selection::set& Set, mesh& Mesh);
void merge_selection(const k3d::selection::set& Set, legacy::mesh& Mesh);

} // namespace geometry

} // namespace k3d

#endif // !K3DSDK_GEOMETRY_H

