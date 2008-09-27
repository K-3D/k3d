#ifndef K3DSDK_NAMED_ATTRIBUTE_ARRAYS_H
#define K3DSDK_NAMED_ATTRIBUTE_ARRAYS_H

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

#include "attribute_arrays.h"

namespace k3d
{

/// Defines a collection of named attribute_arrays objects.  The length of the individual
/// attribute_arrays objects may vary.
class named_attribute_arrays :
	public std::map<string_t, attribute_arrays>
{
public:
	/// Return an attribute_array by name, or NULL
	const attribute_arrays* lookup(const string_t& Name) const;
	/// Return an attribute_array by name, or NULL
	attribute_arrays* writable(const string_t& Name);
	/// Returns true iff two collections are equivalent, using the imprecise semantics of almost_equal to compare values.
	const bool_t almost_equal(const named_attribute_arrays& Other, const uint64_t Threshold) const;
};

/// Specialization of almost_equal that tests named_attribute_arrays for equality
template<>
class almost_equal<named_attribute_arrays>
{
	typedef named_attribute_arrays T;

public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B) const
	{
		return A.almost_equal(B, threshold);
	}

	const uint64_t threshold;
};

} // namespace k3d

#endif // K3DSDK_NAMED_ATTRIBUTE_ARRAYS_H

