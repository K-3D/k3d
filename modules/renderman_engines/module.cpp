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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/module.h>

namespace module
{

namespace renderman
{

namespace engine
{

extern k3d::iplugin_factory& air_factory();
extern k3d::iplugin_factory& aqsis_factory();
extern k3d::iplugin_factory& bmrt_factory();
extern k3d::iplugin_factory& delight_factory();
extern k3d::iplugin_factory& netprman_factory();
extern k3d::iplugin_factory& pixie_factory();
extern k3d::iplugin_factory& povman_factory();
extern k3d::iplugin_factory& prman_factory();
extern k3d::iplugin_factory& rdc_factory();

} // namespace engine

} // namespace renderman

} // namespace modules

K3D_MODULE_START(Registry)
	Registry.register_factory(module::renderman::engine::air_factory());
	Registry.register_factory(module::renderman::engine::aqsis_factory());
	Registry.register_factory(module::renderman::engine::bmrt_factory());
	Registry.register_factory(module::renderman::engine::delight_factory());
	Registry.register_factory(module::renderman::engine::netprman_factory());
	Registry.register_factory(module::renderman::engine::pixie_factory());
	Registry.register_factory(module::renderman::engine::povman_factory());
	Registry.register_factory(module::renderman::engine::prman_factory());
	Registry.register_factory(module::renderman::engine::rdc_factory());
K3D_MODULE_END

