#include "MaterialGroup.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

void MaterialGroup::clearGroup()
{
  std::list<MaterialObj*>::iterator materialIter = m_materials.begin();

  //Clean Up The List Of material Objects (MaterialObj)
  for(; materialIter != m_materials.end(); materialIter++)
      delete (*materialIter);
   
  //Ensure Clean List Storage 
  m_materials.clear();
}

  void MaterialGroup::addMaterial(const MaterialObj* mat)
  {
    m_materials.push_back(const_cast<MaterialObj*>(mat));
  }

  void MaterialGroup::removeMaterial(const MaterialObj* mat)
  {
    m_materials.remove(const_cast<MaterialObj*>(mat));
  }




}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
