#ifndef NGUI_CUSTOM_PROPERTY_PAGE_H
#define NGUI_CUSTOM_PROPERTY_PAGE_H

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
namespace Gtk { class Widget; }

namespace k3d
{

class icommand_node;
class inode;

namespace ngui
{

namespace custom_property_page
{

/// Abstract interface for a custom "property page", a UI component that can be displayed in-place-of the normal "auto-generated" property page for a specific plugin.
class control
{
public:
	virtual ~control() {}

	virtual Gtk::Widget& get_widget(libk3dngui::document_state& DocumentState, icommand_node& Parent, inode& Node) = 0;

protected:
	control() {}
	control(const control&) {}
	control& operator=(const control&) { return *this; }
};

} // namespace custom_property_page

} // namespace ngui

} // namespace k3d

#endif // NGUI_CUSTOM_PROPERTY_PAGE_H

