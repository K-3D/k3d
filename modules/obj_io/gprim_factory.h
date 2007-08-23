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

#include <k3dsdk/mesh.h>
#include <k3dsdk/point4.h>

namespace libk3dobjio
{

/// Provides a simplified interface for adding geometric primitives to a mesh
/** \todo Move this into the sdk once it matures for awhile */
class gprim_factory
{
public:
	gprim_factory(k3d::mesh& Mesh);
	~gprim_factory();

	void add_point(const k3d::point3& Point);
	void add_point(const k3d::point4& Point);
	void add_polygon(const k3d::mesh::indices_t& Points);
	void add_nurbs_patch(const size_t UOrder, const size_t VOrder, const k3d::mesh::indices_t& Points, const k3d::mesh::knots_t& UKnots, const k3d::mesh::knots_t VKnots);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace libk3dobjio

