#ifndef K3DSDK_NGUI_NAVIGATION_INPUT_MODEL_H
#define K3DSDK_NGUI_NAVIGATION_INPUT_MODEL_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/icommand_node.h>
#include <k3dsdk/signal_system.h>

namespace libk3dngui
{

class document_state;
	
/////////////////////////////////////////////////////////////////////////////
// navigation_input_model

/// Input model implementation that handles standard viewport navigation and context menus
class navigation_input_model
{
public:
	navigation_input_model(document_state& DocumentState);
	~navigation_input_model();

	void on_button1_click(viewport::control& Viewport, const GdkEventButton& Event);
	void on_button1_start_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button1_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button1_end_drag(viewport::control& Viewport, const GdkEventButton& Event);

	void on_button2_click(viewport::control& Viewport, const GdkEventButton& Event);
	void on_button2_start_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button2_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button2_end_drag(viewport::control& Viewport, const GdkEventButton& Event);

	void on_scroll(viewport::control& Viewport, const GdkEventScroll& Event);

	/// Connects to a signal that will emit command-node commands
	sigc::connection connect_command_signal(const sigc::slot<void, const std::string&, const std::string&>& Slot);
	
	/// Called to execute a command-node command, returns true iff the command executed successfully
	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	struct implementation;
	implementation* const m_implementation;
};

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_NAVIGATION_INPUT_MODEL_H
