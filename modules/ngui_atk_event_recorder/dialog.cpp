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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/idocument_sink.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/log.h>
#include <k3dsdk/mime_types.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/messages.h>
#include <k3dsdk/ngui/scripting.h>
#include <k3dsdk/options.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/type_registry.h>

#include <atkmm/object.h>

#include <gtkmm/action.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/uimanager.h>

#include <gtk/gtk.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <stack>

namespace module
{

namespace ngui
{

namespace atk_event_recorder
{

namespace detail
{

const k3d::string_t script_name(const k3d::string_t& Name)
{
	return boost::replace_all_copy(Name, " ", "_");
}

/// Returns the index in a "per role" array of children, for the given child number of the parent Object
const k3d::uint_t get_index_in_role(AtkObject* Object)
{
	const k3d::uint_t index_in_parent = atk_object_get_index_in_parent(Object);
	const AtkRole role = atk_object_get_role(Object);
	AtkObject* parent = atk_object_get_parent(Object);
	return_val_if_fail(parent, 0);
	k3d::uint_t result = 0;
	for(k3d::uint_t i = 0; i != index_in_parent; ++i)
	{
		if(atk_object_get_role(atk_object_ref_accessible_child(parent, i)) == role)
			++result;
	}
	return result;
}

} // namespace detail

class dialog :
	public k3d::ngui::application_window,
	public k3d::idocument_sink
{
	typedef k3d::ngui::application_window base;
public:
	dialog() : m_document(0),
		m_running(false),
		m_saving(false),
		m_recording(false),
		m_view(new Gtk::TextView())
	{
		// Set up the UI
		m_actions = Gtk::ActionGroup::create();

		m_actions->add(Gtk::Action::create("file", _("_File")));
		m_actions->add(Gtk::Action::create("new", Gtk::Stock::NEW), sigc::mem_fun(*this, &dialog::on_new));
		m_actions->add(Gtk::Action::create("open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &dialog::on_open));
		m_actions->add(Gtk::ToggleAction::create("record", Gtk::Stock::MEDIA_RECORD, _("Record"), _("Toggle the recording of actions"), false), sigc::mem_fun(*this, &dialog::on_record_toggle));
		m_actions->add(Gtk::Action::create("save", Gtk::Stock::SAVE), sigc::mem_fun(*this, &dialog::on_save));
		m_actions->add(Gtk::Action::create("save_as", Gtk::Stock::SAVE_AS), sigc::hide_return(sigc::mem_fun(*this, &dialog::on_save_as)));
		m_actions->add(Gtk::Action::create("revert", Gtk::Stock::REVERT_TO_SAVED), sigc::mem_fun(*this, &dialog::on_revert));
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
			"      <menuitem action='record'/>"
			"      <menuitem action='save'/>"
			"      <menuitem action='save_as'/>"
			"      <menuitem action='revert'/>"
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
			"    <separator/>"
			"    <toolitem action='execute'/>"
			"    <toolitem action='record'/>"
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

		show_all();

		append_line("#python");
		append_line("import k3d");
		append_line("");

		k3d::log() << info << "Listening to " << atk_get_toolkit_name() << " " << atk_get_toolkit_version() << std::endl;

		add_event_listeners();
		m_record_connection = action_recorded_signal().connect(sigc::mem_fun(*this, &dialog::on_action_recorded));
	}

	~dialog()
	{
		m_record_connection.disconnect();
	}

	void set_document(k3d::idocument* const Document)
	{
		m_document = Document;
	}

	/// Appends the given text, followed by a newline
	void append_line(const k3d::string_t Text)
	{
		Glib::RefPtr<Gtk::TextBuffer> buffer = m_view->get_buffer();
		buffer->insert(buffer->end(), Text);
		buffer->insert(buffer->end(), "\n");
		buffer->place_cursor(buffer->end());
		m_view->scroll_to(buffer->get_insert());
	}

	void set_text(const k3d::string_t& Text)
	{
		m_view->get_buffer()->set_text(Text);
		m_view->get_buffer()->set_modified(false);
	}

	void set_text(const k3d::filesystem::path& Path)
	{
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
		k3d::filesystem::path filepath;
		{
			k3d::ngui::file_chooser_dialog dialog(_("Open File:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
			if(!dialog.get_file_path(filepath))
				return;
		}

		set_text(filepath);
		set_path(filepath);
	}

	void on_record_toggle()
	{
		m_recording = !m_recording;
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
		m_saving = true;
		{
			k3d::ngui::file_chooser_dialog dialog(_("Save Script As::"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
			if(!dialog.get_file_path(m_path))
			{
				m_saving = false;
				return false;
			}
		}

		k3d::filesystem::ofstream stream(m_path);
		stream << m_view->get_buffer()->get_text();

		m_view->get_buffer()->set_modified(false);
		set_path(m_path);

		m_saving = false;
		return true;
	}

	void on_revert()
	{
		if(m_path.empty())
		{
			set_text("");
		}
		else
		{
			set_text(m_path);
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

		k3d::iscript_engine::context context;
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

		if(!m_path.empty())
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
		m_actions->get_action("new")->set_visible(true);
		m_actions->get_action("open")->set_visible(true);
		m_actions->get_action("save")->set_visible(true);
		m_actions->get_action("save_as")->set_visible(true);
		m_actions->get_action("revert")->set_visible(true);
		m_actions->get_action("execute")->set_visible(true);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<dialog> factory(
			k3d::uuid(0xc8ac6ef4, 0xce48212a, 0xed0006b2, 0x8f29e761),
			"NGUIATKEventRecorderDialog",
			_("Sends a stream of ATK user interface events to stderr, primarily intended for troubleshooting"),
			"NGUI Dialog",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "dialog")
			);

		return factory;
	}

private:
	typedef std::stack<Glib::RefPtr<Atk::Object> > click_trace_t;

	// Make sure we only listen once for events
	static void add_event_listeners()
	{
		static bool events_added = false;
		if(!events_added)
		{
			atk_add_global_event_listener(click_event_listener, "Gtk:GtkButton:clicked");
			atk_add_global_event_listener(click_event_listener, "Gtk:GtkMenuItem:activate");
			events_added = true;
		}
	}

	static gboolean click_event_listener(GSignalInvocationHint* signal_hint, guint n_param_values, const GValue* param_values, gpointer data)
	{
		AtkObject* root = atk_get_root();
		Glib::RefPtr<Atk::Object> object;
		if(GTK_IS_WIDGET(g_value_get_object(param_values)))
		{
			Gtk::Widget* widget = Glib::wrap(GTK_WIDGET(g_value_get_object(param_values)));
			object = widget->get_accessible();
		}
		else
		{
			object = Glib::wrap(ATK_OBJECT(g_value_get_object(param_values)), true);
		}
		return_val_if_fail(object, true);
		k3d::bool_t click_found = false;
		if(ATK_IS_ACTION(object->gobj()))
		{
			AtkAction* action = ATK_ACTION(object->gobj());
			for(k3d::uint_t i = 0; i != atk_action_get_n_actions(action); ++i)
			{
				if(std::string(atk_action_get_name(action, i)) == "click")
					click_found = true;
			}
		}
		return_val_if_fail(click_found, true);
		click_trace_t click_trace;
		while(object && object->get_index_in_parent() > -1)
		{
			click_trace.push(object);
			object = object->get_parent();
		}
		action_recorded_signal().emit(click_trace, "click");
		return true;
	}

	void on_action_recorded(click_trace_t& Trace, const k3d::string_t& ActionName)
	{
		if(!m_recording)
			return;
		if(Trace.empty())
			return;
		if(m_running)
			return;
		if(m_saving)
			return;
		Glib::RefPtr<Atk::Object> self = this->get_accessible();
		std::stringstream line;
		line << "k3d.atk.root()";
		while(!Trace.empty())
		{
			Glib::RefPtr<Atk::Object> obj = Trace.top();
			if(obj == self) // skip recording actions on ourself
				return;
			line << "." << detail::script_name(atk_role_get_name(atk_object_get_role(obj->gobj())));
			if(obj->get_name() == "")
				line << "(" << detail::get_index_in_role(obj->gobj()) << ")";
			else
				line << "(\"" << obj->get_name() << "\")";
			Trace.pop();
		}
		line << "." << ActionName << "()";
		append_line(line.str());
	}

	/// (Optional) document for the current text (could be NULL)
	k3d::idocument* m_document;
	/// (Optional) file path source for the current text (could be empty)
	k3d::filesystem::path m_path;
	/// Set to true iff script playback is in progress
	bool m_running, m_saving, m_recording;

	// Menu items
	Glib::RefPtr<Gtk::ActionGroup> m_actions;
	Glib::RefPtr<Gtk::UIManager> m_ui_manager;
	/// Storage and editing for the current text
	Gtk::TextView* const m_view;
	/// Displays cursor position
	Gtk::Label m_cursor_position;

	// Needed to pass the atk event to an object, since we need to connect to a static function
	typedef sigc::signal<void, click_trace_t&, const k3d::string_t&> action_recorded_t;
	static action_recorded_t& action_recorded_signal()
	{
		static action_recorded_t action_recorded;
		return action_recorded;
	}

	sigc::connection m_record_connection;
};

} // namespace atk_event_recorder

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::atk_event_recorder::dialog::get_factory());
K3D_MODULE_END

