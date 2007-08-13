#ifndef K3DSDK_NAMED_ARRAY_OPERATIONS_H
#define K3DSDK_NAMED_ARRAY_OPERATIONS_H

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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include "named_arrays.h"

namespace k3d
{

template<typename array_t>
array_t& get_array(named_arrays& Arrays, const std::string& Name, const size_t Count)
{
	array_t* result = 0;
	if(Arrays.count(Name))
		result = dynamic_cast<array_t*>(Arrays[Name].get());

	if(!result)
	{
		result = new array_t();
		Arrays[Name].reset(result);
	}

	if(result->size() != Count)
		result->resize(Count);

	return *result;
}

template<typename array_t>
const array_t* get_array(const named_arrays& Arrays, const std::string& Name)
{
	array_t* result = 0;
	if(Arrays.count(Name))
		result = dynamic_cast<array_t*>(Arrays.find(Name)->second.get());

	return result;
}

class named_array_copier
{
public:
	named_array_copier(const named_arrays& Source, named_arrays& Target);
	~named_array_copier();

	void push_back(const size_t Index);

private:
	class implementation;
	implementation* const m_implementation;

	named_array_copier(const named_array_copier&);
	named_array_copier& operator=(const named_array_copier&);
};

} // namespace k3d

#endif // !K3DSDK_NAMED_ARRAY_OPERATIONS_H

