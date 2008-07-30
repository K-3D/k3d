// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
//
// ---------------------
//
// Panel That Allows The User To Create / Modify Shaders For Different Renderers.
// Artist Notes Are Also Availible
//
// Material Manager Panel Developed By Alex Curtis
// Contact: alx.curtis@gmail.com

#include <gtk/gtkmain.h>
#include <gtkmm.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/label.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>
#include <gtkmm/textview.h>
#include <gtkmm/toggletoolbutton.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treemodel.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/imaterial_ri.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/command_arguments.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/image_toggle_button.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/text.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/path.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/share.h>
#include <k3dsdk/system.h>
#include <k3dsdk/system.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/types.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <boost/assign/list_of.hpp>
#include <iostream>
#include <sstream>
#include <time.h>

#include "RenderedImage.h"
#include "MaterialGroup.h"
#include "MaterialObj.h"
#include "ContentPanel.h"
#include "MaterialContentPanel.h"
#include "GroupContentPanel.h"

using namespace libk3dngui;
using namespace k3d::data;

namespace module{

namespace ngui{

namespace material_manager{

namespace mechanics{
  
//Material Type Definitions
const k3d::string_t riMaterialStr 	= "Renderman Materials";
const k3d::string_t glMaterialStr 	= "OpenGL Materials";
const k3d::string_t otherStuffStr 	= "Other Stuff";	

class Model
{
public:

  Model()
    :m_rmangrp(riMaterialStr),
     m_glgrp(glMaterialStr),
     m_othergrp(otherStuffStr)
  {
  }

  ~Model()
  {
  }

public:
  void buildModel(document_state& DocumentState);
  void clearModel();

public:
  std::list<MaterialGroup*>     	m_groups;

  MaterialGroup 						m_rmangrp;
  MaterialGroup 			  			m_glgrp;
  MaterialGroup 						m_othergrp;

};//m_Model

void Model::buildModel(document_state& DocumentState)
{
  //Add Compulsory Groups To Group List
  m_groups.push_back(&m_rmangrp);
  m_groups.push_back(&m_glgrp);
  m_groups.push_back(&m_othergrp);

  //Iterate Through The Document Nodes Find Materials
  k3d::inode_collection::nodes_t::const_iterator nodeIter 
    = DocumentState.document().nodes().collection().begin();
	
  for(nodeIter; nodeIter != DocumentState.document().nodes().collection().end(); ++nodeIter)
    {
      //Check IF A Node Is A Material
      if((*nodeIter)->factory().implements(typeid(k3d::imaterial)))
        {
          //Is It A Renderman Material
          if((*nodeIter)->factory().implements(typeid(k3d::ri::imaterial)))
            {
              MaterialObj *renderman_matobj =
                new MaterialObj(&m_rmangrp, (*nodeIter), riMaterialStr);

              //Initialize The Material Object
              renderman_matobj->init();

              //Add Material To Renderman Group
              m_rmangrp.addMaterial(renderman_matobj);
            }

          //Is It A OpenGL Material
          else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial)))
            {
              MaterialObj *gl_matobj 
                = new MaterialObj(&m_glgrp, (*nodeIter), glMaterialStr);

              //Initialize The Material Object
              gl_matobj->init();

              //Add Material To OpenGL Group
              m_glgrp.addMaterial(gl_matobj);
            }

          //Must Be An Uncatagorized Material
          else
            {
              MaterialObj *other_matobj 
                = new MaterialObj(&m_othergrp, (*nodeIter), otherStuffStr);

              //Initialize The Material Object
              other_matobj->init();

              //Add Material To Other Group
              m_othergrp.addMaterial(other_matobj);
            }

        }//if
    }//for
  
}//buildModel


void Model::clearModel()
{
  std::list<MaterialGroup*>::iterator groupIter = m_groups.begin();
  for(; groupIter != m_groups.end(); groupIter++)
    delete (*groupIter);      

  //Ensure Clean Storage 
  m_groups.clear();
}

// [Implementation]*************************************************************

class Implementation : public libk3dngui::asynchronous_update
{
public:
  Implementation(document_state& DocumentState, k3d::icommand_node& Parent) 
    :m_document_state(DocumentState),
     m_model(new Model),
     m_parent(&Parent),
     add_group("Add"),
     remove_group("Remove")
  {
  }

