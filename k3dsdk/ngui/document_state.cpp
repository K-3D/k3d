// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/**		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <gdkmm/screen.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menu.h>
#include <gtkmm/window.h>

#include <gtk/gtkgl.h>
#include <gtk/gtkmain.h>

#include <k3d-i18n-config.h>
#include <k3d-platform-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/batch_mode.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_sink.h>
#include <k3dsdk/imesh_selection_sink.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/ireset_properties.h>
#include <k3dsdk/iscripted_action.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itime_sink.h>
#include <k3dsdk/imatrix_source.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/context_menu.h>
#include <k3dsdk/ngui/pipeline.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/move_tool.h>
#include <k3dsdk/ngui/panel_mediator.h>
#include <k3dsdk/ngui/rotate_tool.h>
#include <k3dsdk/ngui/safe_close_dialog.h>
#include <k3dsdk/ngui/scale_tool.h>
#include <k3dsdk/ngui/selection_tool.h>
#include <k3dsdk/ngui/unsaved_document.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/viewport.h>
#include <k3dsdk/node.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/user_properties.h>

#include <iterator>
#include <functional>
#include <set>

namespace k3d
{

namespace ngui
{

namespace detail
{

/// Provides human-readable labels for the selection::mode enumeration
const k3d::ienumeration_property::enumeration_values_t& selection_mode_values()
{
	static k3d::ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Nodes"), "nodes", _("Select Nodes")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Points"), "points", _("Select Points")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Lines"), "lines", _("Select Lines")));
		values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Faces"), "faces", _("Select Faces")));
	}

