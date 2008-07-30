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

#ifndef MATERIALOBJ_H
#define MATERIALOBJ_H

#include <k3dsdk/nodes.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/data.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/imaterial_ri.h>

#include <iostream>
#include <k3dsdk/log.h>

#include "MaterialGroup.h"

using namespace k3d::data;

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

//forward declaration for list of materials
class MaterialGroup;

class MaterialObj
{
 public:
  MaterialObj(MaterialGroup *_parent, k3d::inode *_node, k3d::string_t _s_type)
    :m_group_parent(_parent), 
    m_doc_node(_node), 
    m_name(init_value(_node->name())),
    m_type(init_value(_s_type)),
    m_datestamp(init_value(k3d::string_t(""))),
    m_artistname(init_value(k3d::string_t(""))),
    m_artistnotes(init_value(k3d::string_t("")))
      {    
      }
 
    ~MaterialObj()
      {	   
      }

 public:
 
    //Initialization Of Object Contents Beyond Initial Values
    void init();

    //Accessor Functions
    const k3d::string_t name() const		{return this->m_name.internal_value();}
    const k3d::string_t type() const		{return this->m_type.internal_value();}
    const k3d::string_t dateStamp() const	{return this->m_datestamp.internal_value();}
    const k3d::string_t artistName() const	{return this->m_artistname.internal_value();}
    const k3d::string_t artistNotes() const	{return this->m_artistnotes.internal_value();}  
    
 /*   m_data_t  nameData() const		{return this->m_name;}
    data_t* typeData() const		{return this->m_type;}
    data_t* dateStampData() const		{return this->m_datestamp;}
    data_t* artistNameData() const	{return this->m_artistname;}
     data_t* artistNotesData() const	{return this->m_artistnotes;} */
    
    const k3d::inode* docNode() const 		{return this->m_doc_node;}
    const k3d::string_t docNodeName() const	{return this->m_doc_node->factory().name();}
    MaterialGroup* groupParent() const	 	{return this->m_group_parent;}

    //Mutator Functions
    void setName(const k3d::string_t &_str)		{this->m_name.set_value(_str);}
    void setType(const k3d::string_t &_str)		{this->m_type.set_value(_str);}
    void setDateStamp(const k3d::string_t &_str)	{this->m_datestamp.set_value(_str);}
    void setArtistName(const k3d::string_t &_str)	{this->m_artistname.set_value(_str);}
    void setArtistNotes(const k3d::string_t &_str)	{this->m_artistnotes.set_value(_str);}
    void setDocNode(k3d::inode *_node_ptr) 		{this->m_doc_node = _node_ptr;}
    void setGroupParent(MaterialGroup *_gp_ptr) 	{this->m_group_parent = _gp_ptr;}

    
    //Material Profile Signal Event Handlers
    void onNameChange(k3d::ihint *_h);
    void onTypeChange(k3d::ihint *_h);
    void onDateStampChange(k3d::ihint *_h);
    void onArtistNameChange(k3d::ihint *_h);
    void onArtistNotesChange(k3d::ihint *h_);


    //Reset Content To Curent Saved Node Meta Data
    void loadFromMeta();  

    //Generate And Insert Todays Date Into Date Stamp Variable
    void insertTodaysDate();
    
    //Find Out If This Is A k3d::ri::imaterial (A K3d Material)
    bool isMaterial()
    {
    if(m_doc_node->factory().implements(typeid(k3d::ri::imaterial)))
    return true;  
    else 
    return false;
    }
   

 public:
    k3d_data(k3d::string_t, no_name, change_signal,
             no_undo, local_storage, no_constraint, 
             no_property, no_serialization) 						m_name;

    k3d_data(k3d::string_t, no_name, change_signal, 
             no_undo, local_storage, no_constraint, 
             no_property, no_serialization) 						m_type;

    k3d_data(k3d::string_t, no_name, change_signal, 
             no_undo, local_storage, no_constraint, 
             no_property, no_serialization) 						m_datestamp;

    k3d_data(k3d::string_t, no_name, change_signal, 
             no_undo, local_storage, no_constraint, 
             no_property, no_serialization) 						m_artistname;

    k3d::metadata_property
      <k3d_data(k3d::string_t, no_name, change_signal, 
                no_undo, local_storage, no_constraint, 
                no_property, no_serialization)> 					m_artistnotes;


    k3d::inode				*m_doc_node;
    MaterialGroup 	  		*m_group_parent;
    
    
    
	  
};//MaterialObj  



}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif


