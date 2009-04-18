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

#include "named_attribute_arrays.h"

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// named_attribute_arrays

const attribute_arrays* named_attribute_arrays::lookup(const string_t& Name) const
{
	const_iterator result = find(Name);
	return result == end() ? static_cast<attribute_arrays*>(0) : &result->second;
}

attribute_arrays* named_attribute_arrays::writable(const string_t& Name)
{
	iterator result = find(Name);
	return result == end() ? static_cast<attribute_arrays*>(0) : &result->second;
}

bool_t named_attribute_arrays::almost_equal(const named_attribute_arrays& Other, const uint64_t Threshold) const
{
	// If our sizes differ, we definitely ain't equal
	if(size() != Other.size())
		return false;

	// Test each pair ...
	named_attribute_arrays::const_iterator a, b;
	for(a = begin(), b = Other.begin(); a != end() && b != Other.end(); ++a, ++b)
	{
		// Each pair must have equal names
		if(a->first != b->first)
			return false;

		// Perform element-wise comparisons of each pair
		if(!a->second.almost_equal(b->second, Threshold))
			return false;
	}

	// If we have any leftovers, we're not equal ...
	if(a != end() || b != Other.end())
		return false;

	return true;
}

} // namespace k3d