	return values;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// document_state::implementation

class document_state::implementation :
	public sigc::trackable/*,
	public k3d::ipersistent*/
{
public:
	implementation(k3d::idocument& Document) :
		m_document(Document),
		m_gdkgl_share_list(0),
		m_last_selection_mode(selection::NODE),
		m_active_tool(0),
		m_selection_tool(0),
		m_move_tool(0),
		m_rotate_tool(0),
		m_scale_tool(0),
		m_context_menu(0),
		m_node_selection(0)
	{
		// Process remove_nodes_signal
		m_document.nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
	}

	~implementation()
	{
		while(!m_tools.empty())
		{
			delete m_tools.begin()->second;
			m_tools.erase(m_tools.begin());
		}

		delete m_context_menu;
		delete m_scale_tool;
		delete m_rotate_tool;
		delete m_move_tool;
		delete m_selection_tool;
		
		m_node_selection_metadata_connection.disconnect();
		m_node_selection_deleted_connection.disconnect();
	}

	/// Returns the document
	k3d::idocument& document()
	{
		return m_document;
	}

#if defined K3D_API_DARWIN

	/// Returns a "global" (to the document) gdkgl context that can be used to share display lists
	GdkGLContext* gdkgl_share_list()
	{
		return 0;
	}

#elif defined K3D_API_WIN32

	/// Returns a "global" (to the document) gdkgl context that can be used to share display lists
	GdkGLContext* gdkgl_share_list()
	{
		if(!m_gdkgl_share_list)
		{
			GdkGLConfig* const config = gdk_gl_config_new_by_mode(
				static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
			return_val_if_fail(config, 0);

			Gtk::Window* const window = new Gtk::Window();
			gtk_widget_set_gl_capability(GTK_WIDGET(window->gobj()), config, 0, true, GDK_GL_RGBA_TYPE);
			window->show();
			window->hide();

			GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(window->gobj()));
			return_val_if_fail(context, 0);

			m_gdkgl_share_list = context;
		}

		return m_gdkgl_share_list;
	}

#else // K3D_API_WIN32

	/// Returns a "global" (to the document) gdkgl context that can be used to share display lists
	GdkGLContext* gdkgl_share_list()
	{
		if(!m_gdkgl_share_list)
		{
			GdkGLConfig* const config = gdk_gl_config_new_by_mode(
				static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
			return_val_if_fail(config, 0);

			GdkPixmap* const pixmap = gdk_pixmap_new(0, 8, 8, gdk_gl_config_get_depth(config));
			return_val_if_fail(pixmap, 0);

			GdkGLPixmap* const glpixmap = gdk_pixmap_set_gl_capability(pixmap, config, 0);
			return_val_if_fail(glpixmap, 0);

			GdkGLContext* const context = gdk_gl_context_new(
				gdk_pixmap_get_gl_drawable(pixmap), 0, true, GDK_GL_RGBA_TYPE);
			return_val_if_fail(context, 0);

			m_gdkgl_share_list = context;
		}

		return m_gdkgl_share_list;
	}

#endif

	/// Returns a signal that can be emitted to acknowledge of a document selection change
	document_selection_change_signal_t& document_selection_change_signal()
	{
		return m_document_selection_change_signal;
	}

	/// Returns the active tool for the document
	tool& active_tool()
	{
		return *m_active_tool;
	}

	/// Sets the active tool for the document
	void set_active_tool(tool& ActiveTool)
	{
		m_active_tool->deactivate();
		m_active_tool = &ActiveTool;
		m_active_tool->activate();
		m_active_tool_changed_signal.emit();
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
assert_not_implemented();
/*
		selection_changed();
*/
	}

/*
	typedef std::set<k3d::legacy::mesh*> meshes_t;
	typedef std::set<k3d::inode*> node_selection_t;

	void on_node_selection_node_changed()
	{
		m_node_selection_metadata_connection.disconnect();
		m_node_selection_deleted_connection.disconnect();
		m_node_selection = 0;
	}
	
	k3d::inode_selection* node_selection()
	{
		if(!m_node_selection)
		{
			k3d::inode_collection::nodes_t nodes = k3d::find_nodes<k3d::inode_selection>(m_document.nodes(), "ngui:unique_node", "node_selection");
			if(nodes.size() != 1)
				return 0;
			m_node_selection = dynamic_cast<k3d::inode_selection*>(nodes.back());
			// Make sure the node gets updated whenever the metadata is changed or the node is deleted
			k3d::imetadata* metadata = dynamic_cast<k3d::imetadata*>(m_node_selection);
			m_node_selection_metadata_connection = metadata->connect_metadata_changed_signal(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
			m_node_selection_deleted_connection = nodes.back()->deleted_signal().connect(sigc::mem_fun(*this, &implementation::on_node_selection_node_changed));
		}
		return m_node_selection;
	}
*/

	document_state::set_cursor_signal_t& set_cursor_signal()
	{
		return m_set_cursor_signal;
	}

	document_state::clear_cursor_signal_t& clear_cursor_signal()
	{
		return m_clear_cursor_signal;
	}

	void popup_context_menu(const bool UserAction)
	{
		if(UserAction)
			m_context_menu->popup(1, gtk_get_current_event_time());
		else
			m_context_menu->popup(0, GDK_CURRENT_TIME);
	}

	/// Stores a reference to the owning document
	k3d::idocument& m_document;
	sigc::signal<bool, k3d::signal::cancelable> m_safe_to_close_signal;
	/// Stores an unused off-screen pixmap so we can create a global gdkgl context for sharing display lists
	Glib::RefPtr<Gdk::Pixmap> m_gdkgl_share_pixmap;
	/// Stores a gdkgl context that can be used to share display lists for the entire document
	GdkGLContext* m_gdkgl_share_list;
	/// A signal that can be emitted to acknowledge of a document selection change
	document_selection_change_signal_t m_document_selection_change_signal;
	/// A signal that will be emitted whenever the active tool changes
	sigc::signal<void> m_active_tool_changed_signal;
	/// Stores the last document-wide selection mode
	selection::mode m_last_selection_mode;

	document_state::set_cursor_signal_t m_set_cursor_signal;
	document_state::clear_cursor_signal_t m_clear_cursor_signal;

	/// Store a reference to the current active tool
	tool* m_active_tool;

	/// Store builtin tools (deprecated)
	k3d::ngui::selection_tool* m_selection_tool;
	tool* m_move_tool;
	tool* m_rotate_tool;
	tool* m_scale_tool;

	/// Store plugin tools
	std::map<k3d::string_t, tool*> m_tools;

	/// Document wide context menu
	Gtk::Menu* m_context_menu;

	/// Used to keep documents sorted in the "safe to close" dialog
	struct sort_by_title
	{
		bool operator()(const safe_close_dialog::entry& LHS, const safe_close_dialog::entry& RHS)
		{
			return LHS.document->unsaved_document_title() < RHS.document->unsaved_document_title();
		}
	};

	typedef sigc::signal<unsaved_document*> safe_close_signal_t;
	/// Signal that will be emitted prior to safe shutdown to "gather" unsaved documents
	safe_close_signal_t m_safe_close_signal;

	/// Stores auto-start plugins
	typedef std::vector<k3d::iunknown*> auto_start_plugins_t;
	/// Stores auto-start plugins
	auto_start_plugins_t m_auto_start_plugins;
	
	/// Cached pointer to the document node selection node
	k3d::inode_selection* m_node_selection;
	sigc::connection m_node_selection_metadata_connection;
	sigc::connection m_node_selection_deleted_connection;
};

/////////////////////////////////////////////////////////////////////////////
// document_state

document_state::document_state(k3d::idocument& Document) :
	m_implementation(new implementation(Document)),
	m_focus_viewport(0)
{
	// Create auto-start plugins for this document ...
	const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup();
	for(k3d::plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		k3d::iplugin_factory::metadata_t metadata = (**factory).metadata();

		if(!metadata.count("ngui:document-start"))
			continue;

		k3d::log() << info << "Creating plugin [" << (**factory).name() << "] via ngui:document-start" << std::endl;

		k3d::iunknown* const plugin = k3d::plugin::create(**factory);
		if(!plugin)
		{
			k3d::log() << error << "Error creating plugin [" << (**factory).name() << "] via ngui:document-start" << std::endl;
			continue;
		}
		m_implementation->m_auto_start_plugins.push_back(plugin);

		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(plugin))
		{
			k3d::iscript_engine::context context;
			context["command"] = k3d::string_t("startup");
			context["document"] = &Document;
			scripted_action->execute(context);
		}
	}

	Document.close_signal().connect(sigc::mem_fun(*this, &document_state::on_document_close));

	m_implementation->m_selection_tool = new k3d::ngui::selection_tool(*this, "selection_tool");
	m_implementation->m_move_tool = new k3d::ngui::move_tool(*this, "move_tool");
	m_implementation->m_rotate_tool = new k3d::ngui::rotate_tool(*this, "rotate_tool");
	m_implementation->m_scale_tool = new k3d::ngui::scale_tool(*this, "scale_tool");

	m_implementation->m_active_tool = m_implementation->m_selection_tool;
	m_implementation->m_active_tool->activate();

	m_implementation->m_context_menu =
		create_context_menu(*this);
}

document_state::~document_state()
{
	// Shut-down any auto-start plugins associated with this document ...
	for(implementation::auto_start_plugins_t::iterator plugin = m_implementation->m_auto_start_plugins.begin(); plugin != m_implementation->m_auto_start_plugins.end(); ++plugin)
	{
		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(*plugin))
		{
			k3d::iscript_engine::context context;
			context["command"] = k3d::string_t("shutdown");
			context["document"] = &document();
			scripted_action->execute(context);
		}
	}

