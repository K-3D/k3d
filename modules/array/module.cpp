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
		\brief Implements procedures required to export K-3D objects from the core module
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the array plugin module, to protect public symbols from name clashes with other modules
namespace libk3darray
{

//extern k3d::iplugin_factory& cube_face_environment_map_factory();
extern k3d::iplugin_factory& alternate_array_1d_factory();
extern k3d::iplugin_factory& alternate_array_2d_factory();
extern k3d::iplugin_factory& alternate_array_3d_factory();
extern k3d::iplugin_factory& rotate_array_1d_factory();
extern k3d::iplugin_factory& rotate_array_2d_factory();
extern k3d::iplugin_factory& rotate_array_3d_factory();
extern k3d::iplugin_factory& translate_array_1d_factory();
extern k3d::iplugin_factory& translate_array_2d_factory();
extern k3d::iplugin_factory& translate_array_3d_factory();

} // namespace libk3darray

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3darray::alternate_array_1d_factory());
	Registry.register_factory(libk3darray::alternate_array_2d_factory());
	Registry.register_factory(libk3darray::alternate_array_3d_factory());
	Registry.register_factory(libk3darray::rotate_array_1d_factory());
	Registry.register_factory(libk3darray::rotate_array_2d_factory());
	Registry.register_factory(libk3darray::rotate_array_3d_factory());
	Registry.register_factory(libk3darray::translate_array_1d_factory());
	Registry.register_factory(libk3darray::translate_array_2d_factory());
	Registry.register_factory(libk3darray::translate_array_3d_factory());
K3D_MODULE_END
