#ifndef K3DSDK_XPATH_H
#define K3DSDK_XPATH_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/types.h>
#include <vector>

namespace k3d
{

namespace xml
{

class element;

/// Defines functionality for working with k3d::xml data using the XPath standard
namespace xpath
{

/// Defines storage for an XPath result-set
typedef std::vector<element*> result_set;

/// Searchs an XML tree using an XPath expression, returning any elements that match
result_set match(element& Tree, const string_t& Expression);

} // namespace xpath

} // namespace xml

} // namespace k3d

/// Serializes an XPath result-set to a stream
std::ostream& operator<<(std::ostream& Stream, const k3d::xml::xpath::result_set& RHS);

#endif // !K3DSDK_XPATH_H

