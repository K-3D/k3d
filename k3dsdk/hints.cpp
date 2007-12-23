// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
// selection_changed

selection_changed_t* selection_changed()
{
	static selection_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_geometry_changed

mesh_geometry_changed_t* mesh_geometry_changed()
{
	static mesh_geometry_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_topology_changed

mesh_topology_changed_t* mesh_topology_changed()
{
	static mesh_topology_changed_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// mesh_deleted

mesh_deleted_t* mesh_deleted()
{
	static mesh_deleted_t hint;
	return &hint;
}

//////////////////////////////////////////////////////////////////////////////
// print

print::print(iunknown* Hint) :
	hint(Hint)
{
}

std::ostream& operator<<(std::ostream& Stream, const print& RHS)
{
	if(RHS.hint)
	{
		if(dynamic_cast<selection_changed_t*>(RHS.hint))
		{
			Stream << "hint: selection changed";
		}
		else if(dynamic_cast<mesh_geometry_changed_t*>(RHS.hint))
		{
			Stream << "hint: mesh geometry changed";
		}
		else if(dynamic_cast<mesh_topology_changed_t*>(RHS.hint))
		{
			Stream << "hint: mesh topology changed";
		}
		else
		{
			Stream << "hint: " << typeid(*RHS.hint).name();
		}
	}
	else
	{
		Stream << "hint: (none)";
	}
	
	return Stream;
}

} // namespace hint

} // namespace k3d

