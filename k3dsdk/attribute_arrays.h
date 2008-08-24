#ifndef K3DSDK_ATTRIBUTE_ARRAYS_H
#define K3DSDK_ATTRIBUTE_ARRAYS_H

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

#include "almost_equal.h"
#include "types.h"

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

namespace k3d
{

class array;

/// Defines a heterogeneous collection of named, shared arrays of equal length.  Note that the length of every
/// array in the collection must remain equal at all times.
/// For a concrete list of the datatypes that can be stored using attribute_arrays, see k3d::named_array_types.
class attribute_arrays :
	public std::map<string_t, boost::shared_ptr<array> >
{
public:
	/// Returns an object containing empty arrays with the same name and type as the originals.
	attribute_arrays clone_types() const;
	/// Returns an object containing deep copies of all the original arrays.
	attribute_arrays clone() const;
	/// Returns an object containing copies of a half-open range of all the original arrays.
	attribute_arrays clone(const uint_t Begin, const uint_t End) const;
	/// Returns true iff two collections are equivalent, using the imprecise semantics of almost_equal to compare values.
	const bool_t almost_equal(const attribute_arrays& Other, const uint64_t Threshold) const;

	typedef std::vector<const attribute_arrays*> attribute_arrays_collection;
	static attribute_arrays clone_types(const attribute_arrays_collection& AttributeArrays);

	/// Sets the size of every array in the collection.
	void resize(const uint_t NewSize);
};

/// Specialization of almost_equal that tests attribute_arrays for equality
template<>
class almost_equal<attribute_arrays>
{
	typedef attribute_arrays T;

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

#endif // K3DSDK_ATTRIBUTE_ARRAYS_H

