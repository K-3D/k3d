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
//#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace ngui
{

namespace custom_property_page
{

/// Abstract interface for a custom "property page", a UI component that can be displayed in-place-of the normal "auto-generated" property page for a specific plugin.
class control :
	public iunknown
{
public:
/*
	/// Set the document and parent command node for this panel - this is ugly, but application plugins don't take ctor arguments
	virtual void initialize(document_state& DocumentState, k3d::icommand_node& Parent) = 0;
	/// Returns a unique string identifying the panel type - panel frames use this to coordinate tutorial recording / layout serialization
	virtual const std::string panel_type() = 0;
	/// Connects a slot to a signel that should be emitted whenever the panel receives the keyboard focus
	virtual sigc::connection connect_focus_signal(const sigc::slot<void>& Slot) = 0;
*/

protected:
	control() {}
	control(const control&) {}
	control& operator=(const control&) { return *this; }
	virtual ~control() {}
};

} // namespace custom_property_page

} // namespace ngui

} // namespace k3d

#endif // NGUI_CUSTOM_PROPERTY_PAGE_H

