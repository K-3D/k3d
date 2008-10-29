#ifndef K3DSDK_NGUI_PROPERTY_BUTTON_H
#define K3DSDK_NGUI_PROPERTY_BUTTON_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Declares property_button, which provides a standard MVC UI for linking properties to channels
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "event_button.h"
#include "property_widget.h"

namespace Gtk { class Image; }

namespace libk3dngui
{

namespace property_button
{

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for manipulating properties (i.e. the view and the controller from model-view-controller)
class control :
	public event_button,
	public property_widget::control
{
	typedef property_widget::control base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<property_widget::idata_proxy> Data);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called whenever the state of the underlying data has changed
	void data_changed();
	/// Called by the signal system whenever property dependencies change
	void on_dependencies_changed(const k3d::ipipeline::dependencies_t& Dependencies);

	/// Displays an image indicating the state of the property (connected / unconnected)
	Gtk::Image* const m_image;
};

} // namespace property_button

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_PROPERTY_BUTTON_H


