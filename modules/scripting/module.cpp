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

/// Namespace reserved for the scripting plugin module, to protect public symbols from name clashes with other modules
namespace scripting
{

extern k3d::iplugin_factory& bitmap_modifier_script_factory();
extern k3d::iplugin_factory& bitmap_source_script_factory();
extern k3d::iplugin_factory& color_source_script_factory();
extern k3d::iplugin_factory& command_node_script_factory();
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

} // namespace scripting

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::scripting::bitmap_modifier_script_factory());
	Registry.register_factory(module::scripting::bitmap_source_script_factory());
	Registry.register_factory(module::scripting::color_source_script_factory());
	Registry.register_factory(module::scripting::command_node_script_factory());
	Registry.register_factory(module::scripting::drawable_script_factory());
	Registry.register_factory(module::scripting::long_source_script_factory());
	Registry.register_factory(module::scripting::mesh_modifier_script_factory());
	Registry.register_factory(module::scripting::mesh_painter_script_factory());
	Registry.register_factory(module::scripting::mesh_source_script_factory());
	Registry.register_factory(module::scripting::null_output_script_factory());
	Registry.register_factory(module::scripting::render_engine_script_factory());
	Registry.register_factory(module::scripting::scalar_source_script_factory());
	Registry.register_factory(module::scripting::string_source_script_factory());
	Registry.register_factory(module::scripting::transform_modifier_script_factory());
	Registry.register_factory(module::scripting::transform_source_script_factory());
	Registry.register_factory(module::scripting::vector3_source_script_factory());
K3D_MODULE_END

