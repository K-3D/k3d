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
	\brief Implements k3d::selection_button, which provides a MVC UI for boolean data sources
	\author Tim Shead (tshead@k-3d.com)
*/

#include "selection_button.h"
#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/state_change_set.h>

#include <gtkmm/button.h>
#include <gtkmm/tooltips.h>

#include <sstream>

namespace k3d
{

namespace ngui
{

namespace selection_button
{

/// Specialization of k3d::selection_button::data_proxy for use with k3d::iproperty objects
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

	k3d::mesh_selection value()
	{
		return boost::any_cast<k3d::mesh_selection>(m_readable_data.property_internal_value());
	}

	void set_value(const k3d::mesh_selection& Value)
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

control::control(std::auto_ptr<idata_proxy> Data) :
	base(false, 0),
	m_data(Data),
	m_select_all_button(new Gtk::Button(_("Select All"))),
	m_deselect_all_button(new Gtk::Button(_("Deselect All"))),
	m_select_null_button(new Gtk::Button(_("Clear")))
{
	set_name("k3d-selection-button");

	m_select_all_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_select_all));
	m_deselect_all_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_deselect_all));
	m_select_null_button->signal_clicked().connect(sigc::mem_fun(*this, &control::on_select_null));

	pack_start(*manage(m_select_all_button));
	pack_start(*manage(m_deselect_all_button));
	pack_start(*manage(m_select_null_button));

	update(0);

	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::update));
}

void control::on_select_all()
{
	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	// Update everything with the new value ...
	m_data->set_value(k3d::mesh_selection::select_all());

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), _("Select All"), K3D_CHANGE_SET_CONTEXT);
}

void control::on_deselect_all()
{
	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	// Update everything with the new value ...
	m_data->set_value(k3d::mesh_selection::deselect_all());

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), _("Deselect All"), K3D_CHANGE_SET_CONTEXT);
}

void control::on_select_null()
{
	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->start_recording(k3d::create_state_change_set(K3D_CHANGE_SET_CONTEXT), K3D_CHANGE_SET_CONTEXT);

	// Update everything with the new value ...
	m_data->set_value(k3d::mesh_selection::select_null());

	// Turn this into an undo/redo -able event ...
	if(m_data->state_recorder)
		m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(K3D_CHANGE_SET_CONTEXT), _("Clear Selection"), K3D_CHANGE_SET_CONTEXT);
}

void control::update(k3d::ihint*)
{
	if(!m_data.get())
		return;

	const k3d::mesh_selection selection = m_data->value();

	m_select_all_button->set_sensitive(selection != k3d::mesh_selection::select_all());
	m_deselect_all_button->set_sensitive(selection != k3d::mesh_selection::deselect_all());
	m_select_null_button->set_sensitive(selection != k3d::mesh_selection::select_null());

	std::ostringstream buffer;
	if(selection == k3d::mesh_selection::select_all())
	{
		buffer << _("All components are selected");
	}
	else if(selection == k3d::mesh_selection::deselect_all())
	{
		buffer << _("All components are deselected");
	}
	else if(selection == k3d::mesh_selection::select_null())
	{
		buffer << _("Using upstream component selection");
	}
	else
	{
		buffer << _("Partial selection");
	}

	tooltips().set_tip(*m_select_all_button, buffer.str());
	tooltips().set_tip(*m_deselect_all_button, buffer.str());
	tooltips().set_tip(*m_select_null_button, buffer.str());
}

} // namespace selection_button

} // namespace ngui

} // namespace k3d

