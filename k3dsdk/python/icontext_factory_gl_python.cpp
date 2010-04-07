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

#include <k3dsdk/python/icontext_factory_gl_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/utility_python.h>
#include <k3dsdk/context_gl.h>
#include <k3dsdk/icontext_factory_gl.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void create(iunknown_wrapper& Self, const uint_t Width, const uint_t Height)
{
	delete Self.wrapped<k3d::gl::icontext_factory>().create(Width, Height);
}

void define_methods_icontext_factory_gl(k3d::iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::gl::icontext_factory*>(&Interface))
		return;

	utility::add_method(utility::make_function(&create, "Creates an OpenGL context."), "create", Instance);
}

} // namespace python

} // namespace k3d

