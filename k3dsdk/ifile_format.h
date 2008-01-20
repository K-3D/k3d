#ifndef K3DSDK_IFILE_FORMAT_H
#define K3DSDK_IFILE_FORMAT_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

namespace filesystem { class path; }

/// Abstract interface for objects that can read or write data from filesystem files
class ifile_format :
	public virtual iunknown
{
public:
	virtual ~ifile_format() {}

protected:
	ifile_format() {}
	ifile_format(const ifile_format&) {}
	ifile_format& operator=(const ifile_format&) { return *this; }
};

} // namespace k3d

#endif // K3DSDK_IFILE_FORMAT_H
