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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

namespace module
{

/// Namespace reserved for the luxrender plugin module, to protect public symbols from name clashes with other modules
namespace luxrender
{

extern k3d::iplugin_factory& bilinear_interpolation_texture_factory();
extern k3d::iplugin_factory& carpaint_material_factory();
extern k3d::iplugin_factory& distant_light_factory();
extern k3d::iplugin_factory& environment_light_factory();
extern k3d::iplugin_factory& glass_material_factory();
extern k3d::iplugin_factory& glossy_material_factory();
extern k3d::iplugin_factory& goniometric_light_factory();
extern k3d::iplugin_factory& image_texture_factory();
extern k3d::iplugin_factory& matte_material_factory();
extern k3d::iplugin_factory& matte_translucent_material_factory();
extern k3d::iplugin_factory& metal_material_factory();
extern k3d::iplugin_factory& mirror_material_factory();
extern k3d::iplugin_factory& mix_material_factory();
extern k3d::iplugin_factory& null_material_factory();
extern k3d::iplugin_factory& point_light_factory();
extern k3d::iplugin_factory& render_engine_factory();
extern k3d::iplugin_factory& scalar_image_texture_factory();
extern k3d::iplugin_factory& sky_light_factory();
extern k3d::iplugin_factory& sun_light_factory();

} // namespace luxrender

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::luxrender::bilinear_interpolation_texture_factory());
	Registry.register_factory(module::luxrender::carpaint_material_factory());
	Registry.register_factory(module::luxrender::distant_light_factory());
	Registry.register_factory(module::luxrender::environment_light_factory());
	Registry.register_factory(module::luxrender::glass_material_factory());
	Registry.register_factory(module::luxrender::glossy_material_factory());
	Registry.register_factory(module::luxrender::goniometric_light_factory());
	Registry.register_factory(module::luxrender::image_texture_factory());
	Registry.register_factory(module::luxrender::matte_material_factory());
	Registry.register_factory(module::luxrender::matte_translucent_material_factory());
	Registry.register_factory(module::luxrender::metal_material_factory());
	Registry.register_factory(module::luxrender::mirror_material_factory());
	Registry.register_factory(module::luxrender::mix_material_factory());
	Registry.register_factory(module::luxrender::null_material_factory());
	Registry.register_factory(module::luxrender::point_light_factory());
	Registry.register_factory(module::luxrender::render_engine_factory());
	Registry.register_factory(module::luxrender::scalar_image_texture_factory());
	Registry.register_factory(module::luxrender::sky_light_factory());
	Registry.register_factory(module::luxrender::sun_light_factory());
K3D_MODULE_END

