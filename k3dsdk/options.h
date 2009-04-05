#ifndef K3DSDK_OPTIONS_H
#define K3DSDK_OPTIONS_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "types.h"

namespace k3d
{

namespace filesystem { class path; }
namespace xml { class element; }

namespace options
{

/// Returns the default file path for the given data type (e.g. bitmap, document, script, etc)
const filesystem::path get_path(const string_t& PathType);
/// Sets the default file path for the given data type (e.g. bitmap, document, script, etc)
void set_path(const string_t& PathType, const filesystem::path& Path);

/// Predefined path types for use with get_path() and set_path()
namespace path
{

inline const string_t render_farm() { return "render_farm"; }
inline const string_t scripts() { return "scripts"; }
inline const string_t documents() { return "documents"; }
inline const string_t bitmaps() { return "bitmaps"; }
inline const string_t tutorials() { return "tutorials"; }
inline const string_t test_cases() { return "test_cases"; }
inline const string_t render_frame() { return "render_frame"; }
inline const string_t render_animation() { return "render_animation"; }
inline const string_t fonts() { return "fonts"; }
inline const string_t displacement_shaders() { return "displacement_shaders"; }
inline const string_t imager_shaders() { return "imager_shaders"; }
inline const string_t light_shaders() { return "light_shaders"; }
inline const string_t surface_shaders() { return "surface_shaders"; }
inline const string_t transformation_shaders() { return "transformation_shaders"; }
inline const string_t volume_shaders() { return "volume_shaders"; }

} // namespace path

/// Returns the external command-line for the given action (e.g. bitmap, document, script, etc)
const string_t get_command(const string_t& CommandType);
/// Sets the default file path for the given data type (e.g. bitmap, document, script, etc)
void set_command(const string_t& CommandType, const string_t& Command);

/// Predefined command types for use with get_command() and set_command()
namespace command
{

inline const string_t bitmap_viewer() { return "bitmap_viewer"; }

} // namespace command

/// Returns the hierarchical tree of user options (so you can store your own data)
xml::element& tree();
/// Stores any modifications to the tree to persistent storage
void commit();

} // namespace options

} // namespace k3d

#endif // !K3DSDK_OPTIONS_H

