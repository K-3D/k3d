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

#include "file_helpers.h"

namespace
{

class endian_test
{
public:
	endian_test()
	{
		m_union.Long = 1;
	}

	bool little_endian()
	{
		return 1 == m_union.Char[0];
	}

private:
	union
	{
	
		long Long;
		char Char[sizeof(long)];
		
	} m_union;
};

} // namespace

namespace k3d
{

void getline(std::istream& Stream, std::ostream& LineBuffer)
{
	for(char c = Stream.get(); !Stream.eof(); c = Stream.get())
	{
		if('\r' == c)
		{
			if('\n' == Stream.peek())
				Stream.get();
			break;
		}
		else if('\n' == c)
		{
			break;
		}

		LineBuffer << c;
	}
}

void getline(std::istream& Stream, std::string& LineBuffer)
{
	std::ostringstream buffer;
	getline(Stream, buffer);
	LineBuffer = buffer.str();
}

bool big_endian()
{
	static endian_test test;
	return !test.little_endian();
}

bool little_endian()
{
	static endian_test test;
	return test.little_endian();
}

} // namespace k3d

