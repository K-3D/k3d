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

#include <string>
#include <typeinfo>
#include <vector>

namespace k3d
{

/// Returns the string representation for a registered type, or empty string
const std::string type_string(const std::type_info& Info);
/// Returns the string representation for a registered type, or emtpy string
template<typename T>
const std::string type_string()
{
	return type_string(typeid(T));
}

/// Returns the type_info representation of a registered type, or NULL
const std::type_info* type_id(const std::string& Name);

const std::type_info& type_id_k3d_bitmap_ptr();

/// Returns the demangled name of a type, or the input string if demangling isn't available
const std::string demangle(const std::type_info& Type);

} // namespace k3d

#endif // K3DSDK_TYPE_REGISTRY_H

