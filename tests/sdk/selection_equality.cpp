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

#include <k3dsdk/geometry.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/selection.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#define test_expression(expression) \
{ \
  if(!(expression)) \
    { \
    std::ostringstream buffer; \
    buffer << "Expression failed at line " << __LINE__ << ": " << #expression; \
    throw std::runtime_error(buffer.str()); \
    } \
}

int main(int argc, char* argv[])
{
	k3d::log_color_level(true);
	k3d::log_show_level(true);
	k3d::log_minimum_level(k3d::K3D_LOG_LEVEL_DEBUG);

	try
	{
		k3d::selection::set a;
		k3d::selection::set b;

		test_expression(a == b);

		k3d::geometry::point_selection::create(a);

		test_expression(a != b);

		k3d::geometry::primitive_selection::create(b);

		test_expression(a != b);		
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