  ~Implementation()
  {	
  }

  //Initialization Of Object Contents Beyond Initial Values
  void init();


public:

  //GTK Method Invoked On A Scheduled Panel Update
  void on_update();

  //Build The Interface
  void buildGui();
  void buildTree();
  void buildContentPanel(Gtk::TreeModel::Row row, bool iMaterialGroup);

  //Event Handlers (Tree)
  void onNodesAdded(const k3d::inode_collection::nodes_t& Nodes) ;
  void onNodesRemoved(const k3d::inode_collection::nodes_t& Nodes);
  void onNodeRenamed(k3d::inode* const Node);
  bool onNodeSelection(k3d::inode* const Node);

  void on_add_button_button_clicked();
  void on_remove_button_button_clicked();
	  
  void onTreeRowChanged();

  //Find Tree Row Based On 1st Argument
  bool getRow(k3d::inode* const Node, Gtk::TreeIter& Row);
	
private:

  k3d::icommand_node 			*m_parent;

  //Property Signal connection
  sigc::connection 				m_pConnection;

public:

  //GTK Widgets
  Gtk::ScrolledWindow 			m_tree_scrolled_window;
  Gtk::HPaned 						m_main_hpaned;
  Gtk::TreeView 					m_tree;
  Gtk::VBox 						m_tool_tree_cont;

  //Toolbar Contents
  Gtk::Toolbar 					m_toolbar; 
  Gtk::ToolButton add_group; //BIN
  Gtk::ToolButton remove_group; //BIN

  //Signal When Control Grabs The Panel Focus
  sigc::signal<void> 			m_panel_grab_signal;

  //A Reference To The Owning Document
  document_state& 				m_document_state;
	  
  //Model That Stores Data
  std::auto_ptr<Model> 			m_model;

  //The Right Content Pane For The Panel
  std::auto_ptr<ContentPanel> m_rpane_content;

  //Tree Model Used By Material Navigator
  class columns :
    public Gtk::TreeModel::ColumnRecord
  {
  public:
    columns()
    {
      add(m_col_name);
      add(m_col_icon);
      add(m_col_ismg);
      add(m_col_mgptr);
      add(m_col_moptr);
    }
    Gtk::TreeModelColumn<MaterialGroup*> 					m_col_mgptr;
    Gtk::TreeModelColumn<MaterialObj*> 					m_col_moptr;
    Gtk::TreeModelColumn<Glib::ustring> 					m_col_name;
    Gtk::TreeModelColumn<bool>		    					m_col_ismg;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > 	m_col_icon;
  };

