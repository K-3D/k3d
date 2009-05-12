#ifndef K3DSDK_NGUI_UI_COMPONENT_H
#define K3DSDK_NGUI_UI_COMPONENT_H

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

namespace Glib { class ustring; }
namespace Gtk { class Tooltips; }

#include <k3dsdk/icommand_node.h>

namespace k3d
{

namespace ngui
{

//////////////////////////////////////////////////////////////////////////////////////////
// ui_component

/// Base-class for user interface components that makes them command-nodes and provides other, standard behavior
class ui_component :
	public icommand_node
{
public:
	ui_component();
	virtual ~ui_component();

	/// Provides a central tooltips object for all components displaying tooltips
	static Gtk::Tooltips& tooltips();
	/// Convenience function for setting a tooltip
	void set_tip(const Glib::ustring& Tooltip);

	const icommand_node::result execute_command(const string_t&, const string_t&);
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_UI_COMPONENT_H	

