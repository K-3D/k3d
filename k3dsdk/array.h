#ifndef K3DSDK_ARRAY_H
#define K3DSDK_ARRAY_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "types.h"

namespace k3d
{

/// Abstract base class for an array - with it, we can create heterogeneous collections of arrays
class array
{
public:
	virtual ~array() {}

	/// Returns an empty array with the same concrete type as this array (a variation on virtual ctor)
	virtual array* clone_type() const = 0;
	/// Returns a copy of this array (virtual ctor)
	virtual array* clone() const = 0;
	/// Returns a copy of a half-open range of this array (a variation on virtual ctor)
	virtual array* clone(const uint_t Begin, const uint_t End) const = 0;
	/// Returns the size of this array
	virtual const uint_t size() const = 0;
	/// Returns true iff this array is empty
	virtual const bool_t empty() const = 0;
	/// Returns true iff this array is equivalent to the given array, using the imprecise semantics of almost_equal to compare values.
	/// \note: Returns false if given an array with a different concrete type.
	virtual const bool_t almost_equal(const array& Other, const uint64_t Threshold) const = 0;
};

} // namespace k3d

#endif // K3DSDK_ARRAY_H

