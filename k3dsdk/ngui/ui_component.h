#ifndef NGUI_UI_COMPONENT_H
#define NGUI_UI_COMPONENT_H

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

#include <k3dsdk/command_node.h>

namespace Glib { class ustring; }
namespace Gtk { class Tooltips; }

namespace libk3dngui
{

//////////////////////////////////////////////////////////////////////////////////////////
// ui_component

/// Base-class for user interface components that makes them command-nodes and provides other, standard behavior
class ui_component :
	public k3d::command_node
{
	typedef k3d::command_node base;

public:
	ui_component();
	virtual ~ui_component();

	/// Records an interactive tutorial event (ignored if recording is disabled)
	void record_command(const std::string& Command, const std::string& Arguments = "");

	/// Controls whether this component should record events for interactive tutorials (enabled by default)
	void enable_recording(bool Enable = true);
	/// Returns true iff this component should record events for interactive tutorials (enabled by default)
	bool recording_enabled();

	/// Provides a central tooltips object for all components displaying tooltips
	static Gtk::Tooltips& tooltips();
	/// Convenience function for setting a tooltip
	void set_tip(const Glib::ustring& Tooltip);

	/// Provides a default implementation of k3d::command_node::implementation::execute_command
	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Set to true iff this component should record events for macros / interactive tutorials
	bool m_enable_recording;
};

} // namespace libk3dngui

#endif // !NGUI_UI_COMPONENT_H	

