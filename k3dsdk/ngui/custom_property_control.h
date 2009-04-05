#ifndef K3DSDK_NGUI_CUSTOM_PROPERTY_CONTROL_H
#define K3DSDK_NGUI_CUSTOM_PROPERTY_CONTROL_H

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

namespace Gtk { class Widget; }

namespace k3d
{

class icommand_node;
class iproperty;

namespace ngui
{

class document_state;

namespace custom_property
{

/// Abstract interface for a "custom property control", a UI component that can be displayed in-place-of the normal "auto-generated" control for a specific property.
/// Note: implementations must publicly derived from Gtk::Widget.
class control
{
public:
	virtual ~control() {}

	/// Called once at startup to initialize the control (this is necessary because plugins can't take constructor arguments).
	virtual void initialize(document_state& DocumentState, icommand_node& Parent, iproperty& Property) = 0;

protected:
	control() {}
	control(const control&) {}
	control& operator=(const control&) { return *this; }
};

} // namespace custom_property

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_CUSTOM_PROPERTY_CONTROL_H

