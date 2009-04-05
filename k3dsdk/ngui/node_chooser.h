#ifndef K3DSDK_NGUI_NODE_CHOOSER_H
#define K3DSDK_NGUI_NODE_CHOOSER_H

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

#include "ui_component.h"

#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/result.h>

#include <gtkmm/box.h>

namespace Gtk { class Button; }
namespace Gtk { class Label; }
namespace Gtk { class Menu; }
namespace Gtk { class MenuItem; }
namespace k3d { class idocument; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

class document_state;

namespace node_chooser
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an node that proxies a data source for an node_chooser::control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual k3d::inode* node() = 0;
	/// Called to set a new data value
	virtual void set_node(k3d::inode* Object) = 0;
	/// Called to return the owning document
	virtual document_state& document() = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, k3d::ihint*> changed_signal_t;
	/// Signal emitted if the underlying data changes
	virtual changed_signal_t& changed_signal() = 0;

	/// Stores an optional state recorder for recording undo/redo data
	k3d::istate_recorder* const state_recorder;
	/// Stores an optional message for labelling undo/redo state changes
	const Glib::ustring change_message;

protected:
	idata_proxy(k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		state_recorder(StateRecorder),
		change_message(ChangeMessage)
	{
	}

private:
	idata_proxy(const idata_proxy& RHS);
	idata_proxy& operator=(const idata_proxy& RHS);
};

/////////////////////////////////////////////////////////////////////////////
// iselection_filter

/// Abstract interface for an node that will be queried to filter the set of available choices
class iselection_filter
{
public:
	virtual ~iselection_filter() {}

	/// Return true iff "None" should be available to the user as a selection
	virtual bool allow_none() = 0;
	/// Return true iff the specified plugin type should be available to the user as a "Create XXX" selection
	virtual bool allow(k3d::iplugin_factory& Factory) = 0;
	/// Return true iff the specified inode should be available to the user for selection
	virtual bool allow(k3d::inode& Object) = 0;

protected:
	iselection_filter() {}
	iselection_filter(const idata_proxy& RHS) {}
	iselection_filter& operator=(const idata_proxy& RHS) { return *this; }
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for choosing nodes (either existing, or newly-created) (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::HBox,
	public ui_component
{
	typedef Gtk::HBox base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data, std::auto_ptr<iselection_filter> Filter);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

private:
	/// Called when new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t&);
	/// Called when nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t&);
	/// Called whenever the underlying data source changes
	void data_changed(k3d::ihint*);
	/// Called to display the set of available choices
	void on_choose();
	/// Called when the user decides to select no node
	void on_select_none();
	/// Called when the user wants to create a new node
	void on_create_node(k3d::iplugin_factory* const Factory);
	/// Called when the user selects an existing node
	void on_select_node(k3d::inode* const Object);
	/// Called when the user wants to edit the current choice
	void on_edit();

	/// Resets the contents of the popup menu so it can be recreated the next time it's needed
	void reset_menu();

	/// Stores a reference to the underlying data node
	std::auto_ptr<idata_proxy> m_data;
	/// Stores a filter node for controlling the set of available choices
	std::auto_ptr<iselection_filter> m_filter;

	Gtk::Label* const m_label;
	Gtk::Button* const m_menu_button;
	Gtk::Button* const m_edit_button;

	std::auto_ptr<Gtk::Menu> m_menu;
	std::map<std::string, Gtk::MenuItem*> m_menu_item_create;
	std::map<std::string, Gtk::MenuItem*> m_menu_item_select;
};

/// Provides an implementation of k3d::node_chooser::idata_proxy that supports any data source that supports the node(), set_node(), node_collection(), and changed_signal() concepts
template<typename data_t>
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_data(Data),
		m_document_state(DocumentState)
	{
	}

	k3d::inode* node()
	{
		return m_data.node();
	}

	void set_node(k3d::inode* Object)
	{
		m_data.set_node(Object);
	}

	document_state& document()
	{
		return m_document_state;
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_data;
	document_state& m_document_state;
};

/// Specialization of k3d::node_chooser::data_proxy for use with k3d::iproperty objects
template<>
class data_proxy<k3d::iproperty> :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	data_proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data)),
		m_document_state(DocumentState)
	{
	}

	k3d::inode* node()
	{
		return boost::any_cast<k3d::inode*>(m_readable_data.property_internal_value());
	}

	void set_node(k3d::inode* Object)
	{
		return_if_fail(m_writable_data);
		m_writable_data->property_set_value(Object);
	}

	document_state& document()
	{
		return m_document_state;
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
	document_state& m_document_state;
};

/// Convenience factory function for creating node_chooser::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = "")
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(DocumentState, Data, StateRecorder, ChangeMessage));
}

/// Provides an implementation of k3d::node_chooser::iselection_filter that filters based on a specific interface type
template<typename interface_t>
class selection_filter :
	public iselection_filter
{
public:
	selection_filter(const bool AllowNone) :
		m_allow_none(AllowNone)
	{
	}

	bool allow_none()
	{
		return m_allow_none;
	}

	bool allow(k3d::iplugin_factory& Factory)
	{
		return Factory.implements(typeid(interface_t));
	}

	bool allow(k3d::inode& Object)
	{
		return dynamic_cast<interface_t*>(&Object) ? true : false;
	}

private:
	bool m_allow_none;
};

/// Convenience factory function for creating k3d::node_chooser::iselection_filter nodes
template<typename interface_t>
std::auto_ptr<iselection_filter> filter(const bool AllowNone)
{
	return std::auto_ptr<iselection_filter>(new selection_filter<interface_t>(AllowNone));
}

/// Convenience factory function for creating k3d::node_chooser::iselection_filter nodes for use with k3d::iproperty nodes
std::auto_ptr<iselection_filter> filter(k3d::iproperty& Data);

} // namespace node_chooser

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_NODE_CHOOSER_H

