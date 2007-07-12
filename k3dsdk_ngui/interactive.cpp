// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\brief Implements functions and helpers for simulating user input and driving interactive tutorials
		\author Tim Shead (tshead@k-3d.com)
*/

#include "collapsible_frame.h"
#include "interactive.h"
#include "options.h"
#include "screen_overlay.h"
#include "utility.h"

#include <k3dsdk/basic_math.h>
#include <k3dsdk/bezier.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/high_res_timer.h>
#include <k3d-platform-config.h>
#include <k3dsdk/system.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/combobox.h>
#include <gtkmm/entry.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/menushell.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/widget.h>
#include <gtkmm/window.h>

#include <gtk/gtk.h>

#if defined K3D_API_WIN32

	//#include <gdkwin32.h>
	#undef min
	#undef max

#else // K3D_API_WIN32

	#include <gdk/gdkx.h>

#endif // !K3D_API_WIN32

namespace libk3dngui
{

namespace interactive
{

namespace detail
{

void get_window_and_coordinates(GdkWindow*& Window, double& WindowX, double& WindowY, double& ScreenX, double& ScreenY)
{
	Window = gdk_window_at_pointer(0, 0);

	{
		int window_x = 0;
		int window_y = 0;
		GdkModifierType modifiers;
		gdk_window_get_pointer(Window, &window_x, &window_y, &modifiers);
		WindowX = window_x;
		WindowY = window_y;
	}

	{
		int screen_x = 0;
		int screen_y = 0;
		Gdk::ModifierType modifiers;
		Gdk::Display::get_default()->get_pointer(screen_x, screen_y, modifiers);
		ScreenX = screen_x;
		ScreenY = screen_y;
	}
}

void button_press(const guint Button)
{
	GdkEvent* const event = gdk_event_new(GDK_BUTTON_PRESS);
	get_window_and_coordinates(event->button.window, event->button.x, event->button.y, event->button.x_root, event->button.y_root);
	event->button.send_event = TRUE;
	event->button.time = GDK_CURRENT_TIME;
	event->button.axes = 0;
	event->button.state = 0;
	event->button.button = Button;
	event->button.device = gdk_device_get_core_pointer();

	g_object_ref(event->button.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);
}

void button_double_press(const guint Button)
{
	GdkEvent* const event = gdk_event_new(GDK_2BUTTON_PRESS);
	get_window_and_coordinates(event->button.window, event->button.x, event->button.y, event->button.x_root, event->button.y_root);
	event->button.send_event = TRUE;
	event->button.time = GDK_CURRENT_TIME;
	event->button.axes = 0;
	event->button.state = 0;
	event->button.button = Button;
	event->button.device = gdk_device_get_core_pointer();

	g_object_ref(event->button.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);
}

void button_triple_press(const guint Button)
{
	GdkEvent* const event = gdk_event_new(GDK_3BUTTON_PRESS);
	get_window_and_coordinates(event->button.window, event->button.x, event->button.y, event->button.x_root, event->button.y_root);
	event->button.send_event = TRUE;
	event->button.time = GDK_CURRENT_TIME;
	event->button.axes = 0;
	event->button.state = 0;
	event->button.button = Button;
	event->button.device = gdk_device_get_core_pointer();

	g_object_ref(event->button.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);
}

void button_release(const guint Button)
{
	GdkEvent* const event = gdk_event_new(GDK_BUTTON_RELEASE);
	get_window_and_coordinates(event->button.window, event->button.x, event->button.y, event->button.x_root, event->button.y_root);
	event->button.send_event = TRUE;
	event->button.time = GDK_CURRENT_TIME;
	event->button.axes = 0;
	event->button.button = Button;
	event->button.device = gdk_device_get_core_pointer();

	switch(Button)
	{
		case 1:
			event->button.state = GDK_BUTTON1_MASK;
			break;
		case 2:
			event->button.state = GDK_BUTTON2_MASK;
			break;
		case 3:
			event->button.state = GDK_BUTTON3_MASK;
			break;
		case 4:
			event->button.state = GDK_BUTTON4_MASK;
			break;
		case 5:
			event->button.state = GDK_BUTTON5_MASK;
			break;
	}

	g_object_ref(event->button.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);
}

void button_click(const guint Button)
{
	button_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
}

void button_double_click(const guint Button)
{
	button_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
	button_press(Button);
	non_blocking_sleep(0.1);
	button_double_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
}

void button_triple_click(const guint Button)
{
	button_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
	button_press(Button);
	non_blocking_sleep(0.1);
	button_double_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
	button_press(Button);
	non_blocking_sleep(0.1);
	button_triple_press(Button);
	non_blocking_sleep(0.1);
	button_release(Button);
	non_blocking_sleep(0.1);
}

void key_press(const guint Character)
{
#ifdef K3D_API_WIN32

	const SHORT virtual_key = VkKeyScan(Character) & 0xff;
	const SHORT modifiers = VkKeyScan(Character) >> 8;

	if(modifiers & 0x01)
		keybd_event(VK_SHIFT, 0, 0, 0);

	keybd_event(virtual_key, 0, 0, 0);

	handle_pending_events();

#else // K3D_API_WIN32

	GdkEvent* const event = gdk_event_new(GDK_KEY_PRESS);
	event->key.window = gdk_window_at_pointer(0, 0);
	event->key.send_event = TRUE;
	event->key.time = GDK_CURRENT_TIME;
	event->key.state = 0;
	event->key.keyval = Character;
	event->key.length = 0;
	event->key.string = 0;
	event->key.hardware_keycode = 0;
	event->key.group = 0;

	g_object_ref(event->key.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);

#endif // !K3D_API_WIN32
}

void key_release(const guint Character)
{
#ifdef K3D_API_WIN32

	const SHORT virtual_key = VkKeyScan(Character) & 0xff;
	const SHORT modifiers = VkKeyScan(Character) >> 8;

	keybd_event(virtual_key, 0, KEYEVENTF_KEYUP, 0);

	if(modifiers & 0x01)
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

	handle_pending_events();

#else // K3D_API_WIN32

	GdkEvent* const event = gdk_event_new(GDK_KEY_RELEASE);
	event->key.window = gdk_window_at_pointer(0, 0);
	event->key.send_event = TRUE;
	event->key.time = GDK_CURRENT_TIME;
	event->key.state = 0;
	event->key.keyval = Character;
	event->key.length = 0;
	event->key.string = 0;
	event->key.hardware_keycode = 0;
	event->key.group = 0;

	g_object_ref(event->key.window);
	gdk_event_put(event);
	handle_pending_events();
	gdk_event_free(event);

#endif // !K3D_API_WIN32
}

void key_click(const guint Character)
{
	key_press(Character);
	key_release(Character);
}

void warp_pointer(const k3d::point2& Offset)
{
#ifdef K3D_API_WIN32

	// Make that pointer jump!
	SetCursorPos(static_cast<int>(Offset[0]), static_cast<int>(Offset[1]));

#else // K3D_API_WIN32

	// Get the X display ...
	Display* xdisplay = GDK_WINDOW_XDISPLAY(Gdk::Display::get_default()->get_default_screen()->get_root_window()->gobj());
	return_if_fail(xdisplay);

	// Get our X window ...
	Window xwindow = GDK_WINDOW_XWINDOW(Gdk::Display::get_default()->get_default_screen()->get_root_window()->gobj());
	return_if_fail(xwindow);

	// Move that pointer!
	XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, static_cast<int>(Offset[0]), static_cast<int>(Offset[1]));
	XFlush(xdisplay);

#endif // !K3D_API_WIN32
}

void warp_pointer(Glib::RefPtr<Gdk::Window> Window, const k3d::point2& Offset)
{
	return_if_fail(Window);

	int left = 0;
	int top = 0;
	Window->get_origin(left, top);

	warp_pointer(k3d::point2(left, top) + Offset);
}

enum MoveTypeT
{
	MOVE_STRAIGHT,
	MOVE_XY,
	MOVE_YX
};

/// Moves the mouse pointer from its current location to the given screen coordinates
void move_pointer(const k3d::point2& Offset, const MoveTypeT MoveType)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	// Get the current mouse pointer position in screen coordinates
	int pointerx = 0;
	int pointery = 0;
	Gdk::ModifierType modifiers;
	Gdk::Display::get_default()->get_pointer(pointerx, pointery, modifiers);

