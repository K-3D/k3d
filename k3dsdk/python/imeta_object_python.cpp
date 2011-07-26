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

#include <boost/python.hpp>

#include <k3dsdk/imeta_object.h>
#include <k3dsdk/python/any_python.h>
#include <k3dsdk/python/imeta_object_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/utility_python.h>

namespace k3d
{

namespace python
{

static boost::python::object execute(iunknown_wrapper& Self, const string_t& Command)
{
	return any_to_python(Self.wrapped<k3d::imeta_object>().execute(Command));
}

/*
static boost::python::object execute1(iunknown_wrapper& Self, const string_t& Command, const boost::python::object& Argument1)
{
	return any_to_python(Self.wrapped<k3d::imeta_object>().execute(Command, python_to_any(Argument1)));
}

static boost::python::object execute2(iunknown_wrapper& Self, const string_t& Command, const boost::python::object& Argument1, const boost::python::object& Argument2)
{
	return any_to_python(Self.wrapped<k3d::imeta_object>().execute(Command, python_to_any(Argument1), python_to_any(Argument2)));
}

static boost::python::object execute3(iunknown_wrapper& Self, const string_t& Command, const boost::python::object& Argument1, const boost::python::object& Argument2, const boost::python::object& Argument3)
{
	return any_to_python(Self.wrapped<k3d::imeta_object>().execute(Command, python_to_any(Argument1), python_to_any(Argument2), python_to_any(Argument3)));
}
*/

void define_methods_imeta_object(k3d::iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::imeta_object*>(&Interface))
		return;

	utility::add_method(utility::make_function(&execute, "Executes an object method."), "execute", Instance);
}

} // namespace python

} // namespace k3d

