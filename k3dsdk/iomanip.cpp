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

#include "iomanip.h"
#include "types.h"

#include <iostream>

namespace k3d
{

long& current_indent(std::ios& Stream)
{
	static const int index = std::ios::xalloc();
	return Stream.iword(index);
}

std::ostream& push_indent(std::ostream& Stream)
{
	current_indent(Stream)++;
	return Stream;
}

std::ostream& pop_indent(std::ostream& Stream)
{
	if(current_indent(Stream) != 0)
		current_indent(Stream)--;

	return Stream;
}

} // namespace k3d

