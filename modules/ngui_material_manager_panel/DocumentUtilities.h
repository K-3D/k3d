#ifndef MODULES_NGUI_MATERIAL_MANAGER_PANEL_DOCUMENTUTILITIES_H
#define MODULES_NGUI_MATERIAL_MANAGER_PANEL_DOCUMENTUTILITIES_H

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
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/share.h>
#include <k3dsdk/itransform_sink.h>

using namespace k3d::ngui;

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{



//Analyse Doc For Specific Embedded Meta Data In Nodes
bool checkDocForMeta(const k3d::string_t meta_tag, 
                     const k3d::string_t meta_data, 
                     k3d::inode **node_ptr, document_state *_document_state);

/// Gets the upstream node if the top node is not a material sink
k3d::inode* get_material_sink(k3d::inode* Node, k3d::ipipeline& Pipeline);



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif // !MODULES_NGUI_MATERIAL_MANAGER_PANEL_DOCUMENTUTILITIES_H

