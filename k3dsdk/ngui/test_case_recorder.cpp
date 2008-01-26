// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "application_window.h"
#include "file_chooser_dialog.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/log.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_cast.h>

#include <boost/scoped_ptr.hpp>

#include <gtkmm/label.h>
#include <gtk/gtk.h>

namespace libk3dngui
{

class test_case_recorder :
	public application_window
{
	typedef application_window base;

public:
	test_case_recorder(const k3d::filesystem::path& Path) :
		base("test_case_recorder", 0),
		m_script_engine(k3d::plugin::create<k3d::iscript_engine>(k3d::classes::PythonEngine())),
		m_stream(Path)
	{
		assert_warning(m_script_engine);

		if(m_script_engine)
			m_script_engine->bless_script(m_stream);

		Gtk::Label* const label = new Gtk::Label(k3d::string_cast(boost::format(_("Recording test case to %1%,\nclose window to cancel recording")) % Path.native_utf8_string().raw()));
		add(*Gtk::manage(label));

		set_border_width(10);

		set_role("test_case_recorder");
		show_all();

		k3d::command_tree().command_signal().connect(sigc::mem_fun(*this, &test_case_recorder::on_command));
	}

private:
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
};

void create_test_case_recorder()
{
	k3d::filesystem::path path;
	{
		file_chooser_dialog dialog(_("Save Test Case As:"), k3d::options::path::test_cases(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.add_pattern_filter(_("Python Script (*.py)"), "*.py");
		dialog.append_extension(".py");
		dialog.add_all_files_filter();

		if(!dialog.get_file_path(path))
			return;
	}

	new test_case_recorder(path);
}

} // namespace libk3dngui

