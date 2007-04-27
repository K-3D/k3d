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

#include <boost/cstdint.hpp>
#include <iosfwd>

namespace k3d
{

/// Identifier used to uniquely identify plugin modules and plugin objects
class uuid
{
public:
	uuid();
	uuid(const uint32_t Data1, const uint32_t Data2, const uint32_t Data3, const uint32_t Data4);

	/// Creates a NULL (all zeros) uuid
	static const uuid null();
	/// Creates a unique uuid based on the current system time and a high-quality pseudo-random number generators
	static const uuid random();

	friend bool operator<(const uuid& LHS, const uuid& RHS);
	friend bool operator==(const uuid& LHS, const uuid& RHS);
	friend bool operator!=(const uuid& LHS, const uuid& RHS);

	friend std::ostream& operator<<(std::ostream& Stream, const uuid& Value);
	friend std::istream& operator>>(std::istream& Stream, uuid& Value);

	uint32_t data1;
	uint32_t data2;
	uint32_t data3;
	uint32_t data4;
};

} // namespace k3d

#endif // K3DSDK_UUID_H

