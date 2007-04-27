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

#include "file_range.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// file_range

file_range::file_range() :
	digits(0),
	begin(0),
	end(0)
{
}

const size_t file_range::max_file_count() const
{
	return static_cast<size_t>(std::pow(10.0, static_cast<double>(digits)));
}

const filesystem::path file_range::file(const size_t N) const
{
	std::ostringstream buffer;
	buffer << std::setfill('0') << std::setw(digits) << N;

	return directory / filesystem::generic_path(before + ustring::from_utf8(buffer.str()) + after);
}

} // namespace k3d

