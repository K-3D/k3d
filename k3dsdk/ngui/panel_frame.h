#ifndef NGUI_PANEL_FRAME_H
#define NGUI_PANEL_FRAME_H

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

#include <k3dsdk/data.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/signal_system.h>

#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>

namespace k3d { class icamera; }
namespace k3d { class iplugin_factory; }
namespace k3d { namespace xml { class element; } }

namespace libk3dngui
{

class document_state;
namespace image_toggle_button { class control; }
namespace panel { class control; }

namespace panel_frame
{

/// Provides a "panel frame" control that can "mount" a panel control
class control :
        public Gtk::Frame,
	public ui_component,
	public k3d::property_collection
{
	typedef Gtk::Frame base;

public:
	typedef sigc::signal<void, control*> panel_focus_signal_t;

	control(document_state& Document, k3d::icommand_node& Parent, panel_focus_signal_t& PanelFocusSignal);
	~control();

	/// Assigns the frame the panel focus
	void grab_panel_focus();

	/// Mounts a panel to the frame (note: mainly for hacking purposes, prefer mount_panel(const std::string& Type) instead)
	void mount_panel(panel::control& Panel, const std::string& Type);
	/// Mounts a panel based on type
	void mount_panel(const std::string& Type);
	/// Unmounts a mounted panel
	void unmount();
	/// Unmounts a mounted panel, placing it in a floating window
	void float_panel();

	/// Highlights panel
	void set_bg_color(const Gdk::Color& color);
	/// Restores normal background
	void unset_bg_color();

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);

	/// Returns the mounted panel, if any (could return NULL)
	panel::control* const mounted_panel();

	/// Serializes panel state
	void save(k3d::xml::element& Document);
	/// Deserializes panel state
	void load(k3d::xml::element& Element);

	/// Stores the pinned state of the frame
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) pinned;
	/// Stores the automagic state of the frame
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) automagic;
	/// Stores the state of the frame decorations (standard buttons, chooser, etc)
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) decorations;

private:
	void add(Widget&);
	void remove();

	void on_mount_panel(const std::string& Type);

	void on_panel_focus_changed(control* Container);
	void on_grab_focus();
	void on_panel_type_changed();
	void on_decorations_changed(k3d::iunknown*);

	const unsigned long index(const std::string& Type);

	/// Called to update the contents of the combo-box for choosing panels
	void set_choices();
	/// Called to add a choice to the combo-box for choosing panels
	void add_choice(const std::string& PanelType, const Glib::RefPtr<Gdk::Pixbuf> Icon, const Glib::ustring& Label, sigc::slot<void> Slot);

	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(type);
			add(label);
			add(slot);
			add(icon);
		}

		Gtk::TreeModelColumn<Glib::ustring> type;
		Gtk::TreeModelColumn<Glib::ustring> label;
		Gtk::TreeModelColumn<sigc::slot<void> > slot;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
	};
	columns m_columns;
	Glib::RefPtr<Gtk::ListStore> m_model;

	/// Stores a mapping from panel type to external plugin factory
	std::map<std::string, k3d::iplugin_factory*> m_type_plugin_map;

	Gtk::HBox m_decorations;
	/// Provides a combo-box for choosing from available panel types
	Gtk::ComboBox m_panel_type;
	/// Displays the title of mounted panels
	Gtk::Label m_title;

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores a frame that will contain the mounted panel
	Gtk::Frame m_frame;
	/// Stores a connection to the mounted panel's focus signal
	sigc::connection m_grab_focus_connection;
	/// Stores a connection to the frame's panel type combo box
	sigc::connection m_panel_type_connection;

	/// Stores a reference to a signal for notifying observers when we get the panel focus
	panel_focus_signal_t& m_panel_focus_signal;
	/// Stores the connection between m_panel_focus_signal and on_panel_focus_changed()
	sigc::connection m_panel_focus_changed_connection;
	/// Set to true iff this panel has the focus
	bool m_panel_focus;
	/// Store the parent node (the document window) for setting panel parent
	k3d::icommand_node& m_parent;
};

/// Defines a collection of panel frames
typedef std::vector<control*> controls;

} // namespace panel_frame

} // namespace libk3dngui

#endif // NGUI_PANEL_FRAME_H

