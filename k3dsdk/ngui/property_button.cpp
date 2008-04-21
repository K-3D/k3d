// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

/** \file
		\brief Implements the property_button::control, which provides a MVC UI for connecting properties in the DAG
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gtkmm/image.h>
#include <gtkmm/menu.h>
#include <gtkmm/targetlist.h>
#include <gtk/gtkmain.h>

#include "document_state.h"
#include "icons.h"
#include "interactive.h"
#include "menu_item.h"
#include "messages.h"
#include "property_button.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>

#include <k3dsdk/color.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>

namespace libk3dngui
{

namespace property_button
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<property_widget::idata_proxy> Data) :
	base(Parent, Name, Data),
	m_image(new Gtk::Image())
{
	set_name("k3d-property-button");
	set_tip(_("LMB-Click to make connections with other properties. RMB-Click for context menu."));

	add(*manage(m_image));

	data_changed();
	m_data->document().document().pipeline().dependency_signal().connect(sigc::mem_fun(*this, &control::on_dependencies_changed));

	signal_button_press_event().connect(sigc::mem_fun(*this, &base::button_press_event));
	signal_button_release_event().connect(sigc::mem_fun(*this, &base::button_release_event));
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "activate")
	{
		interactive::activate(*this);
		return RESULT_CONTINUE;
	}

	return base::execute_command(Command, Arguments);
}

void control::data_changed()
{
	static const Glib::RefPtr<Gdk::Pixbuf> connected = load_icon("connected_plug", Gtk::ICON_SIZE_BUTTON);
	static const Glib::RefPtr<Gdk::Pixbuf> disconnected = load_icon("plug_tool", Gtk::ICON_SIZE_BUTTON);

	if(m_data->document().document().pipeline().dependency(m_data->property()))
		m_image->set(connected);
	else
		m_image->set(disconnected);
}

void control::on_dependencies_changed(const k3d::ipipeline::dependencies_t& Dependencies)
{
	if(Dependencies.count(&m_data->property()))
		data_changed();
}

} // namespace property_button

} // namespace libk3dngui


