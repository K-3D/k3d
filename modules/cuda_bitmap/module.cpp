// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file cuda_bitmap module
	\author Evan Lezar (evanlezar@gmail.com)
*/

#include <k3dsdk/module.h>
namespace module
{

namespace cuda_bitmap
{

extern k3d::iplugin_factory& cuda_add_factory();
extern k3d::iplugin_factory& cuda_multiply_factory();
extern k3d::iplugin_factory& cuda_subtract_factory();

} // namespace cuda_bitmap

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::cuda_bitmap::cuda_add_factory());
	Registry.register_factory(module::cuda_bitmap::cuda_multiply_factory());
	Registry.register_factory(module::cuda_bitmap::cuda_subtract_factory());
K3D_MODULE_END 

