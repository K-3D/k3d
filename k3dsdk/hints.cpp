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
	return new bitmap_dimensions_changed();
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
	return new bitmap_pixels_changed();
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

ihint* selection_changed_t::clone()
{
	return new selection_changed_t();
}

void selection_changed_t::print(std::ostream& Stream)
{
	Stream << "selection_changed_t";
}

selection_changed_t* selection_changed()
{
	static selection_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_geometry_changed

ihint* mesh_geometry_changed_t::clone()
{
	return new mesh_geometry_changed_t();
}

void mesh_geometry_changed_t::print(std::ostream& Stream)
{
	Stream << "mesh_geometry_changed_t";
}

mesh_geometry_changed_t* mesh_geometry_changed()
{
	static mesh_geometry_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_topology_changed

ihint* mesh_topology_changed_t::clone()
{
	return new mesh_topology_changed_t();
}

void mesh_topology_changed_t::print(std::ostream& Stream)
{
	Stream << "mesh_topology_changed_t";
}

mesh_topology_changed_t* mesh_topology_changed()
{
	static mesh_topology_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_deleted

ihint* mesh_deleted_t::clone()
{
	return new mesh_deleted_t();
}

void mesh_deleted_t::print(std::ostream& Stream)
{
	Stream << "mesh_deleted_t";
}

mesh_deleted_t* mesh_deleted()
{
	static mesh_deleted_t hint;
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

