#ifndef K3DSDK_NGUI_PROPERTY_WIDGET_H
#define K3DSDK_NGUI_PROPERTY_WIDGET_H

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
		\brief Declares property_widget, which provides a standard MVC UI for linking properties to channels
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "ui_component.h"

#include <k3dsdk/ipipeline.h>

namespace Gtk { class Menu; }
namespace Gtk { class MenuItem; }
namespace k3d { class inode; }
namespace k3d { class iproperty; }
namespace k3d { class istate_recorder; }

namespace k3d
{

namespace ngui
{

class document_state;

namespace property_widget
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a property_widget (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the document containing the underlying data property
	virtual document_state& document() = 0;
	/// Called to return the underlying data property
	virtual k3d::iproperty& property() = 0;

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
// control

/// Provides a UI for manipulating properties (i.e. the view and the controller from model-view-controller)
class control :
	public ui_component
{
public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);
	virtual ~control();

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

	bool button_press_event(GdkEventButton* Event);
	bool button_release_event(GdkEventButton* Event);

protected:
	void show_menu(const bool UserAction);
	
	void on_show_connected(k3d::inode* Object);
	void show_connected(k3d::inode* Object);

	void on_connect_to(k3d::iproperty* Property);
	void connect_to(k3d::iproperty* Property);

	void on_disconnect(k3d::inode* Object);
	void disconnect(k3d::inode* Object);

	/// Stores a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;
	/// Context menu displayed when the user clicks on the widget
	std::auto_ptr<Gtk::Menu> m_menu;
	/// Caches the "show connected" widget for tutorial playback
	Gtk::MenuItem* m_show_connected;
	/// Maps properties to widgets for tutorial playback
	std::map<k3d::iproperty*, Gtk::MenuItem*> m_menu_items;
	/// Caches the "disconnect" widget for tutorial playback
	Gtk::MenuItem* m_disconnect;
};

/// Provides an implementation of property_widget::idata_proxy that supports any data source that supports the property() concept
template<typename data_t>
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_document_state(DocumentState),
		m_data(Data)
	{
	}

	document_state& document()
	{
		return m_document_state;
	}

	k3d::iproperty& property()
	{
		return m_data.property();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	document_state& m_document_state;
	data_t& m_data;
};

/// Specialization of property_widget::data_proxy for use directly with iproperty objects
template<>
class data_proxy<k3d::iproperty> :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	data_proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_document_state(DocumentState),
		m_data(Data)
	{
	}

	document_state& document()
	{
		return m_document_state;
	}

	k3d::iproperty& property()
	{
		return m_data;
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	document_state& m_document_state;
	data_t& m_data;
};

/// Convenience factory function for creating property_widget::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(document_state& DocumentState, data_t& Data, k3d::istate_recorder* const StateRecorder = 0, const Glib::ustring& ChangeMessage = "")
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(DocumentState, Data, StateRecorder, ChangeMessage));
}

} // namespace property_widget

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_PROPERTY_WIDGET_H

