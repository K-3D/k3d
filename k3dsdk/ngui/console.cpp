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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "console.h"

#include <k3dsdk/log.h>

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include <boost/algorithm/string.hpp>

namespace k3d
{

namespace ngui
{

namespace console
{

/////////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation() :
		command_index(0)
	{
		buffer = Gtk::TextBuffer::create();

		read_only = Gtk::TextTag::create("read-only");
		read_only->property_editable() = false;
		buffer->get_tag_table()->add(read_only);

		begin_input = Gtk::TextMark::create("begin-input");
		buffer->add_mark(begin_input, buffer->end());

		view.set_buffer(buffer);
		view.set_editable(false);
		view.set_cursor_visible(false);
		view.set_wrap_mode(Gtk::WRAP_WORD);
		view.signal_key_press_event().connect(sigc::mem_fun(*this, &implementation::on_key_press_event), false);

		scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
		scrolled_window.add(view);
	}

	bool on_key_press_event(GdkEventKey* event)
	{
		if(event->keyval == GDK_Return)
		{
			const k3d::string_t input = buffer->get_text(buffer->get_iter_at_mark(begin_input), buffer->end()).raw();

			buffer->insert(buffer->end(), "\n");
			buffer->apply_tag(read_only, buffer->get_iter_at_mark(begin_input), buffer->end());
			buffer->move_mark(begin_input, buffer->end());

			if(!input.empty() && (command_history.empty() || input != command_history.back()))
				command_history.push_back(input);
			command_index = command_history.size();
			command_signal.emit(input);

			return true;
		}
		else if(event->keyval == GDK_Left)
		{
			return buffer->get_iter_at_mark(buffer->get_insert()) <= buffer->get_iter_at_mark(begin_input);
		}
		else if(event->keyval == GDK_Up)
		{
			if(command_index)
			{
				if(command_index == command_history.size())
					command_buffer = buffer->get_text(buffer->get_iter_at_mark(begin_input), buffer->end()).raw();

				command_index -= 1;
				buffer->erase(buffer->get_iter_at_mark(begin_input), buffer->end());
				buffer->insert(buffer->end(), command_history[command_index]);
			}

			buffer->place_cursor(buffer->end());
			return true;
		}
		else if(event->keyval == GDK_Down)
		{
			if(command_index < command_history.size())
			{
				command_index += 1;
				buffer->erase(buffer->get_iter_at_mark(begin_input), buffer->end());
				buffer->insert(buffer->end(),
					command_index < command_history.size()
					? command_history[command_index]
					: command_buffer);
			}

			buffer->place_cursor(buffer->end());
			return true;
		}
		else if(event->keyval == GDK_v && (event->state & GDK_CONTROL_MASK))
		{
			const k3d::string_t input = Gtk::Clipboard::get()->wait_for_text();
			
			std::vector<k3d::string_t> lines;
			boost::split(lines, input, boost::is_any_of("\n"));

			for(k3d::uint_t i = 0; i != lines.size(); ++i)
			{
				buffer->insert(buffer->end(), lines[i]);
				
				if(i+1 < lines.size())
				{
					buffer->insert(buffer->end(), "\n");
					buffer->apply_tag(read_only, buffer->get_iter_at_mark(begin_input), buffer->end());
					buffer->move_mark(begin_input, buffer->end());

					if(!lines[i].empty() && (command_history.empty() || lines[i] != command_history.back()))
						command_history.push_back(lines[i]);
					command_index = command_history.size();
					command_signal.emit(lines[i]);
				}
			}

			return true;
		}
		else if(event->keyval != GDK_Control_L && event->keyval != GDK_Control_R)
		{
			buffer->place_cursor(buffer->end());
			return false;
		}

		return false;
	}

	void print_string(const string_t& String)
	{
		if(current_format)
		{
			if(!buffer->get_tag_table()->lookup(current_format->property_name().get_value()))
				buffer->get_tag_table()->add(current_format);

			buffer->insert_with_tag(buffer->end(), String, current_format);
		}
		else
		{
			buffer->insert(buffer->end(), String);
		}

		buffer->apply_tag(read_only, buffer->begin(), buffer->end());
		view.scroll_to(buffer->get_insert());
	}

	std::vector<string_t> command_history;
	std::vector<string_t>::size_type command_index;
	string_t command_buffer;
	sigc::signal<void, const string_t&> command_signal;

	Glib::RefPtr<Gtk::TextBuffer> buffer;
	Glib::RefPtr<Gtk::TextTag> current_format;
	Glib::RefPtr<Gtk::TextTag> read_only;
	Glib::RefPtr<Gtk::TextMark> begin_input;

	Gtk::TextView view;
	Gtk::ScrolledWindow scrolled_window;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(k3d::icommand_node& Parent, const string_t& Name) :
	base(),
	m_implementation(new implementation())
{
	set_name("k3d-console");
	set_shadow_type(Gtk::SHADOW_NONE);
	add(m_implementation->scrolled_window);
}

control::~control()
{
	delete m_implementation;
}

void control::clear()
{
	m_implementation->buffer->erase(m_implementation->buffer->begin(), m_implementation->buffer->end());
}

void control::set_current_format(Glib::RefPtr<Gtk::TextTag>& Tag)
{
	m_implementation->current_format = Tag;
}

void control::print_string(const string_t& String)
{
	m_implementation->print_string(String);
	m_implementation->view.set_editable(false);
	m_implementation->view.set_cursor_visible(false);
}

void control::prompt_string(const string_t& String)
{
	m_implementation->print_string(String);
	m_implementation->buffer->move_mark(m_implementation->begin_input, m_implementation->buffer->end());
	m_implementation->buffer->place_cursor(m_implementation->buffer->end());
	m_implementation->view.set_editable(true);
	m_implementation->view.set_cursor_visible(true);
}

sigc::connection control::connect_command_signal(const sigc::slot<void, const string_t&>& Slot)
{
	return m_implementation->command_signal.connect(Slot);
}

} // namespace console

} // namespace ngui

} // namespace k3d

