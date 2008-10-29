#ifndef K3DSDK_NGUI_TEXT_EDITOR_H
#define K3DSDK_NGUI_TEXT_EDITOR_H

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

#include <k3dsdk/iunknown.h>

namespace libk3dngui { class document_state; }

namespace k3d
{

namespace ngui
{

/// Abstract interface for a text-editor text_editor.
/// Note: implementations must publicly derive from Gtk::Widget.
class text_editor
{
public:
	virtual ~text_editor() {}

	/// Called once at startup to initialize the editor for use as a general-purpose editor with filesystem files
	virtual void initialize(libk3dngui::document_state& DocumentState) = 0;

protected:
	text_editor() {}
	text_editor(const text_editor&) {}
	text_editor& operator=(const text_editor&) { return *this; }
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_TEXT_EDITOR_H