	// Make it our starting point ...
	const k3d::point2 from(pointerx, pointery);

	// Setup our end point ...
	const k3d::point2 to = Offset;

	// Calculate the number of steps, based on the distance to travel (then adjust, based on our speed) ...
	const unsigned long steps = static_cast<unsigned long>((k3d::distance(from, to) / 20 + 30) / speed);
	const double delta = 1.0 / static_cast<double>(steps);

	std::vector<k3d::point2> path_points;
	switch(MoveType)
	{
		case MOVE_STRAIGHT:
			path_points.push_back(from);
			path_points.push_back(from);
			path_points.push_back(to);
			path_points.push_back(to);
			break;
		case MOVE_XY:
			path_points.push_back(from);
			path_points.push_back(k3d::point2(to[0], from[1]));
			path_points.push_back(k3d::point2(to[0], from[1]));
			path_points.push_back(to);
			break;
		case MOVE_YX:
			path_points.push_back(from);
			path_points.push_back(k3d::point2(from[0], to[1]));
			path_points.push_back(k3d::point2(from[0], to[1]));
			path_points.push_back(to);
			break;
	}

	for(unsigned long i = 1; i <= steps; ++i)
	{
		const double percent = delta * i;
		const k3d::point2 position = k3d::bezier<3, k3d::point2>(path_points.begin(), path_points.end(), percent);
		detail::warp_pointer(position);
		non_blocking_sleep(0.01);
	}

