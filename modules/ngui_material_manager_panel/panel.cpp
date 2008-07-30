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

        // [Model Components]***********************************************************************

        //Static Material Type Definitions
        const k3d::string_t riMaterialStr 	= "Renderman Materials";
        const k3d::string_t glMaterialStr 	= "OpenGL Materials";
        const k3d::string_t otherStuffStr 	= "Other Stuff";	

        //forward declaration for list of materials
        class MaterialGroup;

  
          //************
          class Model{
          public:
            Model()
            {
              rman = new MaterialGroup(riMaterialStr);
              gl = new MaterialGroup(glMaterialStr);
              other = new MaterialGroup(otherStuffStr);
            }
            ~Model()
            {
              //Clean Up RMAN, GL AND Other Groups
              if(rman)
                delete rman;
              if(gl)
                delete gl;
              if(other)
                delete other;
            }

          public:
            void buildModel(document_state& DocumentState);
            void clearModel();

          public:
            std::list<MaterialGroup*>     m_groups;

            MaterialGroup *rman;
            MaterialGroup *gl;
            MaterialGroup *other;

          };//m_Model

        void Model::buildModel(document_state& DocumentState)
        {
          //Push Back Global Groups RMAN, GL and other
          m_groups.push_back(rman);
          m_groups.push_back(gl);
          m_groups.push_back(other);

          //Iterate Through The Document Nodes Find Materials
          k3d::inode_collection::nodes_t::const_iterator nodeIter 
            = DocumentState.document().nodes().collection().begin();
	
          for(nodeIter; nodeIter != DocumentState.document().nodes().collection().end(); ++nodeIter)
            {
              //Check IF A Node Is A Material
              if((*nodeIter)->factory().implements(typeid(k3d::imaterial)))
                {
                  //Check To Find Group Catagory and Build Group
                  if((*nodeIter)->factory().implements(typeid(k3d::ri::imaterial)))
                    {
                    MaterialObj *renderman_matobj = new MaterialObj(rman, (*nodeIter), riMaterialStr);
                    renderman_matobj->init();
                    rman->addMaterial(renderman_matobj);
                    }

                  else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial)))
                    {
                      MaterialObj *gl_matobj = new MaterialObj(gl, (*nodeIter), glMaterialStr);
                      gl_matobj->init();
                      gl->addMaterial(gl_matobj);
                    }
                  else
                    {
                      MaterialObj *other_matobj = new MaterialObj(other, (*nodeIter), otherStuffStr);
                      other_matobj->init();
                      other->addMaterial(other_matobj);
                    }

                }//if
            }//for

        }

        void Model::clearModel()
        {

          k3d::log() << "NUMBER OF GROUPS: " << m_groups.size() << std::endl;
          std::list<MaterialGroup*>::iterator groupIter = m_groups.begin();
          for(; groupIter != m_groups.end(); groupIter++)
              delete (*groupIter);      

          //Ensure Clean Storage 
          m_groups.clear();

          k3d::log() << "groups cleared...therefore shader_o cleared" << std::endl;

        }

  

        //************MAIN IMPLEMENTATION
        class Implementation : public libk3dngui::asynchronous_update{
        public:
          Implementation(document_state& DocumentState, k3d::icommand_node& Parent) 
            :m_document_state(DocumentState),
             m_model(new Model),
             m_parent(&Parent),
             add_group("Add"),
             remove_group("Remove")
          {
            //Create The Material Tree Model
            tree_model = Gtk::TreeStore::create(m_columns);
            m_nav.set_model(tree_model);
            tree_selection = m_nav.get_selection();

            // Connect signals
            //m_nav.signal_button_press_event().connect(sigc::mem_fun(*this, &Implementation::on_button_press_event), false);
            //m_nav.get_selection()->set_select_function(sigc::mem_fun(*this, &Implementation::on_select_row));

            //m_nav.get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Implementation::on_selection_changed)); 
	
            tree_selection->signal_changed().connect(sigc::mem_fun(*this,  &Implementation::on_tree_row_changed));

            // Reset the control contents anytime the document node collection changes
            m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &Implementation::on_nodes_added));
            m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &Implementation::on_nodes_removed));
            m_document_state.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &Implementation::on_node_renamed));

            //Group Buttons
            //Create A Signal Connection For Add Group Button
            add_group.signal_clicked().connect(sigc::mem_fun(*this, &Implementation::on_add_button_button_clicked));

            //Create A Signal Connection For Remove Group Button
            remove_group.signal_clicked().connect(sigc::mem_fun(*this, &Implementation::on_remove_button_button_clicked));

            //Delete Model Data
            m_model->clearModel();

            //Rebuild Model Data
            m_model->buildModel(m_document_state);

            build_gui();
            schedule_update();
          }

          ~Implementation()
          {	
          }

        public:
          // Updates the contents of the control
          void on_update();
          // Build The Interface
          void build_gui();
          void build_tree();
          void build_ContentPanel(Gtk::TreeModel::Row row, bool iMaterialGroup);

          //Event Handlers (Tree)
          void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes) ;
          void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes);
          void on_node_renamed(k3d::inode* const Node);
          bool on_node_selection(k3d::inode* const Node);

          void on_add_button_button_clicked();
          void on_remove_button_button_clicked();
	  
          void on_tree_row_changed();

          bool get_row(k3d::inode* const Node, Gtk::TreeIter& Row);
	
        private:
          k3d::icommand_node *m_parent;
          //Property Signal connection
          sigc::connection m_pConnection;

        public:
          //GTK Widgets
          Gtk::ScrolledWindow m_tree_scrolled_window;
          Gtk::HPaned m_HPanedMain;
          Gtk::TreeView m_nav;

          Gtk::VBox toolbar_tree_c;

          //Toolbar Contents
          Gtk::Toolbar m_toolbar; 
          Gtk::ToolButton add_group;
          Gtk::ToolButton remove_group;

        public:

          // Signal that will be emitted whenever this control should grab the panel focus
          sigc::signal<void> m_panel_grab_signal;
          // Stores a reference to the owning document
          document_state& m_document_state;
	  
          //Model That Stores Data (STD Ptr Need To Manage)
          std::auto_ptr<Model> m_model;

          //The Right Content Pane For The Panel
          std::auto_ptr<ContentPanel> m_rpane_content;

          class columns :
            public Gtk::TreeModel::ColumnRecord
          {
          public:
            columns()
            {
              add(name);
              add(icon);
              add(iMaterialGroup);
              add(MaterialGroup_ptr);
              add(MaterialObj_ptr);
            }
            Gtk::TreeModelColumn<MaterialGroup*> 		MaterialGroup_ptr;
            Gtk::TreeModelColumn<MaterialObj*> 		MaterialObj_ptr;
            Gtk::TreeModelColumn<Glib::ustring> 	name;
            Gtk::TreeModelColumn<bool>		    	iMaterialGroup;	//Group Or Object Pointer Used
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
          };

          columns m_columns;
          Glib::RefPtr<Gtk::TreeStore> tree_model;
          Glib::RefPtr<Gtk::TreeSelection> tree_selection;


        };//Implementation


        void Implementation::on_update()
        {
          //Clear The Tree Model
          tree_model->clear();

          //Rebuild Tree From Rebuilt Model
          build_tree();

        }//on_update

        void Implementation::build_gui()
        {
          //Material Tree Setup
          m_nav.set_headers_visible(false);
          m_nav.set_reorderable(false);

          Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
          cell_text->property_editable() = true;
          //cell_text->signal_edited().connect(sigc::mem_fun(*this, &Implementation::on_node_name_edited));

          Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
          name_column->pack_start(*manage(cell_text), true);
          name_column->add_attribute(cell_text->property_text(), m_columns.name);

          m_nav.append_column("icon", m_columns.icon);
          m_nav.append_column(*manage(name_column));

          //Set Rows Reordable
          m_nav.set_reorderable(true);

          //Set Headers & Display Look & Feel Options
          m_nav.set_headers_visible(false);
          m_nav.set_rules_hint(true);  	

          //Setup the Window
          m_tree_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   
          //Add The Tree To The Scrolling Window
          m_tree_scrolled_window.add(m_nav);

          m_HPanedMain.add1(toolbar_tree_c);

          //Create Tree Toolbar
          m_toolbar.append(add_group);
          m_toolbar.append(remove_group);

          toolbar_tree_c.pack_start(m_toolbar, false, false, 0);

          m_tree_scrolled_window.set_size_request(220, -1);	//Required Otherwise Scrolledwindow Is Too Thin

          //Pack Tree
          toolbar_tree_c.pack_start(m_tree_scrolled_window, true, true, 0);
	  
	  
        }//build_gui

        void Implementation::build_tree()
        {
          //Group Through Groups And Add To Tree
          std::list<MaterialGroup*>::const_iterator gIter = m_model->m_groups.begin();
          Gtk::TreeRow first_group_row;
          bool on_first_row = true;

          for(; gIter !=  m_model->m_groups.end(); gIter++)
            {
              Gtk::TreeRow row = *tree_model->append();
              row[m_columns.name] = (*gIter)->name();
              row[m_columns.iMaterialGroup] = true;
              row[m_columns.MaterialGroup_ptr] = (*gIter);
              row[m_columns.MaterialObj_ptr] = 0;

              //Grab First Row For Groups
              if(on_first_row)
                {
                  first_group_row = row;
                  on_first_row = false;
                }

              //Go Through Materials/Other and and to tree under groups
              std::list<MaterialObj*>::const_iterator sIter = (*gIter)->materialBegin();
              for(; sIter !=  (*gIter)->materialEnd(); sIter++)
                {
                  Gtk::TreeModel::Row childrow = *(tree_model->append(row.children()));
                  childrow[m_columns.name] = (*sIter)->name();
                  childrow[m_columns.iMaterialGroup] = false;
                  childrow[m_columns.MaterialGroup_ptr] = 0;
                  childrow[m_columns.MaterialObj_ptr] = (*sIter);
                  childrow[m_columns.icon] = quiet_load_icon((*sIter)->docNodeName(), Gtk::ICON_SIZE_MENU);
                }//for		
            }//for

          //Set Selected Tree Row
          if(!on_first_row)
            tree_selection->select(first_group_row);	   

        }//build_tree

        //Called When Row Changed 
        void Implementation::on_tree_row_changed()
        {
          //Get The Current Row Selection
          Gtk::TreeModel::iterator iter = tree_selection->get_selected();
          if(iter) //If anything is selected
            {
              Gtk::TreeModel::Row row = *iter;
              if(row[m_columns.iMaterialGroup])
                {
                  k3d::log() << row->get_value(m_columns.MaterialGroup_ptr)->name() << std::endl;
                  build_ContentPanel(row, true);
                }
              else
                {
                  //Single Material Selected***

                  //Send Selection Signal For Other Panels
                  const k3d::inode *selectedNode = (row->get_value(m_columns.MaterialObj_ptr))->docNode();

                  k3d::selection::record selected_record = k3d::selection::make_record(const_cast<k3d::inode*>(selectedNode));

                  m_document_state.select(selected_record);

                  //Build The GUI Context
                  build_ContentPanel(row, false);
                }
            }//if

        }//on_tree_row_changed

        //Build the right Content Pane (GUI Stuff)
        void Implementation::build_ContentPanel(Gtk::TreeModel::Row row, bool iMaterialGroup)
        {
          //Check If Building Group Panel Or Material Object Panel
          if(iMaterialGroup)
            {
              //Delete current obj (group or material gui) and create new group gui obj
              m_rpane_content = std::auto_ptr<ContentPanel>(new GroupContentPanel(&m_HPanedMain, row->get_value(m_columns.MaterialGroup_ptr), m_parent, &m_document_state));

              //Initialise GroupContentPanel
              m_rpane_content->init();

            }
          else
            {
              //Delete current obj (group or material gui) and create new group gui obj
              m_rpane_content = std::auto_ptr<ContentPanel>(new MaterialContentPanel(&m_HPanedMain, row->get_value(m_columns.MaterialObj_ptr), m_parent, &m_document_state));

              //Initialise MaterialContentPanel
              m_rpane_content->init();

            }//endif

          //Build The Content Pane From Corrected Pointer
          m_rpane_content->buildPanel();

        }//build_ContentPanel

        void Implementation::on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
        {
          //Flag For Possible Panel Update
          bool material_added = false;

          //Iterate through each node and if appropriate add to tree
          for(k3d::inode_collection::nodes_t::const_iterator nodeIter = Nodes.begin(); nodeIter != Nodes.end(); ++nodeIter)
            {
              //Check If Is A Material Type First
              if((*nodeIter)->factory().implements(typeid(k3d::imaterial))){

                //Set Flag (Material Added) > Will Try To Update Panel
                material_added = true;

                Gtk::TreeNodeChildren rows = tree_model->children();
                Gtk::TreeRow new_row;
                Gtk::TreeIter row = rows.begin();

                k3d::string_t typeStr = "";
                MaterialGroup *groupPtr = 0;

                //Place Renderman Material
                if((*nodeIter)->factory().implements(typeid(k3d::ri::imaterial))){
                  typeStr = riMaterialStr;
                  groupPtr = m_model->rman;
                }
                //Place GL Material
                else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial))){
                  typeStr = glMaterialStr;
                  groupPtr = m_model->gl;
                }
                //Place Other Material
                else{
                  typeStr = otherStuffStr;
                  groupPtr = m_model->other;
                }
                for(; row != rows.end(); ++row)
                  {
                    if(row->get_value(m_columns.name) == typeStr)
                      {
                        //Create material Object
                        MaterialObj *newSObject = new MaterialObj(groupPtr, *nodeIter, typeStr);
                        newSObject->init();

                        //Push The material Onto Groups List
                        groupPtr->addMaterial(newSObject); //check

                        //Create The Tree Entry
                        new_row = *tree_model->append(row->children());
                        new_row[m_columns.MaterialObj_ptr] = newSObject;
                        new_row[m_columns.MaterialGroup_ptr] = 0;
                        new_row[m_columns.iMaterialGroup] = false;
                        new_row[m_columns.name] = (*nodeIter)->name();
                        new_row[m_columns.icon] = quiet_load_icon((*nodeIter)->factory().name(), Gtk::ICON_SIZE_MENU);
                        break;
                      }//if
                  }//for
              }//if
            }//for

          //Check If Material Added -> If So Update Panel Only If Group
          if(material_added)
            {
              Gtk::TreeModel::iterator iter = tree_selection->get_selected();
              if(iter) //If anything is selected
                {
                  Gtk::TreeModel::Row row = *iter;
                  if(row[m_columns.iMaterialGroup])
                    build_ContentPanel(row, true); //Build The Content Pane From Corrected Pointer
   
                }//if
            }//if


        }//on_nodes_added

        void Implementation::on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
        {
          //Flag For Possible Panel Update
          bool material_removed = false;

          for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){

            MaterialGroup *grpPtr = 0;
       
            //Check If Is A Material Type First
            if((*node)->factory().implements(typeid(k3d::imaterial)))
              {
                //Set Flag (Material Added) > Will Try To Update Panel
                material_removed = true;
              }

            Gtk::TreeIter row;
            return_if_fail(get_row(*node, row));
	    
            MaterialObj *tmpSObj = row->get_value(m_columns.MaterialObj_ptr);

            if(tmpSObj)
             
              grpPtr = tmpSObj->groupParent();
	      
            tree_model->erase(row);	
    

            //Delete In Stored Model
            if(grpPtr)
              grpPtr->removeMaterial(tmpSObj);
	    
          }//for

           //Rebuild Currently Selected Pane (Only If Group && Material Removed)
          if(material_removed)
            {
              Gtk::TreeModel::iterator iter = tree_selection->get_selected();
              if(iter) //If anything is selected
                {
                  Gtk::TreeModel::Row row = *iter;
                  if(row[m_columns.iMaterialGroup])
                    build_ContentPanel(row, true); //Build The Content Pane From Corrected Pointer
            		
                }//if
            }//if

        }//on_nodes_removed

        void Implementation::on_node_renamed(k3d::inode* const Node)
        {
          // Rename row
          Gtk::TreeIter row;
          return_if_fail(get_row(Node, row));
          (*row)[m_columns.name] = Node->name();

          //Rename In Stored Model
          (row->get_value(m_columns.MaterialObj_ptr))->setName(Node->name());

        }//on_node_renamed

        /// Looks-up a model row based on a node pointer
        bool Implementation::get_row(k3d::inode* const Node, Gtk::TreeIter& Row){
	
          Gtk::TreeNodeChildren rows = tree_model->children();

          //Iterate Through Each Row (Parent)
          for(Gtk::TreeIter row = rows.begin(); row != rows.end(); row++)
            {
              //  //Iterate Through Each Child Of Parent
              for(Gtk::TreeIter rowCIter = row->children().begin(); rowCIter != row->children().end(); rowCIter++)
                {
                  //Check If material Object and Not Group
                  if(rowCIter->get_value(m_columns.MaterialObj_ptr) && !(rowCIter->get_value(m_columns.iMaterialGroup))){
                    k3d::log() << "checking so" << std::endl;
                    if((rowCIter->get_value(m_columns.MaterialObj_ptr)->docNode()) == Node){
                      Row = rowCIter;
                      return true;
                    }//if
                  }//if
                }//for
            }//for

          return false;
        }//on_node_renamed

      //   bool Implementation::on_node_selection(k3d::inode* const Node)
