#ifndef K3DSDK_GEOMETRY_H
#define K3DSDK_GEOMETRY_H

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

#include "mesh.h"

namespace k3d
{

namespace selection { class set; }
namespace selection { class storage; }

namespace geometry
{

namespace point_selection
{

class storage
{
public:
	storage(
		mesh::indices_t& IndexBegin,
		mesh::indices_t& IndexEnd,
		mesh::selection_t& Value
		);

	mesh::indices_t& index_begin;
	mesh::indices_t& index_end;
	mesh::selection_t& value;
};

storage* create(k3d::selection::set& Set);
storage* validate(k3d::selection::storage& GenericStorage);
void merge(const storage& Storage, mesh& Mesh);

} // namespace point_selection

namespace primitive_selection
{

k3d::selection::storage& create(k3d::selection::set& Set);

} // namespace primitive_selection

} // namespace geometry

} // namespace k3d

#endif // !K3DSDK_GEOMETRY_H

