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

#include "keyboard.h"
#include <iostream>

namespace k3d
{

//////////////////////////////////////////////////////////////////////////////////////////////
// key_modifiers

key_modifiers& key_modifiers::set() { m_storage.set(); return *this; }
key_modifiers& key_modifiers::set_shift(bool val) { m_storage.set(0, val); return *this; }
key_modifiers& key_modifiers::set_lock(bool val) { m_storage.set(1, val); return *this; }
key_modifiers& key_modifiers::set_control(bool val) { m_storage.set(2, val); return *this; }
key_modifiers& key_modifiers::set_mod1(bool val) { m_storage.set(3, val); return *this; }
key_modifiers& key_modifiers::set_mod2(bool val) { m_storage.set(4, val); return *this; }
key_modifiers& key_modifiers::set_mod3(bool val) { m_storage.set(5, val); return *this; }
key_modifiers& key_modifiers::set_mod4(bool val) { m_storage.set(6, val); return *this; }
key_modifiers& key_modifiers::set_mod5(bool val) { m_storage.set(7, val); return *this; }
key_modifiers& key_modifiers::set_button1(bool val) { m_storage.set(8, val); return *this; }
key_modifiers& key_modifiers::set_button2(bool val) { m_storage.set(9, val); return *this; }
key_modifiers& key_modifiers::set_button3(bool val) { m_storage.set(10, val); return *this; }
key_modifiers& key_modifiers::set_button4(bool val) { m_storage.set(11, val); return *this; }
key_modifiers& key_modifiers::set_button5(bool val) { m_storage.set(12, val); return *this; }
key_modifiers& key_modifiers::set_release(bool val) { m_storage.set(13, val); return *this; }

key_modifiers& key_modifiers::reset() { m_storage.reset(); return *this; }
key_modifiers& key_modifiers::reset_shift() { m_storage.reset(0); return *this; }
key_modifiers& key_modifiers::reset_lock() { m_storage.reset(1); return *this; }
key_modifiers& key_modifiers::reset_control() { m_storage.reset(2); return *this; }
key_modifiers& key_modifiers::reset_mod1() { m_storage.reset(3); return *this; }
key_modifiers& key_modifiers::reset_mod2() { m_storage.reset(4); return *this; }
key_modifiers& key_modifiers::reset_mod3() { m_storage.reset(5); return *this; }
key_modifiers& key_modifiers::reset_mod4() { m_storage.reset(6); return *this; }
key_modifiers& key_modifiers::reset_mod5() { m_storage.reset(7); return *this; }
key_modifiers& key_modifiers::reset_button1() { m_storage.reset(8); return *this; }
key_modifiers& key_modifiers::reset_button2() { m_storage.reset(9); return *this; }
key_modifiers& key_modifiers::reset_button3() { m_storage.reset(10); return *this; }
key_modifiers& key_modifiers::reset_button4() { m_storage.reset(11); return *this; }
key_modifiers& key_modifiers::reset_button5() { m_storage.reset(12); return *this; }
key_modifiers& key_modifiers::reset_release() { m_storage.reset(13); return *this; }

key_modifiers& key_modifiers::flip() { m_storage.flip(); return *this; }
key_modifiers& key_modifiers::flip_shift() { m_storage.set(0); return *this; }
key_modifiers& key_modifiers::flip_lock() { m_storage.set(1); return *this; }
key_modifiers& key_modifiers::flip_control() { m_storage.flip(2); return *this; }
key_modifiers& key_modifiers::flip_mod1() { m_storage.flip(3); return *this; }
key_modifiers& key_modifiers::flip_mod2() { m_storage.flip(4); return *this; }
key_modifiers& key_modifiers::flip_mod3() { m_storage.flip(5); return *this; }
key_modifiers& key_modifiers::flip_mod4() { m_storage.flip(6); return *this; }
key_modifiers& key_modifiers::flip_mod5() { m_storage.flip(7); return *this; }
key_modifiers& key_modifiers::flip_button1() { m_storage.flip(8); return *this; }
key_modifiers& key_modifiers::flip_button2() { m_storage.flip(9); return *this; }
key_modifiers& key_modifiers::flip_button3() { m_storage.flip(10); return *this; }
key_modifiers& key_modifiers::flip_button4() { m_storage.flip(11); return *this; }
key_modifiers& key_modifiers::flip_button5() { m_storage.flip(12); return *this; }
key_modifiers& key_modifiers::flip_release() { m_storage.flip(13); return *this; }

bool key_modifiers::any() const { return m_storage.any(); }
bool key_modifiers::none() const { return m_storage.none(); }
bool key_modifiers::shift() const { return m_storage[0]; }
bool key_modifiers::lock() const { return m_storage[1]; }
bool key_modifiers::control() const { return m_storage[2]; }
bool key_modifiers::mod1() const { return m_storage[3]; }
bool key_modifiers::mod2() const { return m_storage[4]; }
bool key_modifiers::mod3() const { return m_storage[5]; }
bool key_modifiers::mod4() const { return m_storage[6]; }
bool key_modifiers::mod5() const { return m_storage[7]; }
bool key_modifiers::button1() const { return m_storage[8]; }
bool key_modifiers::button2() const { return m_storage[9]; }
bool key_modifiers::button3() const { return m_storage[10]; }
bool key_modifiers::button4() const { return m_storage[11]; }
bool key_modifiers::button5() const { return m_storage[12]; }
bool key_modifiers::release() const { return m_storage[13]; }

bool key_modifiers::operator==(const key_modifiers& RHS) const { return m_storage == RHS.m_storage; }
bool key_modifiers::operator!=(const key_modifiers& RHS) const { return m_storage != RHS.m_storage; }
bool key_modifiers::operator<(const key_modifiers& RHS) const { return m_storage.to_ulong() < RHS.m_storage.to_ulong(); }

key_modifiers& key_modifiers::operator&=(const key_modifiers& RHS) { m_storage &= RHS.m_storage; return *this; }
key_modifiers& key_modifiers::operator|=(const key_modifiers& RHS) { m_storage |= RHS.m_storage; return *this; }

const key_modifiers operator&(const key_modifiers& LHS, const key_modifiers& RHS) { key_modifiers temp(LHS); temp &= RHS; return temp; }
const key_modifiers operator|(const key_modifiers& LHS, const key_modifiers& RHS) { key_modifiers temp(LHS); temp |= RHS; return temp; }

namespace
{

void save_state(std::ostream& Stream, const std::string& Serialized, const bool State, bool& AnySoFar)
{
	if(!State)
		return;
		
	if(AnySoFar)
		Stream << "+";
		
	Stream << Serialized;
	AnySoFar = true;
}

} // namespace

std::ostream& operator<<(std::ostream& Stream, const k3d::key_modifiers& RHS)
{
	bool any_so_far = false;
	save_state(Stream, "shift", RHS.shift(), any_so_far);
	save_state(Stream, "lock", RHS.lock(), any_so_far);
	save_state(Stream, "control", RHS.control(), any_so_far);
	save_state(Stream, "mod1", RHS.mod1(), any_so_far);
	save_state(Stream, "mod2", RHS.mod2(), any_so_far);
	save_state(Stream, "mod3", RHS.mod3(), any_so_far);
	save_state(Stream, "mod4", RHS.mod4(), any_so_far);
	save_state(Stream, "mod5", RHS.mod5(), any_so_far);
	save_state(Stream, "button1", RHS.button1(), any_so_far);
	save_state(Stream, "button2", RHS.button2(), any_so_far);
	save_state(Stream, "button3", RHS.button3(), any_so_far);
	save_state(Stream, "button4", RHS.button4(), any_so_far);
	save_state(Stream, "button5", RHS.button5(), any_so_far);
	save_state(Stream, "release", RHS.release(), any_so_far);
	
	if(!any_so_far)
		Stream << "none";
	
	return Stream;
}

std::istream& operator>>(std::istream& Stream, k3d::key_modifiers& RHS)
{
	RHS.reset();

	std::string buffer;
	Stream >> buffer;
	
	RHS.set_shift(buffer.find("shift") != std::string::npos);
	RHS.set_lock(buffer.find("lock") != std::string::npos);
	RHS.set_control(buffer.find("control") != std::string::npos);
	RHS.set_mod1(buffer.find("mod1") != std::string::npos);
	RHS.set_mod2(buffer.find("mod2") != std::string::npos);
	RHS.set_mod3(buffer.find("mod3") != std::string::npos);
	RHS.set_mod4(buffer.find("mod4") != std::string::npos);
	RHS.set_mod5(buffer.find("mod5") != std::string::npos);
	RHS.set_button1(buffer.find("button1") != std::string::npos);
	RHS.set_button2(buffer.find("button2") != std::string::npos);
	RHS.set_button3(buffer.find("button3") != std::string::npos);
	RHS.set_button4(buffer.find("button4") != std::string::npos);
	RHS.set_button5(buffer.find("button5") != std::string::npos);
	RHS.set_release(buffer.find("release") != std::string::npos);
	
	return Stream;
}

} // namespace k3d

