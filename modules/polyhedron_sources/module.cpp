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

namespace module
{

namespace polyhedron
{

namespace sources
{

extern k3d::iplugin_factory& poly_cone_factory();
extern k3d::iplugin_factory& poly_cube_factory();
extern k3d::iplugin_factory& poly_cushion_factory();
extern k3d::iplugin_factory& poly_cylinder_factory();
extern k3d::iplugin_factory& poly_disk_factory();
extern k3d::iplugin_factory& poly_grid_factory();
extern k3d::iplugin_factory& poly_icosahedron_factory();
extern k3d::iplugin_factory& poly_sphere_factory();
extern k3d::iplugin_factory& poly_torus_factory();
extern k3d::iplugin_factory& polyhedron_factory();
extern k3d::iplugin_factory& sphere_eversion_factory();

} // namespace sources

} // namespace polyhedron

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::polyhedron::sources::poly_cone_factory());
	Registry.register_factory(module::polyhedron::sources::poly_cube_factory());
	Registry.register_factory(module::polyhedron::sources::poly_cushion_factory());
	Registry.register_factory(module::polyhedron::sources::poly_cylinder_factory());
	Registry.register_factory(module::polyhedron::sources::poly_disk_factory());
	Registry.register_factory(module::polyhedron::sources::poly_grid_factory());
	Registry.register_factory(module::polyhedron::sources::poly_icosahedron_factory());
	Registry.register_factory(module::polyhedron::sources::poly_sphere_factory());
	Registry.register_factory(module::polyhedron::sources::poly_torus_factory());
	Registry.register_factory(module::polyhedron::sources::polyhedron_factory());
	Registry.register_factory(module::polyhedron::sources::sphere_eversion_factory());
K3D_MODULE_END

