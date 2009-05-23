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

#include "geometry.h"
#include "selection_validation.h"
#include "selection.h"

namespace k3d
{

namespace geometry
{

namespace point_selection
{

//////////////////////////////////////////////////////////////////////
// storage

storage::storage(
	mesh::indices_t& IndexBegin,
	mesh::indices_t& IndexEnd,
	mesh::selection_t& Value
		) :
	index_begin(IndexBegin),
	index_end(IndexEnd),
	value(Value)
{
}

//////////////////////////////////////////////////////////////////////
// create

storage* create(k3d::selection::set& Set)
{
	k3d::selection::storage& generic_storage = Set.create("point");

	storage* const result = new storage(
		generic_storage.structure.create<mesh::indices_t>("index_begin"),
		generic_storage.structure.create<mesh::indices_t>("index_end"),
		generic_storage.structure.create<mesh::selection_t>("value")
		);

	return result;
}

//////////////////////////////////////////////////////////////////////
// validate

storage* validate(k3d::selection::storage& Storage)
{
	if(Storage.type != "point")
		return 0;

	try
	{
		mesh::indices_t& index_begin = require_array<mesh::indices_t>(Storage, "index_begin");
		mesh::indices_t& index_end = require_array<mesh::indices_t>(Storage, "index_end");
		mesh::selection_t& value = require_array<mesh::selection_t>(Storage, "value");

		require_array_size(Storage, index_end, "index_end", index_begin.size());
		require_array_size(Storage, value, "value", index_begin.size());

		return new storage(index_begin, index_end, value);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace point_selection

namespace primitive_selection
{

k3d::selection::storage& create(k3d::selection::set& Set)
{
	k3d::selection::storage& storage = Set.create("primitive");
	k3d::uint_t_array& primitive_begin = storage.structure.create<k3d::uint_t_array>("primitive_begin");
	k3d::uint_t_array& primitive_end = storage.structure.create<k3d::uint_t_array>("primitive_end");
	k3d::typed_array<k3d::int32_t>& selection_type = storage.structure.create<k3d::typed_array<k3d::int32_t> >("primitive_selection_type");
	k3d::uint_t_array& primitive_first_range = storage.structure.create<k3d::uint_t_array>("primitive_first_range");
	k3d::uint_t_array& primitive_range_count = storage.structure.create<k3d::uint_t_array>("primitive_range_count");
	k3d::uint_t_array& index_begin = storage.structure.create<k3d::uint_t_array>("index_begin");
	k3d::uint_t_array& index_end = storage.structure.create<k3d::uint_t_array>("index_end");
	k3d::typed_array<k3d::double_t>& value = storage.structure.create<k3d::typed_array<k3d::double_t> >("value");

	return storage;
}

} // namespace primitive_selection

} // namespace geometry

} // namespace k3d

