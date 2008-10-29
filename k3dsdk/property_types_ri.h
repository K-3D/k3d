#ifndef K3DSDK_PROPERTY_TYPES_RI_H
#define K3DSDK_PROPERTY_TYPES_RI_H

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

#include "types_ri.h"

#include <boost/mpl/vector.hpp>

namespace k3d
{

namespace property
{

namespace ri
{

/// Enumerates all of the types that can be stored using RenderMan attributes.
typedef boost::mpl::vector<
	k3d::ri::color,
	k3d::ri::hpoint,
	k3d::ri::integer,
	k3d::ri::matrix,
	k3d::ri::normal,
	k3d::ri::point,
	k3d::ri::real,
	k3d::ri::string,
	k3d::ri::vector
	> attribute_types;

/// Enumerates all of the types that can be stored using RenderMan options.
typedef boost::mpl::vector<
	k3d::ri::color,
	k3d::ri::hpoint,
	k3d::ri::integer,
	k3d::ri::matrix,
	k3d::ri::normal,
	k3d::ri::point,
	k3d::ri::real,
	k3d::ri::string,
	k3d::ri::vector
	> option_types;

} // namespace ri

} // namespace property

} // namespace k3d

#endif // !K3DSDK_PROPERTY_TYPES_RI_H

