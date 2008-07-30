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

#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <k3dsdk/types.h>

#include<list>

#include "MaterialObj.h"

using std::list;

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

//forward declaration for material object
class MaterialObj;


class MaterialGroup
{
 public:
  MaterialGroup(k3d::string_t _sg_name)
    :m_name(_sg_name)
    {
    }

  ~MaterialGroup()
    {
      //Clean Up All Shader Children
      clearGroup();
    }

 public:

  //Accessor Functions
  std::list<MaterialObj*>::const_iterator materialBegin()
    {
      return m_materials.begin();
    }

  std::list<MaterialObj*>::const_iterator materialEnd()
    {
      return m_materials.end();
    }
    

  const k3d::string_t name()			{return this->m_name;}

  //Mutator Functions
  void addMaterial(const MaterialObj* mat);
  void removeMaterial(const MaterialObj* mat);

  void clearGroup();

 private:

  std::list<MaterialObj*>     	m_materials;
  k3d::string_t            	m_name;

};//MaterialGroup





}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif
