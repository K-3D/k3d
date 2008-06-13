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

#include <k3dsdk/ngui/spin_button.h>

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
#include <k3dsdk/property_collection.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/user_property_changed_signal.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/inode_change_signal.h>

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

#include <k3dsdk/data.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/measurement.h>

#include <k3dsdk/log.h>
#include <iostream>

using namespace libk3dngui;
using namespace k3d::data;

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
	const k3d::string_t holderImgFile = "sp_noShaderSelected.png";
	const k3d::string_t initRManEngine = "sPreview::RenderManEngine";

	typedef std::vector<sigc::connection> connections_t;

        // [sPreviewModel]*********************************************************************************
	
	//Model Constants
        const k3d::uint_t dPSize = 120;


	class pSizeModel : public spin_button::imodel{
	  pSizeModel()
	  {
	    //Set Default Value
	    this->set_value(dPSize);
	  }

	  virtual ~pSizeModel()
	  {
	  }

	  virtual void set_value (const k3d::double_t Value)
	  {
	    pSize = Value;
	  }

	public:
	  //Preview Dimensions
	  /* k3d::uint_t*/ k3d::double_t pSize;
 
	};

        class sPreviewModel{
        public:
          sPreviewModel() 
	    :engineUsed(0)	     
	  {
	    //Default The PDimensions
	    pSize = dPSize;
	  }

          ~sPreviewModel()
	  {
	  }

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
	 
	  void addRmanEngine(rManEngine_t* e){availibleEngines.push_back(e);}
	  void removeRmanEngine(rManEngine_t* e){availibleEngines.remove(e);}

	  void addGeo(geo_t* g){availibleGeo.push_back(g);}
	  void removeGeo(geo_t* g){availibleGeo.remove(g);}
	  
	  //Find Availible RMan Engine Using Ptr
	  bool findRmanEngine(rManEngine_t* e)
	  {
	    std::list<rManEngine_t*>::const_iterator iter = availibleEngines.begin();
	    for(; iter != availibleEngines.end(); iter++){
	      if((*iter) == e)
		return true;
	    }//for

	    return false; //No RMan Engine Found
	  }

	  //Find RenderMan Engine Based On Name
	  rManEngine_t* findRmanEngine(k3d::string_t str)
	  {
	    std::list<rManEngine_t*>::const_iterator iter = availibleEngines.begin();
	    for(; iter != availibleEngines.end(); iter++){
	      k3d::inode* rmanEngineNode = dynamic_cast<k3d::inode*>(*iter);
	      if(rmanEngineNode->name() == str)
		{
		  return (*iter);
		}//if
	    }//for

	    return 0;
	  }

	  //Used For Node Delete And List Reset
	  rManEngine_t* getFirstRManEngine()
	  {
	    return (availibleEngines.front());
	  }

	  //Create Copies Of PRIVATE Containers
	  std::list<rManEngine_t*> getAvailibleRManEngines(){
	    return availibleEngines;
	  }

	  std::list<geo_t*> getAvailibleGeo(){
	    return availibleGeo;
	  }

          //Preview Dimensions
	  k3d::uint_t pSize; 
	  

	private:
	  //List Of Availible RMan Engines
	  std::list<rManEngine_t*> availibleEngines;

	  //List Of Availible Geometry
	  std::list<geo_t*> availibleGeo;

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
	  //Property Signal connection
	  sigc::connection m_pConnection;

	  connections_t propertyConnections;
	  sigc::signal<void, k3d::iunknown*> m_changed_signal;

        };//sPreviewModel

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
	  k3d::filesystem::path imgHolderPath;

        protected:
          //Override default signal handler:
          virtual bool on_expose_event(GdkEventExpose* event);

        };//shaderPreviewImage


        shaderPreviewImage::shaderPreviewImage(k3d::filesystem::path _imgPath)
	{
          imgFilePath = _imgPath;

	  //Define Location of image holder (if no render preview file found)
	  imgHolderPath = k3d::share_path() / k3d::filesystem::generic_path("ngui/rasterized") 
	    / k3d::filesystem::generic_path(holderImgFile);
        }

        shaderPreviewImage::~shaderPreviewImage()
	{
        }

        bool shaderPreviewImage::on_expose_event(GdkEventExpose* event)
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

        }//on_expose_even


        //**************


        class implementation : public libk3dngui::asynchronous_update{
        public:
          implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
            m_document_state(DocumentState),
            m_model(new sPreviewModel),
            previewArea(0),
	    piIntervalUpdate(250)
	    //size_k3dField(Parent, k3d::string_t("psize_field"),spin_button::model(property),) //Init pSize Spin GUI Widget
	  {
		
            //Setup the Window
            m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
            
            m_scrolled_window.add(preview_ctrl_c);

            //Preview Image File_IO Stuff (System Temp Directory)
            k3d::filesystem::path imgPath = k3d::system::get_temp_directory();
	    imgPath = imgPath / k3d::filesystem::generic_path(shaderImgFile);
	    m_model->setPreviewImagePath(imgPath);
	    
	    //Ensure Preview Image Is Deleted From File System
	    deletePImage();

            //Create A New Drawer Widget Using Path
            previewArea = new shaderPreviewImage(m_model->getPreviewImagePath());

            buildModel();

            buildGui();

            //Signal Creators***

            // Reset the Renderer combo contents anytime the document node collection changes
            m_document_state.document().nodes().add_nodes_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_node_added));

            m_document_state.document().nodes().remove_nodes_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_node_removed));

            m_document_state.document().nodes().rename_node_signal()
              .connect(sigc::mem_fun(*this, &implementation::on_node_renamed));

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

	    //Find if any nodes selected when panel launches
	    k3d::inode_collection::nodes_t::const_iterator nodeIter = m_document_state.document().nodes().collection().begin();
	    bool selectedResult = false;
	    
	    for(nodeIter; nodeIter != m_document_state.document().nodes().collection().end(); ++nodeIter){
	      
	      //Check For Existing RManEngine Nodes
	      if((*nodeIter)->factory().implements(typeid(k3d::irender_camera_frame))){
		rManEngine_t* rManEngineNode = dynamic_cast<rManEngine_t*>(*nodeIter);
		if(rManEngineNode)
		  m_model->addRmanEngine(rManEngineNode);
		
	      }//if

	      //Check if selected
	      selectedResult = m_document_state.is_selected((*nodeIter));
	      if(selectedResult){
	     
		//Call Check & Render on selected node
		bool isMat = on_node_selection((*nodeIter));
		//if(isMat)
		  // break;

	      }//if

	    }//for


	    

	    k3d::istate_recorder* const state_recorder = &m_document_state.document().state_recorder();

	    //k3d::iproperty *test = k3d::property::create(0, typeid(k3d::uint_t), "test", "this_test", "description_test", dPSize);
	    
	    //spin_button::control size_k3dField(Parent, k3d::string_t("test"), spin_button::model(m_model->m_pSize), state_recorder);



            schedule_update();
          }

          virtual ~implementation()
	  {
            //Clean Up Dynamic Memory Allocation
            if(previewArea)
              delete previewArea;

	    //Remove Preview Render from tmp dir
	    deletePImage();

	    //Disconnect Any Existing Connection With Properties
	    m_model->m_pConnection.disconnect();

	    //Remove Required Panel Document Nodes

          }

	  //Property Signal Change -> Render Init Wrapper
	  void propertySignalRender(k3d::iunknown* t){
	    this->renderPreview();
	  }

	  //Delete Preview Image From FileSystem
	  void deletePImage(){
	    //Remove Preview Render from tmp dir
	    k3d::filesystem::remove(m_model->getPreviewImagePath());
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
          void on_node_added(const k3d::inode_collection::nodes_t& Nodes);
          void on_node_removed(const k3d::inode_collection::nodes_t& Nodes);
          void on_node_renamed(k3d::inode* const Node);

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

	  //spin_button::control size_k3dField;

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
	  //Add All Geometry To m_model
	  m_model->addGeo(m_model->getPreviewGeo());

          k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
                                            "render_shadows", false);

          k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
                                            "viewport_visible", false);

          //Renderman Engine Setup**************
          m_model->setPreviewEngine(dynamic_cast<rManEngine_t*>
                                    (k3d::plugin::create("RenderManEngine", 
                                                         m_document_state.document(), 
							 initRManEngine)));

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

	  //return result (if RMAN Material)
	  bool result = false;

	  //Check If Node Is A RenderMan Material
	  if((Node)->factory().implements(typeid(k3d::ri::imaterial))){

	    //It Is A RMAN Material!
	    result = true;

	    //If it is, assign to current geometry as surface shader
	    k3d::property::set_internal_value(*(m_model->getPreviewGeo()), 
					      "material", Node);

	    //Disconnect Any Existing Connection With Properties
	    m_model->m_pConnection.disconnect();

	    //Create Connection For Node Change
	    k3d::inode_change_signal *n_sig = dynamic_cast<k3d::inode_change_signal*>(Node);

	    if(n_sig)
	      m_model->m_pConnection =  n_sig->connect_node_changed_signal(sigc::mem_fun(*this, &implementation::propertySignalRender));

	    //Render A Preview Image
	    renderPreview();

	  }//if

	  return result;

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
	  std::list<rManEngine_t*> enginesCopy =  m_model->getAvailibleRManEngines();

	  std::list<rManEngine_t*>::const_iterator engineIter = enginesCopy.begin();
	  for(; engineIter != enginesCopy.end(); engineIter++)
	    renderCombo.append_text((dynamic_cast<k3d::inode*>(*engineIter))->name());

	  //This Will Work As Always Initial RMAN Engine Is One Created
	  renderCombo.set_active_text(initRManEngine);

	  std::list<geo_t*> geoCopy = m_model->getAvailibleGeo();
	  std::list<geo_t*>::const_iterator geoIter = geoCopy.begin();
	  for(; geoIter != geoCopy.end(); geoIter++)
	    geoCombo.append_text((dynamic_cast<k3d::inode*>(*geoIter))->name());

	  //Set Geo Active Text To Current Geo Selection
	  k3d::inode *pGeo = m_model->getPreviewGeo();
	  geoCombo.set_active_text(Glib::ustring(pGeo->name()));

	  k3d::uint_t pFrameSizer = m_model->getPreviewSize() + 22;

	  previewFrame.set_size_request(pFrameSizer, pFrameSizer + 12);
	}//on_update



        //Called by the signal system when the render combo box selection gets changed
        void implementation::onRenderComboSelect()
	{
	  //Find RenderMan Engine Based On Name
	 rManEngine_t* selectedRNode =  m_model->findRmanEngine(renderCombo.get_active_text());
	 if(selectedRNode){
	   m_model->setPreviewEngine(selectedRNode);
	   //renderPreview();
	 }//if
         
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
	void implementation::on_node_added(const k3d::inode_collection::nodes_t& Nodes)
	{
	  for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){
	    if((*node)->factory().implements(typeid(k3d::irender_camera_frame))){
	      
	      //Add To List Of Render Engines In Model
	      m_model->addRmanEngine(dynamic_cast<rManEngine_t*>(*node));

	      //Add To Combo GUI Widget
	      renderCombo.append_text((*node)->name());

	    }//if
	  }//for
        
	}//on_node_added


	// Called by the signal system anytime renderer nodes are removed from document
	void implementation::on_node_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
	  for(k3d::inode_collection::nodes_t::const_iterator node = Nodes.begin(); node != Nodes.end(); ++node){
	    //Check If RManEngine Is Known By m_model
	    rManEngine_t *rmanNode = dynamic_cast<rManEngine_t*>(*node);
	    bool result = m_model->findRmanEngine(rmanNode);
	    if(result){

	      //Remove RMan Node From m_model
	      m_model->removeRmanEngine(rmanNode);

	      //If Selected Is To Be Deleted Then Move To First Element in List
	      if((*node)->name() == renderCombo.get_active_text()){

		k3d::inode* rEngineNode = dynamic_cast<k3d::inode*>(m_model->getFirstRManEngine());
		renderCombo.set_active_text(rEngineNode->name());
		m_model->setPreviewEngine(m_model->getFirstRManEngine());

	      }//if

	      //Remove Node From GUI Widget
	      renderCombo.remove_text((*node)->name());

	    }//if

	  }//for

	}//on_node_removed



	// Called by the signal system anytime a renderer node is renamed
	void implementation::on_node_renamed(k3d::inode* const Node)
	{
	  //NEED TO CHANGE THIS TO BE MORE ROBUST.. PROBLEM: CHANGES SELECTED TO DEFAULT UPON RENAME
	  schedule_update();
        
	}//on_node_renamed


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

          m_implementation = new mechanics::implementation(DocumentState, Parent);
          
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
