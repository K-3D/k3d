// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "ri_python.h"

#include <k3dsdk/ishader_collection_ri.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class ri
{
};

static void use_shader(ri_render_state_wrapper& Self, const string_t& Shader)
{
	Self.wrapped().shaders.use_shader(filesystem::native_path(ustring::from_utf8(Shader)));
}

void define_namespace_ri()
{
	scope outer = class_<ri>("ri", no_init);

	class_<ri_render_state_wrapper>("render_state", 
		"Used to pass RenderMan state from the render engine to an object being rendered.", no_init)
		.def("use_shader", &use_shader,
			"Inform the render engine that an object will be using the given shader (so the render system can compile the shader on-demand).");
}

} // namespace python

} // namespace k3d

