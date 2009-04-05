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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "keyboard.h"
#include <gdk/gdk.h>

namespace k3d
{

namespace ngui
{

const k3d::key_modifiers convert(const unsigned int Modifiers)
{
	k3d::key_modifiers result;

	result.set_shift(Modifiers & GDK_SHIFT_MASK ? true : false);
	result.set_lock(Modifiers & GDK_LOCK_MASK ? true : false);
	result.set_control(Modifiers & GDK_CONTROL_MASK ? true : false);
	result.set_mod1(Modifiers & GDK_MOD1_MASK ? true : false);
	result.set_mod2(Modifiers & GDK_MOD2_MASK ? true : false);
	result.set_mod3(Modifiers & GDK_MOD3_MASK ? true : false);
	result.set_mod4(Modifiers & GDK_MOD4_MASK ? true : false);
	result.set_mod5(Modifiers & GDK_MOD5_MASK ? true : false);
	result.set_button1(Modifiers & GDK_BUTTON1_MASK ? true : false);
	result.set_button2(Modifiers & GDK_BUTTON2_MASK ? true : false);
	result.set_button3(Modifiers & GDK_BUTTON3_MASK ? true : false);
	result.set_button4(Modifiers & GDK_BUTTON4_MASK ? true : false);
	result.set_button5(Modifiers & GDK_BUTTON5_MASK ? true : false);
	result.set_release(Modifiers & GDK_RELEASE_MASK ? true : false);

	return result;
}

const unsigned int convert(const k3d::key_modifiers Modifiers)
{
	int result = 0;
	
	if(Modifiers.shift()) result |= GDK_SHIFT_MASK;
	if(Modifiers.lock()) result |= GDK_LOCK_MASK;
	if(Modifiers.control()) result |= GDK_CONTROL_MASK;
	if(Modifiers.mod1()) result |= GDK_MOD1_MASK;
	if(Modifiers.mod2()) result |= GDK_MOD2_MASK;
	if(Modifiers.mod3()) result |= GDK_MOD3_MASK;
	if(Modifiers.mod4()) result |= GDK_MOD4_MASK;
	if(Modifiers.mod5()) result |= GDK_MOD5_MASK;
	if(Modifiers.button1()) result |= GDK_BUTTON1_MASK;
	if(Modifiers.button2()) result |= GDK_BUTTON2_MASK;
	if(Modifiers.button3()) result |= GDK_BUTTON3_MASK;
	if(Modifiers.button4()) result |= GDK_BUTTON4_MASK;
	if(Modifiers.button5()) result |= GDK_BUTTON5_MASK;
	if(Modifiers.release()) result |= GDK_RELEASE_MASK;
	
	return result;
}

} // namespace ngui

} // namespace k3d

