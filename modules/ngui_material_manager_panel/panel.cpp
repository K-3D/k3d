// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// Material Manager Panel Developed By Alex Curtis
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

using namespace libk3dngui;
using namespace k3d::data;

namespace module{

  namespace ngui{

    namespace material_manager{

      namespace mechanics{

        // [model Components]***********************************************************************

        //Static Material Type Definitions
        const k3d::string_t riMaterialStr 	= "Renderman Materials";
        const k3d::string_t glMaterialStr 	= "OpenGL Materials";
        const k3d::string_t otherStuffStr 	= "Other Stuff";	
       
        const k3d::string_t holderImgFile 	= "renderingShader.png";

        //forward declaration for list of materials
        class s_group;

        //Type Definitions
        typedef k3d::irender_camera_frame rManEngine_t;
        typedef k3d::icamera camera_t;
        typedef k3d::inode light_t;
        typedef k3d::inode lightShader_t;
        typedef k3d::inode geo_t;

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
            //Get Any Availible Data (If There Is Any)
            loadFromMeta();

            //Drop Hint To Multiline Text Item
            so_artistnotes.set_metadata("k3d:property-type", "k3d:multi-line-text");

            //Set Change Signals
            so_name.changed_signal().connect(sigc::mem_fun(*this, &s_object::on_name_changed));
            so_type.changed_signal().connect(sigc::mem_fun(*this, &s_object::on_type_changed));
            so_datestamp.changed_signal().connect(sigc::mem_fun(*this, &s_object::on_datestamp_changed));
            so_artistname.changed_signal().connect(sigc::mem_fun(*this, &s_object::on_artistname_changed));
            so_artistnotes.changed_signal().connect(sigc::mem_fun(*this, &s_object::on_artistnotes_changed));
          }
 
          ~s_object()
          {	   
          }

        public:
          //Accessor Functions
          k3d::string_t getName(){return so_name.internal_value();}

          //Mutator Functions
          void setName(k3d::string_t str){so_name.set_value(str);}

          //SIGNAL EVENTS FROM MATERIAL PROFILE
          void on_name_changed(k3d::ihint *h)
          {
            //Save K3D::DATA so_name to node
            node->set_name(so_name.internal_value());
          }
	  
          void on_type_changed(k3d::ihint *h)
          {
            if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(node))
              metadata->set_metadata("materialManager::material_type", so_type.internal_value());
          }

          void on_datestamp_changed(k3d::ihint *h)
          {
            if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(node))
              metadata->set_metadata("materialManager::material_datestamp", so_datestamp.internal_value());
          }

