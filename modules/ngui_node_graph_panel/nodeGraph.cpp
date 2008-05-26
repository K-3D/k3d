/*
Title:		A Fully User Interactive K-3D Node Graph Visualizer

Details:        This GUI Plugin allows a user to dynamically create bespoke networks of nodes 
                that are extracted from the current K-3D user document. A user can create new 
                instances of nodes, connect nodes together through channels and organise the 
                structure of the networks. 
                All of this is done through simple mouse clicks and mouse drags.
			
Author:		Alex Curtis
Contact:	alx.curtis@gmail.com
*/

/*
 *NOTE: AT THE MOMENT THIS IS JUST A MATERIAL LIST SAMPLE PLUGIN FOR GSOC
*/

#include <k3d-i18n-config.h>

#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtk/gtkmain.h>

#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/command_arguments.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/utility.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>

#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/nodes.h>

#include <boost/assign/list_of.hpp>

#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/imaterial_ri.h>

using namespace libk3dngui;

namespace module{

namespace ngui{

namespace node_graph{

namespace mechanics{


// [matNode]*********************************************************************************

/// Material Document Node. Conceptial Object To Be Displayed


//**A Node needs to be a template as could have any type (saves dynamic cast)

class matNode : public k3d::iunknown{
public:
	
	matNode(k3d::inode* _nodeHandle);

	std::string 			label;
	std::string 			matType;
	k3d::inode				*nodeHandle;

	static std::string 		getMatType(k3d::inode *node);
	void					getAndSetMatType(k3d::inode *node);
	
	static const std::string 	riMaterialStr;
	static const std::string 	glMaterialStr;
	static const std::string 	unknownMaterialStr;
	

};

//Static Material Type Definitions
const std::string matNode::riMaterialStr 	= "Renderman Material";
const std::string matNode::glMaterialStr 	= "OpenGL Material";
const std::string matNode::unknownMaterialStr 	= "K-3D Material";


matNode::matNode(k3d::inode* _nodeHandle) : nodeHandle(_nodeHandle){
}


std::string matNode::getMatType(k3d::inode *node){

	if((node)->factory().implements(typeid(k3d::ri::imaterial)))
		return(riMaterialStr);
		
	else if((node)->factory().implements(typeid(k3d::gl::imaterial)))
		return(glMaterialStr);
		
	else
		return(unknownMaterialStr);

}

void matNode::getAndSetMatType(k3d::inode *node){
	
	//Find The Material Type And Assign to this->matType
	matType = getMatType(node);

}

// [/matNode]********************************************************************************


// [matGraph]********************************************************************************

/// Defines a collection of material nodes

class matGraph{
public:
	~matGraph();

	typedef std::vector<matNode*> nodes_t;
	nodes_t matNodes;
	
};

matGraph::~matGraph(){
	std::for_each(matNodes.begin(), matNodes.end(), k3d::delete_object());
}

// [/matGraph]*******************************************************************************


// [filter_policy]***************************************************************************

// Abstract Class That Filters Particular Nodes For Display

class filter_policy{
public:
	virtual ~filter_policy(){}

	virtual void populate_graph(matGraph& MatGraph) = 0;
};

// [/filter_policy]**************************************************************************


// [material_filter_policy]******************************************************************

/// Implementation of filter_policy that displays every node that is a material

class material_filter_policy : public filter_policy{
public:
	material_filter_policy(k3d::idocument& Document);
	~material_filter_policy(){}

	void populate_graph(matGraph& MatGraph);

private:
	k3d::idocument& m_document;
};

material_filter_policy::material_filter_policy(k3d::idocument& Document) 
	:m_document(Document){
}

void material_filter_policy::populate_graph(matGraph& MatGraph){

	k3d::inode_collection::nodes_t::const_iterator nodeIter 
	= m_document.nodes().collection().begin();
	
	for(nodeIter; nodeIter != m_document.nodes().collection().end(); ++nodeIter){
		
		//Check IF A Node Is A Material
		if((*nodeIter)->factory().implements(typeid(k3d::imaterial))){
			
			//Create a new node object and pass the k3d node pointer
			matNode* const new_node = new matNode((*nodeIter));
			
			//Add Label Of Node
			new_node->label = (*nodeIter)->name();

			//Find What Type Of Material And Add String Note
			new_node->getAndSetMatType(*nodeIter);
				
			//Add To Material Vector
			MatGraph.matNodes.push_back((new_node));
		
		}
	}
}

// [/material_filter_policy]*****************************************************************


// [layout_policy]***************************************************************************

/// Abstract Class for nodes that adjust the layout of a material graph

class layout_policy{
public:
	virtual ~layout_policy() {}

	virtual void update_layout(matGraph& MatGraph) = 0;
};

// [/layout_policy]**************************************************************************


// [Sort Utilities]***************************************************************************

struct sort_by_label{
	bool operator()(const matNode* const LHS, const matNode* const RHS){
		return LHS->label < RHS->label;
	}
};

struct sort_by_name{
	bool operator()(k3d::iplugin_factory* const LHS, k3d::iplugin_factory* const RHS){
		return LHS->name() < RHS->name();
	}
};

// [/Sort Utilities]**************************************************************************


// [sort_by_label_layout_policy]*************************************************************

// Implementation of layout_policy that sorts nodes alphabetically by label

class sort_by_label_layout_policy : public layout_policy{
public:
	~sort_by_label_layout_policy() {}

