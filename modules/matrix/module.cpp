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
	\author Joaquín Duo (joaduo at lugmen org ar)
*/

#include <k3dsdk/module.h>

namespace module
{

namespace matrix
{

extern k3d::iplugin_factory& frozen_matrix_factory();
extern k3d::iplugin_factory& inverse_factory();
extern k3d::iplugin_factory& look_factory();
extern k3d::iplugin_factory& orientation_factory();
extern k3d::iplugin_factory& position_factory();
extern k3d::iplugin_factory& scale_factory();
extern k3d::iplugin_factory& view_factory();

} // namespace matrix

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::matrix::frozen_matrix_factory());
	Registry.register_factory(module::matrix::inverse_factory());
	Registry.register_factory(module::matrix::look_factory());
	Registry.register_factory(module::matrix::orientation_factory());
	Registry.register_factory(module::matrix::position_factory());
	Registry.register_factory(module::matrix::scale_factory());
	Registry.register_factory(module::matrix::view_factory());
K3D_MODULE_END