  //Tree Related Variables
  columns 									m_columns;
  Glib::RefPtr<Gtk::TreeStore> 		tree_model;
  Glib::RefPtr<Gtk::TreeSelection> 	tree_selection;


};//Implementation



void Implementation::init()
{
  //Create The Material Tree Model
  tree_model = Gtk::TreeStore::create(m_columns);
  m_tree.set_model(tree_model);
  tree_selection = m_tree.get_selection();

  //Tree Event Signal Connections
  tree_selection->signal_changed()
    .connect(sigc::mem_fun(*this,  &Implementation::onTreeRowChanged));

  //Document Event Signal Connections
  m_document_state.document().nodes().add_nodes_signal()
    .connect(sigc::mem_fun(*this, &Implementation::onNodesAdded));

  m_document_state.document().nodes().remove_nodes_signal()
    .connect(sigc::mem_fun(*this, &Implementation::onNodesRemoved));

  m_document_state.document().nodes().rename_node_signal()
    .connect(sigc::mem_fun(*this, &Implementation::onNodeRenamed));

  //Tree Editing Signal Connections
  add_group.signal_clicked()
    .connect(sigc::mem_fun(*this, &Implementation::on_add_button_button_clicked));

  //Create A Signal Connection For Remove Group Button
  remove_group.signal_clicked()
    .connect(sigc::mem_fun(*this, &Implementation::on_remove_button_button_clicked));

  //Delete Model Data
  m_model->clearModel();

  //Rebuild Model Data
  m_model->buildModel(m_document_state);

  //Build The GTK GUI
  buildGui();

  //Hint To GTK To Update Interface
  schedule_update();

}//init



void Implementation::on_update()
{
  //Clear The Tree Model
  tree_model->clear();

  //Rebuild Tree From Rebuilt Model
  buildTree();

}//on_update



void Implementation::buildGui()
{
  //Setup The Material Tree
  m_tree.set_headers_visible(false);
  m_tree.set_reorderable(false);
  Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
  cell_text->property_editable() = true;

  //Add A Name Column To The Tree
  Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
  name_column->pack_start(*manage(cell_text), true);
  name_column->add_attribute(cell_text->property_text(), m_columns.m_col_name);

  //Add A Icon Column To The Tree
  m_tree.append_column("icon", m_columns.m_col_icon);
  m_tree.append_column(*manage(name_column));

  //Setup Tree Headers & Display Look & Feel Options
  m_tree.set_headers_visible(false);
  m_tree.set_rules_hint(true);  	

  //Setup the Window
  m_tree_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   
  //Add The Tree To The Scrolling Window
  m_tree_scrolled_window.add(m_tree);

  //Create & Embed Tree Toolbar
  m_main_hpaned.add1(m_tool_tree_cont);
  m_toolbar.append(add_group);
  m_toolbar.append(remove_group);
  m_tool_tree_cont.pack_start(m_toolbar, false, false, 0);

  //Resize The Tree's Scroll Window To Fit Correctly
  m_tree_scrolled_window.set_size_request(220, -1);

  //Embed The Tree
  m_tool_tree_cont.pack_start(m_tree_scrolled_window, true, true, 0);
	  	  
}//buildGui



void Implementation::buildTree()
{
  //Iterate Through Known Groups & Build Tree Hierachy
  std::list<MaterialGroup*>::const_iterator group_iter = m_model->m_groups.begin();

  Gtk::TreeRow first_group_row;
  bool on_first_row = true;

  for(; group_iter !=  m_model->m_groups.end(); group_iter++)
    {
      Gtk::TreeRow row = *tree_model->append();
      row[m_columns.m_col_name] = (*group_iter)->name();
      row[m_columns.m_col_ismg] = true;
      row[m_columns.m_col_mgptr] = (*group_iter);
      row[m_columns.m_col_moptr] = 0;

      //Grab First Row For Groups
      if(on_first_row)
        {
          first_group_row = row;
          on_first_row = false;
        }

      //Build Each Groups Children Using Groups Associated Material Objects
      std::list<MaterialObj*>::const_iterator matobj_iter 
        = (*group_iter)->materialBegin();

      for(; matobj_iter !=  (*group_iter)->materialEnd(); matobj_iter++)
        {
          //Create Row & USe MaterialObj To Fill In The Blanks
          Gtk::TreeModel::Row childrow = *(tree_model->append(row.children()));
          childrow[m_columns.m_col_name] = (*matobj_iter)->name();
          childrow[m_columns.m_col_ismg] = false;
          childrow[m_columns.m_col_mgptr] = 0;
          childrow[m_columns.m_col_moptr] = (*matobj_iter);
          childrow[m_columns.m_col_icon] 
            = quiet_load_icon((*matobj_iter)->docNodeName(), Gtk::ICON_SIZE_MENU);
        }//for	
	
    }//for

  //Ensure Tree Inital Selection Is The First Row 
  if(!on_first_row)
    tree_selection->select(first_group_row);	   

}//buildTree

    

void Implementation::onTreeRowChanged()
{
  //Get The Current Row Selection
  Gtk::TreeModel::iterator iter = tree_selection->get_selected();
  
  if(iter)
    {
      Gtk::TreeModel::Row row = *iter;

      //Is The Selection A Material Group
      if(row[m_columns.m_col_ismg])
        {
          k3d::log() << row->get_value(m_columns.m_col_mgptr)->name() << std::endl;
          buildContentPanel(row, true);
        }

      //Must Be A Material Object
      else
        {
          //Emit Selection Signal For Other Panels (Node Properties Panel etc)
          const k3d::inode *selectedNode 
            = (row->get_value(m_columns.m_col_moptr))->docNode();

          k3d::selection::record selected_record =
            k3d::selection::make_record(const_cast<k3d::inode*>(selectedNode));

          m_document_state.select(selected_record);

          //Build The GTK GUI Context
          buildContentPanel(row, false);
        }

    }//if

}//onTreeRowChanged



void Implementation::buildContentPanel(Gtk::TreeModel::Row row, bool m_col_ismg)
{
  //Check If Building Group Panel Or Material Object Panel
  if(m_col_ismg)
    {
      //Delete Object. Create New GroupContentPanel Through Smart Pointer
      m_rpane_content 
        = std::auto_ptr<ContentPanel>
        (new GroupContentPanel(&m_main_hpaned, 
                               row->get_value(m_columns.m_col_mgptr), 
                               m_parent, &m_document_state));

      //Initialise GroupContentPanel
      m_rpane_content->init();
    }

  else
    {
      //Delete Object. Create New MaterialContentPanel Through Smart Pointer
      m_rpane_content 
        = std::auto_ptr<ContentPanel>
        (new MaterialContentPanel(&m_main_hpaned, 
                                  row->get_value(m_columns.m_col_moptr), 
                                  m_parent, &m_document_state));

      //Initialise MaterialContentPanel
      m_rpane_content->init();

    }

  //Build The Content Pane From Corrected Pointer
  m_rpane_content->buildPanel();

}//buildContentPanel




void Implementation::onNodesAdded(const k3d::inode_collection::nodes_t& Nodes)
{
  //Flag Used For Possible Panel Update
  bool material_added = false;

  //Iterate Through Each Node & Add Appropriate To The Tree Hierachy
  for(k3d::inode_collection::nodes_t::const_iterator node_iter 
        = Nodes.begin(); node_iter != Nodes.end(); ++node_iter)
    {
      //Check If Is A Material Type
      if((*node_iter)->factory().implements(typeid(k3d::imaterial))){

        //Set Flag (Material Added) > Will Try To Update Panel
        material_added = true;

        Gtk::TreeNodeChildren rows = tree_model->children();
        Gtk::TreeRow new_row;
        Gtk::TreeIter row = rows.begin();

        k3d::string_t typeStr = "";
        MaterialGroup *groupPtr = 0;

        //Place Renderman Material
        if((*node_iter)->factory().implements(typeid(k3d::ri::imaterial))){
          typeStr = riMaterialStr;
          groupPtr = &(m_model->m_rmangrp);
        }

        //Place GL Material
        else if((*node_iter)->factory().implements(typeid(k3d::gl::imaterial))){
          typeStr = glMaterialStr;
          groupPtr = &(m_model->m_glgrp);
        }

        //Place Other Material
        else{
          typeStr = otherStuffStr;
          groupPtr = &(m_model->m_othergrp);
        }

        //Type Of Node Found. Now Find It In The Tree & Insert
        for(; row != rows.end(); ++row)
          {
            if(row->get_value(m_columns.m_col_name) == typeStr)
              {
                //Create A New MaterialObj For Material Node
                MaterialObj *newSObject 
                  = new MaterialObj(groupPtr, *node_iter, typeStr);
                
                //Initalize The New MaterialObj
                newSObject->init();

                //Add The MaterialObj To The Groups Listing
                groupPtr->addMaterial(newSObject);

                //Create The Tree Entry
                new_row = *tree_model->append(row->children());
                new_row[m_columns.m_col_moptr] = newSObject;
                new_row[m_columns.m_col_mgptr] = 0;
                new_row[m_columns.m_col_ismg] = false;
                new_row[m_columns.m_col_name] = (*node_iter)->name();
                new_row[m_columns.m_col_icon] 
                  = quiet_load_icon((*node_iter)->factory().name(), 
                                    Gtk::ICON_SIZE_MENU);

                //Row Found. No Need To Continue Searching
                break;

              }//if
          }//for
      }//if
    }//for

  //Check If Material Was Added >> If So Update Panel Only If Group
  if(material_added)
    {
      //Get The Selected Tree Row
      Gtk::TreeModel::iterator iter = tree_selection->get_selected();
      if(iter)
        {
          Gtk::TreeModel::Row row = *iter;

          //Build Content Panel Only If Group Type Selected
          if(row[m_columns.m_col_ismg])
            buildContentPanel(row, true);
   
        }//if
    }//if


}//onNodesAdded




void Implementation::onNodesRemoved(const k3d::inode_collection::nodes_t& Nodes)
{
  //Flag For Possible Panel Update
  bool material_removed = false;

  k3d::inode_collection::nodes_t::const_iterator node_iter = Nodes.begin();

    for(; node_iter != Nodes.end(); ++node_iter)
      {       
        //Check If Is A Material Type First
        if((*node_iter)->factory().implements(typeid(k3d::imaterial)))
          {
            //Set Flag (Material Added) > Will Try To Update Panel
            material_removed = true;
          }

        Gtk::TreeIter row;
        return_if_fail(getRow(*node_iter, row));
	    
        MaterialObj *tmpSObj = row->get_value(m_columns.m_col_moptr);
        MaterialGroup *grpPtr = 0;

        if(tmpSObj)  
          grpPtr = tmpSObj->groupParent();
	      
        //Erase The Material Node From Tree
        tree_model->erase(row);	
    
        //Delete In Stored Model
        if(grpPtr)
          grpPtr->removeMaterial(tmpSObj);
	    
      }//for

  //Rebuild Currently Selected Pane. Only If Material Removed
  if(material_removed)
    {
      //Get Selected Row In Tree
      Gtk::TreeModel::iterator iter = tree_selection->get_selected();
      if(iter)
        {
          Gtk::TreeModel::Row row = *iter;
          if(row[m_columns.m_col_ismg])
            buildContentPanel(row, true);
      		
        }//if
    }//if

}//onNodesRemoved




void Implementation::onNodeRenamed(k3d::inode* const Node)
{
  Gtk::TreeIter row;
  
  //Find The Row On Tree That Has Pointer To Node
  return_if_fail(getRow(Node, row));

  //Rename That Row With The New Node Name
  (*row)[m_columns.m_col_name] = Node->name();

  //Rename In Stored Model
  (row->get_value(m_columns.m_col_moptr))->setName(Node->name());

}//onNodeRenamed




bool Implementation::getRow(k3d::inode* const Node, Gtk::TreeIter& Row)
{
  Gtk::TreeNodeChildren rows = tree_model->children();

  //Iterate Through Each Row (Parent Rows)
  Gtk::TreeIter parent_iter = rows.begin();
  for(; parent_iter != rows.end(); parent_iter++)
    {
      //Iterate Through Each Child Of Parent
      Gtk::TreeIter children_iter = parent_iter->children().begin();
      for(; children_iter != parent_iter->children().end(); children_iter++)
        {
          //Check If material Object and Not Group
          if(children_iter->get_value(m_columns.m_col_moptr) 
             && !(children_iter->get_value(m_columns.m_col_ismg)))
            {
              //Check If Node Ptr Matches The Current Node Argument
              if((children_iter->get_value(m_columns.m_col_moptr)->docNode()) == Node)
                {
                Row = children_iter;
                return true;
            }

          }//if

        }//for
    }//for

  //No Node Found On Tree
  return false;

}//onNodeRenamed





