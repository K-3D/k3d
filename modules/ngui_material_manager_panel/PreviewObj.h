#ifndef MODULES_NGUI_MATERIAL_MANAGER_PANEL_PREVIEWOBJ_H
#define MODULES_NGUI_MATERIAL_MANAGER_PANEL_PREVIEWOBJ_H

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

#include <k3dsdk/nodes.h>
#include <k3dsdk/types.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/system.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/irender_camera_frame.h>


namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

using namespace k3d::ngui;

class PreviewObj
{
 public:

  PreviewObj(k3d::string_t _combo_value, document_state *_document_state)
    :m_doc_node(0),
    m_document_state(_document_state)
    {
    }

  virtual ~PreviewObj()
    {
    }


 public:

  //Abstract Functions To Be Used By Group & Profile Derivatives
  virtual void init(k3d::string_t _node_name, k3d::string_t _meta_nametag, k3d::irender_camera_frame* RenderEngine) = 0;

  //Create The Default Preview Object For Material Preview (Preview Core::...)
  virtual void defaultInit() = 0;

 public:
    k3d::inode 		*m_doc_node;
    k3d::inode			*mesh_instance;

    document_state 	*m_document_state;

    //Preview Geometry MetaTag
    static const k3d::string_t pview_geo_nametag_mt;

    //Preview Object MetaData
    static const k3d::string_t sphere_md; 
    static const k3d::string_t cube_md; 
    static const k3d::string_t torus_md;


    //Preview Object Node Name
    static const k3d::string_t sphere_node_name; 
    static const k3d::string_t cube_node_name; 
    static const k3d::string_t torus_node_name;

};//PreviewObj



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif // !MODULES_NGUI_MATERIAL_MANAGER_PANEL_PREVIEWOBJ_H

