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

#include "DocumentUtilities.h"

#include <k3dsdk/imaterial_sink.h>
#include <k3dsdk/imesh_sink.h>

using namespace libk3dngui;

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{


bool checkDocForMeta(const k3d::string_t meta_tag, 
                     const k3d::string_t meta_data, 
                     k3d::inode **node_ptr, document_state *_document_state)

{
  //Iterate Through Document. If A Node Has Meta Data & Matches, Pass To node_ptr
  k3d::inode_collection::nodes_t::const_iterator node 
    = _document_state->document().nodes().collection().begin();

  for(; node != _document_state->document().nodes().collection().end(); ++node)
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
}

k3d::inode* get_material_sink(k3d::inode* Node, k3d::ipipeline& Pipeline)
{
	k3d::inode* material_sink = Node;
	if(!dynamic_cast<k3d::imaterial_sink*>(material_sink))
	{
		k3d::imesh_sink* mesh_sink = dynamic_cast<k3d::imesh_sink*>(Node);
		if(mesh_sink)
		{
			k3d::iproperty* material_sink_property = Pipeline.dependency(mesh_sink->mesh_sink_input());
			return_val_if_fail(material_sink_property, Node);
			material_sink = material_sink_property->property_node();
		}
	}
	if(!dynamic_cast<k3d::imaterial_sink*>(material_sink))
		k3d::log() << error << "get_material_sink: Failed to find a material sink" << std::endl;
	return material_sink;
}



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