          void on_artistname_changed(k3d::ihint *h)
          {
            if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(node))
              metadata->set_metadata("materialManager::material_artistname", so_artistname.internal_value());
          }

          void on_artistnotes_changed(k3d::ihint *h)
          {
            if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(node))
              metadata->set_metadata("materialManager::material_artistnotes", so_artistnotes.internal_value());
          }	 

          //Reset Content To Curent Saved Node Data
          void loadFromMeta()
          {
            //Get Node Name
            so_name.set_value(node->name());

            //Get Any Availible Meta Data (If There Is Any)
            if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(node))
              {
                so_type.set_value(metadata->get_metadata()["materialManager::material_type"]);
                so_datestamp.set_value(metadata->get_metadata()["materialManager::material_datestamp"]);
                so_artistname.set_value(metadata->get_metadata()["materialManager::material_artistname"]);
                so_artistnotes.set_value(metadata->get_metadata()["materialManager::material_artistnotes"]);
              }//if
          }

          void insert_todays_date()
          {
            //Insert Todays Date Into Date Entry
            time_t rawtime;
            struct tm * timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            std::stringstream stream_for_date;
            //Adding to values as they origionally represent amount from jan (jan = 0) and 1900 
            stream_for_date << (1 + timeinfo->tm_mon) << "/" << timeinfo->tm_mday << "/" << (1900 + timeinfo->tm_year);
            k3d::string_t current_date_str = stream_for_date.str();

            so_datestamp.set_value(current_date_str);
          }

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
            //Clear Up All Of The Shader Objects
            clearGroup();
          }

        public:
          void clearGroup();
          void addMaterial(s_object* shd);
          void removeMaterial(s_object* shd);

        public:
          std::list<s_object*>     m_materials; //children
          k3d::string_t            sg_name;

        };//s_group

        void s_group::clearGroup()
        {
          k3d::log() << "CLEAR GROUP STUFF " << std::endl;
  
          k3d::log() << "M_MATERIAL SIZE " <<  m_materials.size() << std::endl;

          std::list<s_object*>::iterator materialIter = m_materials.begin(); //BUG WITH M_MATERIAL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
          for(; materialIter != m_materials.end(); materialIter++)
            {
              //Clean Up The List Of material Objects (s_object)
              k3d::log() << "DELETEING: " << (*materialIter)->getName() << std::endl;
              delete (*materialIter);
              //m_materials.erase(materialIter);
            }//for

          //Ensure Clean Storage 
          m_materials.clear();
        }

        void s_group::addMaterial(s_object* shd)
        {
          m_materials.push_back(shd);
        }

        void s_group::removeMaterial(s_object* shd)
        {
          m_materials.remove(shd);
        }

          //************
          class model{
          public:
            model()
            {
              rman = new s_group(riMaterialStr);
              gl = new s_group(glMaterialStr);
              other = new s_group(otherStuffStr);
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

            s_group *rman;
            s_group *gl;
            s_group *other;

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
                  if((*nodeIter)->factory().implements(typeid(k3d::ri::imaterial))){
                    rman->addMaterial(new s_object(rman, (*nodeIter), riMaterialStr));
                    k3d::log() << "BASE NAME: " <<  (*nodeIter)->name() << std::endl;
                  }

                  else if((*nodeIter)->factory().implements(typeid(k3d::gl::imaterial)))
                    gl->addMaterial(new s_object(gl, (*nodeIter), glMaterialStr));

                  else
                    other->addMaterial(new s_object(other, (*nodeIter), otherStuffStr));

                }//if
            }//for

        }

        void model::clearModel()
        {

          k3d::log() << "NUMBER OF GROUPS: " << m_groups.size() << std::endl;
          std::list<s_group*>::iterator groupIter = m_groups.begin();
          for(; groupIter != m_groups.end(); groupIter++)
              delete (*groupIter);      

          //Ensure Clean Storage 
          m_groups.clear();

          k3d::log() << "groups cleared...therefore shader_o cleared" << std::endl;

        }


        // [/model Components]**********************************************************************

        // [implementation]*************************************************************************

        //************ PREVIEW IMAGE CONTENT CLASS
        class materialPreviewImage: public Gtk::DrawingArea{
        public:
          materialPreviewImage(k3d::filesystem::path _imgPath);
          virtual ~materialPreviewImage();
	  
          //Function to be called on preview img update request signal
          bool onUpdatePreview();

        public:
          k3d::filesystem::path imgFilePath;
          k3d::filesystem::path imgHolderPath;

        protected:
          //Override default signal handler:
          virtual bool on_expose_event(GdkEventExpose* event);

        };//materialPreviewImage


        materialPreviewImage::materialPreviewImage(k3d::filesystem::path _imgPath)
        {
          imgFilePath = _imgPath;

          //Remove Preview Render from tmp dir -> Required at uncertain close events occur
          k3d::filesystem::remove(imgFilePath);

          //Define Location of image holder (if no render preview file found)
          imgHolderPath = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") 
            / k3d::filesystem::generic_path(holderImgFile);
        }

        materialPreviewImage::~materialPreviewImage()
        {
          //Remove Preview Render from tmp dir
          k3d::filesystem::remove(imgFilePath);
          k3d::log() << "REMOVING IMAGE FROM FILESYSTEM" << std::endl;
        }

        bool materialPreviewImage::on_expose_event(GdkEventExpose* event)
        {
          try
            {
              Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(imgFilePath.native_filesystem_string());
              image->render_to_drawable(get_window(), get_style()->get_black_gc(),
                                        0, 0, 10, 10, image->get_width(), image->get_height(),
                                        Gdk::RGB_DITHER_NONE, 0, 0);
            }
          catch(Glib::FileError)
            {
              //No Image File. Display Default Image Holder
              Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(imgHolderPath.native_filesystem_string());
              image->render_to_drawable(get_window(), get_style()->get_black_gc(),
                                        0, 0, 10, 10, image->get_width(), image->get_height(),
                                        Gdk::RGB_DITHER_NONE, 0, 0);
            }
          catch(Glib::Error)
            {

              //Should not hopefully Get Here!
            }

          return true;

        }//on_expose_event

        //************ GUI CONTENT PANE CLASSES
        class content_pane{
        public:
          content_pane(Gtk::HPaned *_m_Hpane, k3d::icommand_node *_m_parent, document_state *_documentState)
            :m_Hpane(_m_Hpane), m_parent(_m_parent), preview_primitive("Preview Primitive"), 
             m_document_state(_documentState),
             previewSize(200), s_materialImgFile("singlePreviewRender"),
             multi_s_materialImgFile("multiPreviewRender")
	     
          {
            //Get Instance Number From This Value & Mark Render Images Accordingly
            std::stringstream stream_for_id;
            stream_for_id << this;
            k3d::string_t instance_id = stream_for_id.str();

            //Edit Render File Names To Represent Unique Files
            multi_s_materialImgFile.append(instance_id);
            s_materialImgFile.append(instance_id);
            s_materialImgFile += k3d::string_t(".png");


            k3d::log() << "single: " << s_materialImgFile << std::endl;
            k3d::log() << "multi: " << multi_s_materialImgFile << std::endl;


            //Setup The Rendering Components (Using Meta Data Eventually)
            createPreviewNodes(); 

          }
	  
          //Analyse Current Document For Preview Nodes And Create As Appropriate
          void createPreviewNodes()
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

            //Check All Nodes MetaData To See If These Nodes Exist. If They Do Dont Create New Nodes
            k3d::inode_collection::nodes_t::const_iterator node = m_document_state->document().nodes().collection().begin();
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
                        panelCamera = dynamic_cast<camera_t*>(*node);
                      }

                    if(value == "p_geo")
                      {
                        hasGeo = true;
                        panelGeo = dynamic_cast<geo_t*>(*node);
                      }

                    if(value == "p_light")
                      {
                        hasLight = true;
                        panelLight = dynamic_cast<light_t*>(*node);
                      }

                    if(value == "p_fill_light")
                      {
                        hasLightFill = true;
                        panelFillLight = dynamic_cast<light_t*>(*node);
                      }

                    if(value == "p_back_light")
                      {
                        hasLightBack = true;
                        panelBackLight = dynamic_cast<light_t*>(*node);
                      }


                    if(value == "p_light_shader")
                      {
                        hasLight_shader = true;
                        panelLShader = dynamic_cast<k3d::inode*>(*node);				
                      }

                    if(value == "p_fill_light_shader")
                      {
                        hasLightFill_shader = true;
                        panelLShaderFill = dynamic_cast<k3d::inode*>(*node);				
                      }          

                    if(value == "p_rman_engine")
                      {
                        hasRenderman_engine = true;
                        panelEngine = dynamic_cast<rManEngine_t*>(*node);					
                      }		

                  }//if
              }//for


            //Light Shader Setup*******************

            if(!hasLight_shader)
              {
                panelLShader = dynamic_cast<k3d::inode*>(k3d::plugin::create("RenderManLightShader", 
                                                                             m_document_state->document(), 
                                                                             "Preview Core::Light Shader"));

                k3d::property::set_internal_value(*panelLShader, 
                                                  "shader_path", k3d::share_path() /
                                                  k3d::filesystem::generic_path("shaders/light/k3d_pointlight.sl"));

                k3d::property::set_internal_value(*panelLShader, 
                                                  "intensity", k3d::double_t(1200));
	      
                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelLShader))
                  metadata->set_metadata("PreviewCore::nametag", "p_light_shader");
   
              }//if


            if(!hasLightFill_shader)
              {
                panelLShaderFill = dynamic_cast<k3d::inode*>(k3d::plugin::create("RenderManLightShader", 
                                                                             m_document_state->document(), 
                                                                             "Preview Core::Fill Light Shader"));

                k3d::property::set_internal_value(*panelLShaderFill, 
                                                  "shader_path", k3d::share_path() /
                                                  k3d::filesystem::generic_path("shaders/light/k3d_pointlight.sl"));

                k3d::property::set_internal_value(*panelLShaderFill, 
                                                  "intensity", k3d::double_t(650));
	      
                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelLShaderFill))
                  metadata->set_metadata("PreviewCore::nametag", "p_fill_light_shader");
   
              }//if

            //Light Setup*************************

            if(!hasLight)
              {
                panelLight = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                                        m_document_state->document(), 
                                                                        "Preview Core::Light"));

                k3d::property::set_internal_value(*panelLight, 
                                                  "shader", panelLShader);


                k3d::inode* light_transformation = k3d::set_matrix(*panelLight, 
                                                                   k3d::translation3D(k3d::point3(-20, 20, 20)));

                k3d::property::set_internal_value(*panelLight, 
                                                  "viewport_visible", false);



                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelLight))
                  metadata->set_metadata("PreviewCore::nametag", "p_light");

              }//if


            if(!hasLightFill)
              {
                panelFillLight = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                                        m_document_state->document(), 
                                                                        "Preview Core::Fill Light"));

                k3d::property::set_internal_value(*panelFillLight, 
                                                  "shader", panelLShaderFill);


                k3d::inode* light_transformation = k3d::set_matrix(*panelFillLight, 
                                                                   k3d::translation3D(k3d::point3(20, 28, -18)));


                k3d::property::set_internal_value(*panelFillLight, 
                                                  "viewport_visible", false);


                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelFillLight))
                  metadata->set_metadata("PreviewCore::nametag", "p_fill_light");

              }//if


            if(!hasLightBack)
              {
                panelBackLight = dynamic_cast<light_t*>(k3d::plugin::create("RenderManLight", 
                                                                        m_document_state->document(), 
                                                                        "Preview Core::Back Light"));

                k3d::property::set_internal_value(*panelBackLight, 
                                                  "shader", panelLShader);


                k3d::inode* light_transformation = k3d::set_matrix(*panelBackLight, 
                                                                   k3d::translation3D(k3d::point3(0, -38, 18)));

                k3d::property::set_internal_value(*panelBackLight, 
                                                  "viewport_visible", false);


                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelBackLight))
                  metadata->set_metadata("PreviewCore::nametag", "p_back_light");

              }//if


            //Camera Setup************************
	  
            if(!hasCamera)
              {
                panelCamera = dynamic_cast<camera_t*>
                  (k3d::plugin::create("Camera", 
                                       m_document_state->document(), 
                                       "Preview Core::Camera"));
                const k3d::point3 origin = k3d::point3(0, 0, 0);
                const k3d::vector3 world_up = k3d::vector3(0, 0, 1);

                const k3d::point3 position = k3d::point3(0, 13, 0);
                const k3d::vector3 look_vector = origin - position;
                const k3d::vector3 right_vector = look_vector ^ world_up;
                const k3d::vector3 up_vector = right_vector ^ look_vector;

                k3d::inode* const camera_transformation 
                  = k3d::set_matrix(*panelCamera, 
                                    k3d::view_matrix(look_vector, up_vector, position));
       
                camera_transformation->set_name("Camera Transformation");
                k3d::property::set_internal_value(*panelCamera, 
                                                  "world_target", k3d::point3(0, 0, 0));

                k3d::property::set_internal_value(*panelCamera, 
                                                  "viewport_visible", false);

                k3d::property::set_internal_value(*panelCamera, 
                                                  "aspect_ratio", k3d::string_t("Square"));

                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelCamera))
                  metadata->set_metadata("PreviewCore::nametag", "p_camera");

              }//if

            //Geometry Setup**********************

            if(!hasGeo)
              {
                panelGeo = dynamic_cast<geo_t*>
                  (k3d::plugin::create("Sphere", 
                                       m_document_state->document(), 
                                       "Preview Core::Geo::Sphere"));

                k3d::property::set_internal_value(*panelGeo, 
                                                  "render_shadows", false);

                k3d::property::set_internal_value(*panelGeo, 
                                                  "viewport_visible", false);

                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelGeo))
                  metadata->set_metadata("PreviewCore::nametag", "p_geo");

              }//if

            //Aqsis Engine Setup**************

            if(!hasAqsis_renderer)
              {
                aqsis = k3d::plugin::create<k3d::ri::irender_engine>("AqsisRenderManEngine", 
                                                                     m_document_state->document(), 
                                                                     "Preview Core::Aqsis Renderer");
                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(aqsis))
                  metadata->set_metadata("PreviewCore::nametag", "p_aqsis_renderer");

              }//if


            //Renderman Engine Setup**************

            if(!hasRenderman_engine)
              {
                panelEngine = dynamic_cast<rManEngine_t*>
                  (k3d::plugin::create("RenderManEngine", 
                                       m_document_state->document(), 
                                       "Preview Core::RenderManEngine"));

                k3d::property::set_internal_value(*panelEngine, 
                                                  "enabled_lights", 
                                                  k3d::inode_collection_property::nodes_t(1, panelLight));

                k3d::property::set_internal_value(*panelEngine, 
                                                  "visible_nodes", 
                                                  k3d::inode_collection_property::nodes_t(1, panelGeo));

                //Setup the material preview render engine*****
                k3d::property::set_internal_value(*panelEngine, 
                                                  "render_engine", dynamic_cast<k3d::inode*>(aqsis));

                k3d::property::set_internal_value(*panelEngine, 
                                                  "pixel_width", static_cast<k3d::int32_t>(previewSize));


                k3d::property::set_internal_value(*panelEngine, 
                                                  "pixel_height", static_cast<k3d::int32_t>(previewSize));

                k3d::double_t aspectRatio = 1.0;

                k3d::property::set_internal_value(*panelEngine, 
                                                  "pixel_aspect_ratio", aspectRatio);


                //METADATA INSERT
                if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(panelEngine))
                  metadata->set_metadata("PreviewCore::nametag", "p_rman_engine");

              }//if
          }

          virtual ~content_pane(){}

        public:
          //Render The Preview Image
          virtual void renderPreview() = 0;
          virtual bool updatePreviewImage() = 0;

          virtual void buildPane() = 0;

        protected:
          Gtk::HPaned *m_Hpane;
          Gtk::MenuToolButton preview_primitive;

          k3d::icommand_node *m_parent;
          document_state *m_document_state;
   
          k3d::string_t s_materialImgFile;		//Unqiue Identifer Added + Extension
          k3d::string_t multi_s_materialImgFile; 	//Unqiue Identifer Added + Extension (in new str)
	  

        protected:
          //Render Preview Components
          light_t *panelLight;
          light_t *panelFillLight;
          light_t *panelBackLight;

          lightShader_t *panelLShader;
          lightShader_t *panelLShaderFill;

          //NOTE: Back Uses Key Light Shader

          geo_t *panelGeo;
          rManEngine_t *panelEngine;
          camera_t *panelCamera;
          //k3d::ri::irender_engine *panelRenderEngine;
          k3d::uint_t previewSize;

          //GTK Widgets
          Gtk::ScrolledWindow m_scrolled_window;

          sigc::connection timerPreviewConnection;

        };//content_pane

        //GROUP CONTENT PANE
        class g_content_pane : public content_pane{
        public:
          g_content_pane(Gtk::HPaned *_m_Hpane, s_group *_m_grp, k3d::icommand_node *_m_parent, document_state *_documentState)
            :content_pane(_m_Hpane, _m_parent, _documentState), m_grp(_m_grp),
             group_display("List Settings")
          {
	    
            //Preview Update Timer
            //glib timer set that updates preview image every 0.25 seconds
            // << Will Delete when panel closed
            timerPreviewConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &g_content_pane::updatePreviewImage), 250);	    
          }

          ~g_content_pane()
          {
            //Clean Up Dynamic Allocated Memory :)

            //Preview Image Cleanup
            std::vector<materialPreviewImage*>::iterator pIter = materialPreviews.begin();
            for(; pIter != materialPreviews.end(); pIter++)
              delete (*pIter);   

            //All GTK Widgets >>

            //ALL Gtk Hboxes*****************
            std::vector<Gtk::HBox*>::iterator hBoxIter = object_preview_data_c.begin();
            for(; hBoxIter != object_preview_data_c.end(); hBoxIter++)
              delete (*hBoxIter);  

            hBoxIter = material_label_data_c.begin();
            for(; hBoxIter != material_label_data_c.end(); hBoxIter++)
              delete (*hBoxIter);  


            //ALL Gtk Vboxes*****************
            std::vector<Gtk::VBox*>::iterator vBoxIter = material_data_desc_c.begin();
            for(; vBoxIter != material_data_desc_c.end(); vBoxIter++)
              delete (*vBoxIter); 
            
            vBoxIter = material_label_c.begin();
            for(; vBoxIter != material_label_c.end(); vBoxIter++)
              delete (*vBoxIter); 

            vBoxIter = material_data_c.begin();
            for(; vBoxIter != material_data_c.end(); vBoxIter++)
              delete (*vBoxIter); 


            //ALL Gtk Frames*****************
            std::vector<Gtk::Frame*>::iterator frameIter = img_holder_frames.begin();
            for(; frameIter != img_holder_frames.end(); frameIter++)
              delete (*frameIter); 


            //ALL Gtk Labels*****************
            std::vector<Gtk::Label*>::iterator labelIter = s_name_l.begin();
            for(; labelIter != s_name_l.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_type_l.begin();
            for(; labelIter != s_type_l.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_datemod_l.begin();
            for(; labelIter != s_datemod_l.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_artistname_l.begin();
            for(; labelIter != s_artistname_l.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_name_d.begin();
            for(; labelIter != s_name_d.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_type_d.begin();
            for(; labelIter != s_type_d.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_datemod_d.begin();
            for(; labelIter != s_datemod_d.end(); labelIter++)
              delete (*labelIter); 

            labelIter = s_artistname_d.begin();
            for(; labelIter != s_artistname_d.end(); labelIter++)
              delete (*labelIter); 

            
            //ALL Gtk Textviews*****************
            std::vector<Gtk::TextView*>::iterator tvIter = s_artistnotes_view.begin();
            for(; tvIter != s_artistnotes_view.end(); tvIter++)
              delete (*tvIter); 


            //ALL Gtk ScrolledWindows*****************
            std::vector<Gtk::ScrolledWindow*>::iterator swIter = notes_scroll_win.begin();
            for(; swIter != notes_scroll_win.end(); swIter++)
              delete (*swIter); 


            //ALL Gtk HSeparator*****************
            std::vector<Gtk::HSeparator*>::iterator hsepIter = data_notes_breakers.begin();
            for(; hsepIter != data_notes_breakers.end(); hsepIter++)
              delete (*hsepIter); 

            hsepIter = s_breakers.begin();
            for(; hsepIter != s_breakers.end(); hsepIter++)
              delete (*hsepIter); 

            //ALL Gtk Buttons*****************
            std::vector<Gtk::Button*>::iterator buttIter = edit_shader_buttons.begin();
            for(; buttIter != edit_shader_buttons.end(); buttIter++)
              delete (*buttIter); 


            //ALL Gtk VSeparator*****************
            std::vector<Gtk::VSeparator*>::iterator vsepIter = label_data_breakers.begin();
            for(; vsepIter !=  label_data_breakers.end(); vsepIter++)
              delete (*vsepIter); 


            //Disconect Signal Connections***********
            timerPreviewConnection.disconnect();
          }

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

                m_scrolled_window.add(materialBoxes_c);

                //Setup ToolBar
                materialBoxes_c.pack_start(m_toolbar, false, false, 0);

                //Integer To Tack onto filenames to make them unique
                int fileName_int = 0;

                //For Each Of The Elements In Group->m_materials Create A GUI Strip
                std::list<s_object*>::iterator soIter = m_grp->m_materials.begin();
                for(; soIter != m_grp->m_materials.end(); soIter++)
                  {
                    //Create File Name For material Preview Image
                    k3d::string_t int_str;
                    k3d::string_t finalFile_str;
                    std::stringstream out;
                    out << fileName_int;
                    int_str = out.str();
                    int_str += k3d::string_t(".png");
                    finalFile_str.append(multi_s_materialImgFile);
                    finalFile_str.append(int_str);		    

                    //Create The material Preview Image Object
                    materialPreviewImage *s_preview_obj = new materialPreviewImage(k3d::system::get_temp_directory() / k3d::filesystem::generic_path(finalFile_str));
                    materialPreviews.push_back(s_preview_obj);
		    
                    Gtk::HBox *tmpHBox = new Gtk::HBox;
                    object_preview_data_c.push_back(tmpHBox);
                    materialBoxes_c.pack_start(*tmpHBox, false, false, 10);

                    Gtk::Frame *tmpFrame = new Gtk::Frame("Preview Render:");
                    tmpFrame->add(*s_preview_obj);
                    img_holder_frames.push_back(tmpFrame);
                    tmpFrame->set_size_request(previewSize + 25, previewSize + 35);
                    tmpHBox->pack_start(*tmpFrame, false, false, 5);

                    //Data & Notes VBox
                    Gtk::VBox *tmpVBox_dd = new Gtk::VBox;
                    material_data_desc_c.push_back(tmpVBox_dd);
                    tmpHBox->pack_start(*tmpVBox_dd, true, true, 5); //HERE

                    //Data HBox (Contains x3 VBox -> Label, vertical break and Data)
                    Gtk::HBox *tmpHBox_ld = new Gtk::HBox;
                    material_label_data_c.push_back(tmpHBox_ld);
                    tmpVBox_dd->pack_start(*tmpHBox_ld, false, false, 5);
		    
                    //Label VBox
                    Gtk::VBox *tmpVBox_l = new Gtk::VBox;
                    material_label_c.push_back(tmpVBox_l);
                    tmpHBox_ld->pack_start(*tmpVBox_l, false, false, 0);

                    tmpVBox_l->set_spacing(4);

                    //BUILD LABEL STUFF

                    Gtk::Label *tmpName_l = new Gtk::Label("Material Name:");
                    Gtk::Label *tmpType_l = new Gtk::Label("Material Type:");
                    Gtk::Label *tmpdateMod_l = new Gtk::Label("Date Modified:");
                    Gtk::Label *tmpArtistName_l = new Gtk::Label("Artist's Name:");

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


                    //Build Vertical Breaker
                    Gtk::VSeparator *tmpVBreaker = new Gtk::VSeparator();
                    label_data_breakers.push_back(tmpVBreaker);
                    tmpHBox_ld->pack_start(*tmpVBreaker, false, false, 10);

                    //Data VBox
                    Gtk::VBox *tmpVBox_d = new Gtk::VBox;
                    material_data_c.push_back(tmpVBox_d);
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

                    //Horizontal rule between data and notes
                    Gtk::HSeparator *tmpDataHBreaker = new Gtk::HSeparator();
                    data_notes_breakers.push_back(tmpDataHBreaker);
                    tmpVBox_dd->pack_start(*tmpDataHBreaker, false, false, 0);

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
                    materialBoxes_c.pack_start(*tmpHBreaker, false, false, 0);

                    //Edit Shader Buttons
                    Gtk::Button *edit_shader_button = new Gtk::Button;
                    k3d::string_t edit_graphic = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
                                                  / k3d::filesystem::generic_path("mat_edit_shader_icon.xpm")).native_filesystem_string();

                    edit_shader_button->add_pixlabel(edit_graphic, "Edit Material");
                    edit_shader_buttons.push_back(edit_shader_button);

                    tmpHBox_ld->pack_end(*edit_shader_button, false, false, 0);


                    fileName_int++;

                  }//for

                //Set Off Renderer In New Process 
                renderPreview();

              }
            else
              k3d::log() << "Invalid HPaned Pointer" << std::endl;  

            //DEBUG INFO -> DELETE ON RELEASE
            k3d::log() << "building Group panel" << std::endl;
            m_Hpane->show_all(); 

          }//buildPane

          void renderPreview()
          {
            //Re-init The Preview Render Dimensions
            k3d::property::set_internal_value(*panelEngine, 
                                              "pixel_width", static_cast<k3d::int32_t>(previewSize));


            k3d::property::set_internal_value(*panelEngine, 
                                              "pixel_height", static_cast<k3d::int32_t>(previewSize));


            //Ensure Current Preview Engine Has Selected Nodes Only Visible
            k3d::inode_collection::nodes_t::const_iterator node = m_document_state->document().nodes().collection().begin();
            for(; node != m_document_state->document().nodes().collection().end(); ++node){

              if((*node)->factory().implements(typeid(k3d::ri::ilight)))
                {
                  //Disable Node Regardless In RMANEngine::lights and nodes
                  k3d::property::set_internal_value(*panelEngine, 
                                                    "enabled_lights", 
                                                    k3d::inode_collection_property::nodes_t(0, (*node)));
                }//if
              else if((*node)->factory().implements(typeid(k3d::itransform_sink)))
                {
                  k3d::property::set_internal_value(*panelEngine, 
                                                    "visible_nodes", 
                                                    k3d::inode_collection_property::nodes_t(0, (*node)));
                }//else if
	    
            }//for


            //Vector List Of Lights To Be Enabled In Chosen Render Engine
            std::vector<k3d::inode*>lightsEnabled;
            lightsEnabled.push_back(panelLight);
            lightsEnabled.push_back(panelFillLight);
            lightsEnabled.push_back(panelBackLight);

            //Simply Enable Now Only USed Lights & Geo
            k3d::property::set_internal_value(*panelEngine, 
                                              "enabled_lights", lightsEnabled);


            k3d::property::set_internal_value(*panelEngine, 
                                              "visible_nodes", 
                                              k3d::inode_collection_property::nodes_t(1, panelGeo));
  
            //Go Though Every material In Group Render To Image File.. Done
            std::list<s_object*>::const_iterator soIter = m_grp->m_materials.begin();
            std::vector<materialPreviewImage*>::iterator pIter = materialPreviews.begin();   //NEE TO FIX THIS AS ASSUMES TOO MUCH (CORRECT SIZES) -> DANGEROUS
            for(; soIter != m_grp->m_materials.end(); soIter++)
              {
                //Check If NodeIn sobject Is A RenderMan Material
                if((*soIter)->node->factory().implements(typeid(k3d::ri::imaterial)))
                  {
                    //If it is, assign to current geometry as surface shader
                    k3d::property::set_internal_value(*panelGeo, 
                                                      "material", (*soIter)->node);
	   
                    //Render The Preview Using Selected External Renderer
                    panelEngine->render_camera_frame(*panelCamera, (*pIter)->imgFilePath, false);

                  }//if	 
                else
                  {
                    k3d::log() << "Is NOT A Rman Material" << std::endl;
                  }

                pIter++;

              }//for  

          }//renderPreview


          bool updatePreviewImage()
          {
            std::vector<materialPreviewImage*>::iterator simgIter = materialPreviews.begin();
            for(; simgIter != materialPreviews.end(); simgIter++)
              {
                (*simgIter)->queue_resize();
                (*simgIter)->queue_draw();
              }//for
	    
            return true;
          }

        public:
          
          //GTK Widgets
          Gtk::VBox materialBoxes_c;
          std::vector<Gtk::HBox *> object_preview_data_c;
          std::vector<Gtk::Frame *> img_holder_frames;
          std::vector<Gtk::VBox *> material_data_desc_c;
          std::vector<Gtk::HBox *> material_label_data_c;
          std::vector<Gtk::VBox *> material_label_c;
          std::vector<Gtk::VBox *> material_data_c;

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

          std::vector<Gtk::HSeparator*> data_notes_breakers;
          std::vector<Gtk::HSeparator*> s_breakers;

          std::vector<Gtk::VSeparator*> label_data_breakers;

          std::vector<Gtk::Button*> edit_shader_buttons;

          Gtk::HBox s_name_c;
          Gtk::HBox s_type_c;
          Gtk::HBox s_datemod_c;
          Gtk::HBox s_artistname_c;

          Gtk::VBox toolbar_main_c;

          //Toolbar Contents
          Gtk::Toolbar m_toolbar; 
          Gtk::MenuToolButton group_display;

          //material Preview
          std::vector<materialPreviewImage*> materialPreviews;

        private:
          s_group *m_grp;

        };//g_content_pane

        //MATERIAL OBJECT CONTENT PANE
        class so_content_pane : public content_pane{
        public:
          so_content_pane(Gtk::HPaned *_m_Hpane, s_object *_m_so, k3d::icommand_node *_m_parent, document_state *_documentState)
            :content_pane(_m_Hpane, _m_parent, _documentState), m_so(_m_so),
             so_preview_frame("Preview Render:"), artistnotes_frame("Artist's Notes:"), toolbox_frame("Toolbox:"),
             s_name_entry(*_m_parent, k3d::string_t("so_name_field"), entry::model(_m_so->so_name), 0),
             s_type_entry(*_m_parent, k3d::string_t("so_type_field"), entry::model(_m_so->so_type), 0),
             s_datemod_entry(*_m_parent, k3d::string_t("so_datestamp_field"), entry::model(_m_so->so_datestamp), 0),
             s_artistname_entry(*_m_parent, k3d::string_t("so_artistname_field"), entry::model(_m_so->so_artistname), 0),
             s_artistnotes_mltext(*_m_parent, k3d::string_t("so_artistnotes_mltxt"), text::model(_m_so->so_artistnotes), 0),
             materialPreview(k3d::system::get_temp_directory() / k3d::filesystem::generic_path(s_materialImgFile))
          {
            s_name_l.set_text		("Material Name: ");
            s_type_l.set_text		("Material Type: ");
            s_datemod_l.set_text	("Date Modified: ");
            s_artistname_l.set_text	("Artist's Name: ");

            //Preview Update Timer
            //glib timer set that updates preview image every 0.25 seconds
            // << Will Delete when panel closed
            timerPreviewConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &so_content_pane::updatePreviewImage), 250);

            //Date Insert Connection
            insert_date_button.signal_clicked().connect(sigc::mem_fun(*this, &so_content_pane::on_date_button_pressed));
          }

          ~so_content_pane()
          {
            timerPreviewConnection.disconnect();
          }

        public:
          void buildPane()
          {
            if(m_Hpane)
              {
                //Set Off Renderer In New Process 
                renderPreview();

                so_preview_frame.set_size_request(previewSize + 25, previewSize + 35);
                so_preview_frame.add(materialPreview);
                preview_c.pack_start(so_preview_frame, false, false, 8);
 	      
                //Add ScrollWindowed Panel To Right Pane From Implementation
                m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
                m_scrolled_window.add(main_detail_c);

                m_Hpane->add2(m_scrolled_window);

                main_detail_c.pack_start(preview_label_data_c, false, false, 0); 


                preview_label_data_c.pack_start(preview_c, false, false, 0);
                preview_label_data_c.pack_start(data_toolbar_c, false, false, 0);

                data_toolbar_c.pack_start(label_data_c, true, true, 0);


                label_data_c.pack_start(label_c, true, true, 0);
                label_data_c.pack_start(data_c, true, true, 0);

                preview_label_data_c.set_spacing(10);

                //material Name
                s_name_l.set_alignment(0.0);
                label_c.pack_start(s_name_l, true, true, 0);
                data_c.pack_start(s_name_entry, true, true, 2);

                //material Type
                s_type_l.set_alignment(0.0);
                label_c.pack_start(s_type_l, true, true, 0);
                data_c.pack_start(s_type_entry, true, true, 0);

                //Date Modified
                s_datemod_l.set_alignment(0.0);
                label_c.pack_start(s_datemod_l, true, true, 0);

                //Date Button
                k3d::string_t date_graphic = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
                                              / k3d::filesystem::generic_path("mat_preview_date_icon.xpm")).native_filesystem_string();

                insert_date_button.add_pixlabel(date_graphic, "Todays Date");
             
                //Date Entry & Button Pack
                date_dButton_c.pack_start(s_datemod_entry, true, false, 0);
                date_dButton_c.pack_start(insert_date_button, true, false, 0);

                data_c.pack_start(date_dButton_c, true, false, 0);

                //Artist Name
                s_artistname_l.set_alignment(0.0);
                label_c.pack_start(s_artistname_l, true, true, 0);
                data_c.pack_start(s_artistname_entry, true, true, 0);	

                //ToolBox *************************************************
                toolbox_frame.set_size_request(-1, 48);
                data_toolbar_c.pack_start(toolbox_frame, false, false, 8);
                toolbox_frame.add(toolbox_c);
                
                toolbox_c.pack_start(pview_geo_combo, false, false, 2);

                //Temp -> Get gui right first
                pview_geo_combo.append_text("Sphere");
                pview_geo_combo.append_text("Cube");
                pview_geo_combo.set_active_text("Sphere");
                pview_geo_combo.set_size_request(-1, 30);

                //Preview Background Button
                toolbox_c.pack_start(pview_back_button, false, false, 2);

                k3d::string_t bg_graphic_path = (k3d::share_path() / k3d::filesystem::generic_path("ngui/pixmap") 
                                              / k3d::filesystem::generic_path("mat_preview_bg_icon.xpm")).native_filesystem_string();

                pview_back_button.add_pixlabel(bg_graphic_path, "BG");

                //Temp -> Get gui right first
                toolbox_c.pack_start(pview_engine_combo, false, false, 2);
                pview_engine_combo.append_text("Renderman");
                pview_engine_combo.append_text("Yafray");
                pview_engine_combo.set_active_text("Renderman");
                pview_engine_combo.set_size_request(-1, 30);


                //**********************************************************

                //Artist Notes
                main_detail_c.pack_start(artistnotes_frame, true, true, 10);

                s_artistnotes_c.pack_start( s_artistnotes_mltext, true, true, 5);
                artistnotes_frame.add(s_artistnotes_c);

	       
                //DEBUG INFO -> DELETE ON RELEASE
                k3d::log() << "building material panel" << std::endl;
                m_Hpane->show_all(); 
              }
            else
              k3d::log() << "Invalid HPaned Pointer" << std::endl;  	

          }//buildPane

          void renderPreview()
          {
            //Re-init The Preview Render Dimensions
            k3d::property::set_internal_value(*panelEngine, 
                                              "pixel_width", static_cast<k3d::int32_t>(previewSize));


            k3d::property::set_internal_value(*panelEngine, 
                                              "pixel_height", static_cast<k3d::int32_t>(previewSize));

            //Ensure Current Preview Engine Has Selected Nodes Only Visible
            k3d::inode_collection::nodes_t::const_iterator node = m_document_state->document().nodes().collection().begin();
            for(; node != m_document_state->document().nodes().collection().end(); ++node){

              if((*node)->factory().implements(typeid(k3d::ri::ilight)))
                {
                  //Disable Node Regardless In RMANEngine::lights and nodes
                  k3d::property::set_internal_value(*panelEngine, 
                                                    "enabled_lights", 
                                                    k3d::inode_collection_property::nodes_t(0, (*node)));
                }//if
              else if((*node)->factory().implements(typeid(k3d::itransform_sink)))
                {
                  k3d::property::set_internal_value(*panelEngine, 
                                                    "visible_nodes", 
                                                    k3d::inode_collection_property::nodes_t(0, (*node)));
                }//else if
	    
            }//for

            //Vector List Of Lights To Be Enabled In Chosen Render Engine
            std::vector<k3d::inode*>lightsEnabled;
            lightsEnabled.push_back(panelLight);
            lightsEnabled.push_back(panelFillLight);
            lightsEnabled.push_back(panelBackLight);

            //Simply Enable Now Only USed Lights & Geo
            k3d::property::set_internal_value(*panelEngine, 
                                              "enabled_lights", lightsEnabled);
 


            k3d::property::set_internal_value(*panelEngine, 
                                              "visible_nodes", 
                                              k3d::inode_collection_property::nodes_t(1, panelGeo));
  
            //Check If NodeIn sobject Is A RenderMan Material
            if(m_so->node->factory().implements(typeid(k3d::ri::imaterial)))
              {
                //If it is, assign to current geometry as surface shader
                k3d::property::set_internal_value(*panelGeo, 
                                                  "material", m_so->node);
	   

                //Render The Preview Using Selected External Renderer
                panelEngine->render_camera_frame(*panelCamera, k3d::system::get_temp_directory() / k3d::filesystem::generic_path(s_materialImgFile),
                                                 false);

              }//if	 
            else
              {
                k3d::log() << "Is NOT A Rman Material" << std::endl;
              }
	    
          }//renderPreview

          bool updatePreviewImage()
          {
            materialPreview.queue_resize();
            materialPreview.queue_draw();
            return true;
          }	

          void on_date_button_pressed()
          {
            //Insert Todays Date (mm/dd/yyyy) into date entry
            m_so->insert_todays_date();
          }

        public:
          //GTK Widgets
          Gtk::Label 		s_name_l;
          Gtk::Label 		s_type_l;
          Gtk::Label 		s_datemod_l;
          Gtk::Label 		s_artistname_l;

          //Gtk::Entry s_name_entry;
          Gtk::VBox 			main_detail_c;
          Gtk::HBox 			preview_label_data_c;
          Gtk::VBox 			data_toolbar_c;
          Gtk::HBox 			label_data_c;
          Gtk::VBox 			label_c;
          Gtk::VBox 			data_c;
          Gtk::VBox 			preview_c;
          Gtk::HBox 			date_dButton_c;

          Gtk::HBox 			s_name_c;
          Gtk::HBox 			s_type_c;
          Gtk::HBox 			s_datemod_c;
          Gtk::HBox 			s_artistname_c;

          Gtk::HBox 			s_artistnotes_c;

          Gtk::Frame 		so_preview_frame;
          Gtk::Frame 		artistnotes_frame;
          Gtk::Frame 		toolbox_frame;

          entry::control 	s_name_entry;
          entry::control 	s_type_entry;
          entry::control 	s_datemod_entry;
          entry::control 	s_artistname_entry;

          Gtk::Button 		insert_date_button;
          text::control 	s_artistnotes_mltext;

          k3d::string_t 	previewPath;

          //ToolBox Items
          Gtk::HBox				toolbox_c;
          Gtk::ComboBoxText 	pview_geo_combo;
          Gtk::ComboBoxText 	pview_engine_combo;
          Gtk::ToggleButton   pview_back_button;

        private:
          s_object *m_so;
          materialPreviewImage materialPreview;

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
            //Create The Material Tree Model
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

            //Group Buttons
            //Create A Signal Connection For Add Group Button
            add_group.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_add_button_button_clicked));

            //Create A Signal Connection For Remove Group Button
            remove_group.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_remove_button_button_clicked));

            //Delete Model Data
            m_model->clearModel();

            //Rebuild Model Data
            m_model->buildModel(m_document_state);

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

          //Rebuild Tree From Rebuilt Model
          build_tree();

        }//on_update

        void implementation::build_gui()
        {
          //Material Tree Setup
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

        void implementation::build_tree()
        {
          //Group Through Groups And Add To Tree
          std::list<s_group*>::const_iterator gIter = m_model->m_groups.begin();
          Gtk::TreeRow first_group_row;
          bool on_first_row = true;

          for(; gIter !=  m_model->m_groups.end(); gIter++)
            {
              Gtk::TreeRow row = *tree_model->append();
              row[m_columns.name] = (*gIter)->sg_name;
              row[m_columns.is_group] = true;
              row[m_columns.s_group_ptr] = (*gIter);
              row[m_columns.s_object_ptr] = 0;

              //Grab First Row For Groups
              if(on_first_row)
                {
                  first_group_row = row;
                  on_first_row = false;
                }

              //Go Through Materials/Other and and to tree under groups
              std::list<s_object*>::const_iterator sIter = (*gIter)->m_materials.begin();
              for(; sIter !=  (*gIter)->m_materials.end(); sIter++)
                {
                  Gtk::TreeModel::Row childrow = *(tree_model->append(row.children()));
                  childrow[m_columns.name] = (*sIter)->getName();
                  childrow[m_columns.is_group] = false;
                  childrow[m_columns.s_group_ptr] = 0;
                  childrow[m_columns.s_object_ptr] = (*sIter);
                  childrow[m_columns.icon] = quiet_load_icon((*sIter)->node->factory().name(), Gtk::ICON_SIZE_MENU);
                }//for		
            }//for

          //Set Selected Tree Row
          if(!on_first_row)
            tree_selection->select(first_group_row);	   

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
                  //Single Material Selected***

                  //Send Selection Signal For Other Panels
                  k3d::inode *selectedNode = (row->get_value(m_columns.s_object_ptr))->node;

                  k3d::selection::record selected_record = k3d::selection::make_record(selectedNode);

                  m_document_state.select(selected_record);

                  //Build The GUI Context
                  build_content_pane(row, false);
                }
            }//if

        }//on_tree_row_changed

        //Build the right Content Pane (GUI Stuff)
        void implementation::build_content_pane(Gtk::TreeModel::Row row, bool is_group)
        {
          //Check If Building Group Panel Or Material Object Panel
          if(is_group)
            {
              //Delete current obj (group or material gui) and create new group gui obj
              m_rpane_content = std::auto_ptr<content_pane>(new g_content_pane(&m_HPanedMain, row->get_value(m_columns.s_group_ptr), m_parent, &m_document_state));
            }
          else
            {
              //Delete current obj (group or material gui) and create new group gui obj
              m_rpane_content = std::auto_ptr<content_pane>(new so_content_pane(&m_HPanedMain, row->get_value(m_columns.s_object_ptr), m_parent, &m_document_state));

            }//endif

          //Build The Content Pane From Corrected Pointer
          m_rpane_content->buildPane();

        }//build_content_pane

        void implementation::on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
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
                s_group *groupPtr = 0;

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
                        s_object *newSObject = new s_object(groupPtr, *nodeIter, typeStr);

                        //Push The material Onto Groups List
                        groupPtr->addMaterial(newSObject); //check

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

          //Check If Material Added -> If So Update Panel Only If Group
          if(material_added)
            {
              Gtk::TreeModel::iterator iter = tree_selection->get_selected();
              if(iter) //If anything is selected
                {
                  Gtk::TreeModel::Row row = *iter;
                  if(row[m_columns.is_group])
                    build_content_pane(row, true); //Build The Content Pane From Corrected Pointer
   
                }//if
            }//if


        }//on_nodes_added

        void implementation::on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
        {
          //Flag For Possible Panel Update
          bool material_removed = false;

          s_group *grpPtr = 0;
          s_object *tmpSObj = 0;
          for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){

            //Check If Is A Material Type First
            if((*node)->factory().implements(typeid(k3d::imaterial)))
              {
                //Set Flag (Material Added) > Will Try To Update Panel
                material_removed = true;
              }

            Gtk::TreeIter row;
            return_if_fail(get_row(*node, row));
	    
            tmpSObj = row->get_value(m_columns.s_object_ptr);

            if(tmpSObj)
              grpPtr = tmpSObj->parent;
	      
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
                  if(row[m_columns.is_group])
                    build_content_pane(row, true); //Build The Content Pane From Corrected Pointer
            		
                }//if
            }//if

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
                  //Check If material Object and Not Group
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
        }//on_node_renamed

      //   bool implementation::on_node_selection(k3d::inode* const Node)
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
//               m_pConnection =  n_sig->connect_node_changed_signal(sigc::mem_fun(*this, &implementation::propertySignalRender));

