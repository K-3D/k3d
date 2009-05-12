// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Implements the property_label::control, which provides a MVC UI for connecting properties in the DAG
		\author Tim Shead (tshead@k-3d.com)
*/

#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/menu.h>
#include <gtkmm/targetlist.h>
#include <gtk/gtkmain.h>

#include "document_state.h"
#include "icons.h"
#include "menu_item.h"
#include "messages.h"
#include "property_label.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>

#include <k3dsdk/color.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>

namespace k3d
{

namespace ngui
{

namespace property_label
{

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<property_widget::idata_proxy> Data) :
	base(Parent, Name, Data)
{
	set_name("k3d-property-label");

	const std::string label = m_data->property().property_label().empty() ? m_data->property().property_name() : m_data->property().property_label();
	const std::string description = m_data->property().property_description();

	Gtk::Label* gtk_label = new Gtk::Label(" " + label + " ");
	gtk_label->set_alignment(Gtk::ALIGN_LEFT);
	add(*Gtk::manage(gtk_label));

	if(!description.empty())
		set_tip(description);

	data_changed();
	m_data->document().document().pipeline().dependency_signal().connect(sigc::mem_fun(*this, &control::on_dependencies_changed));

	signal_button_press_event().connect(sigc::mem_fun(*this, &control::button_press_event));
	signal_button_release_event().connect(sigc::mem_fun(*this, &control::button_release_event));
}

void control::data_changed()
{
}

void control::on_dependencies_changed(const k3d::ipipeline::dependencies_t& Dependencies)
{
	if(Dependencies.count(&m_data->property()))
		data_changed();
}

} // namespace property_label

} // namespace ngui

} // namespace k3d

