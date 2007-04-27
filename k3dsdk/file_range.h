#ifndef K3DSDK_FILE_RANGE_H
#define K3DSDK_FILE_RANGE_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "path.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// file_range

/// Encapsulates a half-open range of numbered files (useful for working with animation frames)
class file_range
{
public:
	/// Creates an empty range
	file_range();

	/// Returns the maximum number of files that can be represented by this range
	const size_t max_file_count() const;
	/// Returns the full path to the Nth file (N does *not* have to be within the valid range)
	const filesystem::path file(const size_t N) const;

	/// Stores the directory that contains the range of files
	filesystem::path directory;
	/// Stores an optional string that precedes the filename number
	ustring before;
	/// Stores the number of digits used to number the filename
	size_t digits;
	/// Stores an optional string that follows the filename number (including file extension, etc)
	ustring after;
	/// Stores the first filename number in the range
	size_t begin;
	/// Stores the one-past-last filename number in the range
	size_t end;
};

} // namespace k3d

#endif // K3DSDK_FILE_RANGE_H

