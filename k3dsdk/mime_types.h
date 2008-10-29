#ifndef K3DSDK_MIME_TYPES_H
#define K3DSDK_MIME_TYPES_H

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

#include "types.h"

namespace k3d
{

namespace filesystem { class path; }

namespace mime
{

/// Defines storage for a MIME type (RFC 2046)
class type
{
public:
	type();

	/// Returns the MIME type of a file (could return an empty value if the file cannot be identified)
	static const type lookup(const filesystem::path& File);
	/// Returns the MIME type of a data stream (could return an empty value if the data cannot be identified)
	static const type lookup(const string_t& Data);

	const bool operator==(const string_t&) const;
	const bool operator==(const type&) const;
	const bool operator!=(const type&) const;

	const string_t str() const;

	const bool empty() const;
	operator bool() const;

private:
	string_t value;
};

const bool operator==(const string_t&, const type&);

} // namespace mime

} // namespace k3d

#endif // !K3DSDK_MIME_TYPES_H

