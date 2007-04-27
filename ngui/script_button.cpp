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
		\brief Implements k3d::script_button, which provides a MVC UI for boolean data sources
		\author Tim Shead (tshead@k-3d.com)
*/

#include "file_chooser_dialog.h"
#include "interactive.h"
#include "script_button.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/options.h>
#include <k3dsdk/scripting.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/system.h>

#include <gtkmm/tooltips.h>

#include <k3dsdk/fstream.h>
#include <boost/format.hpp>

#include <sstream>

namespace libk3dngui
{

namespace script_button
{

/// Specialization of k3d::script_button::data_proxy for use with k3d::iproperty objects
template<>
class data_proxy<k3d::iproperty> :
	public idata_proxy
{
public:
	typedef k3d::iproperty data_t;

	data_proxy(data_t& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
		idata_proxy(StateRecorder, ChangeMessage),
		m_readable_data(Data),
		m_writable_data(dynamic_cast<k3d::iwritable_property*>(&Data))
	{
	}

	std::string value()
	{
		return boost::any_cast<std::string>(m_readable_data.property_value());
	}

	void set_value(const std::string Value)
	{
		return_if_fail(m_writable_data);
		m_writable_data->property_set_value(Value);
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_readable_data;
	k3d::iwritable_property* const m_writable_data;
};

std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data, k3d::istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage)
{
	return std::auto_ptr<idata_proxy>(new data_proxy<k3d::iproperty>(Data, StateRecorder, ChangeMessage));
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
	base(false, 0),
	ui_component(Name, &Parent),
	m_load(_("Load")),
	m_save(_("Save")),
	m_edit(_("Edit Script")),
	m_data(Data)
{
	m_load.signal_clicked().connect(sigc::mem_fun(*this, &control::on_load));
	m_save.signal_clicked().connect(sigc::mem_fun(*this, &control::on_save));
	m_edit.signal_clicked().connect(sigc::mem_fun(*this, &control::on_edit));

	pack_start(m_load, Gtk::PACK_SHRINK);
	pack_start(m_save, Gtk::PACK_SHRINK);
	pack_start(m_edit, Gtk::PACK_EXPAND_WIDGET);

	tooltips().set_tip(m_load, _("Load a script from disk, replacing the current script"));
	tooltips().set_tip(m_save, _("Save the current script to disk"));
	tooltips().set_tip(m_edit, _("Open the current script for editing"));

	set_name("k3d-script-button");

	// Update the display and ensure we get a notification anytime the data source changes ...
	return_if_fail(m_data.get());

	update(0);
	m_data->changed_signal().connect(sigc::mem_fun(*this, &control::update));
}

void control::update(k3d::iunknown*)
{
	return_if_fail(m_data.get());
}

void control::on_load()
{
	return_if_fail(m_data.get());

	k3d::filesystem::path input_path;

	{
		file_chooser_dialog dialog(_("Load Script:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
		if(!dialog.get_file_path(input_path))
			return;
	}

	std::stringstream buffer;
	k3d::filesystem::ifstream input_stream(input_path);
	input_stream >> buffer.rdbuf();
	input_stream.close();

	if(buffer.str() != m_data->value())
	{
		if(m_data->state_recorder)
			m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

		m_data->set_value(buffer.str());

		if(m_data->state_recorder)
			m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message, __PRETTY_FUNCTION__);
	}
}

void control::on_save()
{
	return_if_fail(m_data.get());

	k3d::filesystem::path output_path;

	{
		file_chooser_dialog dialog(_("Save Script:"), k3d::options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		if(!dialog.get_file_path(output_path))
			return;
	}

	k3d::filesystem::ofstream output_stream(output_path);
	output_stream << m_data->value();
}

void control::on_edit()
{
	return_if_fail(m_data.get());

	try
	{
		k3d::filesystem::path temp_path = k3d::system::generate_temp_file();
		return_if_fail(!temp_path.empty());

		const k3d::script::code original_value(m_data->value());

		k3d::iplugin_factory* const language = k3d::script::language(original_value).factory();
		if(language && language->class_id() == k3d::classes::PythonEngine())
			temp_path = temp_path + ".py";
		else if(language && language->class_id() == k3d::classes::K3DScriptEngine())
			temp_path = temp_path + ".k3dscript";
		else
			k3d::log() << warning << "unknown script language" << std::endl;


		k3d::filesystem::ofstream output_stream(temp_path);
		output_stream << original_value.source();
		output_stream.close();

		boost::format command_line(k3d::options::get_command(k3d::options::command::text_editor()));
		command_line % temp_path.native_utf8_string().raw();
		k3d::system::spawn_sync(command_line.str());

		std::stringstream buffer;
		k3d::filesystem::ifstream input_stream(temp_path);
		input_stream >> buffer.rdbuf();
		input_stream.close();

		if(buffer.str() != original_value.source())
		{
			if(m_data->state_recorder)
				m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

			m_data->set_value(buffer.str());

			if(m_data->state_recorder)
				m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message, __PRETTY_FUNCTION__);
		}

		k3d::filesystem::remove(temp_path);
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Caught exception: " << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "Caught unknown exception" << std::endl;
	}
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	return ui_component::execute_command(Command, Arguments);
}

} // namespace script_button

} // namespace libk3dngui

