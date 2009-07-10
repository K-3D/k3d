#ifndef K3DSDK_SELECTION_VALIDATION_H
#define K3DSDK_SELECTION_VALIDATION_H

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

#include "selection.h"

#include <sstream>
#include <stdexcept>

namespace k3d
{

//@{

/// Helper methods for use with in selection storage validate() functions only!

/// Tests storage to verify that it contains an array with given name and type, throws an exception otherwise.
template<typename ArrayT>
const ArrayT& require_const_array(const selection::storage& Storage, const string_t& Name)
{
	const ArrayT* const array = Storage.structure.lookup<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Storage.type + "] primitive missing array [" + Name + "]");

	return *array;
}

/// Tests storage to verify that it contains an array with given name and type, throws an exception otherwise.
template<typename ArrayT>
ArrayT& require_array(selection::storage& Storage, const string_t& Name)
{
	ArrayT* const array = Storage.structure.writable<ArrayT>(Name);

	if(!array)
		throw std::runtime_error("[" + Storage.type + "] selection missing array [" + Name + "]");

	return *array;
}

/// Tests storage to verify that it matches the given length, throws an exception otherwise.
template<typename ArrayT>
void require_array_size(const selection::storage& Storage, const ArrayT& Array, const string_t& ArrayName, const uint_t Reference)
{
	if(Array.size() != Reference)
	{
		std::ostringstream buffer;
		buffer << "[" << Storage.type << "] selection [" << ArrayName << "] incorrect array length [" << Array.size() << "], expected [" << Reference << "]";
		throw std::runtime_error(buffer.str());
	}
}

//@}

} // namespace k3d

#endif // !K3DSDK_SELECTION_VALIDATION_H

