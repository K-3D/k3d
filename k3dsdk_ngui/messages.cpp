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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application_state.h"
#include "widget_manip.h"

#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>

namespace libk3dngui
{

void message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(application_state::instance().batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

void warning_message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(application_state::instance().batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

void error_message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(application_state::instance().batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

unsigned int query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options)
{
	return_val_if_fail(!application_state::instance().batch_mode(), 0);

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, true);

	for(unsigned long i = 0; i != Options.size(); ++i)
		dialog.add_button(Options[i], i+1);

	if(DefaultOption)
		dialog.set_default_response(DefaultOption-1);
	else
		dialog.set_default_response(Options.size());

	dialog.set_position(Gtk::WIN_POS_CENTER);

	dialog.show_all();
	int result = dialog.run();
	if(Gtk::RESPONSE_DELETE_EVENT == result)
		result = 0;

	return result;
}

} // namespace libk3dngui

