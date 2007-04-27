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

#include "file_chooser_dialog.h"
#include "messages.h"

#include <k3dsdk/i18n.h>
#include <k3dsdk/options.h>
#include <k3dsdk/result.h>
#include <k3dsdk/share.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/system.h>

#include <boost/format.hpp>

#include <gtkmm/checkbutton.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/image.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// file_chooser_dialog

file_chooser_dialog::file_chooser_dialog(const Glib::ustring& Title, const std::string& PathType, Gtk::FileChooserAction Action, const k3d::filesystem::path& InitialPath) :
	base(Title, Action),
	path_type(PathType),
	initial_path(InitialPath),
	compression(false),
	extra_widget_container(new Gtk::VBox(false, 0))
{
	base::set_extra_widget(*extra_widget_container);

	set_select_multiple(false);
	set_local_only();
}

file_chooser_dialog::file_chooser_dialog(const Glib::ustring& Title, const std::string& PathType, k3d::ipath_property::mode_t Action, const k3d::filesystem::path& InitialPath) :
	base(Title),
	path_type(PathType),
	initial_path(InitialPath),
	compression(false),
	extra_widget_container(new Gtk::VBox(false, 0))
{
	base::set_extra_widget(*extra_widget_container);

	set_select_multiple(false);
	set_local_only();

	switch(Action)
	{
		case k3d::ipath_property::READ:
			set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
			break;
		case k3d::ipath_property::WRITE:
			set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
			break;
	}
}

file_chooser_dialog::file_chooser_dialog(Gtk::Window& Parent, const Glib::ustring& Title, const std::string& PathType, Gtk::FileChooserAction Action, const k3d::filesystem::path& InitialPath) :
	base(Parent, Title, Action),
	path_type(PathType),
	initial_path(InitialPath),
	compression(false),
	extra_widget_container(new Gtk::VBox(false, 0))
{
	base::set_extra_widget(*extra_widget_container);

	set_select_multiple(false);
	set_local_only();
}

file_chooser_dialog::file_chooser_dialog(Gtk::Window& Parent, const Glib::ustring& Title, const std::string& PathType, k3d::ipath_property::mode_t Action, const k3d::filesystem::path& InitialPath) :
	base(Parent, Title),
	path_type(PathType),
	initial_path(InitialPath),
	compression(false),
	extra_widget_container(new Gtk::VBox(false, 0))
{
	base::set_extra_widget(*extra_widget_container);

	set_select_multiple(false);
	set_local_only();

	switch(Action)
	{
		case k3d::ipath_property::READ:
			set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
			break;
		case k3d::ipath_property::WRITE:
			set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
			break;
	}
}

file_chooser_dialog::~file_chooser_dialog()
{
	delete extra_widget_container;
}

void file_chooser_dialog::add_pattern_filter(const Glib::ustring& Name, const Glib::ustring& Pattern)
{
	Gtk::FileFilter filter;
	filter.set_name(Name);
	filter.add_pattern(Pattern);
	add_filter(filter);
}

void file_chooser_dialog::add_mime_type_filter(const Glib::ustring& Name, const Glib::ustring& MimeType)
{
	Gtk::FileFilter filter;
	filter.set_name(Name);
	filter.add_mime_type(MimeType);
	add_filter(filter);
}

void file_chooser_dialog::add_all_files_filter()
{
	Gtk::FileFilter filter;
	filter.set_name(_("All Files"));
	filter.add_pattern("*");
	add_filter(filter);
}

void file_chooser_dialog::enable_compression(const bool Compression)
{
	compression = Compression;
}

void file_chooser_dialog::append_extension(const std::string& Extension)
{
	extension = Extension;
}

void file_chooser_dialog::add_extra_widget(Gtk::Widget& ExtraWidget)
{
	extra_widget_container->pack_start(ExtraWidget, Gtk::PACK_SHRINK, 0);
}

bool file_chooser_dialog::get_file_path(k3d::filesystem::path& Result)
{
	bool compressed = false;
	return get_file_path(Result, compressed);
}

