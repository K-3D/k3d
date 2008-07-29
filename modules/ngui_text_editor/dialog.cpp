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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/data.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/check_menu_item.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/image_menu_item.h>
#include <k3dsdk/ngui/image_toggle_button.h>
#include <k3dsdk/ngui/menu_item.h>
#include <k3dsdk/ngui/menubar.h>
#include <k3dsdk/ngui/messages.h>
#include <k3dsdk/ngui/savable_document_window.h>
#include <k3dsdk/ngui/scripting.h>
#include <k3dsdk/ngui/text_editor.h>
#include <k3dsdk/ngui/toolbar.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/options.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/box.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/textview.h>

#include <sstream>

using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace text_editor
{

class dialog :
	public k3d::ngui::text_editor,
	public savable_document_window
{
public:
	dialog() :
		m_unsaved_changes(false),
		m_running(false)
	{
	}

	void initialize(document_state& Document)
	{
		savable_document_window::initialize(Document);

		k3d::command_tree().add(*this, get_factory().name(), dynamic_cast<k3d::icommand_node*>(&Document.document()));

		menubar::control* const menubar = new menubar::control(*this, "menus");

		Gtk::Menu* const menu_file = new Gtk::Menu();
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_new", Gtk::Stock::NEW), sigc::mem_fun(*this, &dialog::on_file_new))));
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &dialog::on_file_open))));
		menu_file->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_save", Gtk::Stock::SAVE), sigc::mem_fun(*this, &dialog::on_file_save))));
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_save_as", Gtk::Stock::SAVE_AS), sigc::mem_fun(*this, &dialog::on_file_save_as))));
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_revert", Gtk::Stock::REVERT_TO_SAVED), sigc::mem_fun(*this, &dialog::on_file_revert))));
		menu_file->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
		menu_file->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_close", Gtk::Stock::CLOSE), sigc::mem_fun(*this, &dialog::safe_close))));

		Gtk::Menu* const menu_edit = new Gtk::Menu();
		menu_edit->items().push_back(*Gtk::manage(
			new menu_item::control(*menubar, "edit_play", _("Play")) <<
			connect_menu_item(sigc::mem_fun(*this, &dialog::on_edit_play))));

		menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), *manage(menu_file)));
		menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), *manage(menu_edit)));

		toolbar::control* const toolbar = new toolbar::control(*this, "toolbar");

		toolbar->row(0).pack_start(*Gtk::manage(
			new button::control(*toolbar, "play",
				*Gtk::manage(new Gtk::Image(load_icon("play", Gtk::ICON_SIZE_BUTTON)))) <<
			connect_button(sigc::mem_fun(*this, &dialog::on_edit_play)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		Gtk::HBox* const hbox1 = new Gtk::HBox(false);
		hbox1->pack_start(*Gtk::manage(menubar), Gtk::PACK_SHRINK);
		hbox1->pack_start(*Gtk::manage(toolbar), Gtk::PACK_SHRINK);

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(m_script);

		m_cursor_position.set_alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER);

		Gtk::VBox* const vbox1 = new Gtk::VBox(false);
		vbox1->pack_start(*Gtk::manage(hbox1), Gtk::PACK_SHRINK);
		vbox1->pack_start(*Gtk::manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);
		vbox1->pack_start(m_cursor_position, Gtk::PACK_SHRINK);

		add(*Gtk::manage(vbox1));
		set_role(get_factory().name());
		resize(600, 300);

		file_new();
		show_all();

		m_script.get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &dialog::on_script_changed));
		m_script.get_buffer()->signal_mark_set().connect(sigc::mem_fun(*this, &dialog::on_mark_set));
	}

	~dialog()
	{
	}

	void on_file_new()
	{
		if(!save_changes())
			return;

		file_new();
	}

	void on_file_open()
	{
		if(!save_changes())
			return;

		k3d::filesystem::path filepath;

		{
			file_chooser_dialog dialog(_("Open Script:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
			if(!dialog.get_file_path(filepath))
				return;
		}

		file_open(filepath);
	}

	void on_file_save()
	{
		file_save();
	}

	void on_file_save_as()
	{
		file_save_as();
	}

	void on_file_revert()
	{
		if(!save_changes())
			return;

		if(m_path.empty())
			file_new();
		else
			file_open(m_path);
	}

	void on_edit_play()
	{
		const k3d::script::code code(m_script.get_buffer()->get_text());
		const k3d::mime::type mime_type = k3d::mime::type::lookup(code.source());

		if(mime_type.empty())
		{
			error_message(
				_("Could not identify the MIME-type for this script.  K-3D supports multiple scripting languages, and the MIME-type is used to "
				"match a script to the correct script engine. You can add a \"magic token\" at the beginning of a script to  "
				"in the first 12 characters of a script for K-3D's built-in K3DScript engine.  If you are writing a K-3D script, check the documentation "
				"for the scripting language you're writing in to see how to make it recognizable."));
			return;
		}

		const k3d::string_t name = get_title();

		k3d::iscript_engine::context_t context;
		context["Document"] = &document();

		m_running = true;
		update_title();

		execute_script(code, name, context);

		m_running = false;
		update_title();
	}

	void on_script_changed()
	{
		m_unsaved_changes = true;
		update_title();
	}

	void on_mark_set(const Gtk::TextIter& Iterator, const Glib::RefPtr<Gtk::TextMark>& Mark)
	{
		if(Mark->get_name() != "insert")
			return;

		m_cursor_position.set_text(k3d::string_cast(boost::format(_("Line: %1% Column: %2%")) % (Iterator.get_line() + 1) % (Iterator.get_visible_line_offset() + 1)));
	}

	void file_new()
	{
		m_script.get_buffer()->set_text("#python\n\nimport k3d\n\n");

		m_path = k3d::filesystem::path();
		m_unsaved_changes = false;
		update_title();
	}

	void file_open(const k3d::filesystem::path& Path)
	{
		k3d::filesystem::igzstream stream(Path);

		std::stringstream script;
	        stream.get(*script.rdbuf(), '\0');
		m_script.get_buffer()->set_text(script.str());

		m_path = Path;
		m_unsaved_changes = false;
		update_title();
	}

	bool file_save()
	{
		if(m_path.empty())
			return file_save_as();

		k3d::filesystem::ofstream stream(m_path);
		stream << m_script.get_buffer()->get_text();

		m_unsaved_changes = false;
		update_title();
		return true;
	}

	bool file_save_as()
	{
		{
			file_chooser_dialog dialog(_("Save Script As::"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(m_path))
				return false;
		}

		k3d::filesystem::ofstream stream(m_path);
		stream << m_script.get_buffer()->get_text();

		m_unsaved_changes = false;
		update_title();
		return true;
	}

	const bool unsaved_changes()
	{
		return m_unsaved_changes;
	}

	const std::string unsaved_document_title()
	{
		return get_script_title();
	}

	const bool save_unsaved_changes()
	{
		return file_save();
	}
	
	std::string get_script_title()
	{
		return m_path.empty() ? _("Untitled Script") : m_path.leaf().raw();
	}

	void update_title()
	{
		std::string title = get_script_title();
		if(m_unsaved_changes)
			title += _(" [changed]");
		if(m_running)
			title += _(" [running]");

		set_title(title);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xca41ccc9, 0xb9433dec, 0x30c65c83, 0x40eb359b),
			"NGUITextEditorDialog",
			_("Provides a general-purpose dialog for editing text, scripts, and shaders."),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	/// Stores the file path (could be empty)
	k3d::filesystem::path m_path;
	/// Set to true iff there are unsaved changes
	bool m_unsaved_changes;
	/// Set to true iff script playback is in progress
	bool m_running;
	/// Stores the script being recorded
	Gtk::TextView m_script;
	/// Displays cursor position
	Gtk::Label m_cursor_position;
};

} // namespace text_editor

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::text_editor::dialog::get_factory());
K3D_MODULE_END

