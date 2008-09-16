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

#include "array.h"
#include "named_arrays.h"

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// named_arrays

const array* named_arrays::lookup(const string_t& Name) const
{
	const_iterator result = find(Name);
	return result == end() ? static_cast<const array*>(0) : result->second.get();
}

array* named_arrays::writable(const string_t& Name)
{
	iterator result = find(Name);
	return result == end() ? static_cast<array*>(0) : &result->second.writable();
}

named_arrays named_arrays::clone_types() const
{
	named_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone_type()));

	return result;
}

named_arrays named_arrays::clone() const
{
	named_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone()));

	return result;
}

const bool_t named_arrays::almost_equal(const named_arrays& Other, const uint64_t Threshold) const
{
	// If we have differing numbers of arrays, we definitely aren't equal
	if(size() != Other.size())
		return false;

	for(named_arrays::const_iterator a = begin(), b = Other.begin(); a != end() && b != Other.end(); ++a, ++b)
	{
		// Each pair of arrays must have equal names
		if(a->first != b->first)
			return false;

		// If both arrays point to the same memory, they're equal
		if(a->second.get() == b->second.get())
			continue;

		// Perform element-wise comparisons of the two arrays
		if(a->second && b->second)
		{
			// The array::almost_equal method correctly handles type-mismatches between arrays
			if(a->second->almost_equal(*b->second, Threshold))
				continue;
		}

		// Either the element-wise comparison failed or one array was NULL and the other wasn't
		return false;
	}

	return true;
}

} // namespace k3d