	non_blocking_sleep(0.5 / speed);
}

void move_pointer(Glib::RefPtr<Gdk::Window> Window, const k3d::point2& Offset, const MoveTypeT MoveType)
{
	return_if_fail(Window);

	int left = 0;
	int top = 0;
	Window->get_origin(left, top);

	move_pointer(k3d::point2(left, top) + Offset, MoveType);
}


void highlight_coordinates(const double Theta, const double Radius, const double XRadius, const double YRadius, const double XCenter, const double YCenter, gint& X, gint& Y)
{
	X = static_cast<gint>(XCenter + (Radius * XRadius * cos(Theta)));
	Y = static_cast<gint>(YCenter + (Radius * YRadius * -sin(Theta)));
}

const k3d::point2 screen_coordinates(Gtk::Widget& Widget)
{
	// Sanity check
	return_val_if_fail(Widget.get_window(), k3d::point2(0, 0));

	int left = 0;
	int top = 0;
	Widget.get_window()->get_origin(left, top);

	if(Widget.has_no_window())
	{
		left += Widget.get_allocation().get_x();
		top += Widget.get_allocation().get_y();
	}

	return k3d::point2(left, top);
}

void move_pointer(Gtk::Widget& Widget, const double XPercent, const double YPercent, const MoveTypeT MoveType)
{
	const int width = Widget.get_width();
	const int height = Widget.get_height();
	detail::move_pointer(screen_coordinates(Widget) + k3d::point2(width * XPercent, height * YPercent), MoveType);
}

void move_pointer(Gtk::Widget& Widget, const MoveTypeT MoveType)
{
	move_pointer(Widget, 0.5, 0.5, MoveType);
}

} // namespace detail

const k3d::point2 get_pointer()
{
	int x = 0;
	int y = 0;
	Gdk::ModifierType modifiers;
	Gdk::Display::get_default()->get_pointer(x, y, modifiers);

	return k3d::point2(x, y);
}

void warp_pointer(const k3d::point2& Coords)
{
	detail::warp_pointer(Coords);
}

void warp_pointer(const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer)
{
	handle_pending_events();
	k3d::system::sleep(Timestamp - Timer.elapsed());
	detail::warp_pointer(Coords);
}

void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords)
{
	detail::warp_pointer(detail::screen_coordinates(Widget) + Coords);
}

void warp_pointer(Gtk::Widget& Widget, const k3d::point2& Coords, const double Timestamp, const k3d::timer& Timer)
{
	warp_pointer(detail::screen_coordinates(Widget) + Coords, Timestamp, Timer);
}

void warp_pointer(Gtk::Widget& Widget)
{
	detail::warp_pointer(detail::screen_coordinates(Widget) + k3d::point2(Widget.get_width() / 2, Widget.get_height() / 2));
}

void move_pointer(Gtk::Widget& Widget)
{
	detail::move_pointer(Widget, 0.5, 0.5, detail::MOVE_STRAIGHT);
}

void move_pointer(Gtk::Widget& Widget, const k3d::point2& Coords)
{
	detail::move_pointer(detail::screen_coordinates(Widget) + Coords, detail::MOVE_STRAIGHT);
}

