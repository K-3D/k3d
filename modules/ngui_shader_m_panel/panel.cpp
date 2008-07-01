// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// Shader Manager Panel Developed By Alex Curtis
// Contact: alx.curtis@gmail.com
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

#include <gtkmm.h>

#include <k3d-i18n-config.h>
		    
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtk/gtkmain.h>

#include <gtkmm/textview.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/toggletoolbutton.h>
#include <gtkmm/menutoolbutton.h>

#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/text.h>

#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/command_arguments.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/utility.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/classes.h>

#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/module.h>
#include <k3dsdk/nodes.h>

#include <boost/assign/list_of.hpp>
#include <k3dsdk/types.h>

#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/imaterial_ri.h>

#include <k3dsdk/metadata.h>

#include <gtkmm/drawingarea.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/system.h>

#include <k3dsdk/log.h>
#include <iostream>

#include <k3dsdk/ngui/icons.h>

using namespace libk3dngui;
using namespace k3d::data;

namespace module{

  namespace ngui{

    namespace shader_manager{

      namespace mechanics{

	// [model Components]***********************************************************************

	//Static Material Type Definitions
	const std::string riMaterialStr 	= "Renderman Materials";
	const std::string glMaterialStr 	= "OpenGL Materials";
	const std::string otherStuffStr 	= "Other Stuff";	
       
	//forward declaration for list of shaders
	class s_group;

	class s_object{
	public:
	  s_object(s_group *_parent, k3d::inode *_node, k3d::string_t _s_type)
	    :parent(_parent), node(_node), 
	     so_name(init_value(_node->name())),
	     so_type(init_value(_s_type)),
	     so_datestamp(init_value(k3d::string_t(""))),
	     so_artistname(init_value(k3d::string_t(""))),
	     so_artistnotes(init_value(k3d::string_t("")))
	  {
	    so_artistnotes.set_metadata("k3d:property-type", "k3d:multi-line-text");
	  }
	  
	  ~s_object()
	  {
	  }

	public:
	  //Accessor Functions
	  k3d::string_t getName(){return so_name.internal_value();}

	  //Mutator Functions
	  void setName(k3d::string_t str){so_name.set_value(str);}


	public:
	  k3d_data(k3d::string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) so_name;
	  k3d_data(k3d::string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) so_type;
	  k3d_data(k3d::string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) so_datestamp;
	  k3d_data(k3d::string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) so_artistname;
	  k3d::metadata_property<k3d_data(k3d::string_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization)> so_artistnotes;
	  k3d::inode       *node;
	  s_group          *parent;

	};//s_object

	//************
	class s_group{
	public:
	  s_group(k3d::string_t _sg_name)
	    :sg_name(_sg_name)
	  {
	  }

	  ~s_group()
	  {
	  }

	public:
	  void clearGroup();
	  void addShader(s_object* shd);
	  void removeShader(s_object* shd);

	public:
	  std::list<s_object*>     m_shaders; //children
	  k3d::string_t            sg_name;

	};//s_group

	void s_group::clearGroup()
	{
	  std::list<s_object*>::iterator shaderIter = m_shaders.begin();
	  for(; shaderIter != m_shaders.end(); shaderIter++)
	    {
	      //Clean Up The List Of Shader Objects (s_object)
	      delete (*shaderIter);
	      m_shaders.erase(shaderIter);

	    }//for
	}

	void s_group::addShader(s_object* shd)
	{
	  m_shaders.push_back(shd);
	}

	void s_group::removeShader(s_object* shd)
	{
	  m_shaders.remove(shd);
	}

	//***********************************************
	  //Constant Groups -> These Wont Be Deleted
	s_group *rman = new s_group(riMaterialStr);
	s_group *gl = new s_group(glMaterialStr);
	s_group *other = new s_group(otherStuffStr);
	//***********************************************

	  //************
	  class model{
	  public:
	    model()
	    {
	    }
	    ~model()
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
	    std::list<s_group*>     m_groups;

	  };//m_model

	void model::buildModel(document_state& DocumentState)
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
		    rman->addShader(new s_object(rman, (*nodeIter), riMaterialStr));

		  else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial)))
		    gl->addShader(new s_object(gl, (*nodeIter), glMaterialStr));

