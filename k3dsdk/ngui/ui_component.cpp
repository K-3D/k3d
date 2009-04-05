// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "interactive.h"
#include "ui_component.h"

#include <k3dsdk/command_tree.h>
#include <k3dsdk/icommand_tree.h>
#include <k3dsdk/result.h>

#include <gtkmm/tooltips.h>
#include <gtkmm/widget.h>

namespace k3d
{

namespace ngui
{

////////////////////////////////////////////////////////////////////////////////////////
// ui_component

ui_component::ui_component() :
	m_enable_recording(true)
{
}

ui_component::~ui_component()
{
}

void ui_component::record_command(const std::string& Command, const std::string& Arguments)
{
	return_if_fail(Command.size());

	if(m_enable_recording)
		k3d::command_tree().command_signal().emit(*this, k3d::icommand_node::COMMAND_INTERACTIVE, Command, Arguments);
}

void ui_component::enable_recording(bool Enable)
{
	m_enable_recording = Enable;
}

bool ui_component::recording_enabled()
{
	return m_enable_recording;
}

Gtk::Tooltips& ui_component::tooltips()
{
	static Gtk::Tooltips tips;
	return tips;
}

void ui_component::set_tip(const Glib::ustring& Tooltip)
{
	Gtk::Widget* const widget = dynamic_cast<Gtk::Widget*>(this);
	return_if_fail(widget);

	tooltips().set_tip(*widget, Tooltip);
}

const k3d::icommand_node::result ui_component::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "highlight")
	{
		Gtk::Widget* const widget = dynamic_cast<Gtk::Widget*>(this);
		return_val_if_fail(widget, RESULT_ERROR);
		return_val_if_fail(widget->is_visible(), RESULT_ERROR);

		interactive::highlight(*widget);
		return RESULT_CONTINUE;
	}

	return base::execute_command(Command, Arguments);
}

} // namespace ngui

} // namespace k3d

