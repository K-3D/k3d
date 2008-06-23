#ifndef K3DSDK_RESOURCE_H
#define K3DSDK_RESOURCE_H

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

#include <k3dsdk/types.h>
#include <vector>

namespace k3d
{

/// Provides functionality for working with "resources" - binary data
/// that is compiled into executables for easy access.
namespace resource
{

/// Initializes a resource, associating a string key (the Path) with the given data.
const bool_t initialize(const string_t& Path, unsigned char* Data, const uint64_t ByteCount);

/// Defines storage for a list of resource keys
typedef std::vector<string_t> keys_t;
/// Returns the current list of resource keys
const keys_t keys();

/// Returns a resource as a string (note: it is up to the caller to decide whether this is safe).
/// Returns empty string if the resource does not exist.
const string_t get_string(const string_t& Path);

} // namespace resource

} // namespace k3d

#endif // K3DSDK_RESOURCE_H

