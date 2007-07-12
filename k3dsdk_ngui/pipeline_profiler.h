#ifndef NGUI_PIPELINE_PROFILER_H
#define NGUI_PIPELINE_PROFILER_H

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

#include "panel.h"
#include "ui_component.h"

#include <gtkmm/box.h>
#include <vector>

namespace libk3dngui
{

class document_state;

namespace pipeline_profiler
{

/////////////////////////////////////////////////////////////////////////////
// control

class control :
	public Gtk::VBox,
	public panel::control,
	public ui_component
{
	typedef Gtk::VBox base;

public:
	control(document_state& DocumentState, k3d::icommand_node& Parent);
	~control();

	const std::string panel_type();
	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot);

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace pipeline_profiler

} // namespace libk3dngui

#endif // !NGUI_PIPELINE_PROFILER_H

