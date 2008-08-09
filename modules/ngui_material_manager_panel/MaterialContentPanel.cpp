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


  //Create  A Preview Image In MaterialObj
  RenderedImage *m_preview_img
    = new RenderedImage(k3d::system::get_temp_directory() 
                        / k3d::filesystem::generic_path(m_single_imgfile));

  m_materialobj->setPviewImg(m_preview_img);


  //glib timer set that updates preview image every 0.25 seconds
  m_timer_connection 
    = Glib::signal_timeout()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::updatePreviewImage), 250);

  //Date Insert Connection
  m_date_button.signal_clicked()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::onDateButtonPressed));


  //Preview Geometry Combo Connection
  m_tool_geo_combo.signal_changed()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::onRenderComboSelect));


  //Background Button Button Connection
  m_tool_bg_button.signal_clicked()
    .connect(sigc::mem_fun(*this, &MaterialContentPanel::onBGButtonPressed));



}//init


void MaterialContentPanel::buildPanel()
{
  if(m_hpane)
    {
      //Set Off Renderer In New Process 
      //renderPreview();

      //Embed Everything In Master HBOX Padder
      m_master_pad_cont.pack_start(m_pview_editor_cont, true, true, 5);

      //Setup Render Preview Frame
      m_pview_frame.set_size_request(m_pview_size + 25, m_pview_size + 35);
      //m_pview_frame.add(m_material_preview);
      m_pview_frame.add(*(m_materialobj->pviewImg()));

      m_pview_cont.pack_start(m_pview_frame, false, false, 8);
 	      
      //Add ScrollWindowed Panel To Right Pane From Implementation
      m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
      m_scrolled_window.add(m_master_pad_cont);

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
      //tool_geo_combo.append_text("Cube");
      m_tool_geo_combo.append_text("Torus");
      //m_tool_geo_combo.set_active_text("Sphere");
      m_tool_geo_combo.set_size_request(-1, 30);

      //Setup Preview Background Button
      m_toolbox_cont.pack_start(m_tool_bg_button, false, false, 2);

      k3d::string_t bg_graphic_path 
        = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
           / k3d::filesystem::generic_path("mat_preview_bg_icon.xpm"))
        .native_filesystem_string();

      m_tool_bg_button.add_pixlabel(bg_graphic_path, "Bg");

      //**********************************************************
      
      //Setup Material Artist Notes (Lable & Entry)
      m_pview_editor_cont.pack_start(m_artnotes_frame, true, true, 10);
      m_artistnotes_cont.pack_start( m_artistnotes_mltext, true, true, 5);
      m_artnotes_frame.add(m_artistnotes_cont);

      //Gtk Build / Show Hint
      m_hpane->show_all(); 

      matobjAttachGeo();

      //Set Off Renderer In New Process 
      renderPreview();


    }
  else
    ; // Invalid HPanel Pointer	

}//buildPanel



void MaterialContentPanel::renderPreview()
{
  //Invoke Generic Render Initialization
  renderInit();

  //Set Alpha Render In Render Engine
  k3d::bool_t toggle_alpha = checkPviewBackground(m_materialobj);
  rEngineAlpha(toggle_alpha, m_materialobj);


  k3d::property::set_internal_value(*m_engine, 
                                    "visible_nodes", 
                                    k3d::inode_collection_property
                                    ::nodes_t(1, m_geometry));

  //Check If Selected Node Is A RenderMan Material
  if(m_materialobj->isMaterial())
    {  
      //If It Is, Assign To Current Geometry As A Surface Shader
      k3d::property
        ::set_internal_value(*m_geometry, 
                             "material", 
                             const_cast<k3d::inode*>(m_materialobj->docNode()));
	   

      //Render The Preview Using Selected External Renderer
      m_engine->render_camera_frame(*m_camera, 
                                    k3d::system::get_temp_directory() 
                                    / k3d::filesystem::generic_path(m_single_imgfile),
                                    false);

    }//if	 
 
  else
    {
      ; //Not A Renderman Material
    }
	    
}//renderPreview



