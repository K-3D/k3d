// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Tim Shead <tshead@k-3d.com>
*/

#include "utility.h"

namespace module
{

namespace luxrender
{

const k3d::point3 convert(const k3d::point3& Value)
{
	return k3d::point3(-Value[0], Value[1], Value[2]);
}

const k3d::vector3 convert(const k3d::vector3& Value)
{
	return k3d::vector3(-Value[0], Value[1], Value[2]);
}

const k3d::matrix4 convert(const k3d::matrix4& Value)
{
	return k3d::transpose(k3d::scale3(-1, 1, 1) * Value);
}

} // namespace luxrender

} // namespace module

