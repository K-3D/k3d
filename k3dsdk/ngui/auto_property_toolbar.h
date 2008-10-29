#ifndef K3DSDK_NGUI_AUTO_PROPERTY_TOOLBAR_H
#define K3DSDK_NGUI_AUTO_PROPERTY_TOOLBAR_H

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

#include <k3dsdk/types.h>

namespace Gtk { class Widget; }
namespace libk3dngui { class document_state; }

namespace k3d
{

class icommand_node;
class iunknown;

namespace ngui
{

namespace auto_property_toolbar
{

/// Provides a toolbar with an auto-generated collection of icons based on an object's capabilities / type.
class control
{
public:
	control(libk3dngui::document_state& DocumentState, icommand_node& Parent, const string_t& Name);
	~control();

	/// Called to set the toolbar contents based on the given object
	void set_object(iunknown* Object);

	/// Called to get the toolbar widget for display
	Gtk::Widget& get_widget();

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace auto_property_toolbar

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_AUTO_PROPERTY_TOOLBAR_H

