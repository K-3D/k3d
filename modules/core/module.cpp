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
		\brief Implements procedures required to export K-3D objects from the core module
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the core plugin module, to protect public symbols from name clashes with other modules
namespace libk3dcore
{

extern k3d::iplugin_factory& axes_factory();
extern k3d::iplugin_factory& camera_factory();
extern k3d::iplugin_factory& color_property_factory();
extern k3d::iplugin_factory& format_time_factory();
extern k3d::iplugin_factory& frozen_transformation_factory();
extern k3d::iplugin_factory& look_at_factory();
extern k3d::iplugin_factory& material_factory();
extern k3d::iplugin_factory& null_factory();
extern k3d::iplugin_factory& orientation_factory();
extern k3d::iplugin_factory& position_factory();
extern k3d::iplugin_factory& print_stdout_factory();
extern k3d::iplugin_factory& scale_factory();
extern k3d::iplugin_factory& show_matrix_factory();
extern k3d::iplugin_factory& snap_factory();
extern k3d::iplugin_factory& view_matrix_factory();

} // namespace libk3dcore

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dcore::axes_factory());
	Registry.register_factory(libk3dcore::camera_factory());
	Registry.register_factory(libk3dcore::color_property_factory());
	Registry.register_factory(libk3dcore::format_time_factory());
	Registry.register_factory(libk3dcore::frozen_transformation_factory());
	Registry.register_factory(libk3dcore::look_at_factory());
	Registry.register_factory(libk3dcore::material_factory());
	Registry.register_factory(libk3dcore::null_factory());
	Registry.register_factory(libk3dcore::orientation_factory());
	Registry.register_factory(libk3dcore::position_factory());
	Registry.register_factory(libk3dcore::print_stdout_factory());
	Registry.register_factory(libk3dcore::scale_factory());
	Registry.register_factory(libk3dcore::show_matrix_factory());
	Registry.register_factory(libk3dcore::snap_factory());
	Registry.register_factory(libk3dcore::view_matrix_factory());
K3D_MODULE_END

