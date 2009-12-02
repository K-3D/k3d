// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

namespace mesh
{

extern k3d::iplugin_factory& array_1d_factory();
extern k3d::iplugin_factory& array_2d_factory();
extern k3d::iplugin_factory& array_3d_factory();
extern k3d::iplugin_factory& merge_mesh_factory();
extern k3d::iplugin_factory& weld_factory();

} // namespace mesh

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::mesh::array_1d_factory());
	Registry.register_factory(module::mesh::array_2d_factory());
	Registry.register_factory(module::mesh::array_3d_factory());
	Registry.register_factory(module::mesh::merge_mesh_factory());
	Registry.register_factory(module::mesh::weld_factory());
K3D_MODULE_END

