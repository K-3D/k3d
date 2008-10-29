#ifndef K3DSDK_AXIS_H
#define K3DSDK_AXIS_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Provides an enumerated type for representing axes (useful in a lot of objects)
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ienumeration_property.h"
#include <iosfwd>

namespace k3d
{

/// Enumerates standard axes
typedef enum
{
	X,
	Y,
	Z
} axis;

/// Enumerates signed axes
typedef enum
{
	NX,
	PX,
	NY,
	PY,
	NZ,
	PZ
} signed_axis;

/// Returns descriptions of the allowed axis values, for use with enumeration properties
const ienumeration_property::enumeration_values_t& axis_values();
/// Returns descriptions of the allowed signed axis values, for use with enumeration properties
const ienumeration_property::enumeration_values_t& signed_axis_values();

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const axis& Value);
/// Serialization
std::istream& operator>>(std::istream& Stream, axis& Value);

/// Serialization
std::ostream& operator<<(std::ostream& Stream, const signed_axis& Value);
/// Serialization
std::istream& operator>>(std::istream& Stream, signed_axis& Value);

} // namespace k3d

#endif // !K3DSDK_AXIS_H
