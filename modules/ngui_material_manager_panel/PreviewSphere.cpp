#include "PreviewSphere.h"
#include <k3dsdk/inode_collection_sink.h>

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{


void PreviewSphere::init(k3d::string_t _node_name, k3d::string_t _meta_nametag, k3d::irender_camera_frame* RenderEngine)
{
  k3d::inode* sphere
    = dynamic_cast<k3d::inode*>(k3d::plugin::create("Sphere", 
                                                    m_document_state->document(), 
                                                    _node_name));
  m_doc_node = m_document_state->instantiate_mesh(sphere);
  return_if_fail(m_doc_node);
  k3d::property::set_internal_value(*m_doc_node, "gl_painter", static_cast<k3d::inode*>(0));
    
  //Create Meta Data
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata_value("PreviewCore::nametag", _meta_nametag);
  
  // Make the node visible
  k3d::inode_collection_sink* visible_nodes_sink = dynamic_cast<k3d::inode_collection_sink*>(RenderEngine);
  return_if_fail(visible_nodes_sink);
  k3d::make_visible(k3d::nodes_t(1, m_doc_node), *visible_nodes_sink);
}


void PreviewSphere::defaultInit()
{


}



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
