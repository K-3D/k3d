#ifndef K3DSDK_TYPE_REGISTRY_H
#define K3DSDK_TYPE_REGISTRY_H

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

#include "types.h"
#include <typeinfo>

namespace k3d
{

/// Returns true iff the given type is registered
const bool_t type_registered(const std::type_info& Info);
/// Returns true iff the given type if registered
template<typename T>
const bool_t type_registered()
{
	return type_registered(typeid(T));
}

/// Returns the string representation for a registered type, or an empty string
const string_t type_string(const std::type_info& Info);
/// Returns the string representation for a registered type, or an empty string
template<typename T>
const string_t type_string()
{
	return type_string(typeid(T));
}

/// Returns the type_info representation of a registered type, or NULL
const std::type_info* type_id(const string_t& Name);

const std::type_info& type_id_k3d_bitmap_ptr();

/// Returns an informal, human-readable string representation for the given type.
/// On platforms that support it, the returned string will be the fully-qualified
/// type-name, including namespaces.  Otherwise, the result will be the mangled
/// symbol name or some other unique string representation.
///
/// Note: this function is only for use in error-messages and for troubleshooting,
/// you cannot make any assumptions about the return values.
const string_t demangle(const std::type_info& Type);

} // namespace k3d

#endif // !K3DSDK_TYPE_REGISTRY_H

