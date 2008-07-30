#include "MaterialContentPanel.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

void MaterialContentPanel::init()
{
  m_name_label.set_text			("Material Name: ");
  m_type_label.set_text			("Material Type: ");
  m_datemod_label.set_text		("Date Modified: ");
  m_artistname_label.set_text	("Artist's Name: ");

  //glib timer set that updates preview image every 0.25 seconds
  m_timer_connection 
    = Glib::signal_timeout()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::updatePreviewImage), 250);

  //Date Insert Connection
  m_date_button.signal_clicked()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::onDateButtonPressed));

}//init


void MaterialContentPanel::buildPanel()
{
  if(m_hpane)
    {
      //Set Off Renderer In New Process 
      renderPreview();

      //Setup Render Preview Frame
      m_pview_frame.set_size_request(m_pview_size + 25, m_pview_size + 35);
      m_pview_frame.add(m_material_preview);
      m_pview_cont.pack_start(m_pview_frame, false, false, 8);
 	      
      //Add ScrollWindowed Panel To Right Pane From Implementation
      m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
      m_scrolled_window.add(m_pview_editor_cont);
      m_hpane->add2(m_scrolled_window);

      //Place Containers In Correct Hierachy
      m_pview_editor_cont.pack_start(m_pview_data_cont, false, false, 0); 
      m_pview_data_cont.pack_start(m_pview_cont, false, false, 0);
      m_pview_data_cont.pack_start(m_data_toolbar_cont, false, false, 0);
      m_data_toolbar_cont.pack_start(m_label_data_cont, true, true, 0);
      m_label_data_cont.pack_start(m_label_cont, true, true, 0);
      m_label_data_cont.pack_start(m_data_cont, true, true, 0);

      m_pview_data_cont.set_spacing(10);

      //Setup Material Name (Lable & Entry)
      m_name_label.set_alignment(0.0);
      m_label_cont.pack_start(m_name_label, true, true, 0);
      m_data_cont.pack_start(m_name_entry, true, true, 2);

      //Setup Material Type (Lable & Entry)
      m_type_label.set_alignment(0.0);
      m_label_cont.pack_start(m_type_label, true, true, 0);
      m_data_cont.pack_start(m_type_entry, true, true, 0);

      //Setup Material Date Modified (Lable & Entry)
      m_datemod_label.set_alignment(0.0);
      m_label_cont.pack_start(m_datemod_label, true, true, 0);

      //Date Button Button & Button Icon
      k3d::string_t date_graphic 
        = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
           / k3d::filesystem::generic_path("mat_preview_date_icon.xpm"))
        .native_filesystem_string();

      m_date_button.add_pixlabel(date_graphic, "Todays Date");
             
      //Embed The Date Entry & Button Container
      m_datebutton_cont.pack_start(m_datemod_entry, true, false, 0);
      m_datebutton_cont.pack_start(m_date_button, true, false, 0);
      m_data_cont.pack_start(m_datebutton_cont, true, false, 0);

      //Setup Material Artist Name (Lable & Entry)
      m_artistname_label.set_alignment(0.0);
      m_label_cont.pack_start(m_artistname_label, true, true, 0);
      m_data_cont.pack_start(m_artistname_entry, true, true, 0);	


      //ToolBox Setup********************************************

      //Setup ToolBox Frame & Internal Container
      m_toolbox_frame.set_size_request(-1, 48);
      m_data_toolbar_cont.pack_start(m_toolbox_frame, false, false, 8);
      m_toolbox_frame.add(m_toolbox_cont);
                
      //Setup Preview Geometry ComboBox
      m_toolbox_cont.pack_start(m_tool_geo_combo, false, false, 2);
      m_tool_geo_combo.append_text("Sphere");
      m_tool_geo_combo.append_text("Cube");
      m_tool_geo_combo.set_active_text("Sphere");
      m_tool_geo_combo.set_size_request(-1, 30);

      //Setup Preview Background Button
      m_toolbox_cont.pack_start(m_tool_gb_button, false, false, 2);

      k3d::string_t bg_graphic_path 
        = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
           / k3d::filesystem::generic_path("mat_preview_bg_icon.xpm"))
        .native_filesystem_string();

      m_tool_gb_button.add_pixlabel(bg_graphic_path, "BG");

      //**********************************************************
      
      //Setup Material Artist Notes (Lable & Entry)
      m_pview_editor_cont.pack_start(m_artnotes_frame, true, true, 10);
      m_artistnotes_cont.pack_start( m_artistnotes_mltext, true, true, 5);
      m_artnotes_frame.add(m_artistnotes_cont);

      //Gtk Build / Show Hint
      m_hpane->show_all(); 

    }
  else
    ; // Invalid HPanel Pointer	

}//buildPanel



void MaterialContentPanel::renderPreview()
{
  //Re-init The Preview Render Dimensions
  k3d::property::set_internal_value(*m_engine, 
                                    "pixel_width", 
                                    static_cast<k3d::int32_t>(m_pview_size));


  k3d::property::set_internal_value(*m_engine, 
                                    "pixel_height", 
                                    static_cast<k3d::int32_t>(m_pview_size));

  //Ensure Current Preview Engine Has Selected Nodes Only Visible
  k3d::inode_collection::nodes_t::const_iterator node 
    = m_document_state->document().nodes().collection().begin();

  for(; node != m_document_state->document().nodes().collection().end(); ++node)
    {
      if((*node)->factory().implements(typeid(k3d::ri::ilight)))
        {
          //Disable Node Regardless In RMANEngine::lights and nodes
          k3d::property::set_internal_value(*m_engine, 
                                            "enabled_lights", 
                                            k3d::inode_collection_property
                                            ::nodes_t(0, (*node)));
        }//if
      else if((*node)->factory().implements(typeid(k3d::itransform_sink)))
        {
          k3d::property::set_internal_value(*m_engine, 
                                            "visible_nodes", 
                                            k3d::inode_collection_property
                                            ::nodes_t(0, (*node)));
        }//else if
	    
    }//for

  //Vector List Of Lights To Be Enabled In Chosen Render Engine
  std::vector<k3d::inode*>lightsEnabled;
  lightsEnabled.push_back(m_main_light);
  lightsEnabled.push_back(m_fill_light);
  lightsEnabled.push_back(m_back_light);

  //Simply Enable Now Only USed Lights & Geo
  k3d::property::set_internal_value(*m_engine, 
                                    "enabled_lights", lightsEnabled);
 


  k3d::property::set_internal_value(*m_engine, 
                                    "visible_nodes", 
                                    k3d::inode_collection_property
                                    ::nodes_t(1, m_geometry));
  
  //Check If NodeIn sobject Is A RenderMan Material
  //if(m_materialobj->docNode()->factory().implements(typeid(k3d::ri::imaterial)))
  if(m_materialobj->isMaterial())
    {
      //If it is, assign to current geometry as surface shader
      k3d::property::set_internal_value(*m_geometry, 
                                        "material", 
                                        const_cast<k3d::inode*>(m_materialobj->docNode()));
	   

      //Render The Preview Using Selected External Renderer
      m_engine->render_camera_frame(*m_camera, k3d::system::get_temp_directory() 
                                    / k3d::filesystem::generic_path(m_single_imgfile),
                                    false);

    }//if	 
  else
    {
      k3d::log() << "Is NOT A Rman Material" << std::endl;
    }
	    
}//renderPreview




}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
