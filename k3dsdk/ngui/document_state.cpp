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
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/context_menu.h>
#include <k3dsdk/ngui/detail.h>
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

	k3d::inode* default_gl_painter()
	{
		const std::vector<k3d::inode*> nodes = k3d::node::lookup(m_document, "GL Default Painter");
	        return (1 == nodes.size()) ? nodes[0] : 0;
	}

	k3d::inode* default_ri_painter()
	{
		const std::vector<k3d::inode*> nodes = k3d::node::lookup(m_document, "RenderMan Default Painter");
	        return (1 == nodes.size()) ? nodes[0] : 0;
	}
	
	k3d::inode* instantiate_mesh(k3d::inode* Node)
	{
		k3d::inode* mesh_instance = 0;
		k3d::iproperty* mesh_source_property = 0;
		if(k3d::classes::MeshInstance() == Node->factory().factory_id()) // Instantiate the input in case of a mesh instance
		{
			k3d::imesh_sink* sink = dynamic_cast<k3d::imesh_sink*>(Node);
			mesh_source_property = Node->document().pipeline().dependency(sink->mesh_sink_input());
			if(mesh_source_property)
				mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), m_document, k3d::unique_name(m_document.nodes(), Node->name() + " Instance"));
		}
		else
		{
			k3d::ipipeline& pipeline = Node->document().pipeline();
			const k3d::ipipeline::dependencies_t& dependencies = pipeline.dependencies();
			k3d::imesh_source* mesh_source = dynamic_cast<k3d::imesh_source*>(Node);
			if(!mesh_source)
				return 0;
			mesh_source_property = &(mesh_source->mesh_source_output());
			if(mesh_source_property)
				mesh_instance = k3d::plugin::create<k3d::inode>(k3d::classes::MeshInstance(), m_document, k3d::unique_name(m_document.nodes(), Node->name() + " Instance"));
				
			k3d::iproperty* instance_mesh_source_property = &dynamic_cast<k3d::imesh_source*>(mesh_instance)->mesh_source_output();
			k3d::iproperty* instance_transform_source_property = &dynamic_cast<k3d::itransform_source*>(mesh_instance)->transform_source_output();
			
			k3d::itransform_source* transform_source = dynamic_cast<k3d::itransform_source*>(Node);
			k3d::iproperty* transform_source_property = 0;
			if(transform_source)
				transform_source_property = &(transform_source->transform_source_output());
			
			// Connect the MeshInstance outputs to the inputs of the downstream node, if any
			k3d::ipipeline::dependencies_t new_dependencies;
			for(k3d::ipipeline::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
			{
				if(dependency->second == mesh_source_property)
				{
					dependency->first->property_set_dependency(0);
					new_dependencies.insert(std::make_pair(dependency->first, instance_mesh_source_property));
				}
				if(transform_source_property && dependency->second == transform_source_property)
				{
					dependency->first->property_set_dependency(0);
					new_dependencies.insert(std::make_pair(dependency->first, instance_transform_source_property));
				}
			}
			pipeline.set_dependencies(new_dependencies);
			
		}
		 
		if(!mesh_source_property)
			return 0;
		
		// Assign a default painter ...
		k3d::property::set_internal_value(*mesh_instance, "gl_painter", default_gl_painter());
		k3d::property::set_internal_value(*mesh_instance, "ri_painter", default_ri_painter());

		// Connect the mesh instance to the source ...
		k3d::ipipeline::dependencies_t dependencies;
		k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);
		if(mesh_sink)
			dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), mesh_source_property));
		Node->document().pipeline().set_dependencies(dependencies);
		
		freeze_transformation(*Node, *mesh_instance, m_document);
		
		return mesh_instance;
	}

	k3d::inode* create_node(k3d::iplugin_factory* Factory)
	{
		return_val_if_fail(Factory, 0);

		// Switch to node selection mode
		if(selection::NODE != selection::state(document()).current_mode())
			selection::state(document()).set_current_mode(selection::NODE);

		// Create the requested node ...
		k3d::record_state_change_set changeset(m_document, k3d::string_cast(boost::format(_("Create %1%")) % Factory->name()), K3D_CHANGE_SET_CONTEXT);
		const std::string node_name = k3d::unique_name(m_document.nodes(), Factory->name());
		k3d::inode* const node = k3d::plugin::create<k3d::inode>(*Factory, m_document, node_name);
		return_val_if_fail(node, 0);

		// Keep track of the node to be selected ...
		k3d::inode* to_be_selected = node;

		// Keep track of every new node created ...
		std::vector<k3d::inode*> new_nodes;
		new_nodes.push_back(node);

		// Prepare to make connections to other nodes ...
		k3d::ipipeline::dependencies_t dependencies;

		// If the new node is a mesh source (but not a MeshInstance!), create a MeshInstance node and attach it so it's immediately visible ...
		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(node);
		if(mesh_source && k3d::classes::MeshInstance() != Factory->factory_id())
		{
			// Create a mesh instance ...
			k3d::inode* const mesh_instance = instantiate_mesh(node);
			return_val_if_fail(mesh_instance, 0);
			new_nodes.push_back(mesh_instance);

			// In this case, we want to select the mesh instance instead of the mesh source ...
			to_be_selected = mesh_instance;
		}

		// If the new node is a material sink, assign a default material ...
		if(k3d::imaterial_sink* const material_sink = dynamic_cast<k3d::imaterial_sink*>(node))
		{
			const std::vector<k3d::imaterial*> materials = k3d::node::lookup<k3d::imaterial>(m_document);
			if(materials.size())
				k3d::property::set_internal_value(material_sink->material_sink_input(), dynamic_cast<k3d::inode*>(*materials.rbegin()));
		}

		// If the new node is a time sink, connect it to the document time source ...
		if(k3d::itime_sink* const time_sink = dynamic_cast<k3d::itime_sink*>(node))
			dependencies.insert(std::make_pair(&time_sink->time_sink_input(), k3d::get_time(m_document)));

		if(!dependencies.empty())
			m_document.pipeline().set_dependencies(dependencies);

		// If the new node is a camera, orient it horizontally
		if(k3d::classes::Camera() == Factory->factory_id())
			k3d::set_matrix(*node, k3d::rotate3(k3d::radians(90.0), k3d::vector3(1, 0, 0)));
		
		// If the new node is a multiple mesh sink, add two mesh inputs
		if(Factory->implements(typeid(k3d::imulti_mesh_sink)))
		{
			k3d::property::create<k3d::mesh*>(*node, "input_mesh1", "Input Mesh 1", "", static_cast<k3d::mesh*>(0));
			k3d::property::create<k3d::mesh*>(*node, "input_mesh2", "Input Mesh 2", "", static_cast<k3d::mesh*>(0));
		}

		// If the new node is a render-engine, default to making every node in the document visible ...
		if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(node))
		{
			const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
			for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				k3d::property::set_internal_value(**property, document().nodes().collection());
		}

		// By default, make the new node visible in any node collection sinks that already exist ...
		const k3d::inode_collection::nodes_t::const_iterator doc_node_end = document().nodes().collection().end();
		for(k3d::inode_collection::nodes_t::const_iterator doc_node = document().nodes().collection().begin(); doc_node != doc_node_end; ++doc_node)
		{
			if(k3d::inode_collection_sink* const node_collection_sink = dynamic_cast<k3d::inode_collection_sink*>(*doc_node))
			{
				const k3d::inode_collection_sink::properties_t properties = node_collection_sink->node_collection_properties();
				for(k3d::inode_collection_sink::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					if(k3d::inode_collection_property* const node_collection_property = dynamic_cast<k3d::inode_collection_property*>(*property))
					{
						k3d::inode_collection_property::nodes_t nodes = k3d::property::internal_value<k3d::inode_collection_property::nodes_t>(**property);
						for(k3d::uint_t i = 0; i != new_nodes.size(); ++i)
						{
							if(node_collection_property->property_allow(*new_nodes[i]))
								nodes.push_back(new_nodes[i]);
						}
						k3d::property::set_internal_value(**property, nodes);
					}
				}
			}
		}

		// Give nodes a chance to initialize their property values based on their inputs, if any ...
		if(k3d::ireset_properties* const reset_properties = dynamic_cast<k3d::ireset_properties*>(node))
			reset_properties->reset_properties();

		// Replace the current selection
		selection::state(document()).deselect_all();
		selection::state(document()).select(*to_be_selected);

		// Make the newly-created node properties visible ...
		panel::mediator(document()).set_focus(*node);

		k3d::gl::redraw_all(m_document, k3d::gl::irender_viewport::ASYNCHRONOUS);

		return node;
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
			k3d::iscript_engine::context_t context;
			context["Command"] = k3d::string_t("startup");
			context["Document"] = &Document;
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
			k3d::iscript_engine::context_t context;
			context["Command"] = k3d::string_t("shutdown");
			context["Document"] = &document();
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

k3d::inode* document_state::create_node(k3d::iplugin_factory* Factory)
{
	return m_implementation->create_node(Factory);
}

k3d::inode* document_state::instantiate_mesh(k3d::inode* Node)
{
	return m_implementation->instantiate_mesh(Node);
}

void document_state::popup_context_menu(const bool UserAction)
{
	m_implementation->popup_context_menu(UserAction);
}

} // namespace ngui

} // namespace k3d

