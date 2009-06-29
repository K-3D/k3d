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

#include <k3dsdk/log_control.h>
#include <k3dsdk/serialization_xml.h>
#include <k3dsdk/xml.h>
#include <k3dsdk/xpath.h>

using namespace k3d::xml;

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
		element document("k3d",
			element("objects",
				element("object",
					attribute("name", "foo")
					),
				element("object",
					attribute("name", "bar")
					)
				),
			element("pipeline"
				)
			);

		std::cerr << "before:" << std::endl;
		std::cerr << document << std::endl;

		upgrade_document(document);

		std::cerr << "after:" << std::endl;
		std::cerr << document << std::endl;

		test_expression(xpath::match(document, "/k3d/objects").size() == 0);
		test_expression(xpath::match(document, "/k3d/nodes").size() == 1);

		test_expression(xpath::match(document, "/k3d/nodes/object").size() == 0);
		test_expression(xpath::match(document, "/k3d/nodes/node").size() == 2);

		test_expression(xpath::match(document, "/k3d/pipeline").size() == 0);
		test_expression(xpath::match(document, "/k3d/dependencies").size() == 1);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

