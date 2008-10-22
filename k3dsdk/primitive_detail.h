#ifndef K3DSDK_PRIMITIVE_DETAIL_H
#define K3DSDK_PRIMITIVE_DETAIL_H

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

#include <sstream>
#include <stdexcept>

namespace k3d
{

//@{

/// Helper methods for use with in mesh primitive validate() functions only!

template<typename ArrayT>
const ArrayT& require_const_array(const mesh::primitive& Primitive, const string_t& Name)
{
	const ArrayT* const array = Primitive.topology.lookup<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing array [" + Name + "]");

	return *array;
}

template<typename ArrayT>
ArrayT& require_array(mesh::primitive& Primitive, const string_t& Name)
{
	ArrayT* const array = Primitive.topology.writable<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Primitive.type + "] primitive missing array [" + Name + "]");

	return *array;
}

const attribute_arrays& require_const_attribute_arrays(const mesh::primitive& Primitive, const string_t& Name);

attribute_arrays& require_attribute_arrays(mesh::primitive& Primitive, const string_t& Name);

template<typename ArrayT>
void require_array_size(const mesh::primitive& Primitive, const ArrayT& Array, const string_t& ArrayName, const uint_t Reference)
{
	if(Array.size() != Reference)
	{
		std::ostringstream buffer;
		buffer << "[" << Primitive.type << "] primitive [" << ArrayName << "] incorrect array length [" << Array.size() << "], expected [" << Reference << "]";
		throw std::runtime_error(buffer.str());
	}
}

void require_attribute_arrays_size(const mesh::primitive& Primitive, const attribute_arrays& Attributes, const string_t& AttributesName, const uint_t Reference);

//@}

} // namespace k3d

#endif // !K3DSDK_PRIMITIVE_DETAIL_H

