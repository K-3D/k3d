#ifndef K3DSDK_IMIME_TYPE_HANDLER_H
#define K3DSDK_IMIME_TYPE_HANDLER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
#include "types.h"

namespace k3d
{

namespace filesystem { class path; }

/// Abstract interface for objects that can identify the MIME Type of a file
class imime_type_handler :
	public virtual iunknown
{
public:
	/// Called to identify the MIME Type of a file.  Returns true iff the MIME Type was successfully identified.
	virtual const bool identify_mime_type(const filesystem::path& File, k3d::string_t& FileType) = 0;
	/// Called to identify the MIME Type of a data stream.  Returns true iff the MIME Type was successfully identified.
	virtual const bool identify_mime_type(const k3d::string_t& Data, k3d::string_t& DataType) = 0;

protected:
	imime_type_handler() {}
	imime_type_handler(const imime_type_handler&) {}
	imime_type_handler& operator=(const imime_type_handler&) { return *this; }
	virtual ~imime_type_handler() {}
};

} // namespace k3d

#endif // !K3DSDK_IMIME_TYPE_HANDLER_H

