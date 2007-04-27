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

/// Namespace reserved for the renderman plugin module, to protect public symbols from name clashes with other modules
namespace libk3drenderman
{

extern k3d::iplugin_factory& area_light_factory();
extern k3d::iplugin_factory& array_1d_factory();
extern k3d::iplugin_factory& array_2d_factory();
extern k3d::iplugin_factory& array_3d_factory();
extern k3d::iplugin_factory& background_plane_factory();
extern k3d::iplugin_factory& background_sphere_factory();
extern k3d::iplugin_factory& csg_operator_factory();
extern k3d::iplugin_factory& csg_solid_factory();
extern k3d::iplugin_factory& delayed_read_archive_factory();
extern k3d::iplugin_factory& direct_texture_map_factory();
extern k3d::iplugin_factory& displacement_shader_factory();
extern k3d::iplugin_factory& imager_shader_factory();
extern k3d::iplugin_factory& lat_long_environment_map_factory();
extern k3d::iplugin_factory& light_factory();
extern k3d::iplugin_factory& light_shader_factory();
extern k3d::iplugin_factory& material_factory();
extern k3d::iplugin_factory& read_archive_factory();
extern k3d::iplugin_factory& render_engine_factory();
extern k3d::iplugin_factory& renderman_script_factory();
extern k3d::iplugin_factory& shadow_map_factory();
extern k3d::iplugin_factory& surface_shader_factory();
extern k3d::iplugin_factory& texture_map_factory();
extern k3d::iplugin_factory& volume_shader_factory();

} // namespace libk3drenderman

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3drenderman::area_light_factory());
	Registry.register_factory(libk3drenderman::array_1d_factory());
	Registry.register_factory(libk3drenderman::array_2d_factory());
	Registry.register_factory(libk3drenderman::array_3d_factory());
	Registry.register_factory(libk3drenderman::background_plane_factory());
	Registry.register_factory(libk3drenderman::background_sphere_factory());
	Registry.register_factory(libk3drenderman::csg_operator_factory());
	Registry.register_factory(libk3drenderman::csg_solid_factory());
	Registry.register_factory(libk3drenderman::delayed_read_archive_factory());
	Registry.register_factory(libk3drenderman::direct_texture_map_factory());
	Registry.register_factory(libk3drenderman::displacement_shader_factory());
	Registry.register_factory(libk3drenderman::imager_shader_factory());
	Registry.register_factory(libk3drenderman::lat_long_environment_map_factory());
	Registry.register_factory(libk3drenderman::light_factory());
	Registry.register_factory(libk3drenderman::light_shader_factory());
	Registry.register_factory(libk3drenderman::material_factory());
	Registry.register_factory(libk3drenderman::read_archive_factory());
	Registry.register_factory(libk3drenderman::render_engine_factory());
	Registry.register_factory(libk3drenderman::renderman_script_factory());
	Registry.register_factory(libk3drenderman::shadow_map_factory());
	Registry.register_factory(libk3drenderman::surface_shader_factory());
	Registry.register_factory(libk3drenderman::texture_map_factory());
	Registry.register_factory(libk3drenderman::volume_shader_factory());
K3D_MODULE_END

