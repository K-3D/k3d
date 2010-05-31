#ifndef K3DSDK_QTUI_SCRIPT_H
#define K3DSDK_QTUI_SCRIPT_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/scripting.h>

namespace k3d
{

namespace qtui
{

namespace script
{

/// Executes a script using a specific language and providing user feedback for errors
bool_t execute(const k3d::script::code& Script, const string_t& ScriptName, k3d::iscript_engine::context& Context, const k3d::script::language& Language);
/// Executes a script, attempting to automatically recognize the language and providing user feedback for errors
bool_t execute(const k3d::script::code& Script, const string_t& ScriptName, k3d::iscript_engine::context& Context);
/// Executes a script, attempting to automatically recognize the language and providing user feedback for errors
bool_t execute(const k3d::filesystem::path& Script, k3d::iscript_engine::context& Context);

} // namespace script

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_SCRIPT_H

