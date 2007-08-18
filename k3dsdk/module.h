#ifndef K3DSDK_MODULE_H
#define K3DSDK_MODULE_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "k3d-platform-config.h"

#include "iplugin_registry.h"
#include "uuid.h"

/////////////////////////////////////////////////////////////////////////////
// K3D_MODULE_EXPORT

#if defined K3D_SYMBOL_VISIBILITY
	#if defined K3D_COMPILER_GCC
		#define K3D_MODULE_EXPORT __attribute__ ((visibility("default")))
	#elif defined K3D_COMPILER_MSVC
		#define K3D_MODULE_EXPORT __declspec(dllexport)
	#else
		#define K3D_MODULE_EXPORT
	#endif
#else
	#define K3D_MODULE_EXPORT
#endif

/////////////////////////////////////////////////////////////////////////////
// K3D_MODULE_START

/// Provides boilerplate for the required entry-point into all K-3D plugin modules
#define K3D_MODULE_START(Registry) \
extern "C" { K3D_MODULE_EXPORT void register_k3d_plugins(k3d::iplugin_registry& Registry) {

/////////////////////////////////////////////////////////////////////////////
// K3D_MODULE_END

/// Provides boilerplate for the required entry-point into all K-3D plugin modules
#define K3D_MODULE_END } }

namespace k3d
{

/// Defines the signature for the register_k3d_plugins function that must be exported by all plugin modules
typedef void (*register_plugins_entry_point)(iplugin_registry&);

} // namespace k3d

#endif // !K3DSDK_MODULE_H

