// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include "config.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/idocument_sink.h>
#include <k3dsdk/iproperty_sink.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/istate_recorder_sink.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/image_menu_item.h>
#include <k3dsdk/ngui/menu_item.h>
#include <k3dsdk/ngui/messages.h>
#include <k3dsdk/ngui/scripting.h>
#include <k3dsdk/ngui/toolbar.h>
#include <k3dsdk/ngui/widget_manip.h>
#include <k3dsdk/options.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/textview.h>
#include <gtkmm/uimanager.h>

#ifdef K3D_GTKSOURCEVIEW_FOUND
	#include <gtksourceview/gtksourceview.h>
	#include <gtksourceview/gtksourcelanguagemanager.h>
#endif // K3D_GTKSOURCEVIEW_FOUND

#include <boost/assign/list_of.hpp>

#include <sstream>

namespace module
{

namespace ngui
{

namespace text_editor
{

class dialog :
	public k3d::ngui::application_window,
	public k3d::idocument_sink,
	public k3d::iproperty_sink,
	public k3d::istate_recorder_sink
{
public:
	dialog() :
		m_document(0),
		m_state_recorder(0),
		m_property(0),
		m_running(false),
#ifdef K3D_GTKSOURCEVIEW_FOUND
		m_view(Glib::wrap(GTK_TEXT_VIEW(gtk_source_view_new())))
#else // K3D_GTKSOURCEVIEW_FOUND
		m_view(new Gtk::TextView())
#endif // !K3D_GTKSOURCEVIEW_FOUND
	{
		m_actions = Gtk::ActionGroup::create();

		m_actions->add(Gtk::Action::create("file", _("_File")));
		m_actions->add(Gtk::Action::create("new", Gtk::Stock::NEW), sigc::mem_fun(*this, &dialog::on_new));
		m_actions->add(Gtk::Action::create("open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &dialog::on_open));
		m_actions->add(Gtk::Action::create("save", Gtk::Stock::SAVE), sigc::mem_fun(*this, &dialog::on_save));
		m_actions->add(Gtk::Action::create("save_as", Gtk::Stock::SAVE_AS), sigc::hide_return(sigc::mem_fun(*this, &dialog::on_save_as)));
		m_actions->add(Gtk::Action::create("revert", Gtk::Stock::REVERT_TO_SAVED), sigc::mem_fun(*this, &dialog::on_revert));
		m_actions->add(Gtk::Action::create("import", _("Import File ...")), sigc::mem_fun(*this, &dialog::on_import));
		m_actions->add(Gtk::Action::create("export", _("Export File ...")), sigc::mem_fun(*this, &dialog::on_export));
		m_actions->add(Gtk::Action::create("commit", _("Commit"), _("Commit modifications to the underlying property.")), sigc::mem_fun(*this, &dialog::on_commit));
		m_actions->add(Gtk::Action::create("close", Gtk::Stock::CLOSE), sigc::mem_fun(*this, &dialog::close));
		m_actions->add(Gtk::Action::create("edit", _("_Edit")));
		m_actions->add(Gtk::Action::create("execute", Gtk::Stock::EXECUTE), sigc::mem_fun(*this, &dialog::on_execute));

		m_ui_manager = Gtk::UIManager::create();
		m_ui_manager->insert_action_group(m_actions);
		add_accel_group(m_ui_manager->get_accel_group());

		m_ui_manager->add_ui_from_string(
			"<ui>"
			"  <menubar name='menubar'>"
			"    <menu action='file'>"
			"      <menuitem action='new'/>"
			"      <menuitem action='open'/>"
			"      <menuitem action='save'/>"
			"      <menuitem action='save_as'/>"
			"      <menuitem action='revert'/>"
			"      <menuitem action='import'/>"
			"      <menuitem action='export'/>"
			"      <menuitem action='commit'/>"
			"      <separator/>"
			"      <menuitem action='close'/>"
			"    </menu>"
			"    <menu action='edit'>"
			"      <menuitem action='execute'/>"
			"    </menu>"
			"  </menubar>"
			"  <toolbar name='toolbar'>"
			"    <toolitem action='new'/>"
			"    <toolitem action='open'/>"
			"    <toolitem action='save'/>"
			"    <toolitem action='commit'/>"
			"    <separator/>"
			"    <toolitem action='execute'/>"
			"  </toolbar>"
			"</ui>"
			);

		Gtk::ScrolledWindow* const scrolled_window = new Gtk::ScrolledWindow();
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(*Gtk::manage(m_view));

		m_cursor_position.set_alignment(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER);

		Gtk::VBox* const vbox1 = new Gtk::VBox(false);
		vbox1->pack_start(*manage(m_ui_manager->get_widget("/menubar")), Gtk::PACK_SHRINK);
		vbox1->pack_start(*manage(m_ui_manager->get_widget("/toolbar")), Gtk::PACK_SHRINK);
		vbox1->pack_start(*manage(scrolled_window), Gtk::PACK_EXPAND_WIDGET);
		vbox1->pack_start(m_cursor_position, Gtk::PACK_SHRINK);

		add(*Gtk::manage(vbox1));
		set_role(get_factory().name());
		resize(600, 300);

		update_title();
		update_widgets();
		show_all();

		m_view->get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &dialog::update_title));
		m_view->get_buffer()->signal_mark_set().connect(sigc::mem_fun(*this, &dialog::update_cursor_position));
	}

	void set_document(k3d::idocument* const Document)
	{
		m_document = Document;
	}

	void set_state_recorder(k3d::istate_recorder* const StateRecorder)
	{
		m_state_recorder = StateRecorder;
	}

	void set_property(k3d::iproperty* const Property)
	{
		if(Property && Property->property_type() != typeid(k3d::string_t))
		{
			k3d::log() << error << "Unsupported property type." << std::endl;
			return;
		}

		m_property = Property;

		set_text(m_property ? k3d::property::internal_value<k3d::string_t>(*m_property) : "");

		update_title();
		update_widgets();
	}

	void set_text(const k3d::string_t& Text)
	{
		const k3d::mime::type mime_type = k3d::mime::type::lookup(Text);

#ifdef K3D_GTKSOURCEVIEW_FOUND
		GtkSourceLanguageManager* const language_manager =  gtk_source_language_manager_get_default();
		GtkSourceLanguage* const language = gtk_source_language_manager_guess_language(language_manager, 0, mime_type.str().c_str());
		GtkSourceBuffer* const buffer = GTK_SOURCE_BUFFER(m_view->get_buffer()->gobj());
		gtk_source_buffer_set_language(buffer, language);
		gtk_source_buffer_set_highlight_syntax(buffer, true);
#endif // K3D_GTKSOURCEVIEW_FOUND

		m_view->get_buffer()->set_text(Text);
		m_view->get_buffer()->set_modified(false);
	}

	void set_text(const k3d::filesystem::path& Path)
	{
		const k3d::mime::type mime_type = k3d::mime::type::lookup(Path);

#ifdef K3D_GTKSOURCEVIEW_FOUND
		GtkSourceLanguageManager* const language_manager =  gtk_source_language_manager_get_default();
		GtkSourceLanguage* const language = gtk_source_language_manager_guess_language(language_manager, 0, mime_type.str().c_str());
		GtkSourceBuffer* const buffer = GTK_SOURCE_BUFFER(m_view->get_buffer()->gobj());
		gtk_source_buffer_set_language(buffer, language);
		gtk_source_buffer_set_highlight_syntax(buffer, true);
#endif // K3D_GTKSOURCEVIEW_FOUND

		k3d::filesystem::igzstream stream(Path);
		std::stringstream text;
	        stream.get(*text.rdbuf(), '\0');

		m_view->get_buffer()->set_text(text.str().c_str());
		m_view->get_buffer()->set_modified(false);
	}

	void set_path(const k3d::filesystem::path& Path)
	{
		m_path = Path;
		update_title();
	}

	void on_new()
	{
//		if(!save_changes())
//			return;

		set_text("");
		update_title();
	}

	void on_open()
	{
//		if(!save_changes())
//			return;

		k3d::filesystem::path filepath;
		{
			k3d::ngui::file_chooser_dialog dialog(_("Open File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
			if(!dialog.get_file_path(filepath))
				return;
		}

		set_text(filepath);
		set_path(filepath);
	}

	void on_save()
	{
		if(m_path.empty())
		{
			on_save_as();
			return;
		}

		k3d::filesystem::ofstream stream(m_path);
		stream << m_view->get_buffer()->get_text();

		m_view->get_buffer()->set_modified(false);
		update_title();
	}

	bool on_save_as()
	{
		{
			k3d::ngui::file_chooser_dialog dialog(_("Save Script As::"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(m_path))
				return false;
		}

		k3d::filesystem::ofstream stream(m_path);
		stream << m_view->get_buffer()->get_text();

		m_view->get_buffer()->set_modified(false);
		set_path(m_path);

		return true;
	}

	void on_revert()
	{
//		if(!save_changes())
//			return;

		if(m_property)
		{
			set_text(m_property ? k3d::property::internal_value<k3d::string_t>(*m_property) : "");
		}
		else if(m_path.empty())
		{
			set_text("");
		}
		else
		{
			set_text(m_path);
		}
	}

	void on_import()
	{
		k3d::filesystem::path path;
		{
			k3d::ngui::file_chooser_dialog dialog(_("Import File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
			if(!dialog.get_file_path(path))
				return;
		}

		set_text(path);
	}

	void on_export()
	{
		k3d::filesystem::path path;
		{
			k3d::ngui::file_chooser_dialog dialog(_("Export File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(path))
				return;
		}

		k3d::filesystem::ofstream stream(path);
		stream << m_view->get_buffer()->get_text();
	}

	void on_commit()
	{
		if(m_property)
		{
			if(m_state_recorder)
				m_state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

			k3d::property::set_internal_value(*m_property, m_view->get_buffer()->get_text().raw());

			if(m_state_recorder)
				m_state_recorder->commit_change_set(m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), m_property->property_name(), K3D_CHANGE_SET_CONTEXT);
		}
	}

	void on_execute()
	{
		const k3d::script::code code(m_view->get_buffer()->get_text());
		const k3d::mime::type mime_type = k3d::mime::type::lookup(code.source());

		if(mime_type.empty())
		{
			k3d::ngui::error_message(
				_("Unknown script language"),
				_("Could not identify the MIME-type for this script. "
				" K-3D supports multiple scripting languages, and the MIME-type is used to "
				"match scripts to the correct script engines. "
				"You can add a \"magic token\" at the beginning of a script to "
				"force identification of its MIME-type - for example, add \"#python\" at the "
				"beginning of a Python script."
				)); 
			return;
		}

		const k3d::string_t name = get_title();

		k3d::iscript_engine::context_t context;
		if(m_document)
			context["Document"] = m_document;

		m_running = true;
		update_title();

		k3d::ngui::execute_script(code, name, context);

		m_running = false;
		update_title();
	}

	void update_title()
	{
		k3d::string_t title;

		if(m_property)
		{
			if(m_property->property_node())
			{
				title = k3d::string_cast(boost::format(_("%1% \"%2%\" property")) % m_property->property_node()->name() % m_property->property_name());
			}
			else
			{
				title = k3d::string_cast(boost::format(_("%1%")) % m_property->property_name());
			}
		}
		else if(!m_path.empty())
		{
			title = m_path.leaf().raw();
		}
		else
		{
			title = _("Untitled");
		}

		if(m_view->get_buffer()->get_modified())
			title += _(" [changed]");

		if(m_running)
			title += _(" [running]");

		set_title(title);
	}

	void update_widgets()
	{
		m_actions->get_action("new")->set_visible(!m_property);
		m_actions->get_action("open")->set_visible(!m_property);
		m_actions->get_action("save")->set_visible(!m_property);
		m_actions->get_action("save_as")->set_visible(!m_property);
		m_actions->get_action("revert")->set_visible(true);
		m_actions->get_action("import")->set_visible(m_property);
		m_actions->get_action("export")->set_visible(m_property);
		m_actions->get_action("commit")->set_visible(m_property);
		m_actions->get_action("execute")->set_visible(!m_property);
	}

	void update_cursor_position(const Gtk::TextIter& Iterator, const Glib::RefPtr<Gtk::TextMark>& Mark)
	{
		if(Mark->get_name() != "insert")
			return;

		m_cursor_position.set_text(k3d::string_cast(boost::format(_("Line: %1% Column: %2%")) % (Iterator.get_line() + 1) % (Iterator.get_visible_line_offset() + 1)));
	}


	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xca41ccc9, 0xb9433dec, 0x30c65c83, 0x40eb359b),
			"NGUITextEditorDialog",
			_("Provides a general-purpose dialog for editing source-code, text, scripts, and shaders."),
			"NGUI Dialog",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}

	/// (Optional) document for the current text (could be NULL)
	k3d::idocument* m_document;
	/// (Optional) state recorder for making changes undoable (could be NULL)
	k3d::istate_recorder* m_state_recorder;
	/// (Optional) property source for the current text (could be NULL)
	k3d::iproperty* m_property;
	/// (Optional) file path source for the current text (could be empty)
	k3d::filesystem::path m_path;
	/// Set to true iff script playback is in progress
	bool m_running;

	// Menu items
	Glib::RefPtr<Gtk::ActionGroup> m_actions;
	Glib::RefPtr<Gtk::UIManager> m_ui_manager;
	/// Storage and editing for the current text
	Gtk::TextView* const m_view;
	/// Displays cursor position
	Gtk::Label m_cursor_position;
};

} // namespace text_editor

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::text_editor::dialog::get_factory());
K3D_MODULE_END

