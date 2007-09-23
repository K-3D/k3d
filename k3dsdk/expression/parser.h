#ifndef K3DSDK_EXPRESSION_PARSER_H
#define K3DSDK_EXPRESSION_PARSER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "basic_parser.h"

namespace k3d
{

namespace expression
{

/// Refinement of basic_parser that provides a standard set of functions and constants
class parser :
	public basic_parser
{
	typedef basic_parser base_t;

public:
	parser();
};

} // namespace expression

} // namespace k3d

#endif // !K3DSDK_EXPRESSION_PARSER-H

