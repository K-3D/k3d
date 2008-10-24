#ifndef K3DSDK_PARALLEL_BLOCKED_RANGE_H
#define K3DSDK_PARALLEL_BLOCKED_RANGE_H

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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-parallel-config.h>
#include <k3dsdk/types.h>

namespace k3d
{

namespace parallel
{

template<typename ValueT>
class blocked_range
{
public:
	//! Type of a value
	/** Called a const_iterator for sake of algorithms that need to treat a blocked_range
	as an STL container. */
	typedef ValueT const_iterator;

	//! Type for size of a range
	typedef uint_t size_type;

	//! Construct range with default-constructed values for begin and end.
	/** Requires that Value have a default constructor. */
	blocked_range() :
		m_begin(),
		m_end()
	{
	}

	//! Construct range over half-open interval [begin,end), with the given grainsize.
	blocked_range(ValueT Begin, ValueT End, size_type Grainsize = 1) : 
		m_end(End),
		m_begin(Begin),
		m_grainsize(Grainsize) 
	{
//		__TBB_ASSERT( m_grainsize>0, "grainsize must be positive" );
	}

	//! Beginning of range.
	const_iterator begin() const
	{
		return m_begin;
	}

	//! One past last value in range.
	const_iterator end() const
	{
		return m_end;
	}

	//! Size of the range
	/** Unspecified if end()<begin(). */
	size_type size() const
	{
//		__TBB_ASSERT( !(end() < begin()), "size() unspecified if end()<begin()" );
		return size_type(m_end - m_begin);
	}

	//! The grain size for this range.
	size_type grainsize() const
	{
		return m_grainsize;
	}

	//------------------------------------------------------------------------
	// Methods that implement Range concept
	//------------------------------------------------------------------------

	//! True if range is empty.
	bool empty() const
	{
		return !(m_begin < m_end);
	}

	//! True if range is divisible.
	/** Unspecified if end()<begin(). */
	bool is_divisible() const
	{
		return m_grainsize < size();
	}

	//! Split range.  
	/** The new Range *this has the second half, the old range r has the first half. 
	Unspecified if end()<begin() or !is_divisible(). */
	template<typename SplitT>
	blocked_range(blocked_range& r, SplitT split) : 
		m_end(r.m_end),
		m_begin(do_split(r)),
		m_grainsize(r.m_grainsize)
	{
	}

private:
	/** NOTE: m_end MUST be declared before m_begin, otherwise the forking constructor will break. */
	ValueT m_end;
	ValueT m_begin;
	size_type m_grainsize;

	//! Auxilary function used by forking constructor.
	/** Using this function lets us not require that Value support assignment or default construction. */
	static ValueT do_split(blocked_range& r)
	{
//		__TBB_ASSERT( r.is_divisible(), "cannot split blocked_range that is not divisible" );
		ValueT middle = r.m_begin + (r.m_end - r.m_begin) / 2u;
		r.m_end = middle;
		return middle;
	}
};

} // namespace parallel

} // namespace k3d

#endif // !K3DSDK_PARALLEL_BLOCKED_RANGE_H

