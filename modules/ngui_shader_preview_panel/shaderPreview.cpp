/*
  Title:A Shader Preview Panel

  Details: Very rough shader preview plugin. Still alot more to do. 
			
  Author:Alex Curtis
  Contact:alx.curtis@gmail.com
*/

#include <gtkmm.h>

#include <k3d-i18n-config.h>

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtk/gtkmain.h>

#include <k3dsdk/iproperty_collection.h>
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

#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/types.h>

#include <gtkmm/drawingarea.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/system.h>
#include <k3dsdk/ngui/render.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/user_property_changed_signal.h>
#include <k3dsdk/signal_system.h>

#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/path.h>
#include <k3dsdk/share.h>
#include <k3dsdk/transform.h>

#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_ri.h>

#include <k3dsdk/log.h>
#include <iostream>

using namespace libk3dngui;

namespace module{

  namespace ngui{

    namespace shader_preview{

      namespace mechanics{

        typedef k3d::irender_camera_frame /*k3d::irender_camera_preview*/ rManEngine_t;
        typedef k3d::icamera camera_t;
        typedef k3d::inode /* k3d::ri::ilight*/ light_t;
        typedef k3d::inode /*k3d::ri::ilight_shader*/ lightShader_t;
        typedef k3d::inode /*k3d::itransform_sink*/ geo_t;

	const k3d::string_t shaderImgFile = "k3d_shader_preview.tiff";
	typedef std::vector<sigc::connection> connections_t;

        // [sPreviewModel]*********************************************************************************

        //Model Constants
        const k3d::uint_t dPSize = 120;

        class sPreviewModel{
        public:

          sPreviewModel();
          ~sPreviewModel();

          void setPreviewSize(const k3d::uint_t &s){pSize  = s;}
          void setPreviewEngine(rManEngine_t* const e){engineUsed = e;}
          void setPreviewImagePath(k3d::filesystem::path img){previewImagePath = img;}
          void setPreviewCamera(camera_t* const cam){currentCamera = cam;}
          void setPreviewGeo(geo_t* const geo){currentGeo = geo;}
          void setPreviewLight(light_t* const light){currentLight = light;}
          void setPreviewLShader(lightShader_t* const lshd){currentLShader = lshd;}

          k3d::uint_t getPreviewSize(){return pSize;}
          rManEngine_t* getPreviewEngine(){return engineUsed;}
          k3d::filesystem::path getPreviewImagePath(){return previewImagePath;}
          camera_t* getPreviewCamera(){return currentCamera;}
          geo_t* getPreviewGeo(){return currentGeo;}
          light_t* getPreviewLight(){return currentLight;}
          lightShader_t* getPreviewLShader(){return currentLShader;}
	 
        private:
          //Preview Dimensions
          k3d::uint_t pSize; 

          //Render Engine & Camera Used For Preview
          rManEngine_t *engineUsed;
          camera_t *currentCamera;

          //Geometry Used For Preview
          geo_t *currentGeo;

          //Light and Light Shader Used In Preview
          light_t *currentLight;
          lightShader_t *currentLShader;

          //File_io
          k3d::filesystem::path previewImagePath;

	public:
	  //Property Signal connections
	  connections_t propertyConnections;
	  sigc::signal<void, k3d::iunknown*> m_changed_signal;

        };//sPreviewModel
        
        
        sPreviewModel::sPreviewModel() :engineUsed(0)
	{
          //Default The PDimensions
          pSize = dPSize;

        }//sPreviewModel()

        sPreviewModel::~sPreviewModel()
	{
        }//~sPreviewModel

        // [/sPreviewModel]***************************************************************************


        // [implementation]***************************************************************************


        //The Shader Preview Image Drawer Class

        class shaderPreviewImage: public Gtk::DrawingArea{
        public:
          shaderPreviewImage(k3d::filesystem::path _imgPath);
          virtual ~shaderPreviewImage();
	  
	  //Function to be called on preview img update request signal
	  bool onUpdatePreview();

        public:
          k3d::filesystem::path imgFilePath;

        protected:
          //Override default signal handler:
          virtual bool on_expose_event(GdkEventExpose* event);

        };//shaderPreviewImage


