#ifndef K3DSDK_OPTIONS_H
#define K3DSDK_OPTIONS_H

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
		\brief Declares k3d::ioptions, an abstract interface for global user options
		\author Tim Shead (tshead@k-3d.com)
*/

#include <string>
#include <vector>

namespace k3d
{

namespace filesystem { class path; }
namespace xml { class element; }

namespace options
{

/// Encapsulates a description of a render engine
struct render_engine
{
	render_engine(const std::string& Type, const std::string& Name, const std::string& Label, const std::string& RenderCommand, const std::string& ShaderBinary, const std::string& ShaderCompileCommand) :
		type(Type),
		name(Name),
		label(Label),
		render_command(RenderCommand),
		shader_binary(ShaderBinary),
		shader_compile_command(ShaderCompileCommand)
	{
	}

	std::string type;
	std::string name;
	std::string label;
	std::string render_command;
	std::string shader_binary;
	std::string shader_compile_command;
};
/// A collection of render engines
typedef std::vector<render_engine> render_engines_t;

/// Returns the current set of render engines configured by the user
const render_engines_t render_engines();
/// Returns the user's preferred render engine of the given type
const std::string default_render_engine(const std::string& Type);
/// Sets the user's preferred render engine of the given type
void set_default_render_engine(const std::string& Type, const std::string& Name);

/// Returns the default file path for the given data type (e.g. bitmap, document, script, etc)
const filesystem::path get_path(const std::string& PathType);
/// Sets the default file path for the given data type (e.g. bitmap, document, script, etc)
void set_path(const std::string& PathType, const filesystem::path& Path);

/// Predefined path types for use with get_path() and set_path()
namespace path
{

inline const std::string render_farm() { return "render_farm"; }
inline const std::string scripts() { return "scripts"; }
inline const std::string documents() { return "documents"; }
inline const std::string bitmaps() { return "bitmaps"; }
inline const std::string tutorials() { return "tutorials"; }
inline const std::string test_cases() { return "test_cases"; }
inline const std::string render_frame() { return "render_frame"; }
inline const std::string render_animation() { return "render_animation"; }
inline const std::string fonts() { return "fonts"; }
inline const std::string displacement_shaders() { return "displacement_shaders"; }
inline const std::string imager_shaders() { return "imager_shaders"; }
inline const std::string light_shaders() { return "light_shaders"; }
inline const std::string surface_shaders() { return "surface_shaders"; }
inline const std::string transformation_shaders() { return "transformation_shaders"; }
inline const std::string volume_shaders() { return "volume_shaders"; }

} // namespace path

/// Returns the external command-line for the given action (e.g. bitmap, document, script, etc)
const std::string get_command(const std::string& CommandType);
/// Sets the default file path for the given data type (e.g. bitmap, document, script, etc)
void set_command(const std::string& CommandType, const std::string& Command);

/// Predefined command types for use with get_command() and set_command()
namespace command
{

inline const std::string bitmap_viewer() { return "bitmap_viewer"; }
inline const std::string text_editor() { return "text_editor"; }

} // namespace command

/// Returns the hierarchical tree of user options (so you can store your own data)
xml::element& tree();
/// Stores any modifications to the tree to persistent storage
void commit();

} // namespace options

} // namespace k3d

#endif // !K3DSDK_OPTIONS_H


