#ifndef K3DSDK_SIGNAL_SLOTS_H
#define K3DSDK_SIGNAL_SLOTS_H

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

namespace k3d
{

class ihint;

namespace signal
{

template<typename SignalT>
class loop_safe_slot
{
public:
	loop_safe_slot(SignalT& Signal) :
		signal(Signal),
		emitting(false)
	{
	}

	void operator()()
	{
		if(emitting)
			return;

		emitting = true;
		signal.emit();
		emitting = false;
	}

	void operator()(ihint* const Hint)
	{
		if(emitting)
			return;

		emitting = true;
		signal.emit(Hint);
		emitting = false;
	}

private:
	SignalT& signal;
	bool emitting;
};

template<typename SignalT>
loop_safe_slot<SignalT> make_loop_safe_slot(SignalT& Signal)
{
	return loop_safe_slot<SignalT>(Signal);
}

} // namespace signal

} // namespace k3d

#endif // !K3DSDK_SIGNAL_SLOTS_H


