// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the register_k3d_plugins entry-point required of all K-3D modules
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

/// Namespace reserved for the yafray plugin module, to protect public symbols from name clashes with other modules
namespace libk3dyafray
{

extern k3d::iplugin_factory& area_light_factory();
extern k3d::iplugin_factory& global_photon_light_factory();
extern k3d::iplugin_factory& hemi_light_factory();
extern k3d::iplugin_factory& material_factory();
extern k3d::iplugin_factory& photon_light_factory();
extern k3d::iplugin_factory& point_light_factory();
extern k3d::iplugin_factory& render_engine_factory();
extern k3d::iplugin_factory& soft_light_factory();
extern k3d::iplugin_factory& spot_light_factory();
extern k3d::iplugin_factory& sun_light_factory();

} // namespace libk3dyafray

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dyafray::area_light_factory());
	Registry.register_factory(libk3dyafray::global_photon_light_factory());
	Registry.register_factory(libk3dyafray::hemi_light_factory());
	Registry.register_factory(libk3dyafray::material_factory());
	Registry.register_factory(libk3dyafray::photon_light_factory());
	Registry.register_factory(libk3dyafray::point_light_factory());
	Registry.register_factory(libk3dyafray::render_engine_factory());
	Registry.register_factory(libk3dyafray::soft_light_factory());
	Registry.register_factory(libk3dyafray::spot_light_factory());
	Registry.register_factory(libk3dyafray::sun_light_factory());
K3D_MODULE_END