bool MaterialContentPanel::updatePreviewImage()
{
  (m_materialobj->pviewImg())->queue_resize();
  (m_materialobj->pviewImg())->queue_draw();

  return true;
}	


void MaterialContentPanel::renderSinglePreview(k3d::inode *node)
{
  renderPreview();
}



void MaterialContentPanel::onRenderComboSelect()
{
  k3d::string_t selected_geo = m_tool_geo_combo.get_active_text();

  k3d::string_t meta_data = "";

  //Work Out Which Geometric Object Was Selected
  if(selected_geo == "Sphere")
    {
      meta_data = PreviewObj::sphere_md;

    }
  else if(selected_geo == "Cube")
    {
      meta_data = PreviewObj::cube_md;
    }
  else
    {
      meta_data = PreviewObj::torus_md;
    }


  //Meta Tag To Be Found / Added
  k3d::string_t meta_tag = PreviewObj::pview_geo_nametag_mt;

  //Node That Points To Actual Doc Node
  k3d::inode *node_ptr = 0;

  //Check If The Node Exists In The Document
  checkDocForMeta(meta_tag, meta_data, &node_ptr, m_document_state);


  if(node_ptr)
    {
      m_materialobj->setPreviewGeo(node_ptr, meta_data);
      m_geometry =  m_materialobj->m_preview_geo; //Node Found In Document!

      // m_geometry = node_ptr; //Node Found In Document!

    }
  else
    {
      //Build The Correct Geometric Object From Scratch
      PreviewObj *built_geo = 0;


      if(selected_geo == "Sphere")
        {
          built_geo = new PreviewSphere("Sphere", m_document_state);
          built_geo->init(PreviewObj::sphere_node_name, PreviewObj::sphere_md);
        }        

      else if(selected_geo == "Cube")
        {
        built_geo = new PreviewCube("Cube", m_document_state);
        built_geo->init(PreviewObj::cube_node_name, PreviewObj::cube_md);
        }

      else
        {
        built_geo = new PreviewTorus("Torus", m_document_state);
        built_geo->init(PreviewObj::torus_node_name, PreviewObj::torus_md);
        }
      
      if(built_geo)
        {
          m_materialobj->setPreviewGeo(built_geo->m_doc_node, meta_data);

          //Set The Current Geometric Object To This
          m_geometry =  m_materialobj->m_preview_geo;

          //m_geometry = built_geo->m_doc_node;

        }

    }//else

  //Render & Refresh Preview
  renderPreview();
  updatePreviewImage();

  k3d::log() << "Render Combo Changed" << std::endl;

}



