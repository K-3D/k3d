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

#include <vector>

namespace k3d
{

/// Abstract base class for an array of objects, used to store heterogeneous collections of arrays
class array
{
public:
	virtual ~array() {}

	/// Returns an empty array of the same type as the original (virtual ctor)
	virtual array* clone_type() const = 0;
	/// Returns a copy of the original array (virtual ctor)
	virtual array* clone() const = 0;
	/// Returns a copy of a half-open range of the original array (virtual ctor)
	virtual array* clone(size_t Begin, size_t End) const = 0;
	/// Returns the size of the array
	virtual const size_t size() const = 0;
	/// Returns true iff the array is empty
	virtual const bool empty() const = 0;
};

/// Strongly-typed dynamic array of objects, based on std::vector
template<typename T>
class typed_array :
	public std::vector<T>,
	public array
{
	typedef std::vector<T> base_type;
	typedef typed_array<T> this_type;

public:
	typedef typename std::vector<T>::size_type size_type;

	typed_array() :
		std::vector<T>()
	{
	}

	explicit typed_array(size_type count) :
		std::vector<T>(count)
	{
	}

	typed_array(size_type count, const T& val) :
		std::vector<T>(count, val)
	{
	}

	typed_array(const typed_array<T>& right) :
		std::vector<T>(right)
	{
	}

	template<class IteratorT>
	typed_array(IteratorT first, IteratorT last) :
		std::vector<T>(first, last)
	{
	}

	array* clone_type() const
	{
	    return new this_type();
	}

	array* clone() const
	{
	    return new this_type(*this);
	}

	array* clone(size_t Begin, size_t End) const
	{
	    return new this_type(this->begin() + Begin, this->begin() + End);
	}

	const size_t size() const
	{
		return base_type::size();
	}

	const bool empty() const
	{
		return base_type::empty();
	}
};

} // namespace k3d

#endif // K3DSDK_ARRAY_H

