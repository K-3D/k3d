#include "ContentPanel.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

void ContentPanel::createPreviewNodes()
{
  //Flags For Each Node
  bool hasAqsis_renderer = 	false;
  bool hasCamera = 		false;
  bool hasGeo = 		false;
  bool hasLight = 		false;
  bool hasLightFill = 		false;
  bool hasLightBack = 		false;
  bool hasLight_shader = 	false;
  bool hasLightFill_shader = 	false;
  bool hasRenderman_engine = 	false;

  //Pointer To Aqsis Engine For RMAN Engine Node
  k3d::ri::irender_engine* aqsis = 0;
  k3d::string_t pointLightPath = "shaders/light/k3d_pointlight.sl";

  //Check Nodes MetaData To See If These Nodes Exist.
  //If They Do Dont Exist Then Create New Nodes

  k3d::inode_collection::nodes_t::const_iterator node 
    = m_document_state->document().nodes().collection().begin();

  for(; node != m_document_state->document().nodes().collection().end(); ++node)
    {
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(*node))
        {
          k3d::string_t value = metadata->get_metadata()["PreviewCore::nametag"];

          if(value == "p_aqsis_renderer")
            {
              hasAqsis_renderer = true;
              aqsis = dynamic_cast<k3d::ri::irender_engine*>(*node);
            }

          if(value == "p_camera")
            {
              hasCamera = true;
              m_camera = dynamic_cast<camera_t*>(*node);
            }

          if(value == "p_geo")
            {
              hasGeo = true;
              m_geometry = dynamic_cast<geo_t*>(*node);
            }

          if(value == "p_light")
            {
              hasLight = true;
              m_main_light = dynamic_cast<light_t*>(*node);
            }

          if(value == "p_fill_light")
            {
              hasLightFill = true;
              m_fill_light = dynamic_cast<light_t*>(*node);
            }

          if(value == "p_back_light")
            {
              hasLightBack = true;
              m_back_light = dynamic_cast<light_t*>(*node);
            }


          if(value == "p_light_shader")
            {
              hasLight_shader = true;
              m_light_shader = dynamic_cast<k3d::inode*>(*node);				
            }

          if(value == "p_fill_light_shader")
            {
              hasLightFill_shader = true;
              m_fill_light_shader = dynamic_cast<k3d::inode*>(*node);				
            }          

          if(value == "p_rman_engine")
            {
              hasRenderman_engine = true;
              m_engine = dynamic_cast<rManEngine_t*>(*node);					
            }		

        }//if
    }//for


  //Setup Light Shader Preview Node**********

  if(!hasLight_shader)
    {
      m_light_shader 
        = dynamic_cast<k3d::inode*>(k3d::plugin::create("RenderManLightShader", 
                                                        m_document_state->document(), 
                                                        "Preview Core::Light Shader"));

      k3d::property::set_internal_value(*m_light_shader, 
                                        "shader_path", k3d::share_path() /
                                        k3d::filesystem::generic_path(pointLightPath));


      k3d::property::set_internal_value(*m_light_shader, 
                                        "intensity", k3d::double_t(1200));
	      
      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_light_shader))
        metadata->set_metadata("PreviewCore::nametag", "p_light_shader");
   
    }//if


  if(!hasLightFill_shader)
    {
      m_fill_light_shader
        = dynamic_cast<k3d::inode*>(k3d::plugin::create("RenderManLightShader", 
                                                        m_document_state->document(), 
                                                        "Preview Core::Fill Light Shader"));

      k3d::property::set_internal_value(*m_fill_light_shader, 
                                        "shader_path", k3d::share_path() /
                                        k3d::filesystem::generic_path(pointLightPath));

      k3d::property::set_internal_value(*m_fill_light_shader, 
                                        "intensity", k3d::double_t(650));
	      
      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_fill_light_shader))
        metadata->set_metadata("PreviewCore::nametag", "p_fill_light_shader");
   
    }//if

  //Setup The Light**********

  if(!hasLight)
    {
      m_main_light 
        = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                     m_document_state->document(), 
                                                     "Preview Core::Light"));

      k3d::property::set_internal_value(*m_main_light, 
                                        "shader", m_light_shader);


      k3d::inode* light_transformation 
        = k3d::set_matrix(*m_main_light, 
                          k3d::translation3D(k3d::point3(-20, 20, 20)));
      

      k3d::property::set_internal_value(*m_main_light, 
                                        "viewport_visible", false);


      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_main_light))
        metadata->set_metadata("PreviewCore::nametag", "p_light");

    }//if


  if(!hasLightFill)
    {
      m_fill_light 
        = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                     m_document_state->document(), 
                                                     "Preview Core::Fill Light"));

      k3d::property::set_internal_value(*m_fill_light, 
                                        "shader", m_fill_light_shader);


      k3d::inode* light_transformation 
        = k3d::set_matrix(*m_fill_light, 
                          k3d::translation3D(k3d::point3(20, 28, -18)));


      k3d::property::set_internal_value(*m_fill_light, 
                                        "viewport_visible", false);


      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_fill_light))
        metadata->set_metadata("PreviewCore::nametag", "p_fill_light");

    }//if


  if(!hasLightBack)
    {
      m_back_light 
        = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                     m_document_state->document(), 
                                                     "Preview Core::Back Light"));

      k3d::property::set_internal_value(*m_back_light, 
                                        "shader", m_light_shader);


      k3d::inode* light_transformation 
        = k3d::set_matrix(*m_back_light, 
                          k3d::translation3D(k3d::point3(0, -38, 18)));


      k3d::property::set_internal_value(*m_back_light, 
                                        "viewport_visible", false);

      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_back_light))
        metadata->set_metadata("PreviewCore::nametag", "p_back_light");

    }//if


  //Setup The Camera**********
	  
  if(!hasCamera)
    {
      m_camera 
        = dynamic_cast<camera_t*>(k3d::plugin::create("Camera", 
                                                      m_document_state->document(), 
                                                      "Preview Core::Camera"));

      //Orientate The Camera In The World
      const k3d::point3 origin = k3d::point3(0, 0, 0);
      const k3d::vector3 world_up = k3d::vector3(0, 0, 1);
      const k3d::point3 position = k3d::point3(0, 13, 0);
      const k3d::vector3 look_vector = origin - position;
      const k3d::vector3 right_vector = look_vector ^ world_up;
      const k3d::vector3 up_vector = right_vector ^ look_vector;

      k3d::inode* const camera_transformation 
        = k3d::set_matrix(*m_camera, 
                          k3d::view_matrix(look_vector, up_vector, position));
       
      camera_transformation->set_name("Camera Transformation");

      k3d::property::set_internal_value(*m_camera, 
                                        "world_target", k3d::point3(0, 0, 0));

      k3d::property::set_internal_value(*m_camera, 
                                        "viewport_visible", false);

      k3d::property::set_internal_value(*m_camera, 
                                        "aspect_ratio", k3d::string_t("Square"));

      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_camera))
        metadata->set_metadata("PreviewCore::nametag", "p_camera");

    }//if

  //Setup The Geometry

  if(!hasGeo)
    {
      m_geometry 
        = dynamic_cast<geo_t*>(k3d::plugin::create("Sphere", 
                                                   m_document_state->document(), 
                                                   "Preview Core::Geo::Sphere"));

      k3d::property::set_internal_value(*m_geometry, 
                                        "render_shadows", false);

      k3d::property::set_internal_value(*m_geometry, 
                                        "viewport_visible", false);

      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_geometry))
        metadata->set_metadata("PreviewCore::nametag", "p_geo");

    }//if

  //Setup The Aqsis Renderman Engine**********

  if(!hasAqsis_renderer)
    {
      aqsis 
        = k3d::plugin::create<k3d::ri::irender_engine>("AqsisRenderManEngine", 
                                                       m_document_state->document(), 
                                                       "Preview Core::Aqsis Renderer");

      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(aqsis))
        metadata->set_metadata("PreviewCore::nametag", "p_aqsis_renderer");

    }//if


  //Setup The Renderman Engine

  if(!hasRenderman_engine)
    {
      m_engine 
        = dynamic_cast<rManEngine_t*>(k3d::plugin::create("RenderManEngine", 
                                                         m_document_state->document(), 
                                                          "Preview Core::RenderManEngine"));

      k3d::property::set_internal_value(*m_engine, 
                                        "enabled_lights", 
                                        k3d::inode_collection_property
                                        ::nodes_t(1, m_main_light));

      k3d::property::set_internal_value(*m_engine, 
                                        "visible_nodes", 
                                        k3d::inode_collection_property
                                        ::nodes_t(1, m_geometry));


      //Assign Aqsis To The Chosen RenderEngine Slot

      k3d::property::set_internal_value(*m_engine, 
                                        "render_engine", 
                                        dynamic_cast<k3d::inode*>(aqsis));

      //Setup Preview Render Dimensions

      k3d::property::set_internal_value(*m_engine, 
                                        "pixel_width", 
                                        static_cast<k3d::int32_t>(m_pview_size));


      k3d::property::set_internal_value(*m_engine, 
                                        "pixel_height", 
                                        static_cast<k3d::int32_t>(m_pview_size));

      k3d::double_t aspectRatio = 1.0;

      k3d::property::set_internal_value(*m_engine, 
                                        "pixel_aspect_ratio", aspectRatio);


      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_engine))
        metadata->set_metadata("PreviewCore::nametag", "p_rman_engine");

    }//if

}//createPreviewNodes



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
