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

/** \file
		\brief Implements the tutorial menu dialog
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application_window.h"
#include "button.h"
#include "document.h"
#include "messages.h"
#include "options.h"
#include "scripting.h"
#include "learning_menu.h"
#include "tutorial_message.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3dsdk/fstream.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/options.h>
#include <k3dsdk/share.h>
#include <k3dsdk/xml.h>

#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// learning_menu

/// Implements a menu of available tutorials
class learning_menu :
	public application_window
{
	typedef application_window base;

public:
	learning_menu() :
		base("learning_menu", 0),
		m_show_at_startup(_("Show tutorials and examples at startup"))
	{
		load_tutorials();
		load_examples();

		set_title(_("K-3D Tutorials and Examples"));
		set_role("learning_menu");
		set_border_width(10);
		resize(300, 470);
		set_position(Gtk::WIN_POS_CENTER);

		Gtk::HButtonBox* const box2 = Gtk::manage(new Gtk::HButtonBox(Gtk::BUTTONBOX_END));
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE) <<
			connect_button(sigc::mem_fun(*this, &learning_menu::close))));

		Gtk::TreeView* const tutorial_list = Gtk::manage(new Gtk::TreeView(m_tutorial_store));
		tutorial_list->set_headers_visible(false);
		tutorial_list->append_column("Interactive Tutorials", m_columns.label);
		tutorial_list->signal_row_activated().connect(sigc::mem_fun(*this, &learning_menu::on_play_tutorial));
		Gtk::ScrolledWindow* const tutorial_window = Gtk::manage(new Gtk::ScrolledWindow());
		tutorial_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		tutorial_window->add(*tutorial_list);

		Gtk::TreeView* const example_list = Gtk::manage(new Gtk::TreeView(m_example_store));
		example_list->set_headers_visible(false);
		example_list->append_column("Example Documents", m_columns.label);
		example_list->signal_row_activated().connect(sigc::mem_fun(*this, &learning_menu::on_open_example));
		Gtk::ScrolledWindow* const example_window = Gtk::manage(new Gtk::ScrolledWindow());
		example_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		example_window->add(*example_list);

		m_show_at_startup.set_active(options::nag("show_learning_menu"));
		m_show_at_startup.signal_toggled().connect(sigc::mem_fun(*this, &learning_menu::on_show_at_startup));

		Gtk::VBox* const box1 = Gtk::manage(new Gtk::VBox(false, 10));
		box1->pack_start(*Gtk::manage(new Gtk::Label() << set_markup(_("<big><b>Welcome to K-3D!</b></big>"))), Gtk::PACK_SHRINK);
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Below are interactive tutorials and example documents you can\nuse to become familiar with K-3D.\n\nIf this is the first time you've used the program, we recommend\nyou begin with the \"Getting Started\" tutorial."))), Gtk::PACK_SHRINK);
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Double-click to play a tutorial:"))), Gtk::PACK_SHRINK);
		box1->pack_start(*tutorial_window, Gtk::PACK_EXPAND_WIDGET);
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Double-click to open an example document:"))), Gtk::PACK_SHRINK);
		box1->pack_start(*example_window, Gtk::PACK_EXPAND_WIDGET);
		box1->pack_start(m_show_at_startup, Gtk::PACK_SHRINK);
		box1->pack_start(*box2, Gtk::PACK_SHRINK);

		add(*box1);

		show_all();
	}

	/// Set to true while a tutorial is playing
	static bool m_tutorial_playing;

private:
	void load_resources(Glib::RefPtr<Gtk::ListStore>& ResourceModel, const k3d::filesystem::path& ResourcePath, const std::string& ContainerTag, const std::string& ResourceTag)
	{
		ResourceModel = Gtk::ListStore::create(m_columns);

		const k3d::filesystem::path index_path = ResourcePath / k3d::filesystem::generic_path("index.k3d");

		if(!k3d::filesystem::exists(index_path))
		{
			k3d::log() << error << "Couldn't find resource index file [" << index_path.native_console_string() << "]" << std::endl;
			return;
		}

		// Load the index file as XML ...
		k3d::xml::element index("k3dml");
		k3d::filesystem::ifstream stream(index_path);
		stream >> index;

		// Get the set of indexed resources ...
		k3d::xml::element* const xml_application = k3d::xml::find_element(index, "application");
		if(!xml_application)
			return;

		k3d::xml::element* const xml_resources = k3d::xml::find_element(*xml_application, ContainerTag);
		if(!xml_resources)
			return;

		unsigned long resource_number = 0;
		for(k3d::xml::element::elements_t::const_iterator xml_resource = xml_resources->children.begin(); xml_resource != xml_resources->children.end(); ++xml_resource)
		{
			if(xml_resource->name != ResourceTag)
				continue;

			const std::string xml_title = k3d::xml::attribute_text(*xml_resource, "title");
			if(xml_title.empty())
			{
				k3d::log() << error << "Resource without title attribute will be ignored" << std::endl;
				continue;
			}

			const std::string xml_path = k3d::xml::attribute_text(*xml_resource, "path");
			if(xml_path.empty())
			{
				k3d::log() << error << "Resource without path attribute will be ignored" << std::endl;
				continue;
			}

			const k3d::xml::element* const xml_description = k3d::xml::find_element(*xml_resource, "description");

			const k3d::filesystem::path resource_path = ResourcePath / k3d::filesystem::native_path(k3d::ustring::from_utf8(xml_path));
			if(!k3d::filesystem::exists(resource_path))
			{
				k3d::log() << error << "Couldn't locate resource [" << resource_path.native_console_string() << "]" << std::endl;
				continue;
			}

			const std::string label = k3d::string_cast(++resource_number) + ". " + xml_title;

			Gtk::TreeRow row = *ResourceModel->append();
			row[m_columns.title] = xml_title;
			row[m_columns.description] = xml_description ? xml_description->text : std::string();
			row[m_columns.path] = resource_path;
			row[m_columns.label] = label;
		}
	}

	void load_tutorials()
	{
		load_resources(m_tutorial_store, k3d::share_path() / k3d::filesystem::generic_path("tutorials"), "tutorials", "tutorial");
	}

	void load_examples()
	{
		load_resources(m_example_store, k3d::share_path() / k3d::filesystem::generic_path("documents"), "examples", "example");
	}

	void on_play_tutorial(const Gtk::TreePath& Path, Gtk::TreeViewColumn* Column)
	{
		// Get the row that was activated ...
		Gtk::TreeRow row = *m_tutorial_store->get_iter(Path);

		// Look-up the path to the actual tutorial implementation ...
		const k3d::filesystem::path tutorial_path = row[m_columns.path];

		// Make sure it exists!
		if(!k3d::filesystem::exists(tutorial_path))
		{
			error_message(
				_("Could not find the tutorial implementation file.  This may be caused by a partial- or incorrect-installation.\n"
				"If you built the application from CVS source, make sure you re-run bootstrap and configure after every update."));
			return;
		}

		k3d::filesystem::igzstream file(tutorial_path);

		hide();
		close();
		handle_pending_events();

		const bool tutorial_playing = m_tutorial_playing;
		m_tutorial_playing = true;

		k3d::iscript_engine::context_t context;
		execute_script(file, tutorial_path.native_utf8_string().raw(), context);

		m_tutorial_playing = tutorial_playing;

		tutorial_message::instance().hide_messages();
	}

	void on_open_example(const Gtk::TreePath& Path, Gtk::TreeViewColumn* Column)
	{
		// Get the row that was activated ...
		Gtk::TreeRow row = *m_example_store->get_iter(Path);

		// Look-up the path to the actual example document ...
		const Glib::ustring title = row[m_columns.title];
		const Glib::ustring description = row[m_columns.description];
		const k3d::filesystem::path path = row[m_columns.path];

		// Make sure it exists!
		if(!k3d::filesystem::exists(path))
		{
			error_message(_("Could not find the example document.  This may be caused by a partial- or incorrect-installation."));
			return;
		}

		// (Optionally) display a description of the document ...
		if(!description.empty())
		{
			const std::string message = k3d::string_cast(boost::format(_("Open example document \"%1%\"?")) % title);

			Gtk::MessageDialog dialog(*this, "", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
			dialog.set_message(message);
			dialog.set_secondary_text(description);

			dialog.set_default_response(Gtk::RESPONSE_YES);

			if(dialog.run() != Gtk::RESPONSE_YES)
				return;
		}

		hide();
		close();
		handle_pending_events();

		open_document(path);
	}

	void on_show_at_startup()
	{
		options::enable_nag("show_learning_menu", m_show_at_startup.get_active());
		k3d::options::commit();
	}

	class resource_columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		resource_columns()
		{
			add(title);
			add(description);
			add(path);
			add(label);
		}

		Gtk::TreeModelColumn<Glib::ustring> title;
		Gtk::TreeModelColumn<Glib::ustring> description;
		Gtk::TreeModelColumn<k3d::filesystem::path> path;
		Gtk::TreeModelColumn<Glib::ustring> label;
	};

	resource_columns m_columns;
	Glib::RefPtr<Gtk::ListStore> m_tutorial_store;
	Glib::RefPtr<Gtk::ListStore> m_example_store;
	Gtk::CheckButton m_show_at_startup;
};

bool learning_menu::m_tutorial_playing = false;


void create_learning_menu()
{
	new learning_menu();
}

const bool tutorial_playing()
{
	return learning_menu::m_tutorial_playing;
}

} // namespace libk3dngui

