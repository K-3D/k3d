#ifndef K3DSDK_NAMED_ARRAY_RANDOM_ACCESS_COPIER_H
#define K3DSDK_NAMED_ARRAY_RANDOM_ACCESS_COPIER_H

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

/** \file Provides  named array copier that allows copying of named arrays in a random order
	\author Timothy M. Shead
*/

#include <k3dsdk/named_arrays.h>

namespace k3d
{

/// Resizes the given named arrays to Size
void resize(named_arrays& Arrays, const uint_t Size);

/// Handles random-access copying between named arrays
class named_array_random_access_copier
{
public:
	named_array_random_access_copier(const named_arrays& Source, named_arrays& Target);
	~named_array_random_access_copier();

	/// Copies the given source index value from each source array to the TargetIndex in each corresponding target array
	void copy(const uint_t SourceIndex, const uint_t TargetIndex);
	/// Computes a weighted sum of N values from each source array and copies the result to the corresponding target array at the given TargetIndex
	void copy(const uint_t Count, const uint_t* Indices, const double_t* Weights, const uint_t TargetIndex);

private:
	class implementation;
	implementation* const m_implementation;

	named_array_random_access_copier(const named_array_random_access_copier&);
	named_array_random_access_copier& operator=(const named_array_random_access_copier&);
};

} // namespace k3d

#endif // !K3DSDK_NAMED_ARRAY_RANDOM_ACCESS_COPIER_H

