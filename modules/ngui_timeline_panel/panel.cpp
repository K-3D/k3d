// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3d-i18n-config.h>

#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/button.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/interactive.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/ngui/utility.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/data.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/module.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/time_source.h>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/entry.h>
#include <gtkmm/scrollbar.h>

#include <boost/assign/list_of.hpp>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui_timeline
{

namespace detail
{

class implementation :
	public libk3dngui::asynchronous_update,
	public k3d::property_collection
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		k3d::property_collection(),
		m_document_state(DocumentState),
		m_playback_mode(init_name("playback_mode") + init_label(_("Playback Mode")) + init_description(_("When on, plays animation")) + init_value(STOP)),
		m_container(false, 0),
		m_rewind(Parent, "rewind", *Gtk::manage(new Gtk::Image(load_icon("rewind", Gtk::ICON_SIZE_BUTTON)))),
		m_loop_reverse_play(Parent, "reverse_play_loop", *Gtk::manage(new Gtk::Image(load_icon("reverse_play_loop", Gtk::ICON_SIZE_BUTTON)))),
		m_reverse_play(Parent, "reverse_play", *Gtk::manage(new Gtk::Image(load_icon("reverse_play", Gtk::ICON_SIZE_BUTTON)))),
		m_stop(Parent, "stop", *Gtk::manage(new Gtk::Image(load_icon("stop", Gtk::ICON_SIZE_BUTTON)))),
		m_play(Parent, "play", *Gtk::manage(new Gtk::Image(load_icon("play", Gtk::ICON_SIZE_BUTTON)))),
		m_loop_play(Parent, "play_loop", *Gtk::manage(new Gtk::Image(load_icon("play_loop", Gtk::ICON_SIZE_BUTTON)))),
		m_fast_forward(Parent, "fast_forward", *Gtk::manage(new Gtk::Image(load_icon("fast_forward", Gtk::ICON_SIZE_BUTTON)))),
		m_start_time(0),
		m_end_time(0),
		m_frame_rate(0),
		m_time(0),
		m_writable_time(0)
	{
		m_frame_label.set_editable(false);
		m_frame_label.set_width_chars(5);

		m_rewind.set_tip(_("Rewind"));
		m_loop_reverse_play.set_tip(_("Continuous reverse playback"));
		m_reverse_play.set_tip(_("Reverse playback"));
		m_stop.set_tip(_("Stop"));
		m_play.set_tip(_("Play"));
		m_loop_play.set_tip(_("Continuous playback"));
		m_fast_forward.set_tip(_("Fast Forward"));

		m_container.pack_start(m_scrollbar, Gtk::PACK_EXPAND_WIDGET);
		m_container.pack_start(m_frame_label, Gtk::PACK_SHRINK);
		m_container.pack_start(m_rewind, Gtk::PACK_SHRINK);
		m_container.pack_start(m_loop_reverse_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_reverse_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_stop, Gtk::PACK_SHRINK);
		m_container.pack_start(m_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_loop_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_fast_forward, Gtk::PACK_SHRINK);

		m_rewind.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_rewind));
		m_loop_reverse_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_loop_reverse_play));
		m_reverse_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_reverse_play));
		m_stop.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_stop));
		m_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_play));
		m_loop_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_loop_play));
		m_fast_forward.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_fast_forward));

		m_playback_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_playback_mode_changed));

		m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
		m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));

		reset_time_properties();
		on_time_changed(0);
		m_scrollbar.get_adjustment()->signal_value_changed().connect(sigc::mem_fun(*this, &implementation::on_scrollbar_value_changed));

		m_container.show_all();
		schedule_update();
	}

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
		reset_time_properties();
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		reset_time_properties();
	}

	void reset_time_properties()
	{
		m_start_time = 0;
		m_end_time = 0;
		m_frame_rate = 0;
		m_time = 0;
		m_writable_time = 0;

		m_start_time_changed_connection.disconnect();
		m_end_time_changed_connection.disconnect();
		m_frame_rate_changed_connection.disconnect();
		m_time_changed_connection.disconnect();

		m_start_time = k3d::get_start_time(m_document_state.document());
		if(m_start_time)
			m_start_time_changed_connection = m_start_time->property_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_start_time_changed));

		m_end_time = k3d::get_end_time(m_document_state.document());
		if(m_end_time)
			m_end_time_changed_connection = m_end_time->property_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_end_time_changed));

		m_frame_rate = k3d::get_frame_rate(m_document_state.document());
		if(m_frame_rate)
			m_frame_rate_changed_connection = m_frame_rate->property_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_frame_rate_changed));

		m_time = k3d::get_time(m_document_state.document());
		if(m_time)
			m_time_changed_connection = m_time->property_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_time_changed));

		m_writable_time = dynamic_cast<k3d::iwritable_property*>(m_time);

		const bool enabled = m_start_time && m_end_time && m_frame_rate && m_time && m_writable_time;

		m_scrollbar.set_sensitive(enabled);
		m_rewind.set_sensitive(enabled);
		m_loop_reverse_play.set_sensitive(enabled);
		m_reverse_play.set_sensitive(enabled);
		m_stop.set_sensitive(enabled);
		m_play.set_sensitive(enabled);
		m_loop_play.set_sensitive(enabled);
		m_fast_forward.set_sensitive(enabled);
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		if(Command == "set_time")
		{
			const double new_time = k3d::from_string<double>(Arguments, 0.0);
			interactive::warp_pointer(m_scrollbar);

			if(m_writable_time)
				m_writable_time->property_set_value(new_time);

			return k3d::icommand_node::RESULT_CONTINUE;
		}

		return k3d::icommand_node::RESULT_UNKNOWN_COMMAND;
	}

	void on_update()
	{
		return_if_fail(m_start_time && m_end_time && m_frame_rate && m_time);

		const double start_time = boost::any_cast<double>(m_start_time->property_value());
		const double end_time = boost::any_cast<double>(m_end_time->property_value());
		const double frame_rate = boost::any_cast<double>(m_frame_rate->property_value());
		const double time = boost::any_cast<double>(m_time->property_value());

		return_if_fail(frame_rate);
		const double frame_length = 1.0 / frame_rate;

		const double step_increment = 1.0;
		const double page_increment = 1.0;
		const double page_size = page_increment;

		const double value = time;
		const double lower = start_time;
		const double upper = end_time - frame_length + page_size;

//		m_scrollbar.set_adjustment(Gtk::Adjustment(value, lower, upper, step_increment, page_increment, page_size));

		m_scrollbar.get_adjustment()->set_value(value);
		m_scrollbar.get_adjustment()->set_lower(lower);
		m_scrollbar.get_adjustment()->set_upper(upper);

		m_scrollbar.get_adjustment()->set_step_increment(step_increment);
		m_scrollbar.get_adjustment()->set_page_increment(page_increment);
		m_scrollbar.get_adjustment()->set_page_size(page_size);
	}

	void on_start_time_changed(k3d::iunknown*)
	{
		schedule_update();
	}

	void on_end_time_changed(k3d::iunknown*)
	{
		schedule_update();
	}

	void on_frame_rate_changed(k3d::iunknown*)
	{
		schedule_update();
	}

	void on_time_changed(k3d::iunknown*)
	{
		return_if_fail(m_time && m_frame_rate);
		const double frame_rate = boost::any_cast<double>(m_frame_rate->property_value());
		const double time = boost::any_cast<double>(m_time->property_value());

		m_frame_label.set_text(k3d::string_cast(k3d::round(time * frame_rate)));
//		m_time_label.set_text(k3d::string_cast(time));

		m_scrollbar.get_adjustment()->set_value(time);
	}

	void on_scrollbar_value_changed()
	{
		const double new_time = m_scrollbar.get_adjustment()->get_value();
		m_command_signal.emit("set_time", k3d::string_cast(new_time));

		if(m_writable_time)
			m_writable_time->property_set_value(new_time);
	}

	void on_rewind()
	{
		m_playback_mode.set_value(STOP);

		return_if_fail(m_start_time && m_writable_time);
		const double start_time = boost::any_cast<double>(m_start_time->property_value());

		m_writable_time->property_set_value(start_time);
	}

	void on_loop_reverse_play()
	{
		m_playback_mode.set_value(LOOP_REVERSE_PLAY);
	}

	void on_reverse_play()
	{
		m_playback_mode.set_value(REVERSE_PLAY);
	}

	void on_stop()
	{
		m_playback_mode.set_value(STOP);
	}

	void on_play()
	{
		m_playback_mode.set_value(PLAY);
	}

	void on_loop_play()
	{
		m_playback_mode.set_value(LOOP_PLAY);
	}

	void on_fast_forward()
	{
		m_playback_mode.set_value(STOP);

		return_if_fail(m_frame_rate && m_end_time && m_writable_time);

		const double end_time = boost::any_cast<double>(m_end_time->property_value());
		const double frame_rate = boost::any_cast<double>(m_frame_rate->property_value());
		return_if_fail(frame_rate);
		const double frame_length = 1.0 / frame_rate;

		m_writable_time->property_set_value(end_time - frame_length);
	}

	void on_playback_mode_changed(k3d::iunknown*)
	{
//k3d::log() << debug << m_playback_mode.value() << std::endl;
		switch(m_playback_mode.value())
		{
			case LOOP_REVERSE_PLAY:
			case REVERSE_PLAY:
			case PLAY:
			case LOOP_PLAY:
				if(!m_playback_connection.connected())
					m_playback_connection = Glib::signal_idle().connect(sigc::mem_fun(*this, &implementation::on_playback_handler));
				return;
			case STOP:
				m_playback_connection.disconnect();
				return;
		}
	}

	bool on_playback_handler()
	{
		on_next_frame();
		return true;
	}

	void on_next_frame()
	{
		k3d::iwritable_property* const writable_time = dynamic_cast<k3d::iwritable_property*>(m_time);
		return_if_fail(m_start_time && m_end_time && m_frame_rate && m_time && writable_time);

		const double start_time = boost::any_cast<double>(m_start_time->property_value());
		const double end_time = boost::any_cast<double>(m_end_time->property_value());
		const double frame_rate = boost::any_cast<double>(m_frame_rate->property_value());
		const double time = boost::any_cast<double>(m_time->property_value());

		return_if_fail(frame_rate != 0.0);
		const double frame_length = 1.0 / frame_rate;

//k3d::log() << debug << start_time << " " << end_time << " " << frame_length << " " << time << std::endl;

		switch(m_playback_mode.value())
		{
			case LOOP_REVERSE_PLAY:
			{
				double new_time = time - frame_length;
				if(new_time <= start_time)
					new_time = end_time - frame_length;
				writable_time->property_set_value(new_time);
				return;
			}
			case REVERSE_PLAY:
			{
				const double new_time = time - frame_length;
				if(new_time <= start_time)
				{
					writable_time->property_set_value(start_time);
					m_playback_mode.set_value(STOP);
					return;
				}
				writable_time->property_set_value(new_time);
				return;
			}
			case PLAY:
			{
				const double new_time = time + frame_length;
				if(new_time >= (end_time - frame_length))
				{
					writable_time->property_set_value(end_time - frame_length);
					m_playback_mode.set_value(STOP);
					return;
				}
				writable_time->property_set_value(new_time);
				return;
			}
			case LOOP_PLAY:
			{
				double new_time = time + frame_length;
				if(new_time >= (end_time - frame_length))
					new_time = start_time;
				writable_time->property_set_value(new_time);
				return;
			}
			case STOP:
				return;
		}

		assert_not_reached();
	}

	/// Stores a reference to the owning document
	document_state& m_document_state;

	/// Enumerates animation playback modes
	typedef enum
	{
		LOOP_REVERSE_PLAY,
		REVERSE_PLAY,
		STOP,
		PLAY,
		LOOP_PLAY
	} playback_t;

	friend std::ostream& operator<<(std::ostream& Stream, const playback_t& RHS)
	{
		switch(RHS)
		{
			case LOOP_REVERSE_PLAY:
				Stream << "loop_reverse_play";
				break;
			case REVERSE_PLAY:
				Stream << "reverse_play";
				break;
			case STOP:
				Stream << "stop";
				break;
			case PLAY:
				Stream << "play";
				break;
			case LOOP_PLAY:
				Stream << "loop_play";
				break;
			default:
				assert_warning(0);
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, playback_t& RHS)
	{
		std::string temp;
		Stream >> temp;

		if(temp == "loop_reverse_play")
			RHS = LOOP_REVERSE_PLAY;
		else if(temp == "reverse_play")
			RHS = REVERSE_PLAY;
		else if(temp == "stop")
			RHS = STOP;
		else if(temp == "play")
			RHS = PLAY;
		else if(temp == "loop_play")
			RHS = LOOP_PLAY;

		return Stream;
	}

	k3d_data(playback_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_playback_mode;
	sigc::connection m_playback_connection;

	Gtk::HBox m_container;
	Gtk::HScrollbar m_scrollbar;
	Gtk::Entry m_frame_label;
	button::control m_rewind;
	button::control m_loop_reverse_play;
	button::control m_reverse_play;
	button::control m_stop;
	button::control m_play;
	button::control m_loop_play;
	button::control m_fast_forward;

	k3d::iproperty* m_start_time;
	sigc::connection m_start_time_changed_connection;

	k3d::iproperty* m_end_time;
	sigc::connection m_end_time_changed_connection;

	k3d::iproperty* m_frame_rate;
	sigc::connection m_frame_rate_changed_connection;

	k3d::iproperty* m_time;
	sigc::connection m_time_changed_connection;
	k3d::iwritable_property* m_writable_time;

	sigc::signal<void, const std::string&, const std::string&> m_command_signal;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public k3d::ideletable,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		base(false, 0),
		ui_component("timeline", 0),
		m_implementation(0)
	{
	}

	~panel()
	{
		delete m_implementation;
	}

	void initialize(document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("timeline", &Parent);

		m_implementation = new detail::implementation(DocumentState, Parent);
		
		m_implementation->m_command_signal.connect(sigc::mem_fun(*this, &panel::record_command));

		pack_start(m_implementation->m_container, Gtk::PACK_SHRINK);
		show_all();
	}

	const std::string panel_type()
	{
		return "timeline";
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return sigc::connection();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		const k3d::icommand_node::result result = m_implementation->execute_command(Command, Arguments);
		if(result != RESULT_UNKNOWN_COMMAND)
			return result;

		return ui_component::execute_command(Command, Arguments);
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
				k3d::uuid(0xc8cab7fd, 0xa14a55af, 0x79779fa5, 0x1b516756),
				"NGUIUndoTreePanel",
				_("Provides a panel for manipulating the current time"),
				"NGUI Panels",
				k3d::iplugin_factory::EXPERIMENTAL,
				boost::assign::map_list_of("NextGenerationUI", "true")("component_type", "panel")("panel_type", "timeline")("panel_label", "Timeline"));

		return factory;
	}
private:
	detail::implementation* m_implementation;
};

} // namespace ngui_timeline

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui_timeline::panel::get_factory());
K3D_MODULE_END