      //   bool Implementation::onNodeSelection(k3d::inode* const Node)
//         {
//           //return result (if M_RMANGRP Material)
//           bool result = false;

//           //Check If Node Is A Rendem_rmangrp Material
//           if((Node)->factory().implements(typeid(k3d::ri::imaterial))){

//             //It Is A M_RMANGRP Material!
//             result = true;

//             //Disconnect Any Existing Connection With Properties
//             m_pConnection.disconnect();

//             //Create Connection For Node Change
//             k3d::inode_change_signal *n_sig = dynamic_cast<k3d::inode_change_signal*>(Node);

//             if(n_sig)
//               m_pConnection =  n_sig->connect_node_changed_signal(sigc::mem_fun(*this, &Implementation::propertySignalRender));

//             //Change Tree Selection (which implicitly renders updated preview)

//           return result;

//         }//onNodeSelection 




        void Implementation::on_add_button_button_clicked()
        {
          k3d::log() << " Add Button Pressed" << std::endl;

          //Create & Embed New Group
          MaterialGroup *newGroup = new MaterialGroup("New Group");
          m_model->m_groups.push_back(newGroup);

          //Add New Group To The Tree
          Gtk::TreeRow row = *tree_model->append();
          row[m_columns.m_col_name] = newGroup->name();
          row[m_columns.m_col_ismg] = true;
          row[m_columns.m_col_mgptr] = newGroup;
          row[m_columns.m_col_moptr] = 0;

        }//on_add_button_button_clicked



