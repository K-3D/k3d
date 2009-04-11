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

namespace scalar
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

} // namespace scalar

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::scalar::format_scalar_factory());
	Registry.register_factory(module::scalar::scalar_add_factory());
	Registry.register_factory(module::scalar::scalar_div_factory());
	Registry.register_factory(module::scalar::scalar_expression_factory());
	Registry.register_factory(module::scalar::scalar_modulo_factory());
	Registry.register_factory(module::scalar::scalar_multiply_factory());
	Registry.register_factory(module::scalar::scalar_property_factory());
	Registry.register_factory(module::scalar::scalar_sine_factory());
	Registry.register_factory(module::scalar::scalar_sub_factory());
K3D_MODULE_END

