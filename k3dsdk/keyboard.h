#ifndef K3DSDK_KEYBOARD_H
#define K3DSDK_KEYBOARD_H

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

#include "signal_accumulators.h"
#include "signal_system.h"

#include <bitset>
#include <iosfwd>

namespace k3d
{

class key_modifiers
{
public:
	key_modifiers& set();
	key_modifiers& set_shift(bool val = true);
	key_modifiers& set_lock(bool val = true);
	key_modifiers& set_control(bool val = true);
	key_modifiers& set_mod1(bool val = true);
	key_modifiers& set_mod2(bool val = true);
	key_modifiers& set_mod3(bool val = true);
	key_modifiers& set_mod4(bool val = true);
	key_modifiers& set_mod5(bool val = true);
	key_modifiers& set_button1(bool val = true);
	key_modifiers& set_button2(bool val = true);
	key_modifiers& set_button3(bool val = true);
	key_modifiers& set_button4(bool val = true);
	key_modifiers& set_button5(bool val = true);
	key_modifiers& set_release(bool val = true);
	
	key_modifiers& reset();
	key_modifiers& reset_shift();
	key_modifiers& reset_lock();
	key_modifiers& reset_control();
	key_modifiers& reset_mod1();
	key_modifiers& reset_mod2();
	key_modifiers& reset_mod3();
	key_modifiers& reset_mod4();
	key_modifiers& reset_mod5();
	key_modifiers& reset_button1();
	key_modifiers& reset_button2();
	key_modifiers& reset_button3();
	key_modifiers& reset_button4();
	key_modifiers& reset_button5();
	key_modifiers& reset_release();
	
	key_modifiers& flip();
	key_modifiers& flip_shift();
	key_modifiers& flip_lock();
	key_modifiers& flip_control();
	key_modifiers& flip_mod1();
	key_modifiers& flip_mod2();
	key_modifiers& flip_mod3();
	key_modifiers& flip_mod4();
	key_modifiers& flip_mod5();
	key_modifiers& flip_button1();
	key_modifiers& flip_button2();
	key_modifiers& flip_button3();
	key_modifiers& flip_button4();
	key_modifiers& flip_button5();
	key_modifiers& flip_release();

	bool any() const;
	bool none() const;
	bool shift() const;
	bool lock() const;
	bool control() const;
	bool mod1() const;
	bool mod2() const;
	bool mod3() const;
	bool mod4() const;
	bool mod5() const;
	bool button1() const;
	bool button2() const;
	bool button3() const;
	bool button4() const;
	bool button5() const;
	bool release() const;

	bool operator==(const key_modifiers& RHS) const;
	bool operator!=(const key_modifiers& RHS) const;
	bool operator<(const key_modifiers& RHS) const;
	
	key_modifiers& operator&=(const key_modifiers& RHS);
	key_modifiers& operator|=(const key_modifiers& RHS);

	friend const key_modifiers operator&(const key_modifiers& LHS, const key_modifiers& RHS);
	friend const key_modifiers operator|(const key_modifiers& LHS, const key_modifiers& RHS);

	friend std::ostream& operator<<(std::ostream& Stream, const k3d::key_modifiers& RHS);
	friend std::istream& operator>>(std::istream& Stream, k3d::key_modifiers& RHS);

private:
	std::bitset<14> m_storage;
};

} // namespace k3d

#endif // !K3DSDK_KEYBOARD_H
