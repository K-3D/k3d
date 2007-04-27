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

/// Namespace reserved for the deformation plugin module, to protect public symbols from name clashes with other modules
namespace libk3ddeformation
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

} // namespace libk3ddeformation

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3ddeformation::bend_points_factory());
	Registry.register_factory(libk3ddeformation::bulge_points_factory());
	Registry.register_factory(libk3ddeformation::center_points_factory());
	Registry.register_factory(libk3ddeformation::cylindrical_wave_points_factory());
	Registry.register_factory(libk3ddeformation::linear_point_noise_factory());
	Registry.register_factory(libk3ddeformation::linear_wave_points_factory());
	Registry.register_factory(libk3ddeformation::rotate_points_factory());
	Registry.register_factory(libk3ddeformation::scale_points_factory());
	Registry.register_factory(libk3ddeformation::shear_points_factory());
	Registry.register_factory(libk3ddeformation::smooth_points_factory());
	Registry.register_factory(libk3ddeformation::sphereize_points_factory());
	Registry.register_factory(libk3ddeformation::taper_points_factory());
	Registry.register_factory(libk3ddeformation::transform_points_factory());
	Registry.register_factory(libk3ddeformation::translate_points_factory());
	Registry.register_factory(libk3ddeformation::tweak_points_factory());
	Registry.register_factory(libk3ddeformation::twist_points_factory());
K3D_MODULE_END