//             //Change Tree Selection (which implicitly renders updated preview)

//           return result;

//         }//on_node_selection




        void implementation::on_add_button_button_clicked()
        {
          k3d::log() << " Add Button Pressed" << std::endl;

          //Create & Embed New Group
          s_group *newGroup = new s_group("New Group");
          m_model->m_groups.push_back(newGroup);

          //Add New Group To The Tree
          Gtk::TreeRow row = *tree_model->append();
          row[m_columns.name] = newGroup->sg_name;
          row[m_columns.is_group] = true;
          row[m_columns.s_group_ptr] = newGroup;
          row[m_columns.s_object_ptr] = 0;

        }//on_add_button_button_clicked



        void implementation::on_remove_button_button_clicked()
        {

          //Get The Currently Selected Row (Group)
          Gtk::TreeModel::iterator iter = tree_selection->get_selected();
          if(iter) //If anything is selected
            {
              Gtk::TreeModel::Row row = *iter;

              //Is It A Group?
              if(row[m_columns.is_group])
                {
		  
                  k3d::log() << " Remove Button Pressed... Will Delete Group (Eventually :) )" << std::endl;

                }//if
            }//if

        }//on_remove_button_button_clicked




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

          m_implementation = new mechanics::implementation(DocumentState, Parent);

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
        mechanics::implementation* m_implementation;
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
