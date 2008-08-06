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

#ifndef GROUPCONTENTPANEL_H
#define GROUPCONTENTPANEL_H

#include <gtkmm/textview.h>

#include <k3dsdk/ngui/text.h>
#include <k3dsdk/itransform_sink.h>

#include <sstream>

#include "ContentPanel.h"
#include "MaterialObj.h"
#include "RenderedImage.h"


namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

class GroupContentPanel : public ContentPanel
{
 public:
  GroupContentPanel(Gtk::HPaned *_m_hpane, MaterialGroup *_m_materialgrp, 
                    k3d::icommand_node *_m_parent, document_state *_documentState)
    :ContentPanel(_m_hpane, _m_parent, _documentState), m_materialgrp(_m_materialgrp)
    {
    }

    ~GroupContentPanel();

    //Initialization Of Object Contents Beyond Initial Values
    void init();

    //Attach Geometry To MaterialObj
    void matobjAttachGeo();

 public:

    //Build The GUI Panel
    void buildPanel();

    //Panel Specific Preview Render Mechanism (ALL)
    void renderPreview();

    //Renders One Of The Materials In Group
    void renderSinglePreview(k3d::inode *node);

    //Invoke A Preview Image Update
    bool updatePreviewImage();

    //Find A Material From A Doc Node
    bool findMaterial(const k3d::inode *node);

    //Find A Material From A MaterialObj Pointer
    bool findMaterial(const MaterialObj *mat);




 private:
          
    //GTK Widget Containers
    Gtk::VBox 										m_materials_cont;
    Gtk::VBox 										m_toolbar_main_cont;
    std::vector<Gtk::HBox *> 					m_pview_data_conts;
    std::vector<Gtk::Frame *> 	  			m_pview_frames;
    std::vector<Gtk::VBox *> 					m_data_desc_conts;
    std::vector<Gtk::HBox *> 					m_label_data_conts;
    std::vector<Gtk::VBox *> 					m_label_conts;
    std::vector<Gtk::VBox *> 					m_data_conts;

    //Gtk Label Widgets
    std::vector<Gtk::Label*> 					m_name_labels;
    std::vector<Gtk::Label*> 					m_type_labels;
    std::vector<Gtk::Label*> 					m_datemod_labels;
    std::vector<Gtk::Label*> 					m_artistname_labels;
    std::vector<Gtk::Label*> 					m_name_data_labels;
    std::vector<Gtk::Label*> 					m_type_data_labels;
    std::vector<Gtk::Label*> 					m_datemod_data_labels;
    std::vector<Gtk::Label*> 					m_artistname_data_labels;

    //Gtk Widgets For Artist Notes View
    std::vector<Gtk::TextView*> 				m_artistnotes_mltext;
    std::vector<Gtk::ScrolledWindow*> 		m_artistnotes_scrollwin;

    //Gtk Seperators For Visual Clarity
    std::vector<Gtk::HSeparator*> 			m_data_notes_seps;
    std::vector<Gtk::HSeparator*> 			m_materials_seps;
    std::vector<Gtk::VSeparator*> 			m_label_data_seps;

    //Gtk Toolbar & Toolbar Contents
    Gtk::Toolbar 									m_toolbar; 

    //The Render Preview Images
    std::vector<RenderedImage*> 				m_material_pviews;

 private:

    MaterialGroup 								*m_materialgrp;


};//GroupContentPanel


}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif
