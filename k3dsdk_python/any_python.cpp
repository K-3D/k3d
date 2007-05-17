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

#include "angle_axis_python.h"
#include "any_python.h"
#include "bitmap_python.h"
#include "mesh_python.h"
#include "node_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/types.h>
#include <k3dsdk/vectors.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

/// Converts a boost::any object to a Python value
const object any_to_python(const boost::any& Value)
{
	// We put the likely types up front for efficiency ...
	const std::type_info& type = Value.type();

	if(type == typeid(bool))
		return object(boost::any_cast<bool>(Value));

	if(type == typeid(double))
		return object(boost::any_cast<double>(Value));

	if(type == typeid(std::string))
		return object(boost::any_cast<std::string>(Value));

	if(type == typeid(filesystem::path))
		return object(boost::any_cast<filesystem::path>(Value));

	if(type == typeid(k3d::color))
		return object(boost::any_cast<k3d::color>(Value));

	if(type == typeid(k3d::point2))
		return object(boost::any_cast<k3d::point2>(Value));

	if(type == typeid(k3d::point3))
		return object(boost::any_cast<k3d::point3>(Value));

	if(type == typeid(k3d::vector3))
		return object(boost::any_cast<k3d::vector3>(Value));

	if(type == typeid(k3d::point4))
		return object(boost::any_cast<k3d::point4>(Value));

	if(type == typeid(k3d::matrix4))
		return object(boost::any_cast<k3d::matrix4>(Value));

	if(type == typeid(angle_axis))
		return object(angle_axis(boost::any_cast<k3d::angle_axis>(Value)));

	if(type == typeid(k3d::euler_angles))
		return object(boost::any_cast<k3d::euler_angles>(Value));

	if(type == typeid(k3d::mesh_selection))
		return object(boost::any_cast<k3d::mesh_selection>(Value));

	if(type == typeid(k3d::bounding_box3))
		return object(boost::any_cast<k3d::bounding_box3>(Value));

	if(type == typeid(int))
		return object(boost::any_cast<int>(Value));

	if(type == typeid(long))
		return object(boost::any_cast<long>(Value));

	if(type == typeid(unsigned long))
		return object(boost::any_cast<unsigned long>(Value));

	if(type == typeid(k3d::dev::mesh*))
	{
		k3d::dev::mesh* const k3d_mesh = boost::any_cast<k3d::dev::mesh*>(Value);
		return k3d_mesh ? object(mesh(k3d_mesh)) : object();
	}

	if(type == typeid(k3d::bitmap*))
	{
		k3d::bitmap* const k3d_bitmap = boost::any_cast<k3d::bitmap*>(Value);
		return k3d_bitmap ? object(bitmap(k3d_bitmap)) : object();
	}

	if(type == typeid(k3d::inode*))
	{
		k3d::inode* const k3d_node = boost::any_cast<k3d::inode*>(Value);
		return k3d_node ? object(node(k3d_node)) : object();
	}

	throw std::invalid_argument("can't convert unrecognized type [" + demangle(type) + "] to boost::python::object");
}

/// Converts a Python value to a boost::any object, with the target type explicitly specified
const boost::any python_to_any(const object& Value, const std::type_info& TargetType)
{
	PyObject* const value = Value.ptr();

	if(TargetType == typeid(bool))
	{
		return boost::any(PyObject_IsTrue(value) ? true : false);
	}

	if(TargetType == typeid(int))
	{
		return_val_if_fail(PyInt_Check(value), boost::any());
		return boost::any(static_cast<int>(PyInt_AsLong(value)));
	}

	if(TargetType == typeid(long))
	{
		if(PyInt_Check(value))
			return boost::any(static_cast<long>(PyInt_AsLong(value)));

		if(PyLong_Check(value))
			return boost::any(static_cast<long>(PyLong_AsLong(value)));

		throw std::invalid_argument("can't convert Python value to long");
	}

	if(TargetType == typeid(unsigned long))
	{
		return_val_if_fail(PyLong_Check(value), boost::any());
		return boost::any(static_cast<unsigned long>(PyLong_AsLong(value)));
	}

	if(TargetType == typeid(double))
	{
		if(PyFloat_Check(value))
			return boost::any(PyFloat_AsDouble(value));

		if(PyInt_Check(value))
			return boost::any(static_cast<double>(PyInt_AsLong(value)));

		if(PyLong_Check(value))
			return boost::any(static_cast<double>(PyLong_AsLong(value)));

		throw std::invalid_argument("can't convert Python value to double");
	}

	if(TargetType == typeid(std::string))
	{
		return_val_if_fail(PyString_Check(value), boost::any());
		return boost::any(std::string(PyString_AsString(value)));
	}

	if(TargetType == typeid(filesystem::path))
	{
		return_val_if_fail(PyString_Check(value), boost::any());
		return boost::any(filesystem::native_path(ustring::from_utf8(PyString_AsString(value))));
	}

	if(TargetType == typeid(k3d::color))
	{
		return boost::any(extract<k3d::color>(Value)());
	}

	if(TargetType == typeid(k3d::point3))
	{
		return boost::any(extract<k3d::point3>(Value)());
	}

	if(TargetType == typeid(k3d::vector3))
	{
		return boost::any(extract<k3d::vector3>(Value)());
	}

	if(TargetType == typeid(k3d::matrix4))
	{
		return boost::any(extract<k3d::matrix4>(Value)());
	}

	if(TargetType == typeid(k3d::angle_axis))
	{
		return boost::any(extract<angle_axis>(Value)());
	}

	if(TargetType == typeid(k3d::mesh_selection))
	{
		return boost::any(extract<k3d::mesh_selection>(Value)());
	}

	if(TargetType == typeid(k3d::bounding_box3))
	{
		return boost::any(extract<k3d::bounding_box3>(Value)());
	}

	if(TargetType == typeid(k3d::inode*))
	{
		return boost::any(static_cast<const inode&>(extract<node>(Value)()).wrapped_ptr());
	}

	if(TargetType == typeid(k3d::bitmap*))
	{
		return boost::any(extract<bitmap>(Value)().wrapped_ptr());
	}

	throw std::invalid_argument("can't convert python value to unrecognized type [" + demangle(TargetType) + "]");
}

} // namespace python

} // namespace k3d

