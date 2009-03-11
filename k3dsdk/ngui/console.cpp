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
		buffer(Gtk::TextBuffer::create())
	{
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

		view.scroll_to(buffer->get_insert());
	}

	Glib::RefPtr<Gtk::TextBuffer> buffer;
	Glib::RefPtr<Gtk::TextTag> current_format;
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

	m_implementation->view.set_buffer(m_implementation->buffer);
	m_implementation->view.set_editable(false);
	m_implementation->view.set_cursor_visible(false);
	m_implementation->view.set_wrap_mode(Gtk::WRAP_CHAR);

	m_implementation->scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_implementation->scrolled_window.add(m_implementation->view);

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
}

} // namespace console

} // namespace ngui

} // namespace k3d

