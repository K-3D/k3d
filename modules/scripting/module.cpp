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

/// Namespace reserved for the scripting plugin module, to protect public symbols from name clashes with other modules
namespace libk3dscripting
{

extern k3d::iplugin_factory& bitmap_modifier_script_factory();
extern k3d::iplugin_factory& bitmap_source_script_factory();
extern k3d::iplugin_factory& color_source_script_factory();
extern k3d::iplugin_factory& drawable_script_factory();
extern k3d::iplugin_factory& long_source_script_factory();
extern k3d::iplugin_factory& mesh_modifier_script_factory();
extern k3d::iplugin_factory& mesh_painter_script_factory();
extern k3d::iplugin_factory& mesh_source_script_factory();
extern k3d::iplugin_factory& null_output_script_factory();
extern k3d::iplugin_factory& render_engine_script_factory();
extern k3d::iplugin_factory& scalar_source_script_factory();
extern k3d::iplugin_factory& string_source_script_factory();
extern k3d::iplugin_factory& transform_modifier_script_factory();
extern k3d::iplugin_factory& transform_source_script_factory();
extern k3d::iplugin_factory& vector3_source_script_factory();

} // namespace libk3dscripting

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dscripting::bitmap_modifier_script_factory());
	Registry.register_factory(libk3dscripting::bitmap_source_script_factory());
	Registry.register_factory(libk3dscripting::color_source_script_factory());
	Registry.register_factory(libk3dscripting::drawable_script_factory());
	Registry.register_factory(libk3dscripting::long_source_script_factory());
	Registry.register_factory(libk3dscripting::mesh_modifier_script_factory());
	Registry.register_factory(libk3dscripting::mesh_painter_script_factory());
	Registry.register_factory(libk3dscripting::mesh_source_script_factory());
	Registry.register_factory(libk3dscripting::null_output_script_factory());
	Registry.register_factory(libk3dscripting::render_engine_script_factory());
	Registry.register_factory(libk3dscripting::scalar_source_script_factory());
	Registry.register_factory(libk3dscripting::string_source_script_factory());
	Registry.register_factory(libk3dscripting::transform_modifier_script_factory());
	Registry.register_factory(libk3dscripting::transform_source_script_factory());
	Registry.register_factory(libk3dscripting::vector3_source_script_factory());
K3D_MODULE_END