        shaderPreviewImage::shaderPreviewImage(k3d::filesystem::path _imgPath)
	{
          imgFilePath = _imgPath;
	  
        }

        shaderPreviewImage::~shaderPreviewImage()
	{
        }

        bool shaderPreviewImage::on_expose_event(GdkEventExpose* event)
        {
          //Glib::FileError Exception catch needs putting in here. So if image dont exist load default image
          Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file(imgFilePath.native_filesystem_string());
          image->render_to_drawable(get_window(), get_style()->get_black_gc(),
                                    0, 0, 10, 10, image->get_width(), image->get_height(),
                                    Gdk::RGB_DITHER_NONE, 0, 0);

          return true;

        }//on_expose_even


        //**************


        class implementation : public libk3dngui::asynchronous_update{
        public:
          implementation(document_state& DocumentState) :
            m_document_state(DocumentState),
            m_model(new sPreviewModel),
            previewArea(0),
	    piIntervalUpdate(250)
	  {
		
            //Setup the Window
            m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            
            m_scrolled_window.add(preview_ctrl_c);

            //Preview Image File_IO Stuff (System Temp Directory)
            k3d::filesystem::path imgPath = k3d::system::get_temp_directory();
	    imgPath = imgPath / k3d::filesystem::generic_path(shaderImgFile);
	    m_model->setPreviewImagePath(imgPath);


            //Create A New Drawer Widget Using Path
            previewArea = new shaderPreviewImage(m_model->getPreviewImagePath());

            buildModel();

            buildGui();

            //Signal Creators***

            // Reset the Renderer combo contents anytime the document node collection changes
            m_document_state.document().nodes().add_nodes_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_renderNode_added));

            m_document_state.document().nodes().remove_nodes_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_renderNode_removed));

