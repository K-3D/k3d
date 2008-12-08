// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "selection_python.h"

#include <k3dsdk/selection.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class selection
{
public:
};

void define_namespace_selection()
{
	scope outer = class_<selection>("selection", no_init)
		;

	enum_<k3d::selection::type>("type")
		.value("NONE", k3d::selection::NONE)
		.value("NODE", k3d::selection::NODE)
		.value("MESH", k3d::selection::MESH)
		.value("PRIMITIVE", k3d::selection::PRIMITIVE)
		.value("CONSTANT", k3d::selection::CONSTANT)
		.value("UNIFORM", k3d::selection::UNIFORM)
		.value("VARYING", k3d::selection::VARYING)
		.value("FACE_VARYING", k3d::selection::FACE_VARYING)
		.value("SPLIT_EDGE", k3d::selection::SPLIT_EDGE)
		.value("VERTEX", k3d::selection::VERTEX)
		.attr("__module__") = "k3d";
}

} // namespace python

} // namespace k3d

