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

extern k3d::iplugin_factory& format_scalar_factory();
extern k3d::iplugin_factory& scalar_add_factory();
extern k3d::iplugin_factory& scalar_div_factory();
extern k3d::iplugin_factory& scalar_expression_factory();
extern k3d::iplugin_factory& scalar_modulo_factory();
extern k3d::iplugin_factory& scalar_multiply_factory();
extern k3d::iplugin_factory& scalar_property_factory();
extern k3d::iplugin_factory& scalar_sine_factory();
extern k3d::iplugin_factory& scalar_sub_factory();

} // namespace libk3dcore

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dcore::format_scalar_factory());
	Registry.register_factory(libk3dcore::scalar_add_factory());
	Registry.register_factory(libk3dcore::scalar_div_factory());
	Registry.register_factory(libk3dcore::scalar_expression_factory());
	Registry.register_factory(libk3dcore::scalar_modulo_factory());
	Registry.register_factory(libk3dcore::scalar_multiply_factory());
	Registry.register_factory(libk3dcore::scalar_property_factory());
	Registry.register_factory(libk3dcore::scalar_sine_factory());
	Registry.register_factory(libk3dcore::scalar_sub_factory());
K3D_MODULE_END

