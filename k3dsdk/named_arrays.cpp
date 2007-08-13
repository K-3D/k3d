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
#include "named_arrays.h"

namespace k3d
{

///////////////////////////////////////////////////////////////////////////
// named_arrays

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

named_arrays named_arrays::clone(size_t Begin, size_t End) const
{
	named_arrays result;

	for(const_iterator array = begin(); array != end(); ++array)
		result.insert(std::make_pair(array->first, array->second->clone(Begin, End)));

	return result;
}

} // namespace k3d

