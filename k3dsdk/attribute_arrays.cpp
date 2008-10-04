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

#include "array.h"
#include "attribute_arrays.h"

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// attribute_arrays

const array* attribute_arrays::lookup(const string_t& Name) const
{
	const_iterator result = find(Name);
	return result == end() ? static_cast<const array*>(0) : result->second.get();
}

array* attribute_arrays::writable(const string_t& Name)
{
	iterator result = find(Name);
	return result == end() ? static_cast<array*>(0) : &result->second.writable();
}

attribute_arrays attribute_arrays::clone_types() const
{
	attribute_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone_type()));

	return result;
}

attribute_arrays attribute_arrays::clone() const
{
	attribute_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone()));

	return result;
}

attribute_arrays attribute_arrays::clone(const uint_t Begin, const uint_t End) const
{
	attribute_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone(Begin, End)));

	return result;
}

const bool_t attribute_arrays::almost_equal(const attribute_arrays& Other, const uint64_t Threshold) const
{
	// If we have differing numbers of arrays, we definitely aren't equal
	if(size() != Other.size())
		return false;

	for(attribute_arrays::const_iterator a = begin(), b = Other.begin(); a != end() && b != Other.end(); ++a, ++b)
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

attribute_arrays attribute_arrays::clone_types(const attribute_arrays_collection& AttributeArrays)
{
	attribute_arrays result;

	if(AttributeArrays.size())
	{
		for(const_iterator array = AttributeArrays[0]->begin(); array != AttributeArrays[0]->end(); ++array)
			result.insert(std::make_pair(array->first, array->second->clone_type()));

/*
		{
			bool_t use_array = true;

			for(uint_t i = 1; i < AttributeArrays.size(); ++i)
			{
				
			}

			if(use_array)
				result.insert(std::make_pair(array->first, array->second->clone_type()));
		}
*/
	}

	return result;
}

void attribute_arrays::resize(const uint_t NewSize)
{
	for(iterator array = begin(); array != end(); ++array)
		array->second.writable().resize(NewSize);
}

const bool_t attribute_arrays::match_size(const uint_t Size) const
{
	for(const_iterator array = begin(); array != end(); ++array)
	{
		if(array->second->size() != Size)
			return false;
	}
	
	return true;
}

} // namespace k3d

