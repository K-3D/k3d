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
	for(const_iterator array = begin(); array != end(); ++array)
		array->second->resize(NewSize);
}

} // namespace k3d

