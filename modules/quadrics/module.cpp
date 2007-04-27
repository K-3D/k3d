// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

/// Namespace reserved for the core plugin module, to protect public symbols from name clashes with other modules
namespace libk3dconics
{

extern k3d::iplugin_factory& cone_factory();
extern k3d::iplugin_factory& cylinder_factory();
extern k3d::iplugin_factory& disk_factory();
extern k3d::iplugin_factory& hyperboloid_factory();
extern k3d::iplugin_factory& paraboloid_factory();
extern k3d::iplugin_factory& sphere_factory();
extern k3d::iplugin_factory& torus_factory();

} // namespace libk3dconics

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dconics::cone_factory());
	Registry.register_factory(libk3dconics::cylinder_factory());
	Registry.register_factory(libk3dconics::disk_factory());
	Registry.register_factory(libk3dconics::hyperboloid_factory());
	Registry.register_factory(libk3dconics::paraboloid_factory());
	Registry.register_factory(libk3dconics::sphere_factory());
	Registry.register_factory(libk3dconics::torus_factory());
K3D_MODULE_END

