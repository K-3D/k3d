#ifndef K3DSDK_STRING_MODIFIERS_H
#define K3DSDK_STRING_MODIFIERS_H

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
		\brief Declares extended std::string functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include <sstream>
#include <string>

namespace k3d
{

/// Replaces all occurrences of the Search string within the Source string with the Replacement string
const std::string replace_all(const std::string& Search, const std::string& Replacement, const std::string& Source);
/// Returns the rightmost N characters of the string
const std::string right(const std::string& Text, std::string::size_type Count);
/// Trims whitespaces from both ends of the string
const std::string trim(const std::string& Text);
/// Trims beginning whitespaces from the string
const std::string trimleft(const std::string& Text);
/// Trims trailing whitespaces from the string
const std::string trimright(const std::string& Text);

} // namespace k3d

#endif // !K3DSDK_STRING_MODIFIERS_H


