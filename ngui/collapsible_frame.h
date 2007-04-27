#ifndef K3D_NGUI_COLLAPSIBLE_FRAME_H
#define K3D_NGUI_COLLAPSIBLE_FRAME_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <gtkmm/frame.h>

// Forward declarations
namespace Gtk { class Button; }

namespace libk3dngui
{

namespace collapsible_frame
{

/// Groups a collection of control objects together so they can be expanded / collapsed as a set
class group
{
public:
	group();
	~group();

	/// Expands every control in the group
	void expand_all();
	/// Collapses every control in the group
	void collapse_all();
	
private:
	class implementation;
	implementation* const m_implementation;
	
	friend class control;
};

/// Provides a container that can be manually collapsed / expanded by users to reduce clutter
class control :
	public Gtk::Frame
{
public:
	control(const Glib::ustring& Name);
	control(const Glib::ustring& Name, group& Group);
	~control();

	/// Programmatically expands the control
	void expand();
	/// Programmatically collapses the control
	void collapse();
	/// Returns true iff the control is currently collapsed
	bool is_collapsed();
	/// Low-level access to the button that toggles the collapsed / expanded state
	Gtk::Button& toggle_button();

private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace collapsible_frame

} // namespace libk3dngui

#endif // !K3D_NGUI_COLLAPSIBLE_FRAME_H
