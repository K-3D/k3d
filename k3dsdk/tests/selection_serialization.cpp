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

#include <k3dsdk/dependencies.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/serialization_xml.h>
#include <k3dsdk/typed_array.h>
#include <k3dsdk/uint_t_array.h>
#include <k3dsdk/xml.h>

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
	try
	{
		k3d::selection::set a;
		k3d::selection::storage& points = a.create("points");
		k3d::uint_t_array& begin = points.structure.create<k3d::uint_t_array>("begin");
		k3d::uint_t_array& end = points.structure.create<k3d::uint_t_array>("end");
		k3d::typed_array<k3d::double_t>& value = points.structure.create<k3d::typed_array<k3d::double_t> >("value");

		begin.push_back(0);
		end.push_back(5);
		value.push_back(1);

		k3d::selection::storage& components = a.create("components");

		const k3d::filesystem::path root_path;
		k3d::dependencies dependencies;
		k3d::persistent_lookup lookup;
		k3d::ipersistent::save_context save_context(root_path, dependencies, lookup);

		k3d::xml::element xml("selection");
		k3d::xml::save(a, xml, save_context);

		k3d::selection::set b;
		test_expression(!b.almost_equal(a, 0));

		k3d::ipersistent::load_context load_context(root_path, lookup);
		k3d::xml::load(b, xml, load_context);

		test_expression(b.almost_equal(a, 0));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

