#ifndef K3DSDK_ICROP_WINDOW_H
#define K3DSDK_ICROP_WINDOW_H

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

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for an object whose output can be cropped (units are in projection-space)
class icrop_window :
	public virtual iunknown
{
public:
	virtual iproperty& crop_left() = 0;
	virtual iproperty& crop_right() = 0;
	virtual iproperty& crop_top() = 0;
	virtual iproperty& crop_bottom() = 0;

protected:
	icrop_window() {}
	icrop_window(const icrop_window&) {}
	icrop_window& operator=(const icrop_window&) { return *this; }
	virtual ~icrop_window() {}
};

} // namespace k3d

#endif // !K3DSDK_ICROP_WINDOW_H