void show(Gtk::Widget& Widget)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	// Build a list of parent widgets, starting with our "leaf" widget ...
	typedef std::deque<Gtk::Widget*> parents_t;
	parents_t parents;
	for(Gtk::Widget* parent = &Widget; parent; )
	{
		parents.push_front(parent);

		if(Gtk::Menu* const menu = dynamic_cast<Gtk::Menu*>(parent))
			parent = menu->get_attach_widget();
		else
			parent = parent->get_parent();
	}

	// The top-level item ought to be a window, so make sure it's visible ...
	if(dynamic_cast<Gtk::Window*>(parents.front()))
	{
		parents.front()->get_window()->show();
		parents.front()->get_window()->raise();
	}

	// Keep track of scrolled windows as we go, so we can make the children visible as-required ...
	Gtk::ScrolledWindow* parent_scrolled_window = 0;

	// Walk the list, modifying widgets as needed ...
	bool menu_bar = false;
	bool menu_bar_item = false;
	for(parents_t::iterator widget = parents.begin(); widget != parents.end(); ++widget)
	{
		// Keep track of scrolled windows as we go ...
		if(Gtk::ScrolledWindow* const scrolled_window = dynamic_cast<Gtk::ScrolledWindow*>(*widget))
		{
			parent_scrolled_window = scrolled_window;
			continue;
		}

		if(dynamic_cast<Gtk::MenuBar*>(*widget))
		{
			menu_bar = true;
			menu_bar_item = true;
		}

		const bool last_item = parents.end() == widget+1;
		Gtk::TreeView* const tree_view = dynamic_cast<Gtk::TreeView*>(*widget);
		Gtk::MenuItem* const menu_item = dynamic_cast<Gtk::MenuItem*>(*widget);
		Gtk::Notebook* const notebook = dynamic_cast<Gtk::Notebook*>(*widget);
		collapsible_frame::control* const collapsible_frame = dynamic_cast<collapsible_frame::control*>(*widget);

		// If this is a child of a scrolled window, ensure that it is visible ...
		if(parent_scrolled_window && (!tree_view) && (last_item || menu_item || notebook || (collapsible_frame && collapsible_frame->is_collapsed())))
		{
			Gtk::Adjustment* const hadj = parent_scrolled_window->get_hadjustment();
			Gtk::Adjustment* const vadj = parent_scrolled_window->get_vadjustment();

			const k3d::rectangle viewport_allocation(
				hadj->get_value(),
				hadj->get_value() + hadj->get_page_size(),
				vadj->get_value(),
				vadj->get_value() + vadj->get_page_size());

			const k3d::rectangle child_allocation(
				(*widget)->get_allocation().get_x(),
				(*widget)->get_allocation().get_x() + (*widget)->get_allocation().get_width(),
				(*widget)->get_allocation().get_y(),
				(*widget)->get_allocation().get_y() + (*widget)->get_allocation().get_height());

			const k3d::point2 start(hadj->get_value(), vadj->get_value());

			k3d::point2 end = start;
			if(child_allocation.right > viewport_allocation.right)
				end[0] = start[0] + (child_allocation.right - viewport_allocation.right);
			if(child_allocation.left < viewport_allocation.left)
				end[0] = start[0] - (viewport_allocation.left - child_allocation.left);
			if(child_allocation.bottom > viewport_allocation.bottom)
				end[1] = start[1] + (child_allocation.bottom - viewport_allocation.bottom);
			if(child_allocation.top < viewport_allocation.top)
				end[1] = start[1] - (viewport_allocation.top - child_allocation.top);

			end[0] = std::min(end[0], hadj->get_upper() - hadj->get_page_size());
			end[1] = std::min(end[1], vadj->get_upper() - vadj->get_page_size());

			const int count = 50;
			for(int i = 0; i != count; ++i)
			{
				const k3d::point2 position = k3d::mix(start, end, (i+1) / static_cast<double>(count));
				parent_scrolled_window->get_hadjustment()->set_value(position[0]);
				parent_scrolled_window->get_vadjustment()->set_value(position[1]);
				handle_pending_events();
			}
		}

		// Open menus ...
		if(menu_item)
		{
			if(menu_bar)
			{
				detail::move_pointer(**widget, 0.7, 0.5, detail::MOVE_STRAIGHT);
				menu_bar = false;
				non_blocking_sleep(0.2);
				detail::button_click(1);
			}
			else if(menu_bar_item)
			{
				detail::move_pointer(**widget, 0.7, 0.5, detail::MOVE_YX);
				menu_bar_item = false;
				non_blocking_sleep(0.2);
			}
			else
			{
				detail::move_pointer(**widget, 0.7, 0.5, detail::MOVE_XY);
				non_blocking_sleep(0.2);
			}

			continue;
		}

		// Switch notebook tabs ...
		if(notebook)
		{
			parents_t::iterator notebook_child = widget + 1;
			if(notebook_child == parents.end())
				continue;

			const int page_number = notebook->page_num(**notebook_child);
			if(page_number == notebook->get_current_page())
				continue;

			Gtk::Widget* const tab_label = notebook->get_tab_label(**notebook_child);
			return_if_fail(tab_label);

			detail::move_pointer(*tab_label, 0.5, 0.5, detail::MOVE_STRAIGHT);
			detail::button_click(1);

			continue;
		}

		// Expand frames ...
		if(collapsible_frame)
		{
			if(collapsible_frame->is_collapsed())
			{
				detail::move_pointer(collapsible_frame->toggle_button(), 0.5, 0.5, detail::MOVE_STRAIGHT);
				detail::button_click(1);
			}

			continue;
		}
	}
}

