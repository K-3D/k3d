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

/** \file
		\author Tim Shead (tshead@k-3d.com)
*/

#include "k3d-platform-config.h"
#include "result.h"
#include "system.h"
#include "uuid.h"

#ifdef K3D_API_WIN32
	#include "win32.h"
#else // K3D_API_WIN32
	#include <uuid/uuid.h>
#endif // !K3D_API_WIN32

#include <iomanip>
#include <iostream>

namespace k3d
{

uuid::uuid() :
	data1(0),
	data2(0),
	data3(0),
	data4(0)
{
}

uuid::uuid(const boost::uint32_t Data1, const boost::uint32_t Data2, const boost::uint32_t Data3, const boost::uint32_t Data4) :
	data1(Data1),
	data2(Data2),
	data3(Data3),
	data4(Data4)
{
}

const uuid uuid::null()
{
	return uuid(0, 0, 0, 0);
}

const uuid uuid::random()
{
	uuid result;

#ifdef K3D_API_WIN32
	CoCreateGuid(reinterpret_cast<UUID*>(&result));
#else // K3D_API_WIN32
	uuid_t temp;
	uuid_generate(temp);
	memcpy(&result, temp, sizeof(result));
#endif // !K3D_API_WIN32

	return result;
}

bool operator<(const uuid& LHS, const uuid& RHS)
{
	if(LHS.data1 == RHS.data1)
	{
		if(LHS.data2 == RHS.data2)
		{
			if(LHS.data3 == RHS.data3)
			{
				return LHS.data4 < RHS.data4;
			}
			else
				return LHS.data3 < RHS.data3;
		}
		else
			return LHS.data2 < RHS.data2;
	}

	return LHS.data1 < RHS.data1;
}

bool operator==(const uuid& LHS, const uuid& RHS)
{
	return (LHS.data1 == RHS.data1) && (LHS.data2 == RHS.data2) && (LHS.data3 == RHS.data3) && (LHS.data4 == RHS.data4);
}

bool operator!=(const uuid& LHS, const uuid& RHS)
{
	return !(LHS == RHS);
}

std::ostream& operator<<(std::ostream& Stream, const k3d::uuid& Value)
{
	std::ostream::fmtflags oldflags(Stream.flags());
	Stream.setf(std::ostream::right);

	const char oldfill(Stream.fill());
	Stream.fill('0');

	Stream << std::hex << std::setw(8) << Value.data1 << " " << std::setw(8) << Value.data2 << " " << std::setw(8) << Value.data3 << " " << std::setw(8) << Value.data4;

	Stream.fill(oldfill);
	Stream.flags(oldflags);

	return Stream;
}

std::istream& operator>>(std::istream& Stream, k3d::uuid& Value)
{
	std::istream::fmtflags oldflags(Stream.flags());

	Stream >> std::hex >> Value.data1 >> Value.data2 >> Value.data3 >> Value.data4;

	Stream.flags(oldflags);

	return Stream;
}

} // namespace