            m_document_state.document().nodes().rename_node_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_renderNode_renamed));

            renderCombo.signal_changed()
              .connect(sigc::mem_fun(*this, &implementation::onRenderComboSelect));

            geoCombo.signal_changed()
              .connect(sigc::mem_fun(*this, &implementation::onGeoComboSelect));

            sizeField.signal_activate()
              .connect(sigc::mem_fun(*this, &implementation::onSizeChange));

	    //Called By Signal System when node selection changed (redo preview render)
	    m_document_state.view_node_properties_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_node_selection));

	    //glib timer set that updates preview image every 0.25 seconds
	    // << Will Delete when panel closed
	    Glib::signal_timeout().connect(sigc::mem_fun(*this, &implementation::on_preview_update),
					   piIntervalUpdate);

	    m_model->m_changed_signal.connect(sigc::mem_fun(*this, &implementation::propertySignalRender));

            schedule_update();
          }

          virtual ~implementation()
	  {
            //Clean Up Dynamic Memory Allocation
            if(previewArea)
              delete previewArea;
          }

	  //Property Signal Change -> Render Init Wrapper
	  void propertySignalRender(k3d::iunknown* t){
	    this->renderPreview();
	  }

          /// Updates the contents of the control
          void on_update();

          //Build The Shader Preview GUI
          void buildGui(); 

          //(re)Build the model
          void buildModel();

	  //Render The Preview Image
	  void renderPreview();

          //Signal Responders
          void on_renderNode_added(const k3d::inode_collection::nodes_t& Nodes);
          void on_renderNode_removed(const k3d::inode_collection::nodes_t& Nodes);
          void on_renderNode_renamed(k3d::inode* const Node);

          void onRenderComboSelect();
          void onGeoComboSelect();

          void onSizeChange();
	  bool on_node_selection(k3d::inode* const Node);
	  
	  bool on_preview_update();

        public:

          //GTK Widgets
          Gtk::ScrolledWindow m_scrolled_window;
          Gtk::HBox preview_ctrl_c;
          Gtk::VBox ctrl_c;
	  Gtk::VBox preview_c;
          Gtk::Label renderLabel;
          Gtk::ComboBoxText renderCombo;
          Gtk::Label geoLabel;
          Gtk::ComboBoxText geoCombo;
          Gtk::Label sizeLabel;
          Gtk::HBox dim_c;
          Gtk::Entry sizeField;
	  Gtk::Frame previewFrame;
	  
	  //Preview Image Update Interval
	  const k3d::uint_t piIntervalUpdate;

          //Shader Image Preview Specifics
          shaderPreviewImage *previewArea;

          // Stores a reference to the owning document
          document_state& m_document_state;

          // Stores The Data Model
          std::auto_ptr<sPreviewModel> m_model;

          // Signal that will be emitted whenever this control should grab the panel focus
          sigc::signal<void> m_panel_grab_signal;

        };

        void implementation::buildGui()
	{
	  previewFrame.set_label("Preview Render:");
	  previewFrame.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

          previewArea->set_size_request(m_model->getPreviewSize(), m_model->getPreviewSize());

          //Pack the Image Preview into preview / control HBox
          preview_ctrl_c.pack_start(preview_c, false, false, 0);

	  preview_c.pack_start(previewFrame, false, true, 0);

	  previewFrame.add(*previewArea);

	  preview_ctrl_c.set_border_width(6);
          preview_ctrl_c.set_spacing(6);

          //Pack The Controls Container (VBox) Into preview / control HBox
          preview_ctrl_c.pack_start(ctrl_c, false, false, 2);
	  
          //Pack The Control container (VBox) with labels and controls
          ctrl_c.set_border_width(6);
          ctrl_c.set_spacing(6);      
   
          ctrl_c.pack_start(renderLabel, false, false, 0);
          renderLabel.set_size_request(-1, 15);
          renderLabel.set_justify(Gtk::JUSTIFY_CENTER);
          

          ctrl_c.pack_start(renderCombo, false, false, 0);
          renderCombo.set_size_request(-1, -1);

          ctrl_c.pack_start(geoLabel, false, false, 0);
          geoLabel.set_size_request(-1, 15);
          geoLabel.set_justify(Gtk::JUSTIFY_CENTER);
        

          ctrl_c.pack_start(geoCombo, false, false, 0);
          geoCombo.set_size_request(-1, -1);


	  //Pack the dimension container (HBox) into size_c
          ctrl_c.pack_start(dim_c, false, false, 0);

	  dim_c.set_homogeneous(false);
	  dim_c.pack_start(sizeLabel, false, false, 0);
          sizeLabel.set_size_request(-1, 18);
          sizeLabel.set_justify(Gtk::JUSTIFY_LEFT);
          sizeLabel.set_padding(0, 3);

          //Finally Pack The dimensions container
          dim_c.pack_start(sizeField, true, false, 0);  
	  sizeField.set_size_request(60, -1); 
	  sizeField.set_editable(true);

          //Fill Labels
          renderLabel.set_label(Glib::ustring("Renderer Used:"));
          geoLabel.set_label(Glib::ustring("Preview Geometry:"));
          sizeLabel.set_label(Glib::ustring("Preview Size: "));
  

        }//buildGui


        void implementation::buildModel()
	{
      
          //create required node objects

          //Light Shader Setup*******************

          m_model->setPreviewLShader(dynamic_cast<k3d::inode*>
                                     ( k3d::plugin::create("RenderManLightShader", 
                                                           m_document_state.document(), 
                                                           "sPreview::Light Shader")));

          k3d::property::set_internal_value(*(m_model->getPreviewLShader()), 
                                            "shader_path", k3d::share_path() /
                                            k3d::filesystem::generic_path("shaders/light/k3d_pointlight.sl"));



          //Light Setup*************************
          m_model->setPreviewLight(dynamic_cast<light_t*>
                                   (k3d::plugin::create("RenderManLight", 
                                                        m_document_state.document(), 
                                                        "sPreview::Light")));

          k3d::property::set_internal_value(*(m_model->getPreviewLight()), 
                                            "shader", m_model->getPreviewLShader());


          k3d::inode* light_transformation = k3d::set_matrix(*(m_model->getPreviewLight()), 
                                                             k3d::translation3D(k3d::point3(-20, 20, 30)));

     

          //Camera Setup************************
          m_model->setPreviewCamera(dynamic_cast<camera_t*>
                                    (k3d::plugin::create("Camera", 
                                                         m_document_state.document(), 
                                                         "sPreview::Camera")));
          const k3d::point3 origin = k3d::point3(0, 0, 0);
          const k3d::vector3 world_up = k3d::vector3(0, 0, 1);

          const k3d::point3 position = k3d::point3(0, 13, 0);
          const k3d::vector3 look_vector = origin - position;
          const k3d::vector3 right_vector = look_vector ^ world_up;
          const k3d::vector3 up_vector = right_vector ^ look_vector;

          k3d::inode* const camera_transformation 
            = k3d::set_matrix(*(m_model->getPreviewCamera()), 
                              k3d::view_matrix(look_vector, up_vector, position));
       
          camera_transformation->set_name("Camera Transformation");
          k3d::property::set_internal_value(*(m_model->getPreviewCamera()), 
                                            "world_target", k3d::point3(0, 0, 0));

          k3d::property::set_internal_value(*(m_model->getPreviewCamera()), 
                                            "viewport_visible", false);

          k3d::property::set_internal_value(*(m_model->getPreviewCamera()), 
                                            "aspect_ratio", k3d::string_t("Square"));

          //Geometry Setup**********************
          m_model->setPreviewGeo(dynamic_cast<geo_t*>
                                 (k3d::plugin::create("Sphere", 
                                                      m_document_state.document(), 
                                                      "sPreview::Geo::Sphere")));

          k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
                                            "render_shadows", false);

          k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
                                            "viewport_visible", false);

          //Renderman Engine Setup**************
          m_model->setPreviewEngine(dynamic_cast<rManEngine_t*>
                                    (k3d::plugin::create("RenderManEngine", 
                                                         m_document_state.document(), 
                                                         "sPreview::RenderManEngine")));

          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "enabled_lights", 
                                            k3d::inode_collection_property::nodes_t(1, m_model->getPreviewLight()));

          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "visible_nodes", 
                                            k3d::inode_collection_property::nodes_t(1, m_model->getPreviewGeo()));


          // To ensure Universal Compatibility With user documents, create a RMAN Node (Aqsis)
          k3d::ri::irender_engine* const aqsis = k3d::plugin::create<k3d::ri::irender_engine>("AqsisRenderManEngine", 
                                                                                              m_document_state.document(), 
                                                                                              "sPreview::Aqsis Renderer");

          //Setup the shader preview render engine*****
          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "render_engine", dynamic_cast<k3d::inode*>(aqsis));

          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "pixel_width", static_cast<k3d::int32_t>(m_model->getPreviewSize()));


          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "pixel_height", static_cast<k3d::int32_t>(m_model->getPreviewSize()));

          k3d::double_t aspectRatio = 1.0;

          k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "pixel_aspect_ratio", aspectRatio);

        }//buildModel

	//This invoked when node selection changes
	bool implementation::on_node_selection(k3d::inode* const Node)
	{
	  //Check If Node Is A RenderMan Material
	  if((Node)->factory().implements(typeid(k3d::ri::imaterial))){

	    //If it is, assign to current geometry as surface shader
	    k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
					      "material", Node);

	    //Clear Property Connections 
	    for(connections_t::iterator cIter = m_model->propertyConnections.begin(); cIter != m_model->propertyConnections.end(); cIter++)
	      cIter->disconnect();

	    //Create New Signal Connections
	    if(k3d::iproperty_collection* const m_collection = dynamic_cast<k3d::iproperty_collection*>(Node)){
      
	      const k3d::iproperty_collection::properties_t& properties = m_collection->properties();
	      k3d::iproperty_collection::properties_t::const_iterator property = properties.begin();

	      for(property; property != properties.end(); ++property)
		  m_model->propertyConnections.push_back((*property)->property_changed_signal().connect(m_model->m_changed_signal.make_slot()));

	    }//if

	    //Render A Preview Image
	    // renderPreview();

	  }//if

	  return false;

	}//on_node_selection

	//Focal Point when preview image needs rendering
	void implementation::renderPreview()
	{
	  //Render The Preview Using Selected External Renderer
	  (m_model->getPreviewEngine())
	    ->render_camera_frame(*(m_model->getPreviewCamera()), 
				  m_model->getPreviewImagePath(), false);
	  on_preview_update();

	}//renderPreview

	bool implementation::on_preview_update()
	{
	  //Update The Drawing Area Image
	  previewArea->queue_resize();
	  previewArea->queue_draw();

	  //Set Preview Frame Sizer including padding
	  k3d::uint_t pFrameSizer = m_model->getPreviewSize() + 22;
	  previewFrame.set_size_request(pFrameSizer, pFrameSizer + 12);

	}//on_preview_update

        void implementation::on_update()
	{
          //Repopulate the Model And GUI Elements
          renderCombo.clear();

	  previewArea->set_size_request(m_model->getPreviewSize(), m_model->getPreviewSize());

          sizeField.set_text(Glib::ustring(boost::lexical_cast<k3d::string_t>(m_model->getPreviewSize())));
 
	  //Update Combo Boxes (Renderer & Geometry)

	  //Following temp untill more important stuff done
	  k3d::inode *pGeo = m_model->getPreviewGeo();
	  renderCombo.append_text(Glib::ustring("sPreview::RenderManEngine"));
	  renderCombo.set_active_text(Glib::ustring("sPreview::RenderManEngine"));
	  geoCombo.append_text(Glib::ustring(pGeo->name()));
	  geoCombo.set_active_text(Glib::ustring(pGeo->name()));

	  k3d::uint_t pFrameSizer = m_model->getPreviewSize() + 22;

	  previewFrame.set_size_request(pFrameSizer, pFrameSizer + 12);
	}//on_update



        //Called by the signal system when the render combo box selection gets changed
        void implementation::onRenderComboSelect()
	{

         
        }//onRenderComboSelect


        void implementation::onGeoComboSelect()
	{


        }//onGeoComboSelect



        void implementation::onSizeChange()
	{

	  //Update model variable
	  m_model->setPreviewSize(boost::lexical_cast<k3d::int32_t>(sizeField.get_text()));

	  k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "pixel_width", static_cast<k3d::int32_t>(m_model->getPreviewSize()));
	  k3d::property::set_internal_value(*(m_model->getPreviewEngine()), 
                                            "pixel_height", static_cast<k3d::int32_t>(m_model->getPreviewSize()));

	  //Re-Render The Preview Image
	  renderPreview();

        }//onSizeChange



        // Called by the signal system anytime any new nodes are added to the document
	void implementation::on_renderNode_added(const k3d::inode_collection::nodes_t& Nodes)
	{

         
          
	}//on_renderNode_added


	// Called by the signal system anytime renderer nodes are removed from document
	void implementation::on_renderNode_removed(const k3d::inode_collection::nodes_t& Nodes)
	{

         
          
	}//on_renderNode_removed



	// Called by the signal system anytime a renderer node is renamed
	void implementation::on_renderNode_renamed(k3d::inode* const Node)
	{


        
	}//on_renderNode_renamed


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
          ui_component("shader_preview", 0),
          m_implementation(0)
	{
	}

	~panel()
	{
          delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
          ui_component::set_parent("shader_preview", &Parent);

          m_implementation = new mechanics::implementation(DocumentState);
          
          m_implementation->sizeField.signal_focus_in_event()
            .connect(sigc::bind_return(sigc::hide(m_implementation->m_panel_grab_signal.make_slot()), 
                                       false), false);

          pack_start(m_implementation->m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

          show_all();

	}

	const k3d::string_t panel_type()
	{
          return "shader_preview";
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
          return m_implementation->m_panel_grab_signal.connect(Slot);
	}


	static k3d::iplugin_factory& get_factory()
	{
          static k3d::application_plugin_factory<panel> 
            factory(
                    k3d::uuid(0xf11983c0, 0x974a039c, 0xc5d36bb9, 0xe65cf8a0),
                    "shaderPreview",
                    _("Shader Preview Panel"),
                    "NGUI Panels",
                    k3d::iplugin_factory::EXPERIMENTAL,
                    boost::assign::map_list_of("ngui:component-type", "panel")
                    ("ngui:panel-type", "shader_preview")("ngui:panel-label", "Shader Preview"));

          return factory;
	}

      private:
	mechanics::implementation* m_implementation;

      };

      // [/panel]***********************************************************************************


    } // namespace shader_preview

  } // namespace ngui

} // namespace module


//Register The Plugin (K-3D)************************************************************************
K3D_MODULE_START(Registry)
  Registry.register_factory(module::ngui::shader_preview::panel::get_factory());
K3D_MODULE_END
//**************************************************************************************************
