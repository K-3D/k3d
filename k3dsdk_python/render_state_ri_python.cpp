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

#include "render_state_ri_python.h"

#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/ishader_collection_ri.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

render_state_ri::render_state_ri() :
	base()
{
}

render_state_ri::render_state_ri(const k3d::ri::render_state* State) :
	base(State)
{
}

render_state_ri::render_state_ri(const k3d::ri::render_state& State) :
	base(State)
{
}

void render_state_ri::use_shader(const std::string& Shader)
{
	wrapped().shaders.use_shader(filesystem::native_path(ustring::from_utf8(Shader)));
}

void export_render_state_ri()
{
	class_<render_state_ri>("render_state_ri")
		.def("use_shader", &render_state_ri::use_shader);
}

} // namespace python

} // namespace k3d

