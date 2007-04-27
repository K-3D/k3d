#ifndef K3DSDK_SIGNAL_ACCUMULATORS_H
#define K3DSDK_SIGNAL_ACCUMULATORS_H

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

#include "log.h"
#include <iostream>

namespace k3d
{

namespace signal
{

/** Defines a signal accumulator for libsigc++ signals that require slots returning bool.
iff any slot returns "false", signal emission is stopped immediately and the signal will
return "false".  Otherwise, the signal will return "true".  This is handy for signals that
represent some action that any observer can cancel, e.g. a document object can prevent
the application from closing if the document hasn't been saved. */

class  cancelable
{
public:
	typedef bool result_type;

	template<typename IteratorT>
	result_type operator()(IteratorT First, IteratorT Last)
	{
		for(; First != Last; ++First)
			{
				if(false == *First)
					return false;
			}

		return true;
	}
};

/** Defines a signal accumulator for libsigc++ signals that require slots returning bool.
iff any slot returns "true", signal emission is stopped immediately and the signal will
return "true".  Otherwise, the signal will return "false".  This is handy for signals that
represent some event that is "consumed" by an observer, e.g. a keyboard event that
might-or-might-not be used in a hotkey.  If used, emission of the event should stop;
if not, it should continue. */

class  consumable
{
public:
	typedef bool result_type;

	/// New implementation for use with libsigc++ 2.0
	template<typename IteratorT>
	result_type operator()(IteratorT First, IteratorT Last)
	{
		for(; First != Last; ++First)
			{
				if(true == *First)
					return true;
			}

		return false;
	}
};

} // namespace signal

} // namespace k3d

#endif // !K3DSDK_SIGNAL_ACCUMULATORS_H