	delete m_implementation;
}

void document_state::on_document_close()
{
	delete this;
}

k3d::idocument& document_state::document()
{
	return m_implementation->document();
}

sigc::connection document_state::connect_safe_close_signal(const sigc::slot<unsaved_document*>& Slot)
{
	return m_implementation->m_safe_close_signal.connect(Slot);
}

bool document_state::safe_close(Gtk::Window& Parent)
{
	if(k3d::batch_mode())
	{
		k3d::application().close_document(document());
		return true;
	}

	safe_close_dialog::entries_t entries;

	implementation::safe_close_signal_t::slot_list_type slots = m_implementation->m_safe_close_signal.slots();
	for(implementation::safe_close_signal_t::slot_list_type::iterator slot = slots.begin(); slot != slots.end(); ++slot)
	{
		unsaved_document* doc = (*slot)();
		if(doc && doc->unsaved_changes())
			entries.push_back(safe_close_dialog::entry(doc));
	}

	if(!entries.empty())
	{
		switch(safe_close_dialog::run(Parent, entries))
		{
			case Gtk::RESPONSE_NONE:
			case Gtk::RESPONSE_CANCEL:
			case Gtk::RESPONSE_DELETE_EVENT:
				return false;
			case Gtk::RESPONSE_CLOSE:
				k3d::application().close_document(document());
				return true;
			case Gtk::RESPONSE_OK:
				for(safe_close_dialog::entries_t::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
				{
					if(entry->save)
					{
						if(!entry->document->save_unsaved_changes())
							return false;
					}
				}
				k3d::application().close_document(document());
				return true;
		}
	}

	k3d::application().close_document(document());
	return true;
}

GdkGLContext* document_state::gdkgl_share_list()
{
	return m_implementation->gdkgl_share_list();
}

document_state::document_selection_change_signal_t& document_state::document_selection_change_signal()
{
	return m_implementation->document_selection_change_signal();
}

tool& document_state::active_tool()
{
	return m_implementation->active_tool();
}

void document_state::set_active_tool(tool& ActiveTool)
{
	m_implementation->set_active_tool(ActiveTool);
}

sigc::connection document_state::connect_active_tool_changed_signal(const sigc::slot<void>& Slot)
{
	return m_implementation->m_active_tool_changed_signal.connect(Slot);
}

tool* document_state::get_tool(const k3d::string_t& Name)
{
	if(!m_implementation->m_tools.count(Name))
	{
		tool* const new_tool = k3d::plugin::create<tool>(Name);
		return_val_if_fail(new_tool, new_tool);

		new_tool->initialize(*this);

		m_implementation->m_tools.insert(std::make_pair(Name, new_tool));

		return new_tool;
	}

	return m_implementation->m_tools[Name];
}

tool& document_state::selection_tool()
{
	return *m_implementation->m_selection_tool;
}

tool& document_state::move_tool()
{
	return *m_implementation->m_move_tool;
}

tool& document_state::rotate_tool()
{
	return *m_implementation->m_rotate_tool;
}

tool& document_state::scale_tool()
{
	return *m_implementation->m_scale_tool;
}

bool document_state::pick_backfacing()
{
	return m_implementation->m_selection_tool->pick_backfacing();
}

bool document_state::paint_backfacing()
{
	return m_implementation->m_selection_tool->paint_backfacing();
}

bool document_state::rubber_band_backfacing()
{
	return m_implementation->m_selection_tool->rubber_band_backfacing();
}

document_state::set_cursor_signal_t& document_state::set_cursor_signal()
{
	return m_implementation->set_cursor_signal();
}

document_state::clear_cursor_signal_t& document_state::clear_cursor_signal()
{
	return m_implementation->clear_cursor_signal();
}

void document_state::popup_context_menu(const bool UserAction)
{
	m_implementation->popup_context_menu(UserAction);
}

} // namespace ngui

} // namespace k3d

