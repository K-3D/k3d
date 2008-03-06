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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

namespace module
{

namespace deformation
{

extern k3d::iplugin_factory& bend_points_factory();
extern k3d::iplugin_factory& bulge_points_factory();
extern k3d::iplugin_factory& center_points_factory();
extern k3d::iplugin_factory& cylindrical_wave_points_factory();
extern k3d::iplugin_factory& linear_point_noise_factory();
extern k3d::iplugin_factory& linear_wave_points_factory();
extern k3d::iplugin_factory& rotate_points_factory();
extern k3d::iplugin_factory& scale_points_factory();
extern k3d::iplugin_factory& shear_points_factory();
extern k3d::iplugin_factory& smooth_points_factory();
extern k3d::iplugin_factory& sphereize_points_factory();
extern k3d::iplugin_factory& taper_points_factory();
extern k3d::iplugin_factory& transform_points_factory();
extern k3d::iplugin_factory& translate_points_factory();
extern k3d::iplugin_factory& tweak_points_factory();
extern k3d::iplugin_factory& twist_points_factory();

} // namespace deformation

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::deformation::bend_points_factory());
	Registry.register_factory(module::deformation::bulge_points_factory());
	Registry.register_factory(module::deformation::center_points_factory());
	Registry.register_factory(module::deformation::cylindrical_wave_points_factory());
	Registry.register_factory(module::deformation::linear_point_noise_factory());
	Registry.register_factory(module::deformation::linear_wave_points_factory());
	Registry.register_factory(module::deformation::rotate_points_factory());
	Registry.register_factory(module::deformation::scale_points_factory());
	Registry.register_factory(module::deformation::shear_points_factory());
	Registry.register_factory(module::deformation::smooth_points_factory());
	Registry.register_factory(module::deformation::sphereize_points_factory());
	Registry.register_factory(module::deformation::taper_points_factory());
	Registry.register_factory(module::deformation::transform_points_factory());
	Registry.register_factory(module::deformation::translate_points_factory());
	Registry.register_factory(module::deformation::tweak_points_factory());
	Registry.register_factory(module::deformation::twist_points_factory());
K3D_MODULE_END

