// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the test plugin module, to protect public symbols from name clashes with other modules
namespace libk3dtest
{

extern k3d::iplugin_factory& legacy_mesh_conversion_factory();
extern k3d::iplugin_factory& memory_pools_factory();
extern k3d::iplugin_factory& mesh_diff_factory();
extern k3d::iplugin_factory& mesh_to_stdout_factory();

} // namespace libk3dtest

K3D_MODULE_START(Registry)

	Registry.register_factory(libk3dtest::legacy_mesh_conversion_factory());
	Registry.register_factory(libk3dtest::memory_pools_factory());
	Registry.register_factory(libk3dtest::mesh_diff_factory());
	Registry.register_factory(libk3dtest::mesh_to_stdout_factory());

K3D_MODULE_END

