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
#include <k3dsdk/fstream.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/application_window.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>

#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

#include <gtkmm/label.h>
#include <gtk/gtk.h>

using namespace k3d::ngui;

namespace module
{

namespace ngui
{

namespace test_case_recorder
{

class dialog :
	public application_window
{
	typedef application_window base;

public:
	dialog(const k3d::filesystem::path& Path) :
		m_script_engine(k3d::plugin::create<k3d::iscript_engine>(k3d::classes::PythonEngine())),
		m_stream(Path)
	{
		k3d::command_tree().add(*this, "test_case_recorder");

		assert_warning(m_script_engine);

		if(m_script_engine)
			m_script_engine->bless_script(m_stream);

		Gtk::Label* const label = new Gtk::Label(k3d::string_cast(boost::format(_("Recording test case to %1%,\nclose window to cancel recording")) % Path.native_utf8_string().raw()));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("test_case_recorder");
		show_all();

		k3d::command_tree().command_signal().connect(sigc::mem_fun(*this, &dialog::on_command));
	}

	void on_command(k3d::icommand_node& Node, const k3d::icommand_node::type Type, const std::string& Command, const std::string& Arguments)
	{
		// Sanity checks ...
		return_if_fail(m_script_engine);
		return_if_fail(Command.size());

		// Skip everything but UI events ...
		if(Type != k3d::icommand_node::COMMAND_INTERACTIVE)
			return;

		m_script_engine->append_command(m_stream, Node, Command, Arguments);
		m_stream.flush();
	}

	/// Script engine for the script being recorded
	boost::scoped_ptr<k3d::iscript_engine> m_script_engine;
	/// Stores the output file stream
	k3d::filesystem::ofstream m_stream;

	/// Provides a custom plugin factory for creating test_case_recorder::dialog instances
	class plugin_factory :
		public k3d::plugin_factory,
		public k3d::iapplication_plugin_factory
	{
	public:
		plugin_factory() :
			k3d::plugin_factory(k3d::uuid(0x35c7678c, 0x4b4a32cc, 0x552223be, 0x593772b9),
				"NGUITestCaseRecorderDialog",
				_("Provides a user interface for recording test cases, including situations where the application crashes."),
				"NGUI Dialog",
				k3d::iplugin_factory::STABLE,
				boost::assign::map_list_of("ngui:component-type", "dialog"))
		{
		}

		virtual k3d::iunknown* create_plugin()
		{
			k3d::filesystem::path path;
			{
				file_chooser_dialog dialog(_("Save Test Case As:"), k3d::options::path::test_cases(), Gtk::FILE_CHOOSER_ACTION_SAVE);
				dialog.add_pattern_filter(_("Python Script (*.py)"), "*.py");
				dialog.append_extension(".py");
				dialog.add_all_files_filter();

				if(!dialog.get_file_path(path))
					return 0;
			}

			return new dialog(path);
		}

		bool implements(const std::type_info& InterfaceType)
		{
			return false;
		}

		const interfaces_t interfaces()
		{
			return interfaces_t();
		}
	};

	static k3d::iplugin_factory& get_factory()
	{
		static dialog::plugin_factory factory;
		return factory;
	}
};

} // namespace test_case_recorder

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::test_case_recorder::dialog::get_factory());
K3D_MODULE_END