void highlight(Gtk::Widget& Widget)
{
	return_if_fail(Widget.get_window());

	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	show(Widget);

	// Get widget position relative to the root window ...
	int width = Widget.get_width();
	int height = Widget.get_height();
	int left = 0;
	int top = 0;
	Widget.get_window()->get_origin(left, top);

	if(Widget.has_no_window())
	{
		left += Widget.get_allocation().get_x();
		top += Widget.get_allocation().get_y();
	}

	const unsigned long samples = 400;

	// Adjust the rate at which we update the display, based on the size of the widget,
	// because making changes to the overlay will get slower as the overlay gets larger
	const unsigned long update_rate = static_cast<unsigned long>(std::max(1.0, sqrt(width * height / 2000.0)));

	const double start_angle = 130;
	const double end_angle = start_angle + 360 + 30;
	const double start_radius = 0.87;
	const double end_radius = 0.95;

	const int line_width = 6;
	left -= 2 * line_width;
	top -= 2 * line_width;
	width += 4 * line_width;
	height += 4 * line_width;

	screen_overlay overlay(Gdk::Rectangle(left, top, width, height), k3d::color(1, 0, 0));
	overlay.show_all();

	const double xradius = width * 0.5 - line_width;
	const double yradius = height * 0.5 - line_width;
	const double xcenter = width * 0.5;
	const double ycenter = height * 0.5;

	int x, y = 0;
	detail::highlight_coordinates(k3d::radians(start_angle), start_radius, xradius, yradius, xcenter, ycenter, x, y);
	detail::move_pointer(overlay.get_window(), k3d::point2(x, y), detail::MOVE_STRAIGHT);

	overlay.mask_gc()->set_line_attributes(line_width, Gdk::LINE_SOLID, Gdk::CAP_ROUND, Gdk::JOIN_ROUND);
	for(unsigned long i = 0, update = 0; i != samples; ++i, ++update)
	{
		const double percent1 = i / static_cast<double>(samples);
		const double percent2 = (i + 1) / static_cast<double>(samples);

		int x1, y1 = 0;
		const double radius1 = k3d::mix(start_radius, end_radius, percent1);
		detail::highlight_coordinates(k3d::radians(k3d::mix(start_angle, end_angle, percent1)), radius1, xradius, yradius, xcenter, ycenter, x1, y1);

		int x2, y2 = 0;
		const double radius2 = k3d::mix(start_radius, end_radius, percent2);
		detail::highlight_coordinates(k3d::radians(k3d::mix(start_angle, end_angle, percent2)), radius2, xradius, yradius, xcenter, ycenter, x2, y2);

		detail::warp_pointer(overlay.get_window(), k3d::point2(x2, y2));
		overlay.mask()->draw_line(overlay.mask_gc(), x1, y1, x2, y2);

		if(0 == update % update_rate)
			overlay.update();
	}

	non_blocking_sleep(static_cast<unsigned long>(1.0 / speed));

	// Special-case menu items, closing the menu so it doesn't stick-around forever ...
	if(dynamic_cast<Gtk::MenuItem*>(&Widget))
	{
		typedef std::vector<Gtk::MenuItem*> menu_items_t;
		menu_items_t menu_items;

		for(Gtk::Widget* ancestor = &Widget; ancestor; )
		{
			Gtk::MenuItem* const menu_item = dynamic_cast<Gtk::MenuItem*>(ancestor);
			if(menu_item)
				menu_items.push_back(menu_item);

			Gtk::Menu* const menu = dynamic_cast<Gtk::Menu*>(ancestor);
			if(menu)
				ancestor = menu->get_attach_widget();
			else
				ancestor = ancestor->get_parent();
		}

		for(menu_items_t::iterator menu_item = menu_items.begin(); menu_item != menu_items.end(); ++menu_item)
			(*menu_item)->deselect();
	}
}

