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

////////////////////////////////////////////////////////////////////////////////////////////
// current_indent

long& current_indent(std::ios& Stream)
{
	static const int index = std::ios::xalloc();
	return Stream.iword(index);
}

////////////////////////////////////////////////////////////////////////////////////////////
// push_indent

std::ostream& push_indent(std::ostream& Stream)
{
	current_indent(Stream)++;
	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// pop_indent

std::ostream& pop_indent(std::ostream& Stream)
{
	if(current_indent(Stream) != 0)
		current_indent(Stream)--;

	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// standard_indent

std::ostream& standard_indent(std::ostream& Stream)
{
	Stream << std::string(2 * current_indent(Stream), ' '); 
	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// block_size

static long& block_size(std::ios& Stream)
{
	static const int index = std::ios::xalloc();
	return Stream.iword(index);
}

////////////////////////////////////////////////////////////////////////////////////////////
// block_index

static long& block_index(std::ios& Stream)
{
	static const int index = std::ios::xalloc();
	return Stream.iword(index);
}

////////////////////////////////////////////////////////////////////////////////////////////
// start_block

start_block::start_block(const uint_t BlockSize) :
	block_size(BlockSize)
{
}

std::ostream& operator<<(std::ostream& Stream, const start_block& RHS)
{
	block_size(Stream) = RHS.block_size;
	block_index(Stream) = 0;
	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// block_delimiter

std::ostream& block_delimiter(std::ostream& Stream)
{
	if(block_size(Stream))
	{
		if(0 == (block_index(Stream) % block_size(Stream)))
		{
			if(block_index(Stream))
				Stream << "\n";

			Stream << standard_indent;
		}
		else
		{
			Stream << " ";
		}

		block_index(Stream) += 1;
	}
	else
	{
		Stream << " ";
	}

	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// finish_block

std::ostream& finish_block(std::ostream& Stream)
{
	block_size(Stream) = 0;
	return Stream;
}

} // namespace k3d

