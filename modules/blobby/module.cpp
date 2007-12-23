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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

namespace module
{

namespace blobby
{

extern k3d::iplugin_factory& blobby_add_operator_factory();
extern k3d::iplugin_factory& blobby_div_operator_factory();
extern k3d::iplugin_factory& blobby_ellipsoid_factory();
extern k3d::iplugin_factory& blobby_max_operator_factory();
extern k3d::iplugin_factory& blobby_min_operator_factory();
extern k3d::iplugin_factory& blobby_mult_operator_factory();
extern k3d::iplugin_factory& blobby_segment_factory();
extern k3d::iplugin_factory& blobby_sub_operator_factory();
extern k3d::iplugin_factory& edges_to_blobby_factory();
extern k3d::iplugin_factory& points_to_blobby_factory();
extern k3d::iplugin_factory& polygonize_blobbies_factory();

} // namespace blobby

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::blobby::blobby_add_operator_factory());
	Registry.register_factory(module::blobby::blobby_div_operator_factory());
	Registry.register_factory(module::blobby::blobby_ellipsoid_factory());
	Registry.register_factory(module::blobby::blobby_max_operator_factory());
	Registry.register_factory(module::blobby::blobby_min_operator_factory());
	Registry.register_factory(module::blobby::blobby_mult_operator_factory());
	Registry.register_factory(module::blobby::blobby_segment_factory());
	Registry.register_factory(module::blobby::blobby_sub_operator_factory());
	Registry.register_factory(module::blobby::edges_to_blobby_factory());
	Registry.register_factory(module::blobby::points_to_blobby_factory());
	Registry.register_factory(module::blobby::polygonize_blobbies_factory());
K3D_MODULE_END

