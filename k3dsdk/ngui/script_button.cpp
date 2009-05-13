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

#include "file_chooser_dialog.h"
#include "script_button.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/iproperty_sink.h>
#include <k3dsdk/istate_recorder_sink.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/scripting.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/system.h>

#include <gtkmm/tooltips.h>

#include <boost/format.hpp>

#include <sstream>

namespace k3d
{

namespace ngui
{

namespace script_button
{

/////////////////////////////////////////////////////////////////////////////
// property_model

/// Implementation of script_button::imodel for use with iproperty objects
class property_model :
	public imodel
{
public:
	property_model(iproperty& Data) :
		m_readable_data(Data),
		m_writable_data(dynamic_cast<iwritable_property*>(&Data))
	{
	}

	iproperty* property()
	{
		return &m_readable_data;
	}

	const string_t value()
	{
		return boost::any_cast<string_t>(m_readable_data.property_internal_value());
	}

	void set_value(const string_t& Value)
	{
		return_if_fail(m_writable_data);
		m_writable_data->property_set_value(Value);
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

private:
	iproperty& m_readable_data;
	iwritable_property* const m_writable_data;
};

/////////////////////////////////////////////////////////////////////////////
// model

imodel* const model(iproperty& Property)
{
	return new property_model(Property);
}

/////////////////////////////////////////////////////////////////////////////
// control

control::control(imodel* const Model, istate_recorder* const StateRecorder, const Glib::ustring& ChangeMessage) :
	base(false, 0),
	m_load(_("Load")),
	m_save(_("Save")),
	m_edit(_("Edit Script")),
	m_model(Model),
	m_state_recorder(StateRecorder),
	m_change_message(ChangeMessage)
{
	m_load.signal_clicked().connect(sigc::mem_fun(*this, &control::on_load));
	m_save.signal_clicked().connect(sigc::mem_fun(*this, &control::on_save));
	m_edit.signal_clicked().connect(sigc::mem_fun(*this, &control::on_edit));

	pack_start(m_load, Gtk::PACK_SHRINK);
	pack_start(m_save, Gtk::PACK_SHRINK);
	pack_start(m_edit, Gtk::PACK_EXPAND_WIDGET);

	m_load.set_tooltip_text(_("Load a script from disk, replacing the current script"));
	m_save.set_tooltip_text(_("Save the current script to disk"));
	m_edit.set_tooltip_text(_("Open the current script for editing"));

	set_name("k3d-script-button");

	// Update the display and ensure we get a notification anytime the data source changes ...
	return_if_fail(m_model);

	update(0);
	m_model->changed_signal().connect(sigc::mem_fun(*this, &control::update));
}

control::~control()
{
	delete m_model;
}

void control::update(ihint*)
{
	return_if_fail(m_model);
}

void control::on_load()
{
	return_if_fail(m_model);

	filesystem::path input_path;

	{
		file_chooser_dialog dialog(_("Load Script:"), options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_OPEN);
		if(!dialog.get_file_path(input_path))
			return;
	}

	std::stringstream buffer;
	filesystem::ifstream input_stream(input_path);
	input_stream >> buffer.rdbuf();
	input_stream.close();

	if(buffer.str() != m_model->value())
	{
		if(m_state_recorder)
			m_state_recorder->start_recording(create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

		m_model->set_value(buffer.str());

		if(m_state_recorder)
			m_state_recorder->commit_change_set(m_state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), m_change_message, K3D_CHANGE_SET_CONTEXT);
	}
}

void control::on_save()
{
	return_if_fail(m_model);

	filesystem::path output_path;

	{
		file_chooser_dialog dialog(_("Save Script:"), options::path::scripts(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		if(!dialog.get_file_path(output_path))
			return;
	}

	filesystem::ofstream output_stream(output_path);
	output_stream << m_model->value();
}

void control::on_edit()
{
	return_if_fail(m_model);

	if(Gtk::Window* const window = plugin::create<Gtk::Window>("NGUITextEditorDialog"))
	{
		if(m_model->property())
		{
			if(iproperty_sink* const property_sink = dynamic_cast<iproperty_sink*>(window))
				property_sink->set_property(m_model->property());
		}

		if(istate_recorder_sink* const state_recorder_sink = dynamic_cast<istate_recorder_sink*>(window))
			state_recorder_sink->set_state_recorder(m_state_recorder);

		if(Gtk::Window* const top_level = dynamic_cast<Gtk::Window*>(get_toplevel()))
			window->set_transient_for(*top_level);
	}
}

} // namespace script_button

} // namespace ngui

} // namespace k3d

