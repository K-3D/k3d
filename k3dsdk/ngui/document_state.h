#ifndef NGUI_DOCUMENT_STATE_H
#define NGUI_DOCUMENT_STATE_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "selection.h"

#include <k3dsdk/data.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/selection.h>

#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

#include <gdk/gdkglcontext.h>

namespace Gdk { class Pixbuf; }
namespace Gtk { class Window; }
namespace k3d { class ianimation_render_engine; }
namespace k3d { class idocument; }
namespace k3d { class inode; }
namespace k3d { class ipreview_render_engine; }
namespace k3d { class istill_render_engine; }

namespace libk3dngui
{

class tool;
class unsaved_document;
namespace viewport { class control; }

/// This ugly little fellow stores document-wide user interface state in one place to minimize coupling between the various per-document UI components
class document_state :
	public sigc::trackable
{
public:
	document_state(k3d::idocument& Document);
	~document_state();

	/// Stores a reference to the owning document
	k3d::idocument& document();

	/// Connects a slot to a signal that will be called prior to safe shutdown.
	sigc::connection connect_safe_close_signal(const sigc::slot<unsaved_document*>& Slot);
	/// Attempts to safely close the application, saving unsaved documents and giving the user a chance to cancel.  Returns true iff the application will close.
	bool safe_close(Gtk::Window& Parent);

	/// Returns a gdkgl context node that can be used to share display lists between viewports
	GdkGLContext* gdkgl_share_list();

	/// Defines a signal that can be emitted to request display of the history for an node
	typedef sigc::signal1<bool, k3d::inode*, k3d::signal::consumable> view_node_history_signal_t;
	/// Returns a signal that can be emitted to request display of the history for an node
	view_node_history_signal_t& view_node_history_signal();
	/// Defines a signal that can be emitted to request display of the properties for an node
	typedef sigc::signal1<bool, k3d::inode*, k3d::signal::consumable> view_node_properties_signal_t;
	/// Returns a signal that can be emitted to request display of the properties for an node
	view_node_properties_signal_t& view_node_properties_signal();
	/// Defines a signal that can be emitted to request display of the properties for a tool
	typedef sigc::signal1<bool, k3d::iproperty_collection*, k3d::signal::consumable> view_tool_properties_signal_t;
	/// Returns a signal that can be emitted to request display of the properties for a tool
	view_tool_properties_signal_t& view_tool_properties_signal();
	/// Defines a signal that can be emitted to acknowledge of a document selection change
	typedef sigc::signal<void> document_selection_change_signal_t;
	/// Returns a signal that can be emitted to acknowledge of a document selection change
	document_selection_change_signal_t& document_selection_change_signal();

	/// Returns the active tool for the document
	tool& active_tool();
	/// Sets the active tool for the document
	void set_active_tool(tool& ActiveTool);
	/// Defines a signal that will be emitted whenever the active tool changes
	typedef sigc::signal<void, k3d::iunknown*> active_tool_changed_signal_t;
	/// Returns a signal that will be emitted whenever the active tool changes
	active_tool_changed_signal_t& active_tool_changed_signal();

	/// Returns an instance of a tool plugin by name (could return NULL).
	tool* get_tool(const k3d::string_t& Name);

	/// Returns a reference to the builtin Selection Tool that can be passed to set_active_tool()
	tool& selection_tool();
	/// Returns a reference to the builtin Move Tool that can be passed to set_active_tool()
	tool& move_tool();
	/// Returns a reference to the builtin Rotate Tool that can be passed to set_active_tool()
	tool& rotate_tool();
	/// Returns a reference to the builtin Scale Tool that can be passed to set_active_tool()
	tool& scale_tool();
	/// Returns a reference to the builtin Snap Tool that can be passed to set_active_tool()
	tool& snap_tool();

	/// Defines storage for the current document-wide selection mode
	typedef k3d_data(selection_mode_t, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, no_property, no_serialization) selection_mode_property_t;
	/// Returns the current document-wide selection mode
	selection_mode_property_t& selection_mode();

	void set_selection_mode(selection_mode_t Mode);

	const bool is_selected(k3d::inode* Node);
	const bool is_selected(const k3d::selection::record&);
	
	/// True if backfacing components are to be picked
	bool pick_backfacing();
	
	/// True if backfacing components are to be paint-selected
	bool paint_backfacing();
	
	/// True if backfacing components are to be rubber-band selected
	bool rubber_band_backfacing();

	/// Selects a collection of components
	void select(const k3d::selection::record& Selection);
	/// Selects a collection of components
	void select(const k3d::selection::records& Selection);
	/// Selects all nodes or components (depending on current selection mode)
	void select_all();
	/// Deselects a collection of components
	void deselect(const k3d::selection::record& Selection);
	/// Deselects a collection of components
	void deselect(const k3d::selection::records& Selection);
	/// Deselects all selected nodes and components
	void deselect_all();
	/// Inverts current selection
	void invert_selection();

	/// Returns the current set of selected nodes
	const k3d::nodes_t selected_nodes();

	/// Hides selected nodes
	void hide_selection();
	/// Shows selected nodes
	void show_selection();
	/// Hides unselected nodes
	void hide_unselected();
	/// Shows all nodes
	void show_all_nodes();

	/// Defines a signal to set the document-wide cursor
	typedef sigc::signal<void, const Glib::RefPtr<Gdk::Pixbuf> > set_cursor_signal_t;
	/// Returns a signal for setting the document-wide cursor
	set_cursor_signal_t& set_cursor_signal();
	/// Defines a signal to reset the document-wide cursor
	typedef sigc::signal<void> clear_cursor_signal_t;
	/// Returns a signal for setting the document-wide cursor
	clear_cursor_signal_t& clear_cursor_signal();

	/// Defines a signal to push a new message onto the status bar
	typedef sigc::signal<void, const Glib::ustring> push_status_message_signal_t;
	/// Returns a signal to push a new message onto the status bar
	push_status_message_signal_t& push_status_message_signal();
	/// Defines a signal to pop the current message from the status bar
	typedef sigc::signal<void> pop_status_message_signal_t;
	/// Returns a signal to pop the current message from the status bar
	pop_status_message_signal_t& pop_status_message_signal();

	/// Returns focused viewport
	viewport::control* get_focus_viewport() { return m_focus_viewport; }
	/// Sets focused viewport
	void set_focus_viewport(viewport::control* Viewport) { m_focus_viewport = Viewport; }

	/// Creates the requested node, attach a MeshInstance iff it's a mesh source
	k3d::inode* create_node(k3d::iplugin_factory* const Factory);

	/// Shows context menu, pass false in interactive mode
	void popup_context_menu(const bool UserAction = true);

private:
	class implementation;
	implementation* const m_implementation;

	void on_document_close();

	/// Stores focused viewport
	viewport::control* m_focus_viewport;
};

} // namespace libk3dngui

#endif // NGUI_DOCUMENT_STATE_H

