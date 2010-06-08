// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/result.h>
#include <k3dsdk/system.h>
#include <k3dsdk/uuid.h>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>

#include <iomanip>
#include <iostream>

namespace k3d
{

uuid::uuid() :
	boost::uuids::uuid(boost::uuids::nil_generator()())
{
}

uuid::uuid(const boost::uuids::uuid& Other) :
	boost::uuids::uuid(Other)
{
}

uuid::uuid(const uint32_t Data1, const uint32_t Data2, const uint32_t Data3, const uint32_t Data4)
{
	data[0] = Data1 >> 24 & 0xff;
	data[1] = Data1 >> 16 & 0xff;
	data[2] = Data1 >> 8 & 0xff;
	data[3] = Data1 >> 0 & 0xff;

	data[4] = Data2 >> 24 & 0xff;
	data[5] = Data2 >> 16 & 0xff;
	data[6] = Data2 >> 8 & 0xff;
	data[7] = Data2 >> 0 & 0xff;

	data[8] = Data3 >> 24 & 0xff;
	data[9] = Data3 >> 16 & 0xff;
	data[10] = Data3 >> 8 & 0xff;
	data[11] = Data3 >> 0 & 0xff;

	data[12] = Data4 >> 24 & 0xff;
	data[13] = Data4 >> 16 & 0xff;
	data[14] = Data4 >> 8 & 0xff;
	data[15] = Data4 >> 0 & 0xff;
}

const uuid uuid::null()
{
	return uuid(boost::uuids::nil_generator()());
}

const uuid uuid::random()
{
	return uuid(boost::uuids::random_generator()());
}

std::ostream& operator<<(std::ostream& Stream, const k3d::uuid& Value)
{
	std::ostream::fmtflags oldflags(Stream.flags());
	Stream.setf(std::ostream::right);

	const char oldfill(Stream.fill());
	Stream.fill('0');

	Stream << std::hex;
	for(int i = 0; i != Value.size(); ++i)
	{
		if(i && (0 == i % 4))
			Stream << " ";
		Stream << std::setw(2) << static_cast<uint32_t>(Value.data[i]);
	}

	Stream.fill(oldfill);
	Stream.flags(oldflags);

	return Stream;
}

std::istream& operator>>(std::istream& Stream, k3d::uuid& Value)
{
	std::istream::fmtflags oldflags(Stream.flags());

	uint32_t data1, data2, data3, data4;
	Stream >> std::hex >> data1 >> data2 >> data3 >> data4;
	Stream.flags(oldflags);

	Value = k3d::uuid(data1, data2, data3, data4);

	return Stream;
}

} // namespace

