// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3d-i18n-config.h>

#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/options.h>
#include <k3dsdk/ngui/widget_manip.h>

#include <k3dsdk/batch_mode.h>

#include <gtkmm/alignment.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

namespace k3d
{

namespace ngui
{

void message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

void warning_message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

void error_message(const std::string& Message, const std::string& SecondaryMessage)
{
	if(batch_mode())
		return;

	Gtk::MessageDialog dialog(Message, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage);
	dialog.run();
}

unsigned int query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options)
{
	return_val_if_fail(!batch_mode(), 0);

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

void nag_message(const std::string& Type, const k3d::ustring& Message, const k3d::ustring& SecondaryMessage)
{
	if(batch_mode())
		return;

	if(!options::nag(Type))
		return;

	Gtk::MessageDialog dialog(Message.raw(), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
	if(!SecondaryMessage.empty())
		dialog.set_secondary_text(SecondaryMessage.raw());

	Gtk::CheckButton show_message(_("Display this message again in the future"));
	show_message.set_active(true);
	show_message.show();

	Gtk::Alignment alignment(0.5, 0.5, 0, 0);
	alignment.add(show_message);
	alignment.show();

	dialog.get_vbox()->pack_start(alignment);

	dialog.run();

	options::enable_nag(Type, show_message.get_active());
}

bool_t cancel_plugin(iplugin_factory& Factory)
{
	if(batch_mode())
		return false;

	switch(Factory.quality())
	{
		case iplugin_factory::EXPERIMENTAL:
		{
			if(!options::nag("warn_experimental"))
				return false;

			Gtk::MessageDialog dialog(_("Use Experimental Plugin?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL, true);
			dialog.set_secondary_text(_("Experimental plugins are still under development and may be modified or removed at any time.  If this happens, documents that include them may become unusable."));

			Gtk::CheckButton show_message(_("Display this message again in the future"));
			show_message.set_active(true);
			show_message.show();

			Gtk::Alignment alignment(0.5, 0.5, 0, 0);
			alignment.add(show_message);
			alignment.show();

			dialog.get_vbox()->pack_start(alignment);

			dialog.set_default_response(Gtk::RESPONSE_CANCEL);

			const int response = dialog.run();

			options::enable_nag("warn_experimental", show_message.get_active());

			return response != Gtk::RESPONSE_OK;
		}
		case iplugin_factory::STABLE:
		{
			return false;
		}
		case iplugin_factory::DEPRECATED:
		{
			if(!options::nag("warn_deprecated"))
				return false;

			Gtk::MessageDialog dialog(_("Use Deprecated Plugin?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL, true);
			dialog.set_secondary_text(_("Deprecated plugins are scheduled for removal in future versions of K-3D.  When this happens, documents that include them may become unusable.  We strongly recommend against using deprecated plugins, unless you plan to keep a specific version of K-3D on-hand to maintain them."));

			Gtk::CheckButton show_message(_("Display this message again in the future"));
			show_message.set_active(true);
			show_message.show();

			Gtk::Alignment alignment(0.5, 0.5, 0, 0);
			alignment.add(show_message);
			alignment.show();

			dialog.get_vbox()->pack_start(alignment);

			dialog.set_default_response(Gtk::RESPONSE_CANCEL);

			const int response = dialog.run();

			options::enable_nag("warn_deprecated", show_message.get_active());

			return response != Gtk::RESPONSE_OK;
		}
	}
}

} // namespace ngui

} // namespace k3d

