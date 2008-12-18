#ifndef K3DSDK_METADATA_KEYS_H
#define K3DSDK_METADATA_KEYS_H

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

#include "types.h"

namespace k3d
{

namespace metadata
{

namespace key
{

/// Storage for a collection of authors
const string_t authors();
/// Storage for a copyright notice
const string_t copyright();

/// Metadata key "k3d:domain" is used to define the domain over which a value / collection of values is defined.
/// It is commonly used with generic mesh primitives to specify that one array contains indices into another.
const string_t domain();

/// Metadata key "k3d:selection-component" is used to identify mesh arrays that hold selection values.  The
/// key value will be a selection type from the k3d::selection::type enumeration.
const string_t selection_component();

} // namespace key

namespace value
{

/// Metadata value for use with "k3d:domain" that specifies the mesh point array
const string_t mesh_point_indices_domain();
/// Metadata value for use with "k3d:domain" that specifies that an array of floating-point values is a NURBS knot vector
const string_t nurbs_knot_vector_domain();

} // namespace value

} // namespace metadata

} // namespace k3d

#endif // !K3DSDK_METADATA_KEYS_H

