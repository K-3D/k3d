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

/** \file
		\brief Timeline panel that allows keyframe representation/manipulation. Buttons copied from old timeline.
		\author Tim Shead (tshead@k-3d.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

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
#include <k3dsdk/ikeyframer.h>
#include <k3dsdk/module.h>
#include <k3dsdk/property.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/time_source.h>

#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/image.h>
#include <gtkmm/entry.h>
#include <gtkmm/scrollbar.h>

#include <gdkmm/colormap.h>
#include <gdkmm/cursor.h>
#include <gdkmm/window.h>

#include <boost/assign/list_of.hpp>

// Temporary hack
using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace animation_timeline
{

namespace detail
{

/// Custom timeline widget
class timeline : public Gtk::DrawingArea
{
public:
	typedef sigc::signal<void, double> time_changed_signal_t;
	typedef sigc::signal<void, const std::string&, double> key_changed_signal_t;
	
	timeline();
	virtual ~timeline();
	
	/// Set start and end time
	void set_range(double Min, double Max)
	{
		m_tmin = Min;
		m_tmax = Max;
	}
	
	/// Set current time
	void set_time(double T);
	
	/// Add a keymarker at the specified time
	void add_key(const std::string& Name, double Time);
	
	/// Delete the named key
	void delete_key(const std::string& Name)
	{
		m_keys.erase(Name);
	}
	
	/// Remove all keys
	void clear_keys()
	{
		m_keys.clear();
		redraw();
	}
	
	/// Emitted when the timeline widget wants the time to change
	time_changed_signal_t& time_changed_signal()
	{
		return m_time_changed_signal;
	}
	
	/// Emitted when a key time was dragged
	key_changed_signal_t& key_changed_signal()
	{
		return m_key_changed_signal;
	}

protected:
  //Overridden default signal handlers:
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* Event);
  virtual bool on_button_release_event(GdkEventButton* Event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  // draw key using specified colors
  void draw_key(double Time, Gdk::Color& ArrowColor, Gdk::Color& LineColor);
	
	Glib::RefPtr<Gdk::GC> m_gc;
  Gdk::Color m_blue;
  Gdk::Color m_red;
  Gdk::Color m_black;
  Gdk::Color m_green;
  Gdk::Color m_white;
  
  int m_barleft;
  int m_bartop;
  int m_barwidth;
  int m_barheight;
  int m_keyheight;
  
  double m_tmin;
  double m_tmax;
  double m_t;
  
  time_changed_signal_t m_time_changed_signal;
  key_changed_signal_t m_key_changed_signal;
  
  bool m_timeline_click;
  std::string m_clicked_key;
  
  Gdk::Cursor m_active_cursor;
  
 private:
 	// helper functions
 	int to_w(double Time)
 	{
 		return static_cast<int>(static_cast<double>(m_barwidth) * Time / (m_tmax - m_tmin));
 	}
 	
 	double to_time(double x)
 	{
 		return m_tmin + ((x - static_cast<double>(m_barleft))/static_cast<double>(m_barwidth)) * (m_tmax - m_tmin);
 	}
 	
 	void redraw();
 	
 	/// True if (x,y) is on the timeline
 	bool hit_timeline(double x, double y);
 	
 	typedef std::map<std::string, double> keys_t;
 	
 	/// True if a key was hit
 	keys_t::iterator hit_key(double x, double y);
 	
 	/// Set the cursor, based on the given position
 	void set_cursor(double x, double y);
 	
 	keys_t m_keys; 
};

timeline::timeline() :
	m_active_cursor(Gdk::TCROSS)
{
	m_keyheight = 5;
	
	Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap();

  m_blue = Gdk::Color("blue");
  m_red = Gdk::Color("red");
  m_black = Gdk::Color("black");
  m_green = Gdk::Color("green");
  m_white = Gdk::Color("white");

  colormap->alloc_color(m_blue);
  colormap->alloc_color(m_red);
  colormap->alloc_color(m_black);
  colormap->alloc_color(m_green);
  colormap->alloc_color(m_white);
  
  m_timeline_click = false;
  m_clicked_key = "";

  add_events(Gdk::EXPOSURE_MASK);
  add_events(Gdk::BUTTON_PRESS_MASK);
  add_events(Gdk::BUTTON_MOTION_MASK);
  add_events(Gdk::BUTTON_RELEASE_MASK);
  add_events(Gdk::POINTER_MOTION_MASK);
}

timeline::~timeline()
{
}

void timeline::set_time(double T)
{
	if(!(T >= m_tmin && T <= m_tmax))
		return;
	m_t = T;
	
	redraw();
}

void timeline::add_key(const std::string& Name, double Time)
{
	m_keys[Name] = Time;
	redraw();
}

void timeline::draw_key(double Time, Gdk::Color& ArrowColor, Gdk::Color& LineColor)
{
	Glib::RefPtr<Gdk::Window> window = get_window();
  
  // window geometry: x, y, width, height, depth
  int winx, winy, winw, winh, wind;
  window->get_geometry(winx, winy, winw, winh, wind);
  
  int x = m_barleft + to_w(Time);
  int arrow_size = m_keyheight;
  
  std::vector<Gdk::Point> vec3;
  vec3.push_back(Gdk::Point(x, m_bartop));
  vec3.push_back(Gdk::Point(x-1, m_bartop));
  vec3.push_back(Gdk::Point(x-arrow_size,m_bartop-arrow_size));
  vec3.push_back(Gdk::Point(x+arrow_size,m_bartop-arrow_size));
	
	m_gc->set_foreground(ArrowColor);
  window->draw_polygon(m_gc, true, vec3);
  m_gc->set_foreground(LineColor);
  window->draw_line(m_gc, x, m_bartop, x, m_bartop+m_barheight-1);
}

void timeline::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  Glib::RefPtr<Gdk::Window> window = get_window();

  m_gc = Gdk::GC::create(window);

  //window->set_background(black_);
  //window->clear();

  m_gc->set_foreground(m_blue);
}

bool timeline::on_expose_event(GdkEventExpose*)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  // window geometry: x, y, width, height, depth
  int winx, winy, winw, winh, wind;
  window->get_geometry(winx, winy, winw, winh, wind);

  window->clear();
  
  m_barleft = 5;
  m_bartop = 10;
  m_barwidth = winw-10;
  m_barheight = 5;
  
  set_time(m_t);
  
  return true;
}

bool timeline::on_button_press_event(GdkEventButton* Event)
{
	double x = Event->x;
	double y = Event->y;
	// If the click happened in the timebar, set the time
	if (hit_timeline(x, y))
	{
		double time = to_time(x);
		m_time_changed_signal.emit(time);
		m_timeline_click = true;
	}
	else
	{
		keys_t::iterator key = hit_key(x, y);
		if (key != m_keys.end())
		{
			m_time_changed_signal.emit(key->second);
			m_clicked_key = key->first;
		}
	}	
	return true;
}

bool timeline::on_button_release_event(GdkEventButton* Event)
{
	m_timeline_click = false;
	m_clicked_key = "";
	set_cursor(Event->x, Event->y);
	return true;
}

bool timeline::on_motion_notify_event(GdkEventMotion* event)
{
	if (m_timeline_click)
	{
		double new_time = to_time(event->x);
		if (new_time >= m_tmin && new_time <= m_tmax)
			m_time_changed_signal.emit(new_time);
	}
	else
	{
		keys_t::iterator key = m_keys.find(m_clicked_key);
		if (key != m_keys.end())
		{
			double new_time = to_time(event->x);
			if (new_time >= m_tmin && new_time <= m_tmax)
			{
				m_t = new_time;
				m_key_changed_signal.emit(key->first, new_time);
				m_time_changed_signal.emit(new_time);
			}
		}
		else
		{ // No click, but set cursor to indicate key and timeline clickability
			set_cursor(event->x, event->y);
		}
	}
	return true;
}

void timeline::redraw()
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	
	if (!window)
		return;
	
	window->clear();
	
	// Draw the timeline
	int timewidth = to_w(m_t);
	m_gc->set_foreground(m_red);
	window->draw_rectangle(m_gc, true, m_barleft, m_bartop, timewidth, m_barheight);
	m_gc->set_foreground(m_blue);
	window->draw_rectangle(m_gc, true, m_barleft+timewidth, m_bartop, m_barwidth-timewidth, m_barheight);
	m_gc->set_foreground(m_black);
	window->draw_line(m_gc, m_barleft-1, m_bartop, m_barleft-1, m_bartop+m_barheight-1);
	window->draw_line(m_gc, m_barleft-1, m_bartop-1, m_barleft+m_barwidth, m_bartop-1);
	m_gc->set_foreground(m_white);
	window->draw_line(m_gc, m_barleft+m_barwidth, m_bartop, m_barleft+m_barwidth, m_bartop+m_barheight-1);
	window->draw_line(m_gc, m_barleft-1, m_bartop+m_barheight, m_barleft+m_barwidth, m_bartop+m_barheight);
	
	// Show the keys
	for (keys_t::iterator key = m_keys.begin(); key != m_keys.end(); ++key)
	{
		if (key->second != m_t)
		{
			draw_key(key->second, m_black, m_black);
		}
		else
		{
			draw_key(key->second, m_green, m_green);
		}
	}
}

bool timeline::hit_timeline(double x, double y)
{
	if (y >= m_bartop && y <= m_bartop + m_barheight)
	{
		if (x >= m_barleft && x <= m_barleft + m_barwidth)
		{
			return true;
		}
	}
	return false;
}

timeline::keys_t::iterator timeline::hit_key(double x, double y)
{
	if (y >= m_bartop - m_keyheight && y <= m_bartop) // Check if a key was clicked
	{
		double d = m_bartop - y;
		for (keys_t::iterator key = m_keys.begin(); key != m_keys.end(); ++key)
		{
			if (std::abs(x-(m_barleft + to_w(key->second))) <= d)
			{
				return key;
			}
		}
	}
	return m_keys.end();
}

void timeline::set_cursor(double x, double y)
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if (hit_timeline(x, y) || hit_key(x, y) != m_keys.end())
	{
		window->set_cursor(m_active_cursor);
	}
	else
	{
		window->set_cursor();
	}
}

class implementation :
	public libk3dngui::asynchronous_update,
	public k3d::property_collection
{
public:
	implementation(document_state& DocumentState, k3d::icommand_node& Parent) :
		k3d::property_collection(),
		m_document_state(DocumentState),
		m_no_track("Select Track..."),
		m_rewind(Parent, "rewind", *Gtk::manage(new Gtk::Image(load_icon("rewind", Gtk::ICON_SIZE_BUTTON)))),
		m_loop_reverse_play(Parent, "reverse_play_loop", *Gtk::manage(new Gtk::Image(load_icon("reverse_play_loop", Gtk::ICON_SIZE_BUTTON)))),
		m_reverse_play(Parent, "reverse_play", *Gtk::manage(new Gtk::Image(load_icon("reverse_play", Gtk::ICON_SIZE_BUTTON)))),
		m_stop(Parent, "stop", *Gtk::manage(new Gtk::Image(load_icon("stop", Gtk::ICON_SIZE_BUTTON)))),
		m_play(Parent, "play", *Gtk::manage(new Gtk::Image(load_icon("play", Gtk::ICON_SIZE_BUTTON)))),
		m_loop_play(Parent, "play_loop", *Gtk::manage(new Gtk::Image(load_icon("play_loop", Gtk::ICON_SIZE_BUTTON)))),
		m_fast_forward(Parent, "fast_forward", *Gtk::manage(new Gtk::Image(load_icon("fast_forward", Gtk::ICON_SIZE_BUTTON)))),
		m_playback_mode(init_name("playback_mode") + init_label(_("Playback Mode")) + init_description(_("When on, plays animation")) + init_value(STOP))
	{
		m_frame_label.set_editable(false);
		m_frame_label.set_width_chars(5);
		
		m_container.pack_start(m_track_selector, Gtk::PACK_SHRINK);
		m_container.pack_start(m_timeline, Gtk::PACK_EXPAND_WIDGET);
		m_container.pack_start(m_frame_label, Gtk::PACK_SHRINK);
		m_container.pack_start(m_rewind, Gtk::PACK_SHRINK);
		m_container.pack_start(m_loop_reverse_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_reverse_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_stop, Gtk::PACK_SHRINK);
		m_container.pack_start(m_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_loop_play, Gtk::PACK_SHRINK);
		m_container.pack_start(m_fast_forward, Gtk::PACK_SHRINK);
		m_container.show_all();
		
		m_document_state.document().nodes().add_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_added));
		m_document_state.document().nodes().remove_nodes_signal().connect(sigc::mem_fun(*this, &implementation::on_nodes_removed));
		
		m_timeline.time_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_set_time));
		m_timeline.key_changed_signal().connect(sigc::mem_fun(*this, &implementation::on_move_key));
		
		m_track_selector.signal_changed().connect(sigc::mem_fun(*this, &implementation::update_keys));
		
		m_rewind.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_rewind));
		m_loop_reverse_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_loop_reverse_play));
		m_reverse_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_reverse_play));
		m_stop.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_stop));
		m_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_play));
		m_loop_play.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_loop_play));
		m_fast_forward.signal_clicked().connect(sigc::mem_fun(*this, &implementation::on_fast_forward));

		m_playback_mode.changed_signal().connect(sigc::mem_fun(*this, &implementation::on_playback_mode_changed));
		
		reset_time_properties();
		update_track_list();
		
		on_time_changed(0);
		
		schedule_update();
	}

	/// Called by the signal system anytime new nodes are added to the document
	void on_nodes_added(const k3d::inode_collection::nodes_t& Nodes)
	{
		reset_time_properties();
		update_track_list();
	}

	/// Called by the signal system anytime nodes are removed from the document
	void on_nodes_removed(const k3d::inode_collection::nodes_t& Nodes)
	{
		reset_time_properties();
		update_track_list();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		return k3d::icommand_node::RESULT_CONTINUE;
	}

	void on_update()
	{
		return_if_fail(m_start_time && m_end_time && m_time);

		const double start_time = boost::any_cast<double>(m_start_time->property_value());
		const double end_time = boost::any_cast<double>(m_end_time->property_value());
		const double time = boost::any_cast<double>(m_time->property_value());
		
		m_timeline.set_range(start_time, end_time);
		m_timeline.set_time(time);
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
		schedule_update();
	}
	
	void on_set_time(double Time)
	{
		if (m_writable_time)
			m_writable_time->property_set_value(Time);
	}
	
	void on_keytime_changed(k3d::iunknown* Hint, k3d::iproperty* Property)
	{
		m_timeline.delete_key(Property->property_label());
		m_timeline.add_key(Property->property_label(), k3d::property::pipeline_value<double>(*Property));
	}
	
	void on_move_key(const std::string& Key, double Time)
	{
		k3d::property::set_internal_value(*m_keys[Key], Time);
	}
	
	void reset_time_properties()
	{
		m_start_time = 0;
		m_end_time = 0;
		m_time = 0;
		m_writable_time = 0;

		m_start_time_changed_connection.disconnect();
		m_end_time_changed_connection.disconnect();
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
		
		m_rewind.set_sensitive(enabled);
		m_loop_reverse_play.set_sensitive(enabled);
		m_reverse_play.set_sensitive(enabled);
		m_stop.set_sensitive(enabled);
		m_play.set_sensitive(enabled);
		m_loop_play.set_sensitive(enabled);
		m_fast_forward.set_sensitive(enabled);
	}
	
	typedef std::map<std::string, k3d::ikeyframer*> tracks_t;
	
	void update_track_list()
	{
		std::string selection = m_track_selector.get_active_text();
		if (selection == "")
			selection = m_no_track;
		m_track_selector.clear_items();
		m_tracks.clear();
		m_track_selector.append_text(m_no_track);
		const k3d::inode_collection::nodes_t& nodes = m_document_state.document().nodes().collection();
		for (k3d::inode_collection::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			k3d::ikeyframer* keyframer = dynamic_cast<k3d::ikeyframer*>(*node);
			if (keyframer)
			{
				m_track_selector.append_text((*node)->name());
				m_tracks[(*node)->name()] = keyframer;
			}
		}
		if (m_tracks.find(selection) == m_tracks.end()) // Display tooltip if the old selection was removed
			selection = m_no_track;
		m_track_selector.set_active_text(selection);
		update_keys();
	}
	
	void update_keys()
	{
		for (size_t i = 0; i != m_connections.size(); ++i)
			m_connections[i].disconnect();
		m_keys_changed_connection.disconnect();
		m_connections.clear();
		m_keys.clear();
		m_timeline.clear_keys();
		tracks_t::iterator track_it = m_tracks.find(m_track_selector.get_active_text());
		if (track_it != m_tracks.end())
		{
			k3d::ikeyframer* track = track_it->second;
			m_keys_changed_connection = track->keys_changed_signal().connect(sigc::mem_fun(*this, &implementation::update_keys));
			k3d::ikeyframer::keys_t keys = track->get_keys();
			for (k3d::ikeyframer::keys_t::iterator key = keys.begin(); key != keys.end(); ++key)
			{
				k3d::iproperty* prop = *key;
				m_timeline.add_key(prop->property_label(), k3d::property::pipeline_value<double>(*prop));
				m_keys[prop->property_label()] = prop;
				m_connections.push_back(prop->property_changed_signal().connect(sigc::bind(sigc::mem_fun(*this, &implementation::on_keytime_changed), prop)));
			}
		}
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
		switch(m_playback_mode.internal_value())
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

		switch(m_playback_mode.internal_value())
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

	Gtk::HBox m_container;
	sigc::signal<void, const std::string&, const std::string&> m_command_signal;
	
	document_state& m_document_state;
	timeline m_timeline;
	
	k3d::iproperty* m_start_time;
	sigc::connection m_start_time_changed_connection;

	k3d::iproperty* m_end_time;
	sigc::connection m_end_time_changed_connection;
	
	k3d::iproperty* m_frame_rate;
	sigc::connection m_frame_rate_changed_connection;

	k3d::iproperty* m_time;
	sigc::connection m_time_changed_connection;
	k3d::iwritable_property* m_writable_time;
	
	Gtk::ComboBoxText m_track_selector;
	std::string m_no_track;
	tracks_t m_tracks;
	
	std::vector<sigc::connection> m_connections;
	std::map<std::string, k3d::iproperty*> m_keys;
	sigc::connection m_keys_changed_connection;

	Gtk::Entry m_frame_label;
	button::control m_rewind;
	button::control m_loop_reverse_play;
	button::control m_reverse_play;
	button::control m_stop;
	button::control m_play;
	button::control m_loop_play;
	button::control m_fast_forward;
	
	k3d_data(playback_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_playback_mode;
	sigc::connection m_playback_connection;
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

		pack_start(m_implementation->m_container, Gtk::PACK_EXPAND_WIDGET);
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
			k3d::uuid(0xd6ddc369, 0x674697b8, 0xb7e6fd81, 0xd42aed12),
			"NGUIAnimationTimelinePanel",
			_("Provides a panel for manipulating the current animation time"),
			"NGUI Panels",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-type", "animation_timeline")("ngui:panel-label", "Animation Timeline"));

		return factory;
	}
private:
	detail::implementation* m_implementation;
};

} // namespace animation_timeline

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::animation_timeline::panel::get_factory());
K3D_MODULE_END

