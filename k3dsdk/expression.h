#ifndef K3DSDK_EXPRESSION_H
#define K3DSDK_EXPRESSION_H

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

#include <iosfwd>
#include <string>

namespace k3d
{

namespace expression
{

/**

\brief Parses a mathematical expression
\param Stream Input stream containing the expression to be parsed
\param Value Iff the input expression is completely parsed, contains the expression value; unchanged otherwise
\return true, iff the input expression is completely parsed

*/
bool parse(std::istream& Stream, double& Value);

/**

\brief Parses a mathematical expression
\param Buffer Input buffer containing the expression to be parsed
\param Value Iff the input expression is completely parsed, contains the expression value; unchanged otherwise
\return true, iff the input expression is completely parsed

*/
bool parse(const std::string& Buffer, double& Value);

} // namespace expression

} // namespace k3d

#endif // !K3DSDK_EXPRESSION_H