void set_text(Gtk::Entry& Entry, const std::string& Text)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	show(Entry);
	detail::move_pointer(Entry, 0.1, 0.5, detail::MOVE_STRAIGHT);
	detail::button_triple_click(1);
	non_blocking_sleep(0.5 / speed);

	for(unsigned int i = 0; i < Text.size(); ++i)
	{
		detail::key_click(Text[i]);
		non_blocking_sleep((0.05 + ((static_cast<double>(rand()) / RAND_MAX) * 0.1)) / speed);
	}

	non_blocking_sleep(0.2 / speed);
	/** \todo Figure-out why a simple tab won't work */
	Entry.activate();
	non_blocking_sleep(0.4 / speed);
}

void activate(Gtk::Button& Button)
{
	show(Button);
	move_pointer(Button);
	detail::button_click(1);
}

/** \todo Get this working properly with sub-menus */
void activate(Gtk::MenuItem& MenuItem)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	show(MenuItem);

	// Ensure that the instance doesn't self-destruct when we activate it (e.g: a chooser menu)
	MenuItem.reference();

	/** \todo Figure out why a simple detail::button_click(1) doesn't work */
	MenuItem.activate();

	for(Gtk::Widget* widget = &MenuItem; widget; )
	{
		if(Gtk::MenuShell* const menu_shell = dynamic_cast<Gtk::MenuShell*>(widget))
		{
			menu_shell->deselect();
			menu_shell->deactivate();
			g_signal_emit_by_name(menu_shell->gobj(), "selection-done", 0);


			if(Gtk::Menu* const menu = dynamic_cast<Gtk::Menu*>(widget))
			{
				widget = menu->get_attach_widget();
			}
			else
			{
				widget = 0;
			}
		}
		else
		{
			widget = widget->get_parent();
		}
	}

	// Give away our reference
	MenuItem.unreference();
	
	non_blocking_sleep(0.1);
}

void move_pointer(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, const Gtk::TreeIter& Row)
{
	// Get the center of the row in tree coordinates ...
	TreeView.scroll_to_row(Gtk::TreePath(Row), 0.5);
	handle_pending_events();
	Gdk::Rectangle cell_area;
	TreeView.get_cell_area(Gtk::TreePath(Row), Column, cell_area);
	int tx = cell_area.get_x() + (cell_area.get_width() / 2);
	int ty = cell_area.get_y() + (cell_area.get_height() / 2);

	// Convert to widget coordinates and move the pointer ...
	int wx = tx;
	int wy = ty;

	/** \note The gtkmm / gtk+ documentation says that Gtk::TreeView::get_cell_area() returns tree coordinates,
	 * but the returned coordinates are widget coordinates, so the following transformation isn't needed */
	// TreeView.tree_to_widget_coords(tx, ty, wx, wy);

	detail::move_pointer(TreeView.get_window(), k3d::point2(wx, wy), detail::MOVE_STRAIGHT);
}

void select_row(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, const Gtk::TreeIter& Row)
{
	show(TreeView);
	move_pointer(TreeView, Column, Row);
	detail::button_click(1);
}

void set_text(Gtk::TreeView& TreeView, Gtk::TreeViewColumn& Column, Gtk::CellRenderer& Cell, const Gtk::TreeIter& Row, const std::string& Text)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	show(TreeView);
	move_pointer(TreeView, Column, Row);

	detail::button_double_click(1);
	non_blocking_sleep(0.5 / speed);

	for(unsigned int i = 0; i < Text.size(); ++i)
	{
		detail::key_click(Text[i]);
		non_blocking_sleep((0.05 + ((static_cast<double>(rand()) / RAND_MAX) * 0.1)) / speed);
	}

	non_blocking_sleep(0.2 / speed);
	//detail::key_click('\t');
	TreeView.grab_focus();
	non_blocking_sleep(0.4 / speed);
}

/** \todo Improve the implementation of this so we see the mouse pointer move over the item to be chosen */
void select_row(Gtk::ComboBox& ComboBox, const Gtk::TreeIter& Row)
{
	const double speed = options::tutorial_speed();
	return_if_fail(speed);

	show(ComboBox);
	move_pointer(ComboBox);

	ComboBox.popup();
	handle_pending_events();
	non_blocking_sleep(0.5 / speed);

	ComboBox.set_active(Row);
	ComboBox.popdown();
	handle_pending_events();
	non_blocking_sleep(0.5 / speed);
}

} // namespace interactive

} // namespace libk3dngui

