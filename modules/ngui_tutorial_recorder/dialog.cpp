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
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_state.h>
#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/check_menu_item.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/image_menu_item.h>
#include <k3dsdk/ngui/image_toggle_button.h>
#include <k3dsdk/ngui/menu_item.h>
#include <k3dsdk/ngui/menubar.h>
#include <k3dsdk/ngui/messages.h>
#include <k3dsdk/ngui/safe_close_dialog.h>
#include <k3dsdk/ngui/savable_application_window.h>
#include <k3dsdk/ngui/scripting.h>
#include <k3dsdk/ngui/toolbar.h>
#include <k3dsdk/ngui/tutorials.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/scripting.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/user_interface.h>

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

namespace tutorial_recorder
{

/// Provides a UI for recording interactive tutorials
class dialog :
	public savable_application_window,
	public asynchronous_update
{
	typedef savable_application_window base;

public:
	dialog() :
		base("tutorial_recorder", 0),
		m_script_engine(k3d::plugin::create<k3d::iscript_engine>(k3d::classes::K3DScriptEngine())),
		m_compression(true),
		m_unsaved_changes(false),
		m_recording(init_name("recording") + init_label(_("Recording")) + init_description(_("Tells whether the tutorial recorder records user actions")) + init_value(true)),
		m_running(false)
	{
		k3d::ngui::tutorial::recording_started();

		assert_warning(m_script_engine);

		k3d::command_tree().command_signal().connect(sigc::mem_fun(*this, &dialog::on_command));
		m_recording.changed_signal().connect(sigc::mem_fun(*this, &dialog::on_edit_recording));

		menubar::control* const menubar = new menubar::control(*this, "menus");

		Gtk::Menu* const file_menu = Gtk::manage(new Gtk::Menu());

		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_new", Gtk::Stock::NEW), sigc::mem_fun(*this, &dialog::on_file_new))));
		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &dialog::on_file_open))));
		file_menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_save", Gtk::Stock::SAVE), sigc::mem_fun(*this, &dialog::on_file_save))));
		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_save_as", Gtk::Stock::SAVE_AS), sigc::mem_fun(*this, &dialog::on_file_save_as))));
		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_revert", Gtk::Stock::REVERT_TO_SAVED), sigc::mem_fun(*this, &dialog::on_file_revert))));
		file_menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
		file_menu->items().push_back(*Gtk::manage(connect(new image_menu_item::control(*menubar, "file_close", Gtk::Stock::CLOSE), sigc::mem_fun(*this, &dialog::safe_close))));

		Gtk::Menu* const edit_menu = Gtk::manage(new Gtk::Menu());

		edit_menu->items().push_back(*Gtk::manage(new check_menu_item::control(*menubar, "edit_recording", check_menu_item::proxy(m_recording), _("Recording"))));
		edit_menu->items().push_back(*Gtk::manage(
			new menu_item::control(*menubar, "edit_record_message", _("Record Message")) <<
			connect_menu_item(sigc::mem_fun(*this, &dialog::on_edit_record_message))));
		edit_menu->items().push_back(*Gtk::manage(
			new menu_item::control(*menubar, "edit_stop_recording", _("Stop Recording")) <<
			connect_menu_item(sigc::mem_fun(*this, &dialog::on_edit_stop_recording))));
		edit_menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
		edit_menu->items().push_back(*Gtk::manage(
			new menu_item::control(*menubar, "edit_play", _("Play")) <<
			connect_menu_item(sigc::mem_fun(*this, &dialog::on_edit_play))));
		edit_menu->items().push_back(*Gtk::manage(
			new menu_item::control(*menubar, "edit_play_from_cursor", _("Play From Cursor")) <<
			connect_menu_item(sigc::mem_fun(*this, &dialog::on_edit_play_from_cursor))));

		Gtk::Menu* const language_menu = Gtk::manage(new Gtk::Menu());

		const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup<k3d::iscript_engine>();
		for(k3d::plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
		{
			language_menu->items().push_back(*Gtk::manage(
				new menu_item::control(*menubar, "script_engine_" + (*factory)->name(), (*factory)->name())
				<< connect_menu_item(sigc::bind(sigc::mem_fun(*this, &dialog::on_tools_language), (*factory)))));
		}

		Gtk::Menu* const tools_menu = Gtk::manage(new Gtk::Menu());

		if(k3d::plugin::factory::lookup("NGUICommandNodeInspectorDialog"))
		{
			tools_menu->items().push_back(*Gtk::manage(
				new menu_item::control(*menubar, "tools_command_node_inspector", _("Command Node Inspector ...")) <<
				connect_menu_item(sigc::mem_fun(*this, &dialog::on_tools_command_node_inspector))));
		}

		if(k3d::plugin::factory::lookup("NGUIEventRecorderDialog"))
		{
			tools_menu->items().push_back(*Gtk::manage(
				new menu_item::control(*menubar, "tools_event_recorder", _("Event Recorder ...")) <<
				connect_menu_item(sigc::mem_fun(*this, &dialog::on_tools_event_recorder))));
		}

		tools_menu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Script Language"), *language_menu));

		menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), *file_menu));
		menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), *edit_menu));
		menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Tools"), *tools_menu));

		toolbar::control* const toolbar = new toolbar::control(*this, "toolbar");
		toolbar->row(0).pack_start(*Gtk::manage(
			new image_toggle_button::control(*toolbar, "record",
				image_toggle_button::proxy(m_recording),
				load_icon("record", Gtk::ICON_SIZE_BUTTON),
				load_icon("record", Gtk::ICON_SIZE_BUTTON)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		toolbar->row(0).pack_start(*Gtk::manage(
			new button::control(*toolbar, "stop_recording",
				*Gtk::manage(new Gtk::Image(load_icon("stop", Gtk::ICON_SIZE_BUTTON)))) <<
			connect_button(sigc::mem_fun(*this, &dialog::on_edit_stop_recording)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		toolbar->row(0).pack_start(*Gtk::manage(
			new button::control(*toolbar, "play",
				*Gtk::manage(new Gtk::Image(load_icon("play", Gtk::ICON_SIZE_BUTTON)))) <<
			connect_button(sigc::mem_fun(*this, &dialog::on_edit_play)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		toolbar->row(0).pack_start(*Gtk::manage(
			new button::control(*toolbar, "play_from_cursor",
				*Gtk::manage(new Gtk::Image(load_icon("play_from_cursor", Gtk::ICON_SIZE_BUTTON)))) <<
			connect_button(sigc::mem_fun(*this, &dialog::on_edit_play_from_cursor)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		toolbar->row(0).pack_start(*Gtk::manage(
			new button::control(*toolbar, "record_message", _("Record Message")) <<
			connect_button(sigc::mem_fun(*this, &dialog::on_edit_record_message)) <<
			make_toolbar_button()), Gtk::PACK_SHRINK);

		Gtk::HBox* const hbox1 = new Gtk::HBox(false);
		hbox1->pack_start(*Gtk::manage(menubar), Gtk::PACK_SHRINK);
		hbox1->pack_start(*Gtk::manage(toolbar), Gtk::PACK_SHRINK);

		m_script.set_wrap_mode(Gtk::WRAP_WORD_CHAR);

		m_script_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_script_window.add(m_script);

		m_recording_message.set_markup("<span foreground=\"red\" size=\"x-large\" weight=\"bold\">Recording in\nProgress</span>");
		m_recording_message.set_justify(Gtk::JUSTIFY_CENTER);
		
		Gtk::VBox* const vbox2 = new Gtk::VBox(false);
		vbox2->pack_start(m_script_window, Gtk::PACK_EXPAND_WIDGET);
		vbox2->pack_start(m_recording_message, Gtk::PACK_EXPAND_WIDGET);
		
		Gtk::VPaned* const vpaned = new Gtk::VPaned();
		vpaned->pack1(m_message);
		vpaned->pack2(*Gtk::manage(vbox2));
		vpaned->set_position(30);

		Gtk::VBox* const vbox1 = new Gtk::VBox(false);
		vbox1->pack_start(*Gtk::manage(hbox1), Gtk::PACK_SHRINK);
		vbox1->pack_start(*Gtk::manage(vpaned), Gtk::PACK_EXPAND_WIDGET);

		add(*Gtk::manage(vbox1));
		set_role("tutorial_recorder");
		resize(600, 300);

		file_new();
		show_all();

		m_script.get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &dialog::on_script_changed));

		on_edit_recording();
	}

	~dialog()
	{
		k3d::ngui::tutorial::recording_finished();
		delete m_script_engine;
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
			file_chooser_dialog dialog(_("Open Tutorial:"), k3d::options::path::tutorials(), Gtk::FILE_CHOOSER_ACTION_OPEN);
			dialog.add_pattern_filter(_("K-3D Script (*.k3dscript)"), "*.k3dscript");
			dialog.add_pattern_filter(_("Python Script (*.py)"), "*.py");
			dialog.add_all_files_filter();

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

	void on_edit_recording(k3d::iunknown* = 0)
	{
		if(m_recording.internal_value())
		{
			m_script_window.hide();
			m_recording_message.show();
		}
		else
		{
			if(!m_buffer.empty())
			{
				m_script.get_buffer()->insert(m_script.get_buffer()->end(), m_buffer);
				Gtk::TextBuffer::iterator end = m_script.get_buffer()->end();
				m_script.scroll_to(end, 0.0);

				m_unsaved_changes = true;
				m_buffer.clear();
			}

			m_script_window.show();
			m_recording_message.hide();
		}

		update_title();
	}

	void on_edit_record_message()
	{
		// Strip excess whitespace from the message, it's the UI's responsibility to display it
		// correctly even if it's a long line ...
		std::string message = m_message.get_buffer()->get_text();
		std::replace(message.begin(), message.end(), '\r', ' ');
		std::replace(message.begin(), message.end(), '\n', ' ');

		k3d::user_interface().tutorial_message(message);
	}

	void on_edit_stop_recording()
	{
		m_recording.set_value(false);
	}

	void on_edit_play()
	{
		on_edit_stop_recording();

		m_running = true;
		update_title();

		iconify();
		handle_pending_events();

		const k3d::script::code script(m_script.get_buffer()->get_text());
		k3d::iscript_engine::context_t context;
		execute_script(script, get_title(), context);

		m_running = false;
		update_title();
	}

	void on_edit_play_from_cursor()
	{
		on_edit_stop_recording();
		m_running = true;
		update_title();

		// We have to get the entire script first, so we can identify the correct scripting engine ...
		Glib::RefPtr<Gtk::TextBuffer> buffer = m_script.get_buffer();
		const k3d::script::language language(k3d::script::code(buffer->get_text()));

		// Now execute just the text after the cursor ...
		const k3d::script::code script(buffer->get_text(buffer->get_iter_at_mark(buffer->get_insert()), buffer->end()));
		k3d::iscript_engine::context_t context;
		execute_script(script, get_title(), context, language);

		m_running = false;
		update_title();
	}

	void on_tools_language(k3d::iplugin_factory* Factory)
	{
		return_if_fail(Factory);

		if(!save_changes())
			return;

		delete m_script_engine;
		m_script_engine = k3d::plugin::create<k3d::iscript_engine>(*Factory);

		file_new();
	}
	
	void on_tools_command_node_inspector()
	{
		Gtk::Window* const window = k3d::plugin::create<Gtk::Window>("NGUICommandNodeInspectorDialog");
		return_if_fail(window);

		window->set_transient_for(*this);
	}

	void on_tools_event_recorder()
	{
		Gtk::Window* const window = k3d::plugin::create<Gtk::Window>("NGUIEventRecorderDialog");
		return_if_fail(window);

		window->set_transient_for(*this);
	}

	void on_script_changed()
	{
		m_unsaved_changes = true;
		update_title();
	}

	void file_new()
	{
		return_if_fail(m_script_engine);

		m_message.get_buffer()->set_text(_("-- Enter a message to be recorded --"));

		std::stringstream script;
		m_script_engine->bless_script(script);
		m_script.get_buffer()->set_text(script.str());

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

		if(m_compression)
		{
			k3d::filesystem::ogzstream stream(m_path);
			stream << m_script.get_buffer()->get_text();
		}
		else
		{
			k3d::filesystem::ofstream stream(m_path);
			stream << m_script.get_buffer()->get_text();
		}

		m_unsaved_changes = false;
		update_title();
		return true;
	}

	bool file_save_as()
	{
		{
			file_chooser_dialog dialog(_("Save Tutorial As:"), k3d::options::path::tutorials(), Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(m_script_engine->factory().factory_id() == k3d::classes::K3DScriptEngine())
			{
				dialog.add_pattern_filter(_("K-3D Script (*.k3dscript)"), "*.k3dscript");
				dialog.append_extension(".k3dscript");
			}
			else if(m_script_engine->factory().factory_id() == k3d::classes::PythonEngine())
			{
				dialog.add_pattern_filter(_("Python Script (*.py)"), "*.py");
				dialog.append_extension(".py");
			}
			dialog.add_all_files_filter();
			dialog.enable_compression();

			if(!dialog.get_file_path(m_path, m_compression))
				return false;
		}

		if(m_compression)
		{
			k3d::filesystem::ogzstream stream(m_path);
			stream << m_script.get_buffer()->get_text();
		}
		else
		{
			k3d::filesystem::ofstream stream(m_path);
			stream << m_script.get_buffer()->get_text();
		}

		m_unsaved_changes = false;
		update_title();
		return true;
	}

	void on_command(k3d::icommand_node& CommandNode, const k3d::icommand_node::type Type, const std::string& Command, const std::string& Arguments)
	{
		// Sanity checks ...
		return_if_fail(m_script_engine);
		return_if_fail(Command.size());

		// If we aren't recording ...
		if(!m_recording.internal_value())
			return;

		// Skip everything but UI events ...
		if(Type != k3d::icommand_node::COMMAND_INTERACTIVE)
			return;

		// Don't record our own events ...
		if(k3d::command_node::is_descendant(this, &CommandNode))
			return;

		std::stringstream buffer;
		m_script_engine->append_command(buffer, CommandNode, Command, Arguments);
		m_buffer += buffer.str();

/** \todo Restore this code once we figure-out why updates during recording cause an X server crash
		schedule_update();
*/
	}

	void on_update()
	{
/** \todo Restore this code once we figure-out why updates during recording cause an X server crash
		m_script.get_buffer()->insert(m_script.get_buffer()->end(), m_buffer);
		Gtk::TextBuffer::iterator end = m_script.get_buffer()->end();
		m_script.scroll_to(end, 0.0);

		m_unsaved_changes = true;
		m_buffer.clear();
*/
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
		return m_path.empty() ? _("Untitled Tutorial") : m_path.leaf().raw();
	}

	void update_title()
	{
		std::string title = get_script_title();
		if(m_unsaved_changes)
			title += _(" [changed]");
		if(m_recording.internal_value())
			title += _(" [recording]");
		if(m_running)
			title += _(" [running]");

		set_title(title);
	}

	/// Script engine for the script being recorded
	k3d::iscript_engine* m_script_engine;
	/// Stores the file path (could be empty)
	k3d::filesystem::path m_path;
	bool m_compression;
	/// Set to true iff there are unsaved changes
	bool m_unsaved_changes;
	/// Set to true iff recording is in progress
	k3d_data(bool, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_recording;
	/// Set to true iff tutorial playback is in progress
	bool m_running;
	/// Stores an interactive message
	Gtk::TextView m_message;
	/// Stores the script being recorded
	Gtk::TextView m_script;
	/// Scroll window that contains the script
	Gtk::ScrolledWindow m_script_window;
	/// Displays a message while recording is in progress
	Gtk::Label m_recording_message;
	/// Buffers incoming command text so we can maintain responsiveness while recording
	std::string m_buffer;

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xb5ba9aa0, 0xb2428a8c, 0xb4a7e181, 0xaeca9126),
			"NGUITutorialRecorderDialog",
			_(""),
			"NGUI Dialogs",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

} // namespace tutorial_recorder

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::tutorial_recorder::dialog::get_factory());
K3D_MODULE_END

