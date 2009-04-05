#ifndef K3DSDK_NGUI_FILE_CHOOSER_DIALOG_H
#define K3DSDK_NGUI_FILE_CHOOSER_DIALOG_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3dsdk/ipath_property.h>
#include <k3dsdk/path.h>

#include <gtkmm/filechooserdialog.h>

namespace k3d
{

namespace ngui
{

/// Wraps the standard file dialog widget, adding K-3D-specific functionality
class file_chooser_dialog :
	public Gtk::FileChooserDialog
{
	typedef Gtk::FileChooserDialog base;

public:
	file_chooser_dialog(const Glib::ustring& Title, const std::string& PathType, Gtk::FileChooserAction Action = Gtk::FILE_CHOOSER_ACTION_OPEN, const k3d::filesystem::path& InitialPath = k3d::filesystem::path());
	file_chooser_dialog(const Glib::ustring& Title, const std::string& PathType, k3d::ipath_property::mode_t Action, const k3d::filesystem::path& InitialPath = k3d::filesystem::path());
 	file_chooser_dialog(Gtk::Window& Parent, const Glib::ustring& Title, const std::string& PathType, Gtk::FileChooserAction Action = Gtk::FILE_CHOOSER_ACTION_OPEN, const k3d::filesystem::path& InitialPath = k3d::filesystem::path());
	file_chooser_dialog(Gtk::Window& Parent, const Glib::ustring& Title, const std::string& PathType, k3d::ipath_property::mode_t Action, const k3d::filesystem::path& InitialPath = k3d::filesystem::path());

	~file_chooser_dialog();

	/// Adds a GLOB style pattern filter to the list of available files
	void add_pattern_filter(const Glib::ustring& Name, const Glib::ustring& Pattern);
	/// Adds a Mime-type filter to the list of available files
	void add_mime_type_filter(const Glib::ustring& Name, const Glib::ustring& MimeType);
	/// Adds an "all files" filter to the list of available files
	void add_all_files_filter();

	/// Adds an option to compress output files.
	void enable_compression(const bool Compression = true);

	/// Adds an option to force the user's choice of file to end in the given extension.  Pass the extension (including dot) to be added.  Pass an empty string to disable.  Disabled by default.
	void append_extension(const std::string& Extension);

	/// Adds an extra widget (for previewing, special options, etc) to the dialog
	void add_extra_widget(Gtk::Widget& ExtraWidget);

	/// Displays a modal dialog and prompts the user to choose a file, returning false if the user cancels
	bool get_file_path(k3d::filesystem::path& Result);
	/// Displays a modal dialog and prompts the user to choose a file, returning false if the user cancels
	bool get_file_path(k3d::filesystem::path& Result, bool& Compressed);

private:
	// Force callers to use add_extra_widget() instead
	void set_extra_widget(Gtk::Widget&);

	bool prompt_file_overwrite(const k3d::filesystem::path& Path);

	const std::string path_type;
	const k3d::filesystem::path initial_path;
	bool compression;
	std::string extension;
	Gtk::VBox* const extra_widget_container;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_FILE_CHOOSER_DIALOG_H

