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
#include "messages.h"
#include "options.h"
#include "scripting.h"
#include "tutorial_menu.h"
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
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// tutorial_menu

/// Implements a menu of available tutorials
class tutorial_menu :
	public application_window
{
	typedef application_window base;

public:
	tutorial_menu() :
		base("tutorial_menu", 0),
		m_show_tutorials(_("Show tutorials at startup"))
	{
		load_tutorials();

		set_title(_("K-3D Tutorials"));
		set_role("tutorial_menu");
		set_border_width(10);
		resize(250, 350);
		set_position(Gtk::WIN_POS_CENTER);

		Gtk::HButtonBox* const box2 = Gtk::manage(new Gtk::HButtonBox(Gtk::BUTTONBOX_END));
		box2->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE) <<
			connect_button(sigc::mem_fun(*this, &tutorial_menu::close))));

		Gtk::TreeView* const list = Gtk::manage(new Gtk::TreeView(m_tutorial_store));
		list->set_headers_visible(false);
		list->append_column("Tutorials", m_tutorial_columns.name);
		list->signal_row_activated().connect(sigc::mem_fun(*this, &tutorial_menu::on_play_tutorial));

		Gtk::ScrolledWindow* const scrolled_window = Gtk::manage(new Gtk::ScrolledWindow());
		scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		scrolled_window->add(*list);

		m_show_tutorials.set_active(options::nag("show_tutorials"));
		m_show_tutorials.signal_toggled().connect(sigc::mem_fun(*this, &tutorial_menu::on_show_tutorials));

		Gtk::VBox* const box1 = Gtk::manage(new Gtk::VBox(false, 10));
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Welcome to K-3D!"))), Gtk::PACK_SHRINK);
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Below is a list of interactive tutorials you can\nrun to become familiar with K-3D.  If this is the\nfirst time you've used the program, we recommend\nyou begin with the \"Getting Started\" tutorial."))), Gtk::PACK_SHRINK);
		box1->pack_start(*Gtk::manage(new Gtk::Label(_("Double-click a tutorial to begin:"))), Gtk::PACK_SHRINK);
		box1->pack_start(*scrolled_window, Gtk::PACK_EXPAND_WIDGET);
		box1->pack_start(m_show_tutorials, Gtk::PACK_SHRINK);
		box1->pack_start(*box2, Gtk::PACK_SHRINK);

		add(*box1);

		show_all();
	}

	/// Set to true while a tutorial is playing
	static bool m_tutorial_playing;

private:
	void load_tutorials()
	{
		m_tutorial_store = Gtk::ListStore::create(m_tutorial_columns);

		const k3d::filesystem::path tutorials_path = k3d::share_path() / k3d::filesystem::generic_path("tutorials");
		const k3d::filesystem::path index_path = tutorials_path / k3d::filesystem::generic_path("index.k3d");

		if(!k3d::filesystem::exists(index_path))
		{
			k3d::log() << error << "Couldn't find tutorial index file [" << index_path.native_console_string() << "]" << std::endl;
			return;
		}

		// Load the index file as XML ...
		k3d::xml::element index("k3dml");
		k3d::filesystem::ifstream stream(index_path);
		stream >> index;

		// Get the set of indexed tutorials ...
		k3d::xml::element* const xml_application = k3d::xml::find_element(index, "application");
		if(!xml_application)
			return;

		k3d::xml::element* const xml_tutorials = k3d::xml::find_element(*xml_application, "tutorials");
		if(!xml_tutorials)
			return;

		unsigned long tutorialnumber = 0;
		for(k3d::xml::element::elements_t::const_iterator tutorial = xml_tutorials->children.begin(); tutorial != xml_tutorials->children.end(); ++tutorial)
		{
			if(tutorial->name != "tutorial")
				continue;

			const std::string xml_title = k3d::xml::attribute_text(*tutorial, "title");
			if(xml_title.empty())
			{
				k3d::log() << error << "Tutorial without title attribute will be ignored" << std::endl;
				continue;
			}

			const std::string xml_path = k3d::xml::attribute_text(*tutorial, "path");
			if(xml_path.empty())
			{
				k3d::log() << error << "Tutorial without path attribute will be ignored" << std::endl;
				continue;
			}

			const k3d::filesystem::path tutorial_path = tutorials_path / k3d::filesystem::native_path(k3d::ustring::from_utf8(xml_path));
			if(!k3d::filesystem::exists(tutorial_path))
			{
				k3d::log() << error << "Couldn't locate tutorial [" << tutorial_path.native_console_string() << "]" << std::endl;
				continue;
			}

			const std::string tutorial_title = k3d::string_cast(++tutorialnumber) + ". " + xml_title;

			Gtk::TreeRow row = *m_tutorial_store->append();
			row[m_tutorial_columns.name] = tutorial_title;
			row[m_tutorial_columns.path] = tutorial_path;
		}
	}

	void on_play_tutorial(const Gtk::TreePath& Path, Gtk::TreeViewColumn* Column)
	{
		// Get the row that was activated ...
		Gtk::TreeRow row = *m_tutorial_store->get_iter(Path);

		// Look-up the path to the actual tutorial implementation ...
		const k3d::filesystem::path tutorial_path = row[m_tutorial_columns.path];

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

	void on_show_tutorials()
	{
		options::enable_nag("show_tutorials", m_show_tutorials.get_active());
		k3d::options::commit();
	}

	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(name);
			add(path);
		}

		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<k3d::filesystem::path> path;
	};

	columns m_tutorial_columns;
	Glib::RefPtr<Gtk::ListStore> m_tutorial_store;
	Gtk::CheckButton m_show_tutorials;
};

bool tutorial_menu::m_tutorial_playing = false;


void create_tutorial_menu()
{
	new tutorial_menu();
}

const bool tutorial_playing()
{
	return tutorial_menu::m_tutorial_playing;
}

} // namespace libk3dngui

