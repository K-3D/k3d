#ifndef MODULES_NGUI_MATERIAL_MANAGER_PANEL_CONTENTPANEL_H
#define MODULES_NGUI_MATERIAL_MANAGER_PANEL_CONTENTPANEL_H

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

#include <k3dsdk/types.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/system.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/ilight_ri.h>
#include <k3dsdk/ilight_shader_ri.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/share.h>
#include <k3dsdk/itransform_sink.h>

#include <gtkmm.h>

#include <sstream>

#include "PreviewTorus.h"
#include "PreviewSphere.h"
#include "PreviewCube.h"

#include "MaterialObj.h"

#include "DocumentUtilities.h"

using namespace k3d::ngui;

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

class ContentPanel
{
 public:

  //Type Definitions
  typedef k3d::irender_camera_frame rManEngine_t;
  typedef k3d::icamera camera_t;
  typedef k3d::inode light_t;
  typedef k3d::inode lightShader_t;
  typedef k3d::inode geo_t;

  ContentPanel(Gtk::HPaned *_m_hpane, k3d::icommand_node *_m_parent, 
               document_state *_documentState)
    :m_hpane(_m_hpane), 
    m_parent(_m_parent), 
    m_document_state(_documentState),
    m_pview_size(200), m_single_imgfile("singlePreviewRender"),
    m_multi_imgfile("multiPreviewRender")    
      {
        //Get Instance Number From This Value & Mark Render Images Accordingly
        std::stringstream stream_for_id;
        stream_for_id << this;
        k3d::string_t instance_id = stream_for_id.str();

        //Edit Render File Names To Represent Unique Files
        m_multi_imgfile.append(instance_id);
        m_single_imgfile.append(instance_id);
        m_single_imgfile += k3d::string_t(".png");

        //Setup The Rendering Components Using Meta Data
        createPreviewNodes(); 
      }
	  
      
    virtual ~ContentPanel()
      {
        //Clean Up Used Geometry
        std::list<PreviewObj*>::iterator geo_iter
          = m_used_geometry.begin();

        for(; geo_iter != m_used_geometry.end(); geo_iter++)
          delete (*geo_iter);    
      }

 public:

    //Analyse Doc For Preview Nodes And Create As Appropriate
    void createPreviewNodes();

    //Analyse Doc For Embedded Meta Data In Nodes
    /* bool checkDocForMeta(const k3d::string_t meta_tag,  */
/*                          const k3d::string_t meta_data,  */
/*                          k3d::inode **node_ptr); */

    //Abstract Functions To Be Used By Group & Profile Derivatives
    virtual void renderPreview() 				  				= 0;
    virtual void renderSinglePreview(k3d::inode *node) 	= 0;
    virtual bool updatePreviewImage() 							= 0;
    virtual void buildPanel() 									= 0;

    //Initialization Of Object Contents Beyond Initial Values
    virtual void init()												= 0;

    //Attach Geometry To MaterialObj
    virtual void matobjAttachGeo()								= 0;

    //Generic Render Initialization
    void renderInit();

    //Turn Render Engine Alpha On Or Off
    void rEngineAlpha(k3d::bool_t toggle, MaterialObj *mat);

    //Render Preview Background Checker
    bool checkPviewBackground(MaterialObj *mat);

    //Find A Material From A Doc Node
    virtual bool findMaterial(const k3d::inode *node) 	= 0;

    //Accessor Functions
    //const k3d::string_t name() const		{return this->m_name.internal_value();}

 protected:

    //Main Panel GTK Widgets
    Gtk::HPaned 				*m_hpane;
    Gtk::ScrolledWindow 	m_scrolled_window;

    //Preview Update Timer Connection
    sigc::connection 		m_timer_connection;

    //Document Info
    k3d::icommand_node 		*m_parent;
    document_state 			*m_document_state;
   
    //Initial Preview File Name ( + unique val + ext)
    k3d::string_t 			m_single_imgfile;	  
    k3d::string_t 			m_multi_imgfile;
	  

 protected:

    //Render Preview Components
    light_t 			*m_main_light;
    light_t 			*m_fill_light;
    light_t 			*m_back_light;

    lightShader_t 	*m_light_shader;
    lightShader_t 	*m_fill_light_shader;

    //NOTE: Back Uses Key Light Shader

    geo_t 				*m_geometry;	//Current Selected Geometry
    rManEngine_t 		*m_engine;
    camera_t 			*m_camera;

    //Size Of Render Preview
    k3d::uint_t 		m_pview_size;

    //All Used Geometry
    std::list<PreviewObj*> m_used_geometry;

   
};//ContentPanel



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif // !MODULES_NGUI_MATERIAL_MANAGER_PANEL_CONTENTPANEL_H