void MaterialContentPanel::matobjAttachGeo()
{
  //Get The Doc Node From MaterialObj Ptr
  k3d::inode *mat_node = m_materialobj->m_doc_node;

  //Geometry Flags For Combo Update
  bool sphereUsed = false;
  bool cubeUsed = false;
  bool torusUsed = false;

  //Search m_doc_node Material For Attached Geometry
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(mat_node))
    {
      k3d::string_t value = metadata->get_metadata()[MaterialObj::attached_geo_nametag_mt];

      k3d::inode *attached_geo = 0; //Geometry Is Already Availible
      PreviewObj *attached_new_geo = 0; //Geometery Is New & Needs To Be Created

      k3d::string_t meta_attachedgeo = "";

      k3d::log() << "VALUE: " << value << std::endl;
          
      //Check If Sphere Attached
      if(value == PreviewObj::sphere_md)
        {
          //Check If Doesnt Exist In Document
          if(!checkDocForMeta(PreviewObj::pview_geo_nametag_mt, PreviewObj::sphere_md, &attached_geo, m_document_state))
            {
              //Create The Default Sphere For Preview
              attached_new_geo = new PreviewSphere("Sphere", m_document_state);
              attached_new_geo->init(PreviewObj::sphere_node_name, PreviewObj::sphere_md);
            }

          meta_attachedgeo = PreviewObj::sphere_md;
          sphereUsed = true;
        }

      //Check If Cube Attached
      else if(value == PreviewObj::cube_md)
        {
          //Check If Doesnt Exist In Document
          if(!checkDocForMeta(PreviewObj::pview_geo_nametag_mt, PreviewObj::cube_md, &attached_geo, m_document_state))
            {
              //Create The Default Cube For Preview
              attached_new_geo = new PreviewCube("Cube", m_document_state);
              attached_new_geo->init(PreviewObj::cube_node_name, PreviewObj::cube_md);
            }

          meta_attachedgeo = PreviewObj::cube_md;
          cubeUsed = true;

        }

      //Check If Torus Attached
      else if(value == PreviewObj::torus_md)
        {
          //Check If Doesnt Exist In Document
          if(!checkDocForMeta(PreviewObj::pview_geo_nametag_mt, PreviewObj::torus_md, &attached_geo, m_document_state))
            {

              k3d::log() << "TORUS NEW BUILT!" << std::endl;
              //Create The Default Torus For Preview
              attached_new_geo = new PreviewTorus("torus", m_document_state);
              attached_new_geo->init(PreviewObj::torus_node_name, PreviewObj::torus_md);
            }

          meta_attachedgeo = PreviewObj::torus_md;
          torusUsed = true;
        }

      else
        {
          //No Meta Data. Possibly Create & Attach Default Sphere
          //k3d::inode *existing_sphere = 0;
          if(!checkDocForMeta(PreviewObj::pview_geo_nametag_mt, PreviewObj::sphere_md, &attached_geo, m_document_state))
            {
              attached_new_geo =  new PreviewSphere("Sphere", m_document_state);
              attached_new_geo->init(PreviewObj::sphere_node_name, PreviewObj::sphere_md);

              m_materialobj->setPreviewGeo(attached_new_geo->m_doc_node, PreviewObj::sphere_md);

              k3d::log() << "no meta, sphere node created: " << attached_new_geo->m_doc_node << std::endl;

            }
          else
            {
              //Default Sphere Exists In Document
              if(attached_geo)
                {
                  m_materialobj->setPreviewGeo(attached_geo, PreviewObj::sphere_md);
                  k3d::log() << "no meta, sphere node Not created: " << attached_geo << std::endl;
                }

            }


          meta_attachedgeo = PreviewObj::sphere_md;
          sphereUsed = true;


        }


      //Attach The Geometry To The MaterialObj**********************************

      if(attached_geo)
        {
        m_materialobj->setPreviewGeo(attached_geo, meta_attachedgeo);
        k3d::log() << "attached geo: " << m_materialobj->m_preview_geo << std::endl;
        }

      else if(attached_new_geo)
        {
          m_materialobj->setPreviewGeo(attached_new_geo->m_doc_node, meta_attachedgeo);
          m_used_geometry.push_back(attached_new_geo);
          k3d::log() << "attached new geo: " << m_materialobj->m_preview_geo << std::endl;
        }


      //Update The Gui Combo
      if(sphereUsed)
        m_tool_geo_combo.set_active_text("Sphere");
      else if(cubeUsed)
        m_tool_geo_combo.set_active_text("Cube");
      else if(torusUsed)
        m_tool_geo_combo.set_active_text("Torus");



      //************************************************************************

          k3d::log() << "OI HERE BABY!" << std::endl;

    }//if



  m_geometry =  m_materialobj->m_preview_geo;

}//matobjAttachGeo




void MaterialContentPanel::onBGButtonPressed()
{
  if(checkPviewBackground(m_materialobj))
    {
      rEngineAlpha(false, m_materialobj);
    }
  else
    {
      rEngineAlpha(true, m_materialobj);
    }

  //Render & Refresh Preview
  renderPreview();
  updatePreviewImage();

  k3d::log() << "Change The Background!" << std::endl;

}//onBGButtonPressed



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