bool file_chooser_dialog::get_file_path(k3d::filesystem::path& Result, bool& Compressed)
{
	// Create buttons that will be (optionally) added to the dialog.  We do this on the stack
	// so the same buttons don't get added to the widget multiple times if get_file_path() is
	// called more-than-once.

	Gtk::CheckButton compression_widget(_("Compress output file"));
	compression_widget.set_active(Compressed);

	Gtk::CheckButton append_extension_widget(_("Append filename extension automatically"));
	append_extension_widget.set_active(true);

	Gtk::Button cancel_widget(Gtk::Stock::CANCEL);
	cancel_widget.set_flags(cancel_widget.get_flags() | Gtk::CAN_DEFAULT);
	cancel_widget.show();

	Gtk::Button open_widget(Gtk::Stock::OPEN);
	open_widget.set_flags(open_widget.get_flags() | Gtk::CAN_DEFAULT);
	open_widget.show();

	Gtk::Button save_widget(Gtk::Stock::SAVE);
	save_widget.set_flags(save_widget.get_flags() | Gtk::CAN_DEFAULT);
	save_widget.show();

	// Add the K-3D share path as a shortcut ...
	add_shortcut_folder(k3d::share_path().native_utf8_string().raw());

	add_action_widget(cancel_widget, Gtk::RESPONSE_CANCEL);
	switch(Gtk::FileChooser::get_action())
	{
		case Gtk::FILE_CHOOSER_ACTION_OPEN:
		case Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER:
			add_action_widget(open_widget, Gtk::RESPONSE_OK);
			set_default_response(Gtk::RESPONSE_OK);
			break;
		case Gtk::FILE_CHOOSER_ACTION_SAVE:
			// Setup the (optional) automatic extension control ...
			if(!extension.empty())
				extra_widget_container->pack_end(append_extension_widget, Gtk::PACK_SHRINK, 0);

			// Setup the (optional) compression control ...
			if(compression)
				extra_widget_container->pack_end(compression_widget, Gtk::PACK_SHRINK, 0);

			add_action_widget(save_widget, Gtk::RESPONSE_OK);
			set_default_response(Gtk::RESPONSE_OK);
			break;
		case Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER:
			add_action_widget(save_widget, Gtk::RESPONSE_OK);
			set_default_response(Gtk::RESPONSE_OK);
			break;
	}

	// Setup the initial path to display ...
	k3d::filesystem::path start_path = initial_path;
	if(start_path.empty())
		start_path = k3d::options::get_path(path_type);

	if(start_path.empty())
		start_path = k3d::system::get_home_directory();

	if(k3d::filesystem::exists(start_path) && k3d::filesystem::is_directory(start_path))
		set_current_folder(start_path.native_filesystem_string());
	else
		set_filename(start_path.native_filesystem_string());

	// Run the dialog ...
	set_position(Gtk::WIN_POS_CENTER);
	extra_widget_container->show_all();
	if(Gtk::RESPONSE_OK != run())
		return false;

	// Optionally add an extension if the user didn't ...
	Result = k3d::filesystem::native_path(k3d::ustring::from_utf8(Glib::filename_to_utf8(get_filename()).raw()));
	if(!extension.empty() && append_extension_widget.get_active() && (k3d::filesystem::extension(Result).lowercase().raw() != extension))
		Result = Result + extension;

	// Prompt the user if they're about to overwrite an existing file ...
	if(!prompt_file_overwrite(Result))
		return false;

	// Record the path for posterity ...
	k3d::options::set_path(path_type, Result);

	switch(Gtk::FileChooser::get_action())
	{
		case Gtk::FILE_CHOOSER_ACTION_OPEN:
		case Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER:
			break;
		case Gtk::FILE_CHOOSER_ACTION_SAVE:
			if(compression)
				Compressed = compression_widget.get_active();
			break;
		case Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER:
			break;
	}

	return true;
}

bool file_chooser_dialog::prompt_file_overwrite(const k3d::filesystem::path& File)
{
	switch(Gtk::FileChooser::get_action())
	{
		case Gtk::FILE_CHOOSER_ACTION_OPEN:
		case Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER:
			return true;
		case Gtk::FILE_CHOOSER_ACTION_SAVE:
		case Gtk::FILE_CHOOSER_ACTION_CREATE_FOLDER:
			break;
	}

	if(!k3d::filesystem::exists(File))
		return true;

	Gtk::MessageDialog dialog(*this, "", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);

	dialog.set_message(k3d::string_cast(boost::format(_("A file named \"%1%\" already exists.")) % File.native_utf8_string().raw()));
	dialog.set_secondary_text(_("Do you want to replace it with the one you are saving?"));

	Gtk::Image replace_icon(Gtk::Stock::REFRESH, Gtk::ICON_SIZE_BUTTON);

	Gtk::Button replace_button(_("Replace"));
	replace_button.set_image(replace_icon);
	replace_button.set_flags(replace_button.get_flags() | Gtk::CAN_DEFAULT);
	replace_button.show();

	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_action_widget(replace_button, Gtk::RESPONSE_OK);

	dialog.set_default_response(Gtk::RESPONSE_CANCEL);

	return dialog.run() == Gtk::RESPONSE_OK;
}

} // namespace libk3dngui