		  else
		    other->addShader(new s_object(other, (*nodeIter), otherStuffStr));

		}//if
	    }//for

	}

	void model::clearModel()
	{
	  std::list<s_group*>::iterator groupIter = m_groups.begin();
	  for(; groupIter != m_groups.end(); groupIter++)
	    {
	      (*groupIter)->clearGroup();
	      delete (*groupIter);
	      m_groups.erase(groupIter);
	    }//for

	}


	// [/model Components]**********************************************************************

	// [implementation]*************************************************************************

	//Forward declarations of Gui Objects to make life easier
	//class implementation;

	//************ GUI CONTENT PANE CLASSES
	
	class content_pane{
	public:
	  content_pane(Gtk::HPaned *_m_Hpane, k3d::icommand_node *_m_parent)
	    :m_Hpane(_m_Hpane), m_parent(_m_parent), preview_primitive("Preview Primitive")	  
	  {
	  }
	  virtual ~content_pane(){}

	public:
	  virtual void buildPane() = 0;

	protected:
	  Gtk::HPaned *m_Hpane;
	  k3d::icommand_node *m_parent;

	  Gtk::MenuToolButton preview_primitive;

	};//content_pane

	//GROUP CONTENT PANE
	class g_content_pane : public content_pane{
	public:
	  g_content_pane(Gtk::HPaned *_m_Hpane, s_group *_m_grp, k3d::icommand_node *_m_parent)
	    :content_pane(_m_Hpane, _m_parent), m_grp(_m_grp),
	     group_display("List Settings")
	  {
	    
	  }

	  ~g_content_pane(){}

	public:
	  void buildPane()
	  {
	    if(m_Hpane)
	      {
		//Add Container To Right Pane From Implementation
		m_Hpane->add2(toolbar_main_c);

		//ToolBar Setup
		toolbar_main_c.pack_start(m_toolbar, false, false, 0);
		m_toolbar.append(group_display);
		m_toolbar.append(preview_primitive);

		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		toolbar_main_c.pack_start(m_scrolled_window, true, true, 0);

		m_scrolled_window.add(shaderBoxes_c);

		//Setup ToolBar
		shaderBoxes_c.pack_start(m_toolbar, false, false, 0);

		//For Each Of The Elements In Group->m_shaders Create A GUI Strip
		std::list<s_object*>::iterator soIter = m_grp->m_shaders.begin();
		for(; soIter != m_grp->m_shaders.end(); soIter++)
		  {
		    Gtk::HBox *tmpHBox = new Gtk::HBox;
		    object_preview_data_c.push_back(tmpHBox);
		    shaderBoxes_c.pack_start(*tmpHBox, false, false, 10);

		    Gtk::Frame *tmpFrame = new Gtk::Frame("PREVIEW");
		    img_holder_frames.push_back(tmpFrame);
		    tmpFrame->set_size_request(200, 200);
		    tmpHBox->pack_start(*tmpFrame, false, false, 5);

		    //Data & Notes VBox
		    Gtk::VBox *tmpVBox_dd = new Gtk::VBox;
		    shader_data_desc_c.push_back(tmpVBox_dd);
		    tmpHBox->pack_start(*tmpVBox_dd, true, true, 5); //HERE

		    //Data HBox (Contains x2 VBox -> Label and Data)
		    Gtk::HBox *tmpHBox_ld = new Gtk::HBox;
		    shader_label_data_c.push_back(tmpHBox_ld);
		    tmpVBox_dd->pack_start(*tmpHBox_ld, false, false, 5);
		    
		    //Label VBox
		    Gtk::VBox *tmpVBox_l = new Gtk::VBox;
		    shader_label_c.push_back(tmpVBox_l);
		    tmpHBox_ld->pack_start(*tmpVBox_l, false, false, 0);

		    tmpVBox_l->set_spacing(4);

		    //BUILD LABEL STUFF

		    Gtk::Label *tmpName_l = new Gtk::Label("Shader Name: ");
		    Gtk::Label *tmpType_l = new Gtk::Label("Shader Type: ");
		    Gtk::Label *tmpdateMod_l = new Gtk::Label("Date Modified: ");
		    Gtk::Label *tmpArtistName_l = new Gtk::Label("Artist's Name: ");

		    s_name_l.push_back(tmpName_l);
		    s_type_l.push_back(tmpType_l);
		    s_datemod_l.push_back(tmpdateMod_l);
		    s_artistname_l.push_back(tmpArtistName_l);

		    tmpVBox_l->pack_start(*tmpName_l, false, false, 0);
		    tmpVBox_l->pack_start(*tmpType_l, false, false, 0);
		    tmpVBox_l->pack_start(*tmpdateMod_l, false, false, 0);
		    tmpVBox_l->pack_start(*tmpArtistName_l, false, false, 0);

		    tmpName_l->set_alignment(0.0);

		    tmpType_l->set_alignment(0.0);
		    tmpdateMod_l->set_alignment(0.0);
		    tmpArtistName_l->set_alignment(0.0);

		    //Data VBox
		    Gtk::VBox *tmpVBox_d = new Gtk::VBox;
		    shader_data_c.push_back(tmpVBox_d);
		    tmpHBox_ld->pack_start(*tmpVBox_d, false, false, 0);
		    tmpVBox_d->set_spacing(4);

		    //BUILD DATA (READ ONLY) STUFF

		    Gtk::Label *tmpName_d = new Gtk::Label((*soIter)->so_name.internal_value());
		    Gtk::Label *tmpType_d = new Gtk::Label((*soIter)->so_type.internal_value());
		    Gtk::Label *tmpdateMod_d = new Gtk::Label((*soIter)->so_datestamp.internal_value());
		    Gtk::Label *tmpArtistName_d = new Gtk::Label((*soIter)->so_artistname.internal_value());

		    s_name_d.push_back(tmpName_d);
		    s_type_d.push_back(tmpType_d);
		    s_datemod_d.push_back(tmpdateMod_d);
		    s_artistname_d.push_back(tmpArtistName_d);

		    tmpVBox_d->pack_start(*tmpName_d, false, false, 0);
		    tmpVBox_d->pack_start(*tmpType_d, false, false, 0);
		    tmpVBox_d->pack_start(*tmpdateMod_d, false, false, 0);
		    tmpVBox_d->pack_start(*tmpArtistName_d, false, false, 0);

		    tmpName_d->set_alignment(0.0);
		    tmpType_d->set_alignment(0.0);
		    tmpdateMod_d->set_alignment(0.0);
		    tmpArtistName_d->set_alignment(0.0);

		    //Artist Notes Build

		    Glib::RefPtr<Gtk::TextBuffer> txtDisplay = Gtk::TextBuffer::create();
		    txtDisplay->set_text((*soIter)->so_artistnotes.internal_value());

		    Gtk::TextView *tmpNotesView = new Gtk::TextView(txtDisplay);
		    tmpNotesView->set_editable(false);	    
		    s_artistnotes_view.push_back(tmpNotesView);

		    Gtk::ScrolledWindow *tmpNotesScrollWin = new Gtk::ScrolledWindow;
		    notes_scroll_win.push_back(tmpNotesScrollWin);
		    tmpNotesScrollWin->add(*tmpNotesView);

		    tmpVBox_dd->pack_start(*tmpNotesScrollWin, true, true, 0);

		    //Add Horizontal Breaker At Bottom
		    Gtk::HSeparator *tmpHBreaker = new Gtk::HSeparator();
		    s_breakers.push_back(tmpHBreaker);
		    shaderBoxes_c.pack_start(*tmpHBreaker, false, false, 0);


		  }//for
	      }
	    else
	      k3d::log() << "Invalid HPaned Pointer" << std::endl;  

	    //DEBUG INFO -> DELETE ON RELEASE
	    k3d::log() << "building Group panel" << std::endl;
	    m_Hpane->show_all(); 

	  }//buildPane

	public:
	  //GTK Widgets
	  Gtk::ScrolledWindow m_scrolled_window;

	  Gtk::VBox shaderBoxes_c;
	  std::vector<Gtk::HBox *> object_preview_data_c;
	  std::vector<Gtk::Frame *> img_holder_frames;
	  std::vector<Gtk::VBox *> shader_data_desc_c;
	  std::vector<Gtk::HBox *> shader_label_data_c;
	  std::vector<Gtk::VBox *> shader_label_c;
	  std::vector<Gtk::VBox *> shader_data_c;

	  std::vector<Gtk::Label*> s_name_l;
	  std::vector<Gtk::Label*> s_type_l;
	  std::vector<Gtk::Label*> s_datemod_l;
	  std::vector<Gtk::Label*> s_artistname_l;

	  std::vector<Gtk::Label*> s_name_d;
	  std::vector<Gtk::Label*> s_type_d;
	  std::vector<Gtk::Label*> s_datemod_d;
	  std::vector<Gtk::Label*> s_artistname_d;

	  std::vector<Gtk::TextView*> s_artistnotes_view;
	  std::vector<Gtk::ScrolledWindow*> notes_scroll_win;

	  std::vector<Gtk::HSeparator*> s_breakers;

	  Gtk::HBox s_name_c;
	  Gtk::HBox s_type_c;
	  Gtk::HBox s_datemod_c;
	  Gtk::HBox s_artistname_c;

	  Gtk::VBox toolbar_main_c;

	  //Toolbar Contents
	  Gtk::Toolbar m_toolbar; 
	  Gtk::MenuToolButton group_display;

	  

	private:
	  s_group *m_grp;

	};//g_content_pane

	//SHADER OBJECT CONTENT PANE
	class so_content_pane : public content_pane{
	public:
	  so_content_pane(Gtk::HPaned *_m_Hpane, s_object *_m_so, k3d::icommand_node *_m_parent)
	    :content_pane(_m_Hpane, _m_parent), m_so(_m_so), so_preview_frame("PREVIEW"), artistnotes_frame("ARTIST NOTES"),
	     s_name_entry(*_m_parent, k3d::string_t("so_name_field"), entry::model(_m_so->so_name), 0),
	     s_type_entry(*_m_parent, k3d::string_t("so_type_field"), entry::model(_m_so->so_type), 0),
	     s_datemod_entry(*_m_parent, k3d::string_t("so_datestamp_field"), entry::model(_m_so->so_datestamp), 0),
	     s_artistname_entry(*_m_parent, k3d::string_t("so_artistname_field"), entry::model(_m_so->so_artistname), 0),
	     s_artistnotes_mltext(*_m_parent, k3d::string_t("so_artistnotes_mltxt"), text::model(_m_so->so_artistnotes), 0)
	  {
	    s_name_l.set_text		("Shader Name: ");
	    s_type_l.set_text		("Shader Type: ");
	    s_datemod_l.set_text	("Date Modified: ");
	    s_artistname_l.set_text	("Artist's Name: ");
	  }

	  ~so_content_pane(){}

	public:
	  void buildPane()
	  {
	    if(m_Hpane)
	      {
		so_preview_frame.set_size_request(200, 200);
		preview_c.pack_start(so_preview_frame, false, false, 10);
 	      
		//Add Container To Right Pane From Implementation
		m_Hpane->add2(main_detail_c);

		main_detail_c.pack_start(preview_label_data_c, false, false, 0); 

		preview_label_data_c.pack_start(preview_c, false, false, 0);
		preview_label_data_c.pack_start(label_c, false, false, 0);
		preview_label_data_c.pack_start(data_c, false, false, 0);

		preview_label_data_c.set_spacing(10);

		//Shader Name
		s_name_l.set_alignment(0.0);
		label_c.pack_start(s_name_l, true, true, 0);
		data_c.pack_start(s_name_entry, true, true, 0);

		//Shader Type
		s_type_l.set_alignment(0.0);
		label_c.pack_start(s_type_l, true, true, 0);
		data_c.pack_start(s_type_entry, true, true, 0);

		//Date Modified
		s_datemod_l.set_alignment(0.0);
		label_c.pack_start(s_datemod_l, true, true, 0);
		data_c.pack_start(s_datemod_entry, true, true, 0);

		//Artist Name
		s_artistname_l.set_alignment(0.0);
		label_c.pack_start(s_artistname_l, true, true, 0);
		data_c.pack_start(s_artistname_entry, true, true, 0);

		//Artist Notes
		main_detail_c.pack_start(artistnotes_frame, true, true, 10);
		artistnotes_frame.add(s_artistnotes_mltext);

		//INSERT K3D TEXT VIEW HERE FOR ARTIST NOTES
	       
		//DEBUG INFO -> DELETE ON RELEASE
		k3d::log() << "building shader panel" << std::endl;
		m_Hpane->show_all(); 
	      }
	    else
	      k3d::log() << "Invalid HPaned Pointer" << std::endl;  	

	  }//buildPane
	public:
	  //GTK Widgets
	  Gtk::Label s_name_l;
	  Gtk::Label s_type_l;
	  Gtk::Label s_datemod_l;
	  Gtk::Label s_artistname_l;

	  //Gtk::Entry s_name_entry;
	  Gtk::VBox main_detail_c;
	  Gtk::HBox preview_label_data_c;
	  Gtk::VBox label_c;
	  Gtk::VBox data_c;
	  Gtk::VBox preview_c;

	  Gtk::HBox s_name_c;
	  Gtk::HBox s_type_c;
	  Gtk::HBox s_datemod_c;
	  Gtk::HBox s_artistname_c;

	  Gtk::Frame so_preview_frame;
	  Gtk::Frame artistnotes_frame;

	  entry::control s_name_entry;
	  entry::control s_type_entry;
	  entry::control s_datemod_entry;
	  entry::control s_artistname_entry;

	  text::control s_artistnotes_mltext;

	private:
	  s_object *m_so;

	};//so_content_pane

	//************MAIN IMPLEMENTATION
	class implementation : public libk3dngui::asynchronous_update{
	public:
	  implementation(document_state& DocumentState, k3d::icommand_node& Parent) 
	    :m_document_state(DocumentState),
	     m_model(new model),
	     m_parent(&Parent),
	     add_group("Add"),
	     remove_group("Remove")
	  {
	    //Create The Shader Tree Model
	    tree_model = Gtk::TreeStore::create(m_columns);
	    m_nav.set_model(tree_model);
	    tree_selection = m_nav.get_selection();

	    // Connect signals
	    //m_nav.signal_button_press_event().connect(sigc::mem_fun(*this, &implementation::on_button_press_event), false);
	    //m_nav.get_selection()->set_select_function(sigc::mem_fun(*this, &implementation::on_select_row));

	    //m_nav.get_selection()->signal_changed().connect(sigc::mem_fun(*this, &implementation::on_selection_changed)); 
	
	    tree_selection->signal_changed().connect(sigc::mem_fun(*this,  &implementation::on_tree_row_changed));

	    // Reset the control contents anytime the document node collection changes
	    m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
	    m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
	    m_document_state.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &implementation::on_node_renamed));


	    build_gui();
	    schedule_update();
	  }

	  ~implementation()
	  {	      
	  }

	public:
	  // Updates the contents of the control
          void on_update();
	  // Build The Interface
	  void build_gui();
	  void build_tree();
	  void build_content_pane(Gtk::TreeModel::Row row, bool is_group);

	  //Event Handlers (Tree)
	  void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes) ;
          void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes);
          void on_node_renamed(k3d::inode* const Node);

	  void on_tree_row_changed();

	  bool get_row(k3d::inode* const Node, Gtk::TreeIter& Row);
	
	private:
	  k3d::icommand_node *m_parent;

	public:
	  //GTK Widgets
          Gtk::ScrolledWindow m_scrolled_window;
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
	  std::auto_ptr<model> m_model;

	  //The Right Content Pane For The Panel
	  std::auto_ptr<content_pane> m_rpane_content;

	  class columns :
	    public Gtk::TreeModel::ColumnRecord
	  {
	  public:
	    columns()
	    {
	      add(name);
	      add(icon);
	      add(is_group);
	      add(s_group_ptr);
	      add(s_object_ptr);
	    }
	    Gtk::TreeModelColumn<s_group*> 		s_group_ptr;
	    Gtk::TreeModelColumn<s_object*> 		s_object_ptr;
	    Gtk::TreeModelColumn<Glib::ustring> 	name;
	    Gtk::TreeModelColumn<bool>		    	is_group;	//Group Or Object Pointer Used
	    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
	  };

	  columns m_columns;
	  Glib::RefPtr<Gtk::TreeStore> tree_model;
	  Glib::RefPtr<Gtk::TreeSelection> tree_selection;


	};//implementation


	void implementation::on_update()
	{
	  //Clear The Tree Model
	  tree_model->clear();

	  //Delete Model Data
	  m_model->clearModel();

	  //Rebuild Model Data
	  m_model->buildModel(m_document_state);

	  //Rebuild Tree From Rebuilt Model
	  build_tree();

	}//on_update

	void implementation::build_gui()
	{
	  //Shader Tree Setup
	  m_nav.set_headers_visible(false);
	  m_nav.set_reorderable(false);

	  Gtk::CellRendererText* const cell_text = new hotkey_cell_renderer_text();
	  cell_text->property_editable() = true;
	  //cell_text->signal_edited().connect(sigc::mem_fun(*this, &implementation::on_node_name_edited));

	  Gtk::TreeViewColumn* const name_column = new Gtk::TreeViewColumn;
	  name_column->pack_start(*manage(cell_text), true);
	  name_column->add_attribute(cell_text->property_text(), m_columns.name);

	  m_nav.append_column("icon", m_columns.icon);
	  m_nav.append_column(*manage(name_column));

	  //Setup the Window
	  m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   
	  //Embed The HPanel Container Widget
	  m_scrolled_window.add(m_HPanedMain);

	  m_HPanedMain.add1(toolbar_tree_c);

	  //Create Tree Toolbar
	  m_toolbar.append(add_group);
	  m_toolbar.append(remove_group);

	  toolbar_tree_c.pack_start(m_toolbar, false, false, 0);


	  //Pack Tree
	  toolbar_tree_c.pack_start(m_nav, true, true, 0);
	  
	  
	}//build_gui

	void implementation::build_tree()
	{
	  //Gro Through Groups And Add To Tree
	  std::list<s_group*>::const_iterator gIter = m_model->m_groups.begin();
	  for(; gIter !=  m_model->m_groups.end(); gIter++)
	    {
	      Gtk::TreeRow row = *tree_model->append();
	      row[m_columns.name] = (*gIter)->sg_name;
	      row[m_columns.is_group] = true;
	      row[m_columns.s_group_ptr] = (*gIter);
	      row[m_columns.s_object_ptr] = 0;

	      //Go Through Shaders/Other and and to tree under groups
	      std::list<s_object*>::const_iterator sIter = (*gIter)->m_shaders.begin();
	      for(; sIter !=  (*gIter)->m_shaders.end(); sIter++)
		{
		  Gtk::TreeModel::Row childrow = *(tree_model->append(row.children()));
		  childrow[m_columns.name] = (*sIter)->getName();
		  childrow[m_columns.is_group] = false;
		  childrow[m_columns.s_group_ptr] = 0;
		  childrow[m_columns.s_object_ptr] = (*sIter);
		  childrow[m_columns.icon] = quiet_load_icon((*sIter)->node->factory().name(), Gtk::ICON_SIZE_MENU);
		}//for		
	    }//for

	}//build_tree

	//Called When Row Changed 
	void implementation::on_tree_row_changed()
	{
	  //Get The Current Row Selection
	  Gtk::TreeModel::iterator iter = tree_selection->get_selected();
	  if(iter) //If anything is selected
	    {
	      Gtk::TreeModel::Row row = *iter;
	      if(row[m_columns.is_group])
		{
		  k3d::log() << row->get_value(m_columns.s_group_ptr)->sg_name << std::endl;
		  build_content_pane(row, true);
		}
	      else
		{
		  build_content_pane(row, false);
		}
 
	    }//if

	}//on_tree_row_changed

	//Build the right Content Pane (GUI Stuff)
	void implementation::build_content_pane(Gtk::TreeModel::Row row, bool is_group)
	{
	  //Check If Building Group Panel Or Shader Object Panel
	  if(is_group)
	    {
	      //Delete current obj (group or shader gui) and create new group gui obj
	      m_rpane_content = std::auto_ptr<content_pane>(new g_content_pane(&m_HPanedMain, row->get_value(m_columns.s_group_ptr), m_parent));
	    }
	  else
	    {
	      //Delete current obj (group or shader gui) and create new group gui obj
	      m_rpane_content = std::auto_ptr<content_pane>(new so_content_pane(&m_HPanedMain, row->get_value(m_columns.s_object_ptr), m_parent));

	    }//endif

	  //Build The Content Pane From Corrected Pointer
	  m_rpane_content->buildPane();

	}//build_content_pane

	void implementation::on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
	  //Iterate through each node and if appropriate add to tree
	  for(k3d::inode_collection::nodes_t::const_iterator nodeIter = Nodes.begin(); nodeIter != Nodes.end(); ++nodeIter)
	    {
	      //Check If Is A Material Type First
	      if((*nodeIter)->factory().implements(typeid(k3d::imaterial))){

		Gtk::TreeNodeChildren rows = tree_model->children();
		Gtk::TreeRow new_row;
		Gtk::TreeIter row = rows.begin();

		k3d::string_t typeStr = "";
		s_group *groupPtr = 0;

		//Place Renderman Material
		if((*nodeIter)->factory().implements(typeid(k3d::ri::imaterial))){
		  typeStr = riMaterialStr;
		  groupPtr = rman;
		}
		//Place GL Material
		else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial))){
		  typeStr = glMaterialStr;
		  groupPtr = gl;
		}
		//Place Other Material
		else{
		  typeStr = otherStuffStr;
		  groupPtr = other;
		}
		for(; row != rows.end(); ++row)
		  {
		    if(row->get_value(m_columns.name) == typeStr)
		      {
			//Create Shader Object
			s_object *newSObject = new s_object(groupPtr, *nodeIter, typeStr);

			//Push The Shader Onto Groups List
			groupPtr->addShader(newSObject);

			//Create The Tree Entry
			new_row = *tree_model->append(row->children());
			new_row[m_columns.s_object_ptr] = newSObject;
			new_row[m_columns.s_group_ptr] = 0;
			new_row[m_columns.is_group] = false;
			new_row[m_columns.name] = (*nodeIter)->name();
			new_row[m_columns.icon] = quiet_load_icon((*nodeIter)->factory().name(), Gtk::ICON_SIZE_MENU);
			break;
		      }//if
		  }//for
	      }//if
	    }//for

	}//on_nodes_added

	void implementation::on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
	  s_group *grpPtr = 0;
	  s_object *tmpSObj = 0;
	  for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){
	    Gtk::TreeIter row;
	    return_if_fail(get_row(*node, row));
	    
	    tmpSObj = row->get_value(m_columns.s_object_ptr);

	    if(tmpSObj)
		grpPtr = tmpSObj->parent;
	      
	    tree_model->erase(row);
	  }

	  //Delete In Stored Model
	  if(grpPtr)
	      grpPtr->removeShader(tmpSObj);
	    

	}//on_nodes_removed

	void implementation::on_node_renamed(k3d::inode* const Node)
	{
	  // Rename row
	  Gtk::TreeIter row;
	  return_if_fail(get_row(Node, row));
	  (*row)[m_columns.name] = Node->name();

	  //Rename In Stored Model
	  (row->get_value(m_columns.s_object_ptr))->setName(Node->name());

	}//on_node_renamed

	/// Looks-up a model row based on a node pointer
	bool implementation::get_row(k3d::inode* const Node, Gtk::TreeIter& Row){
	
		Gtk::TreeNodeChildren rows = tree_model->children();

		//Iterate Through Each Row (Parent)
		for(Gtk::TreeIter row = rows.begin(); row != rows.end(); row++)
		  {
		   //  //Iterate Through Each Child Of Parent
		    for(Gtk::TreeIter rowCIter = row->children().begin(); rowCIter != row->children().end(); rowCIter++)
		      {
			//Check If Shader Object and Not Group
			if(rowCIter->get_value(m_columns.s_object_ptr) && !(rowCIter->get_value(m_columns.is_group))){
			  k3d::log() << "checking so" << std::endl;
			  if((rowCIter->get_value(m_columns.s_object_ptr)->node) == Node){
			    Row = rowCIter;
			    return true;
			  }//if
			}//if
		      }//for
		}//for

		return false;
	}



	// [/implementation]************************************************************************

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
          ui_component("shader_manager", 0)//,
	     //m_implementation(0)
	{
	}

	~panel()
	{
          //delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
          ui_component::set_parent("shader_manager", &Parent);

          m_implementation = new mechanics::implementation(DocumentState, Parent);

          pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

          show_all();

	}

	const k3d::string_t panel_type()
	{
          return "shader_manager";
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
                    "shaderManager",
                    _("Shader Manager Panel"),
                    "NGUI Panels",
                    k3d::iplugin_factory::EXPERIMENTAL,
                    boost::assign::map_list_of("ngui:component-type", "panel")
                    ("ngui:panel-type", "shader_manager")("ngui:panel-label", "Shader Manager"));

          return factory;
	}

      private:
	mechanics::implementation* m_implementation;
      };

      // [/panel]***********************************************************************************

    } // namespace shader_manager

  } // namespace ngui

} // namespace module


//Register The Plugin (K-3D)************************************************************************
K3D_MODULE_START(Registry)
  Registry.register_factory(module::ngui::shader_manager::panel::get_factory());
K3D_MODULE_END
//**************************************************************************************************
//0xd363f420, 0x7240b35e, 0x7cf38788, 0xda06e8e6
