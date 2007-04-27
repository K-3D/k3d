#ifndef K3DSDK_RESOLUTIONS_H
#define K3DSDK_RESOLUTIONS_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "ienumeration_property.h"

#include <string>
#include <vector>

namespace k3d
{

/// Encapsulates a named image resolution
struct resolution
{
	resolution(const std::string& Name, const std::string& Description, const unsigned long Width, const unsigned long Height) : name(Name), description(Description), width(Width), height(Height) { }

	std::string name;
	std::string description;
	unsigned long width;
	unsigned long height;
};

/// Defines a collection of named image resolutions
typedef std::vector<resolution> resolutions_t;

/// Returns a list of useful image resolutions
const resolutions_t& resolutions();
/// Returns descriptions of the available resolutions for use with enumeration properties
const ienumeration_property::enumeration_values_t& resolution_values();

} // namespace k3d

#endif // !K3DSDK_RESOLUTIONS_H