        void Implementation::on_remove_button_button_clicked()
        {

          //Get The Currently Selected Row (Group)
          Gtk::TreeModel::iterator iter = tree_selection->get_selected();
          if(iter) //If anything is selected
            {
              Gtk::TreeModel::Row row = *iter;

              //Is It A Group?
              if(row[m_columns.m_col_ismg])
                {
		  
                  k3d::log() << " Remove Button Pressed... Will Delete Group (Eventually :) )" << std::endl;

                }//if
            }//if

        }//on_remove_button_button_clicked


} //namespace mechanics


// [Panel]************************************************************************************
     
class Panel :
  public libk3dngui::panel::control,
  public libk3dngui::ui_component,
  public Gtk::VBox
{
  //baseContainer is the preview & ctrl container
  typedef Gtk::VBox base_cont_t;

public:
  Panel() 
    :base_cont_t(false, 0),
     m_implementation(0)
  {
  }

  ~Panel()
  {
    delete m_implementation;
  }

  void initialize(document_state& _document_sate, k3d::icommand_node& _parent)
  {
    k3d::command_tree().add(*this, "material_manager", &_parent);

    //Create New Implementation Object
    m_implementation = new mechanics::Implementation(_document_sate, _parent);
    m_implementation->init();

    //Pack Implementation Into This Panel
    pack_start(m_implementation->m_main_hpaned, Gtk::PACK_EXPAND_WIDGET);

    show_all();
  }

  const k3d::string_t panel_type()
  {
    return "material_manager";
  }

  sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
  {
    return m_implementation->m_panel_grab_signal.connect(Slot);
  }


  static k3d::iplugin_factory& get_factory()
  {
    static k3d::application_plugin_factory<Panel> 
      factory(k3d::uuid(0xd363f420, 0x7240b35e, 0x7cf38788, 0xda06e8e6),
              "materialManager",
              _("Material Manager Panel"),
              "NGUI Panels",
              k3d::iplugin_factory::EXPERIMENTAL,
              boost::assign::map_list_of("ngui:component-type", "panel")
              ("ngui:panel-type", "material_manager")
              ("ngui:panel-label", "Material Manager"));

    return factory;
  }

private:

  mechanics::Implementation* m_implementation;

};//Panel

      
} // namespace material_manager

} // namespace ngui

} // namespace module


//Register The Plugin (K-3D)*****************************************************
K3D_MODULE_START(Registry)
  Registry.register_factory(module::ngui::material_manager::Panel::get_factory());
K3D_MODULE_END
//*******************************************************************************