//         {
//           //return result (if RMAN Material)
//           bool result = false;

//           //Check If Node Is A RenderMan Material
//           if((Node)->factory().implements(typeid(k3d::ri::imaterial))){

//             //It Is A RMAN Material!
//             result = true;

//             //Disconnect Any Existing Connection With Properties
//             m_pConnection.disconnect();

//             //Create Connection For Node Change
//             k3d::inode_change_signal *n_sig = dynamic_cast<k3d::inode_change_signal*>(Node);

//             if(n_sig)
//               m_pConnection =  n_sig->connect_node_changed_signal(sigc::mem_fun(*this, &Implementation::propertySignalRender));

//             //Change Tree Selection (which implicitly renders updated preview)

//           return result;

//         }//on_node_selection




        void Implementation::on_add_button_button_clicked()
        {
          k3d::log() << " Add Button Pressed" << std::endl;

          //Create & Embed New Group
          MaterialGroup *newGroup = new MaterialGroup("New Group");
          m_model->m_groups.push_back(newGroup);

          //Add New Group To The Tree
          Gtk::TreeRow row = *tree_model->append();
          row[m_columns.name] = newGroup->name();
          row[m_columns.iMaterialGroup] = true;
          row[m_columns.MaterialGroup_ptr] = newGroup;
          row[m_columns.MaterialObj_ptr] = 0;

        }//on_add_button_button_clicked



        void Implementation::on_remove_button_button_clicked()
        {

          //Get The Currently Selected Row (Group)
          Gtk::TreeModel::iterator iter = tree_selection->get_selected();
          if(iter) //If anything is selected
            {
              Gtk::TreeModel::Row row = *iter;

              //Is It A Group?
              if(row[m_columns.iMaterialGroup])
                {
		  
                  k3d::log() << " Remove Button Pressed... Will Delete Group (Eventually :) )" << std::endl;

                }//if
            }//if

        }//on_remove_button_button_clicked




        // [/Implementation]************************************************************************

      } // namespace mechanics

      // [panel]************************************************************************************
     
      class panel :
        public libk3dngui::panel::control,
        public libk3dngui::ui_component,
        public Gtk::VBox
      {
        //baseContainer is the preview & ctrl container
        typedef Gtk::VBox baseContainer;

      public:
        panel() :
          baseContainer(false, 0),
          m_implementation(0)
        {

        }

        ~panel()
        {
          delete m_implementation;
        }

        void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
        {
          k3d::command_tree().add(*this, "material_manager", &Parent);

          m_implementation = new mechanics::Implementation(DocumentState, Parent);

          pack_start(m_implementation->m_HPanedMain, Gtk::PACK_EXPAND_WIDGET);

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
          static k3d::application_plugin_factory<panel> 
            factory(
                    k3d::uuid(0xd363f420, 0x7240b35e, 0x7cf38788, 0xda06e8e6),
                    "materialManager",
                    _("Material Manager Panel"),
                    "NGUI Panels",
                    k3d::iplugin_factory::EXPERIMENTAL,
                    boost::assign::map_list_of("ngui:component-type", "panel")
                    ("ngui:panel-type", "material_manager")("ngui:panel-label", "Material Manager"));

          return factory;
        }

      private:
        mechanics::Implementation* m_implementation;
      };

      // [/panel]***********************************************************************************

    } // namespace material_manager

  } // namespace ngui

} // namespace module


//Register The Plugin (K-3D)************************************************************************
K3D_MODULE_START(Registry)
  Registry.register_factory(module::ngui::material_manager::panel::get_factory());
K3D_MODULE_END
//**************************************************************************************************
//0xd363f420, 0x7240b35e, 0x7cf38788, 0xda06e8e6
