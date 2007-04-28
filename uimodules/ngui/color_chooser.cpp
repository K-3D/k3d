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
#include "button.h"
#include "color_chooser.h"
#include "interactive.h"
#include "utility.h"
#include "widget_manip.h"

#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/string_cast.h>

#include <gtkmm/box.h>
#include <gtkmm/colorselection.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

#include <iomanip>

namespace libk3dngui
{

namespace color_chooser
{

namespace detail
{

/// Tutorial-enabled implementation of a standard color selection dialog
class color_selection_dialog:
	public application_window
{
	typedef application_window base;

public:
	color_selection_dialog(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data) :
		base(Name, &Parent),
		m_data(Data)
	{
		Gtk::VBox* const vbox = new Gtk::VBox(false);
		add(*manage(vbox));

		m_color_selection.set_has_opacity_control(false);
		m_color_selection.set_has_palette(true);
		m_color_changed_connection = m_color_selection.signal_color_changed().connect(sigc::mem_fun(*this, &color_selection_dialog::on_color_changed));
		vbox->pack_start(m_color_selection, Gtk::PACK_EXPAND_WIDGET);

		Gtk::HButtonBox* const bbox = new Gtk::HButtonBox(Gtk::BUTTONBOX_END);
		vbox->pack_start(*Gtk::manage(bbox));

		bbox->pack_start(*Gtk::manage(
			new button::control(*this, "close", Gtk::Stock::CLOSE) <<
			connect_button(sigc::mem_fun(*this, &color_selection_dialog::close))));

		on_data_changed(0);

		if(m_data.get())
			m_data->changed_signal().connect(sigc::mem_fun(*this, &color_selection_dialog::on_data_changed));

		show_all();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		if(Command == "set_value")
		{
			interactive::show(m_color_selection);
			interactive::move_pointer(m_color_selection);
			m_color_selection.set_current_color(convert(k3d::from_string<k3d::color>(Arguments, k3d::color(1.0, 1.0, 1.0))));

			return RESULT_CONTINUE;
		}

		return base::execute_command(Command, Arguments);
	}

private:
	void on_color_changed()
	{
		return_if_fail(m_data.get());

		const k3d::color color = convert(m_color_selection.get_current_color());
		if(color == m_data->value())
			return;

		record_command("set_value", k3d::string_cast(color));

		if(m_data->state_recorder)
			m_data->state_recorder->start_recording(k3d::create_state_change_set(__PRETTY_FUNCTION__), __PRETTY_FUNCTION__);

		m_data->set_value(color);

		if(m_data->state_recorder)
			m_data->state_recorder->commit_change_set(m_data->state_recorder->stop_recording(__PRETTY_FUNCTION__), m_data->change_message + " " + k3d::string_cast(color), __PRETTY_FUNCTION__);
	}

	void on_data_changed(k3d::iunknown*)
	{
		return_if_fail(m_data.get());

		const Gdk::Color color(convert(m_data->value()));
		if(color != m_color_selection.get_current_color())
		{
			m_color_changed_connection.block();
			m_color_selection.set_current_color(color);
			m_color_changed_connection.unblock();
		}
	}

	Gtk::ColorSelection m_color_selection;
	sigc::connection m_color_changed_connection;
	std::auto_ptr<idata_proxy> m_data;
};

} // namespace detail

/// Specialization of k3d::color_chooser::data_proxy for use with k3d::iproperty objects
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

	k3d::color value()
	{
		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(k3d::color))
			return boost::any_cast<k3d::color>(m_readable_data.property_value());
		else if(type == typeid(k3d::ri::color))
			return boost::any_cast<k3d::ri::color>(m_readable_data.property_value());
		else
			k3d::log() << error << "unknown property type: " << type.name() << std::endl;

		return k3d::color(0, 0, 0);
	}

	void set_value(const k3d::color Value)
	{
		return_if_fail(m_writable_data);

		const std::type_info& type = m_readable_data.property_type();
		if(type == typeid(k3d::color))
			m_writable_data->property_set_value(Value);
		else if(type == typeid(k3d::ri::color))
			m_writable_data->property_set_value(k3d::ri::color(Value));
		else
			k3d::log() << error << "unknown property type: " << type.name() << std::endl;
	}

	changed_signal_t& changed_signal()
	{
		return m_readable_data.property_changed_signal();
	}

	std::auto_ptr<idata_proxy> clone()
	{
		return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(m_readable_data, state_recorder, change_message));
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
	ui_component(Name, &Parent),
	m_area(new Gtk::DrawingArea()),
	m_data(Data)
{
	m_area->signal_expose_event().connect(sigc::hide(sigc::mem_fun(*this, &control::on_redraw)));
	add(*manage(m_area));

	data_changed(0);

	if(m_data.get())
		m_data->changed_signal().connect(sigc::mem_fun(*this, &control::data_changed));
}

control::~control()
{
	m_deleted_signal.emit();
}

const k3d::icommand_node::result control::execute_command(const std::string& Command, const std::string& Arguments)
{
	if(Command == "activate")
	{
		interactive::activate(*this);
		return RESULT_CONTINUE;
	}

	return ui_component::execute_command(Command, Arguments);
}

bool control::on_redraw()
{
	return_val_if_fail(m_data.get(), false);

	Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(m_area->get_window());
	Gdk::Color color = convert(m_data->value());
	m_area->get_default_colormap()->alloc_color(color);
	gc->set_foreground(color);

	m_area->get_window()->draw_rectangle(gc, true, 0, 0, m_area->get_width(), m_area->get_height());

	return true;
}

void control::on_clicked()
{
	record_command("activate");

	return_if_fail(m_data.get());

	detail::color_selection_dialog* const color_selection = new detail::color_selection_dialog(*this, "selection", m_data->clone());
	m_deleted_signal.connect(sigc::mem_fun(*color_selection, &detail::color_selection_dialog::close));
	color_selection->show();

	base::on_clicked();
}

void control::data_changed(k3d::iunknown*)
{
	m_area->queue_draw();
}

} // namespace color_chooser

} // namespace libk3dngui

