#include "PreviewCube.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{


void PreviewCube::init(k3d::string_t _node_name, k3d::string_t _meta_nametag)
{
  //Create The Poly Cube
  m_doc_node 
    = dynamic_cast<k3d::inode*>(k3d::plugin::create("Torus", 
                                                    m_document_state->document(), 
                                                    _node_name));

  // //Create The Mesh Instance
//   mesh_instance = dynamic_cast<k3d::inode*>(k3d::plugin::create("MeshInstance", 
//                                                     m_document_state->document(), 
//                                                     "Preview Core::Geo::Cube Mesh Instance"));



//Scale The Torus To Fit In Camera
//   k3d::point4 v1(0.7642975655067189, 0, 0, 0);
//   k3d::point4 v2(0, 0.63264167284287942, 0.42885345098536998, 5.5511151231257827e-17);
//   k3d::point4 v3(0, -0.42885345098536998, 0.63264167284287942, 0.75945015577859709);
//   k3d::point4 v4(0, 0, 0, 1);

//   k3d::matrix4 torus_transform(v1, v2, v3, v4);
//   k3d::inode* torus_transformation = k3d::set_matrix(*m_doc_node, torus_transform);


  k3d::property::set_internal_value(*m_doc_node, 
                                    "render_shadows", false);
  
  k3d::property::set_internal_value(*m_doc_node, 
                                    "viewport_visible", false);

  //Create Meta Data
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata_value("PreviewCore::nametag", _meta_nametag);
}


void PreviewCube::defaultInit()
{


}

}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
