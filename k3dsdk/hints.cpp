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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "hints.h"
#include "result.h"

#include <ostream>

namespace k3d
{

namespace hint
{

//////////////////////////////////////////////////////////////////////////////
// bitmap_dimensions_changed

ihint* bitmap_dimensions_changed::clone()
{
	return new bitmap_dimensions_changed(*this);
}

void bitmap_dimensions_changed::print(std::ostream& Stream)
{
	Stream << "bitmap_dimensions_changed";
}

bitmap_dimensions_changed* bitmap_dimensions_changed::instance()
{
	static bitmap_dimensions_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// bitmap_pixels_changed

ihint* bitmap_pixels_changed::clone()
{
	return new bitmap_pixels_changed(*this);
}

void bitmap_pixels_changed::print(std::ostream& Stream)
{
	Stream << "bitmap_pixels_changed";
}

bitmap_pixels_changed* bitmap_pixels_changed::instance()
{
	static bitmap_pixels_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// selection_changed

ihint* selection_changed::clone()
{
	return new selection_changed(*this);
}

void selection_changed::print(std::ostream& Stream)
{
	Stream << "selection_changed";
}

selection_changed* selection_changed::instance()
{
	static selection_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_geometry_changed

ihint* mesh_geometry_changed::clone()
{
	return new mesh_geometry_changed(*this);
}

void mesh_geometry_changed::print(std::ostream& Stream)
{
	Stream << "mesh_geometry_changed";
}

mesh_geometry_changed* mesh_geometry_changed::instance()
{
	static mesh_geometry_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_topology_changed

ihint* mesh_topology_changed::clone()
{
	return new mesh_topology_changed(*this);
}

void mesh_topology_changed::print(std::ostream& Stream)
{
	Stream << "mesh_topology_changed";
}

mesh_topology_changed* mesh_topology_changed::instance()
{
	static mesh_topology_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_deleted

ihint* mesh_deleted::clone()
{
	return new mesh_deleted(*this);
}

void mesh_deleted::print(std::ostream& Stream)
{
	Stream << "mesh_deleted";
}

mesh_deleted* mesh_deleted::instance()
{
	static mesh_deleted hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// file_changed

ihint* file_changed::clone()
{
	return new file_changed(*this);
}

void file_changed::print(std::ostream& Stream)
{
	Stream << "file_changed";
}

file_changed* file_changed::instance()
{
	static file_changed hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// print

print::print(ihint* Hint) :
	hint(Hint)
{
}

std::ostream& operator<<(std::ostream& Stream, const print& RHS)
{
	if(RHS.hint)
		RHS.hint->print(Stream);
	else
		Stream << "(none)";
	
	return Stream;
}

} // namespace hint

} // namespace k3d

