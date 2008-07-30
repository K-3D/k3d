#include "MaterialObj.h"

namespace module
{
namespace ngui
{
namespace material_manager
{
namespace mechanics
{

void MaterialObj::init()
{
  //Get Any Availible Data (If There Is Any)
  loadFromMeta();

  //Drop Hint To Multiline Text Item
  m_artistnotes.set_metadata("k3d:property-type", "k3d:multi-line-text");

  //Change Signals For Data Variables
  m_name.changed_signal()
    .connect(sigc::mem_fun(*this, &MaterialObj::onNameChange));

  m_type.changed_signal()
    .connect(sigc::mem_fun(*this, &MaterialObj::onTypeChange));

  m_datestamp.changed_signal()
    .connect(sigc::mem_fun(*this, &MaterialObj::onDateStampChange));

  m_artistname.changed_signal()
    .connect(sigc::mem_fun(*this, &MaterialObj::onArtistNameChange));

  m_artistnotes.changed_signal()
    .connect(sigc::mem_fun(*this, &MaterialObj::onArtistNotesChange));

}//init


void MaterialObj::onNameChange(k3d::ihint *_h)
{
  m_doc_node->set_name(m_name.internal_value());
}
	  

void MaterialObj::onTypeChange(k3d::ihint *_h)
{
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata("materialManager::material_type", type());
}


void MaterialObj::onDateStampChange(k3d::ihint *_h)
{
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata("materialManager::material_datestamp", dateStamp());
}


void MaterialObj::onArtistNameChange(k3d::ihint *_h)
{
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata("materialManager::material_artistname", artistName());
}


void MaterialObj::onArtistNotesChange(k3d::ihint *h_)
{
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    metadata->set_metadata("materialManager::material_artistnotes", artistNotes());
}	


void MaterialObj::loadFromMeta()
{
  //Get Node Name
  m_name.set_value(m_doc_node->name());

  //Get Any Availible Meta Data (If There Is Any)
  if(k3d::imetadata* const metadata = dynamic_cast<k3d::imetadata*>(m_doc_node))
    {
      m_type.set_value(metadata->get_metadata()["materialManager::material_type"]);
      m_datestamp.set_value(metadata->get_metadata()["materialManager::material_datestamp"]);
      m_artistname.set_value(metadata->get_metadata()["materialManager::material_artistname"]);
      m_artistnotes.set_value(metadata->get_metadata()["materialManager::material_artistnotes"]);
    }//if
}

 
void MaterialObj::insertTodaysDate()
{
  //Insert Todays Date Into Date Entry
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  //Use String Stream To Convert Numerical Values To Strings
  std::stringstream stream_for_date;
  //Adding to values as they origionally represent amount from jan (jan = 0) and 1900 
  stream_for_date << (1 + timeinfo->tm_mon) << "/" << timeinfo->tm_mday << "/" << (1900 + timeinfo->tm_year);
  k3d::string_t current_date_str = stream_for_date.str();

  setDateStamp(current_date_str);
}


}//namespace mechanics

}//namespace material_manager

}//namespace ngui

}//namespace module
