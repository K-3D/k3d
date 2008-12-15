#include "PreviewTorus.h"
#include <k3dsdk/inode_collection_sink.h>

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{


void PreviewTorus::init(k3d::string_t _node_name, k3d::string_t _meta_nametag, k3d::irender_camera_frame* RenderEngine)
{
	k3d::inode* torus 
    = dynamic_cast<k3d::inode*>(k3d::plugin::create("Torus", 
                                                    m_document_state->document(), 
                                                     _node_name));
	m_doc_node = m_document_state->instantiate_mesh(torus);
	return_if_fail(m_doc_node);
	k3d::property::set_internal_value(*m_doc_node, "gl_painter", static_cast<k3d::inode*>(0));

  //Scale The Torus To Fit In Camera
  k3d::vector4 v1(0.7642975655067189, 0, 0, 0);
  k3d::vector4 v2(0, 0.63264167284287942, 0.42885345098536998, 5.5511151231257827e-17);
  k3d::vector4 v3(0, -0.42885345098536998, 0.63264167284287942, 0.75945015577859709);
  k3d::vector4 v4(0, 0, 0, 1);

  k3d::matrix4 torus_transform(v1, v2, v3, v4);
  k3d::inode* torus_transformation = k3d::set_matrix(*m_doc_node, torus_transform);

  //Create Meta Data
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata_value("PreviewCore::nametag", _meta_nametag);
  
  // Make the node visible
	k3d::inode_collection_sink* visible_nodes_sink = dynamic_cast<k3d::inode_collection_sink*>(RenderEngine);
	return_if_fail(visible_nodes_sink);
	k3d::make_visible(k3d::nodes_t(1, m_doc_node), *visible_nodes_sink);
}


void PreviewTorus::defaultInit()
{


}


}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
