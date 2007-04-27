#ifndef K3DSDK_ASPECT_RATIOS_H
#define K3DSDK_ASPECT_RATIOS_H

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

/// Encapsulates a named aspect ratio
struct aspect_ratio
{
	aspect_ratio(const std::string& Name, const std::string& Description, const double Value) : name(Name), description(Description), value(Value) { }

	std::string name;
	std::string description;
	double value;	
};

/// Defines a collection of named aspect ratios
typedef std::vector<aspect_ratio> aspect_ratios_t;

/// Returns a list of useful aspect ratios
const aspect_ratios_t& aspect_ratios();
/// Returns descriptions of the available aspect ratios for use with enumeration properties
const ienumeration_property::enumeration_values_t& aspect_ratio_values();

} // namespace k3d

#endif // !K3DSDK_ASPECT_RATIOS_H


