#include "ContentPanel.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

bool ContentPanel::checkDocForMeta(const k3d::string_t meta_tag, 
                                   const k3d::string_t meta_data, 
                                   k3d::inode **node_ptr)
{
  //Iterate Through Document. If A Node Has Meta Data & Matches, Pass To node_ptr
  k3d::inode_collection::nodes_t::const_iterator node 
    = m_document_state->document().nodes().collection().begin();

  for(; node != m_document_state->document().nodes().collection().end(); ++node)
    {
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(*node))
        {
          k3d::string_t value = metadata->get_metadata()[meta_tag];
          
          if(value == meta_data)
            {
              //There Is A Match!
              *node_ptr = *node;
              return true;
            }
        }//if
    }//for

  //No Match Found
  node_ptr = 0;
  return false;

}//checkDocForMeta



void ContentPanel::createPreviewNodes()
{
  //Flags For Each Node
  bool hasAqsis_renderer = 	false;
  bool hasCamera = 				false;
  bool hasGeo = 					false;
  bool hasLight = 				false;
  bool hasLightFill = 			false;
  bool hasLightBack = 			false;
  bool hasLight_shader = 		false;
  bool hasLightFill_shader = 	false;
  bool hasRenderman_engine = 	false;


  //Meta Data Strings
  k3d::string_t aqsis_render_meta 	= "p_aqsis_renderer";
  k3d::string_t camera_meta 			= "p_camera";

  k3d::string_t sphere_geo_meta 		= "p_sphere_geo";
  k3d::string_t cube_geo_meta 		= "p_cube_geo";
  k3d::string_t torus_geo_meta 		= "p_torus_geo";

  k3d::string_t key_light_meta 		= "p_light";
  k3d::string_t fill_light_meta 		= "p_fill_light";
  k3d::string_t back_light_meta 		= "p_back_light";

  k3d::string_t key_bck_lshade_meta = "p_light_shader";
  k3d::string_t fill_lshade_meta 	= "p_fill_light_shader";
  
  k3d::string_t rman_engine_meta   	= "p_rman_engine";

  k3d::string_t nametag_metatag 		= "PreviewCore::nametag";

  //Pointer To Aqsis Engine For RMAN Engine Node
  k3d::ri::irender_engine* aqsis = 0;
  k3d::string_t pointLightPath = "shaders/light/k3d_pointlight.sl";

  //Check Nodes MetaData To See If These Nodes Exist.
  //If They Do Dont Exist Then Create New Nodes

  k3d::inode *node_ptr = 0;
  
  //Check For Aqsis Engine
  if(checkDocForMeta(nametag_metatag, aqsis_render_meta, &node_ptr))
    {
      hasAqsis_renderer = true;
      if(node_ptr)
        aqsis = dynamic_cast<k3d::ri::irender_engine*>(node_ptr);

    }

  //Check For Camera
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, camera_meta, &node_ptr))
    {
       hasCamera = true;
       if(node_ptr)
         m_camera = dynamic_cast<camera_t*>(node_ptr);
    }
                                     
  //Check For Sphere Geometry
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, sphere_geo_meta, &node_ptr))
    {
      hasGeo = true;
      if(node_ptr)
        m_geometry = dynamic_cast<geo_t*>(node_ptr);
    }


  //Check For Main Key Light
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, key_light_meta, &node_ptr))
    {
      hasLight = true;
      if(node_ptr)
        m_main_light = dynamic_cast<light_t*>(node_ptr);
    }


  //Check For Fill Light
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, fill_light_meta, &node_ptr))
    {
      hasLightFill = true;
      if(node_ptr)
        m_fill_light = dynamic_cast<light_t*>(node_ptr);
    }


  //Check For Back Light
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, back_light_meta, &node_ptr))
    {
      hasLightBack = true;
      if(node_ptr)
        m_back_light = dynamic_cast<light_t*>(node_ptr);
    }


  //Check For Key + Back Light Shader
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, key_bck_lshade_meta, &node_ptr))
    {
      hasLight_shader = true;
      if(node_ptr)
        m_light_shader = dynamic_cast<k3d::inode*>(node_ptr);	
    }


  //Check For Fill Light Shader
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, fill_lshade_meta, &node_ptr))
    {
      hasLightFill_shader = true;
      if(node_ptr)
        m_fill_light_shader = dynamic_cast<k3d::inode*>(node_ptr);
    }


  //Check For Render Engine
  node_ptr = 0;
  if(checkDocForMeta(nametag_metatag, rman_engine_meta, &node_ptr))
    {
      hasRenderman_engine = true;
      if(node_ptr)
        m_engine = dynamic_cast<rManEngine_t*>(node_ptr);
    }




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
        metadata->set_metadata(nametag_metatag, key_bck_lshade_meta);
   
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
        metadata->set_metadata(nametag_metatag, fill_lshade_meta);
   
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
        metadata->set_metadata(nametag_metatag, key_light_meta);

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
        metadata->set_metadata(nametag_metatag, fill_light_meta);

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
        metadata->set_metadata(nametag_metatag, back_light_meta);

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
        metadata->set_metadata(nametag_metatag, camera_meta);

    }//if

  //Setup The Geometry

  if(!hasGeo)
    {
      //Create The Default Sphere Geometric Object
      PreviewSphere *default_sphereObj = new PreviewSphere("Sphere", m_document_state);
      default_sphereObj->init("Preview Core::Geo::Sphere", sphere_geo_meta);
      m_used_geometry.push_back(default_sphereObj);

      //Set Current Geometry To The Sphere
      m_geometry = default_sphereObj->m_doc_node;

    }//if

  //Setup The Aqsis Renderman Engine**********

  if(!hasAqsis_renderer)
    {
			// Try the bundled engine first, for win32 installed versions
			aqsis 
  	        = k3d::plugin::create<k3d::ri::irender_engine>("BundledAqsisRenderManEngine", 
  	                                                       m_document_state->document(), 
  	                                                       "Preview Core::Aqsis Renderer");
  	  if(!aqsis)
  	  {
				aqsis 
					= k3d::plugin::create<k3d::ri::irender_engine>("AqsisRenderManEngine", 
																												 m_document_state->document(), 
																												 "Preview Core::Aqsis Renderer");
  	  }

      //Create Meta Data
      if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(aqsis))
        metadata->set_metadata(nametag_metatag, aqsis_render_meta);

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
        metadata->set_metadata(nametag_metatag, rman_engine_meta);

    }//if

}//createPreviewNodes



void ContentPanel::renderInit()
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

}//renderInit




}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
