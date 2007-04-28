// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \author Timothy M. Shead (tshead@k-3d.com)
*/

#include "application_state.h"
#include "safe_close_dialog.h"
#include "unsaved_document.h"

#include <k3dsdk/application.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/iapplication.h>

#include <gtkmm/dialog.h>

#include <set>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// application_state::implementation

class application_state::implementation :
	public sigc::trackable
{
public:
	implementation() :
		m_batch_mode(false),
		m_custom_layouts(true),
		m_assign_hotkeys(false)
	{
	}

	/// Used to keep documents sorted in the "safe to close" dialog
	struct sort_by_title
	{
		bool operator()(const safe_close_dialog::entry& LHS, const safe_close_dialog::entry& RHS)
		{
			return LHS.document->unsaved_document_title() < RHS.document->unsaved_document_title();
		}
	};

	typedef sigc::signal<unsaved_document*> safe_close_signal_t;
	/// Signal that will be emitted prior to safe shutdown to "gather" unsaved documents
	safe_close_signal_t m_safe_close_signal;
	/// Stores the batch (no user intervention) mode state
	bool m_batch_mode;
	/// Stores the user-customizable UI layouts state
	bool m_custom_layouts;
	/// Stores the current global assign hotkeys state
	bool m_assign_hotkeys;
};

////////////////////////////////////////////////////////////////////////////
// application_state

application_state& application_state::instance()
{
	static application_state* g_instance = 0;
	if(!g_instance)
		g_instance = new application_state();

	return *g_instance;
}

application_state::application_state() :
	m_implementation(new implementation())
{
}

application_state::~application_state()
{
	delete m_implementation;
}

sigc::connection application_state::connect_safe_close_signal(const sigc::slot<unsaved_document*>& Slot)
{
	return m_implementation->m_safe_close_signal.connect(Slot);
}

bool application_state::safe_close(Gtk::Window& Parent)
{
	if(m_implementation->m_batch_mode)
		return k3d::application().exit();
	
	safe_close_dialog::entries_t entries;

	implementation::safe_close_signal_t::slot_list_type slots = m_implementation->m_safe_close_signal.slots();
	for(implementation::safe_close_signal_t::slot_list_type::iterator slot = slots.begin(); slot != slots.end(); ++slot)
	{
		unsaved_document* doc = (*slot)();
		if(doc && doc->unsaved_changes())
			entries.push_back(safe_close_dialog::entry(doc));
	}

	if(!entries.empty())
	{
		std::sort(entries.begin(), entries.end(), implementation::sort_by_title());

		switch(safe_close_dialog::run(Parent, entries))
		{
			case Gtk::RESPONSE_NONE:
			case Gtk::RESPONSE_CANCEL:
			case Gtk::RESPONSE_DELETE_EVENT:
				return false;
			case Gtk::RESPONSE_CLOSE:
				return k3d::application().exit();
			case Gtk::RESPONSE_OK:
				for(safe_close_dialog::entries_t::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
				{
					if(entry->save)
					{
						if(!entry->document->save_unsaved_changes())
							return false;
					}
				}
				return k3d::application().exit();
		}
	}
	
	return k3d::application().exit();
}

void application_state::enable_batch_mode(const bool Enabled)
{
	m_implementation->m_batch_mode = Enabled;
}

const bool application_state::batch_mode()
{
	return m_implementation->m_batch_mode;
}

void application_state::enable_custom_layouts(const bool Enabled)
{
	m_implementation->m_custom_layouts = Enabled;
}

const bool application_state::custom_layouts()
{
	return m_implementation->m_custom_layouts;
}

void application_state::enable_hotkey_assignment(const bool Enabled)
{
	m_implementation->m_assign_hotkeys = Enabled;
}

const bool application_state::assign_hotkeys()
{
	return m_implementation->m_assign_hotkeys;
}

} // namespace libk3dngui

