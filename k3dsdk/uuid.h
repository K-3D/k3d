#ifndef K3DSDK_UUID_H
#define K3DSDK_UUID_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <k3dsdk/types.h>
#define BOOST_UUID_NO_SIMD // This can be removed again once the Boost 1.56 - 1.57 regression in the SSE2 implementation of operator< is fixed.
#include <boost/uuid/uuid.hpp>
#include <iosfwd>

namespace k3d
{

/// Identifier used to uniquely identify plugin modules and plugin objects
class uuid :
	public boost::uuids::uuid
{
public:
	uuid();
	explicit uuid(const boost::uuids::uuid&);
	uuid(const uint32_t Data1, const uint32_t Data2, const uint32_t Data3, const uint32_t Data4);

	/// Creates a NULL (all zeros) uuid
	static const uuid null();
	/// Creates a unique uuid based on the current system time and a high-quality pseudo-random number generators
	static const uuid random();

	friend std::ostream& operator<<(std::ostream& Stream, const uuid& Value);
	friend std::istream& operator>>(std::istream& Stream, uuid& Value);
};

} // namespace k3d

#endif // !K3DSDK_UUID_H

