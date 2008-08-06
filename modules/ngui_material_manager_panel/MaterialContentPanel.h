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

#ifndef MATERIALCONTENTPANEL_H
#define MATERIALCONTENTPANEL_H

#include <k3dsdk/ngui/entry.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/image_toggle_button.h>
#include <k3dsdk/ngui/text.h>
#include <k3dsdk/itransform_sink.h>

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

class MaterialContentPanel : public ContentPanel
  {
  public:

    MaterialContentPanel(Gtk::HPaned *_m_hpane, MaterialObj *_m_materialobj, 
                         k3d::icommand_node *_m_parent, document_state *_documentState)
      :ContentPanel(_m_hpane, _m_parent, _documentState), 
      m_materialobj(_m_materialobj),
      m_pview_frame("Preview Render:"), 
      m_artnotes_frame("Artist's Notes:"), 
      m_toolbox_frame("Toolbox:"),
      m_name_entry(*_m_parent, k3d::string_t("so_name_field"), 
                   entry::model(_m_materialobj->m_name), 0),
      m_type_entry(*_m_parent, k3d::string_t("so_type_field"), 
                   entry::model(_m_materialobj->m_type), 0),
      m_datemod_entry(*_m_parent, k3d::string_t("so_datestamp_field"), 
                      entry::model(_m_materialobj->m_datestamp), 0),
      m_artistname_entry(*_m_parent, k3d::string_t("so_artistname_field"), 
                         entry::model(_m_materialobj->m_artistname), 0),
      m_artistnotes_mltext(*_m_parent, k3d::string_t("so_artistnotes_mltxt"), 
                           text::model(_m_materialobj->m_artistnotes), 0)


      /* m_material_preview(k3d::system::get_temp_directory()  */
/*                       / k3d::filesystem::generic_path(m_single_imgfile)) */

        {
        }


      ~MaterialContentPanel()
        {
          m_timer_connection.disconnect();
        }


      //Initialization Of Object Contents Beyond Initial Values
      void init();

      //Attach Geometry To MaterialObj
      void matobjAttachGeo();


  public:
      
      //Build The GUI Panel
      void buildPanel();

      //Panel Specific Preview Render Mechanism
      void renderPreview();

      //Simply Calls renderPreview (Required For Group Render Update)
      void renderSinglePreview(k3d::inode *node);
      
      //Invoke A Preview Image Update
      bool updatePreviewImage();

      //Todays Date Button Event Handler
      void onDateButtonPressed()
      {
        //Insert Todays Date (mm/dd/yyyy) into date entry
        m_materialobj->insertTodaysDate();
      }

      //Switch Preview Geometry On Combo Select
      void onRenderComboSelect();

      //Find A Material From A Doc Node
      bool findMaterial(const k3d::inode *node)
      {
        if(m_materialobj->m_doc_node == node)
          return true;

        else
          return false;

      }

  private:

      //Main Panel GTK Widgets
      Gtk::Label 			m_name_label;
      Gtk::Label 			m_type_label;
      Gtk::Label 			m_datemod_label;
      Gtk::Label 			m_artistname_label;

      Gtk::VBox 			m_pview_editor_cont;
      Gtk::HBox 			m_pview_data_cont;
      Gtk::VBox 			m_data_toolbar_cont;
      Gtk::HBox 			m_label_data_cont;
      Gtk::VBox 			m_label_cont;
      Gtk::VBox 			m_data_cont;
      Gtk::VBox 			m_pview_cont;
      Gtk::HBox 			m_datebutton_cont;

      Gtk::HBox 			m_name_cont;
      Gtk::HBox 			m_type_cont;
      Gtk::HBox 			m_datemod_cont;
      Gtk::HBox 			m_artistname_cont;

      Gtk::HBox 			m_artistnotes_cont;

      Gtk::Frame 			m_pview_frame;
      Gtk::Frame 			m_artnotes_frame;
      Gtk::Frame 			m_toolbox_frame;

      entry::control 	m_name_entry;
      entry::control 	m_type_entry;
      entry::control 	m_datemod_entry;
      entry::control 	m_artistname_entry;

      Gtk::Button 		m_date_button;
      text::control 		m_artistnotes_mltext;

      //ToolBox Gtk Widgets
      Gtk::HBox			 	m_toolbox_cont;
      Gtk::ComboBoxText   	m_tool_geo_combo;
      Gtk::ToggleButton   	m_tool_gb_button;

  private:

      MaterialObj 			*m_materialobj;


  };//MaterialContentPanel




}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module

#endif