	void update_layout(matGraph& MatGraph);
};

void sort_by_label_layout_policy::update_layout(matGraph& MatGraph){
	std::sort(MatGraph.matNodes.begin(), MatGraph.matNodes.end(), mechanics::sort_by_label());
}

// [/sort_by_label_layout_policy]*************************************************************


// [implementation]***************************************************************************

// The Plugin Mechanism Implementation

class implementation : public libk3dngui::asynchronous_update{
public:
	implementation(document_state& DocumentState) :
		m_document_state(DocumentState),
		m_filter_policy(new material_filter_policy(DocumentState.document())),
		m_layout_policy(new sort_by_label_layout_policy()){
		
		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_view);

		m_model = Gtk::ListStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(true);
		m_view.set_reorderable(false);

		Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
		cell_text->property_editable() = true;

		Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
		name_column->pack_start(*manage(cell_text), true);
		name_column->add_attribute(cell_text->property_text(), m_columns.name);

		//Material Name Column
		m_view.append_column("Material Name", m_columns.name);
		
		//Material Type Column
		m_view.append_column("Material Type", m_columns.typeName);

		// Reset the control contents anytime the document node collection changes
		m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
		m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
		m_document_state.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &implementation::on_node_renamed));

		schedule_update();
	}

	Gtk::ScrolledWindow m_scrolled_window;

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node)
		{
			if((*node)->factory().implements(typeid(k3d::imaterial))){
				Gtk::TreeRow new_row;
				new_row = *m_model->append();

				new_row[m_columns.name] = (*node)->name();
				new_row[m_columns.typeName] = matNode::getMatType(*node);
				
				new_row[m_columns.node] = (*node);
			}
		}

		
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){
			Gtk::TreeIter row;
			return_if_fail(get_row(*node, row));
			m_model->erase(row);
		}
	}

	/// Called by the signal system anytime a node is renamed
	void on_node_renamed(k3d::inode* const Node){
	
		// Rename row
		Gtk::TreeIter row;
		return_if_fail(get_row(Node, row));

		(*row)[m_columns.name] = Node->name();
	}

	/// Looks-up a model row based on a node pointer
	bool get_row(k3d::inode* const Node, Gtk::TreeIter& Row){
	
		Gtk::TreeNodeChildren rows = m_model->children();
		for(Gtk::TreeIter row = rows.begin(); row != rows.end(); ++row){	
			if(row->get_value(m_columns.node) == Node){
				Row = row;
				return true;
			}
		}

		return false;
	}

	/// Looks-up a node for a given row
	k3d::inode* const get_node(const Gtk::TreeIter& Row){
		return (*Row)[m_columns.node];
	}

	/// Looks-up a node based on a model row path
	k3d::inode* const get_node(const Gtk::TreePath& Path){
		if(Path.empty())
			return 0;

		Gtk::TreeRow row = *m_model->get_iter(Path);
		return row[m_columns.node];
	}

	/// Updates the contents of the control
	void on_update(){
	
		m_model->clear();

		m_current_graph.reset(new matGraph());
		m_filter_policy->populate_graph(*m_current_graph);
		m_layout_policy->update_layout(*m_current_graph);
		
		matGraph::nodes_t::const_iterator node = m_current_graph->matNodes.begin();

		for(node; node != m_current_graph->matNodes.end(); ++node){
			Gtk::TreeRow row = *m_model->append();
			row[m_columns.name] = (*node)->label;
			row[m_columns.typeName] = (*node)->matType;

			k3d::inode* const nodePtr = dynamic_cast<k3d::inode*>((*node)->nodeHandle);
			row[m_columns.node] = nodePtr;
			
		}
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores the current graph being visualized
	std::auto_ptr<matGraph> m_current_graph;
	/// Stores a policy that controls what's visible in the current graph
	std::auto_ptr<filter_policy> m_filter_policy;
	/// Stores a policy that handles layout of the current graph
	std::auto_ptr<layout_policy> m_layout_policy;

	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(node);
			add(name);
			add(typeName);
		}

		Gtk::TreeModelColumn<k3d::inode*> node;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> typeName;
	};

	columns m_columns;
	Glib::RefPtr<Gtk::ListStore> m_model;
	Gtk::TreeView m_view;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;
};

// [/implementation]**************************************************************************

} // namespace mechanics

// [panel]************************************************************************************


class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		base(false, 0),
		ui_component("mat_list", 0),
		m_implementation(0){
	}

	~panel(){
		delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent){
		ui_component::set_parent("mat_list", &Parent);

		m_implementation = new mechanics::implementation(DocumentState);

		m_implementation->m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), false), false);

		pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot){
		return m_implementation->m_panel_grab_signal.connect(Slot);
	}

	

	static k3d::iplugin_factory& get_factory(){
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0xfb11a3d8, 0x8c42e4d8, 0x2162d48e, 0x5e236a4b),
			"NGUINodeGraphPanel",
			_("Visual Graphing Of K3D Nodes"),
			"NGUI Panel",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Material List"));

		return factory;
	}
	
private:
	mechanics::implementation* m_implementation;
};

} // namespace node_graph

} // namespace ngui

} // namespace module


//Register The Plugin (K-3D)********************************************************************
K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::node_graph::panel::get_factory());
K3D_MODULE_END
//**********************************************************************************************
