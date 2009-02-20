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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the primitives plugin module, to protect public symbols from name clashes with other modules
namespace libk3dprimitives
{

extern k3d::iplugin_factory& poly_cone_factory();
extern k3d::iplugin_factory& poly_cube_factory();
extern k3d::iplugin_factory& poly_cushion_factory();
extern k3d::iplugin_factory& poly_cylinder_factory();
extern k3d::iplugin_factory& poly_disk_factory();
extern k3d::iplugin_factory& poly_grid_factory();
extern k3d::iplugin_factory& poly_icosahedron_factory();
extern k3d::iplugin_factory& poly_sphere_factory();
extern k3d::iplugin_factory& poly_terrain_fft_factory();
extern k3d::iplugin_factory& poly_terrain_hfbm_factory();
extern k3d::iplugin_factory& poly_torus_factory();
extern k3d::iplugin_factory& polyhedron_factory();
extern k3d::iplugin_factory& sphere_eversion_factory();

} // namespace libk3dprimitives

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dprimitives::poly_cone_factory());
	Registry.register_factory(libk3dprimitives::poly_cube_factory());
	Registry.register_factory(libk3dprimitives::poly_cushion_factory());
	Registry.register_factory(libk3dprimitives::poly_cylinder_factory());
	Registry.register_factory(libk3dprimitives::poly_disk_factory());
	Registry.register_factory(libk3dprimitives::poly_grid_factory());
	Registry.register_factory(libk3dprimitives::poly_icosahedron_factory());
	Registry.register_factory(libk3dprimitives::poly_sphere_factory());
	Registry.register_factory(libk3dprimitives::poly_terrain_fft_factory());
	Registry.register_factory(libk3dprimitives::poly_terrain_hfbm_factory());
	Registry.register_factory(libk3dprimitives::poly_torus_factory());
	Registry.register_factory(libk3dprimitives::polyhedron_factory());
	Registry.register_factory(libk3dprimitives::sphere_eversion_factory());
K3D_MODULE_END

