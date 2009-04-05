#ifndef K3DSDK_NGUI_NODE_COLLECTION_CHOOSER_H
#define K3DSDK_NGUI_NODE_COLLECTION_CHOOSER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "ui_component.h"
#include <k3dsdk/inode_collection_property.h>
#include <gtkmm/box.h>

namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

namespace node_collection_chooser
{

/////////////////////////////////////////////////////////////////////////////
// imodel

/// Abstract data model for a node_collection_chooser::control
class imodel
{
public:
	virtual ~imodel() {}

	/// Returns a human-readable label for the underlying data
	virtual const Glib::ustring label() = 0;
	/// Called to return the set of all available nodes
	virtual const k3d::inode_collection_property::nodes_t available_nodes() = 0;
	/// Called to return the underlying data value
	virtual const k3d::inode_collection_property::nodes_t selected_nodes() = 0;
	/// Called to set a new data value
	virtual void set_selected_nodes(const k3d::inode_collection_property::nodes_t& Value) = 0;
	/// Connects a slot to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed_signal(const sigc::slot<void>& Slot) = 0;

protected:
	imodel() {}

private:
	imodel(const imodel&);
	imodel& operator=(const imodel&);
};

/// Factory method for creating an imodel object given a node collection property
imodel* const model(k3d::iproperty& Property);

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for selecting a set of nodes
class control :
	public Gtk::HBox,
	public ui_component
{
	typedef Gtk::HBox base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, imodel* const Model, k3d::istate_recorder* StateRecorder);
	~control();

/*
	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
*/

private:
	/// Called to modify the current set of nodes
	void on_edit();

	class implementation;
	implementation* const m_implementation;
};

} // namespace node_collection_chooser

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_NODE_COLLECTION_CHOOSER_H

