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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "angle_axis_python.h"
#include "any_python.h"
#include "bitmap_python.h"
#include "const_bitmap_python.h"
#include "idocument_python.h"
#include "inode_python.h"
#include "mesh_python.h"
#include "node_python.h"
#include "ri_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/texture3.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/vectors.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

const object any_to_python(const boost::any& Value)
{
	const std::type_info& type = Value.type();

	if(type == typeid(k3d::bool_t))
		return object(boost::any_cast<k3d::bool_t>(Value));

	if(type == typeid(k3d::int8_t))
		return object(boost::any_cast<k3d::int8_t>(Value));

	if(type == typeid(k3d::int16_t))
		return object(boost::any_cast<k3d::int16_t>(Value));

	if(type == typeid(k3d::int32_t))
		return object(boost::any_cast<k3d::int32_t>(Value));

	if(type == typeid(k3d::int64_t))
		return object(boost::any_cast<k3d::int64_t>(Value));

	if(type == typeid(k3d::uint8_t))
		return object(boost::any_cast<k3d::uint8_t>(Value));

	if(type == typeid(k3d::uint16_t))
		return object(boost::any_cast<k3d::uint16_t>(Value));

	if(type == typeid(k3d::uint32_t))
		return object(boost::any_cast<k3d::uint32_t>(Value));

	if(type == typeid(k3d::uint64_t))
		return object(boost::any_cast<k3d::uint64_t>(Value));

	if(type == typeid(k3d::double_t))
		return object(boost::any_cast<k3d::double_t>(Value));

	if(type == typeid(k3d::string_t))
		return object(boost::any_cast<k3d::string_t>(Value));

	if(type == typeid(k3d::ustring))
		return object(boost::any_cast<k3d::ustring>(Value).raw());

	if(type == typeid(filesystem::path))
		return object(boost::any_cast<filesystem::path>(Value));

	if(type == typeid(k3d::color))
		return object(boost::any_cast<k3d::color>(Value));

	if(type == typeid(k3d::point2))
		return object(boost::any_cast<k3d::point2>(Value));

	if(type == typeid(k3d::point3))
		return object(boost::any_cast<k3d::point3>(Value));

	if(type == typeid(k3d::normal3))
		return object(boost::any_cast<k3d::normal3>(Value));

	if(type == typeid(k3d::texture3))
		return object(boost::any_cast<k3d::texture3>(Value));

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

	if(type == typeid(k3d::mesh*))
	{
		k3d::mesh* const value = boost::any_cast<k3d::mesh*>(Value);
		return value ? object(mesh(value)) : object();
	}

	if(type == typeid(const k3d::bitmap*))
	{
		return wrap(boost::any_cast<const k3d::bitmap*>(Value));
	}

	if(type == typeid(k3d::bitmap*))
	{
		return wrap(boost::any_cast<k3d::bitmap*>(Value));
	}

	if(type == typeid(k3d::inode*))
	{
		k3d::inode* const value = boost::any_cast<k3d::inode*>(Value);
		return value ? object(node(value)) : object();
	}

	if(type == typeid(k3d::idocument*))
	{
		k3d::idocument* const value = boost::any_cast<k3d::idocument*>(Value);
		return value ? object(wrap(value)) : object();
	}

	if(type == typeid(const k3d::ri::render_state*))
	{
		return wrap(boost::any_cast<const k3d::ri::render_state*>(Value));
	}

	if(type == typeid(std::vector<k3d::inode*>))
	{
		std::vector<k3d::inode*> nodes = boost::any_cast<std::vector<k3d::inode*> >(Value);

		boost::python::list results;
		for(k3d::uint_t i = 0; i != nodes.size(); ++i)
			results.append(wrap(nodes[i]));

		return results;
	}

	if(type == typeid(k3d::typed_array<k3d::double_t>))
	{
		k3d::typed_array<k3d::double_t> nodes = boost::any_cast<k3d::typed_array<k3d::double_t> >(Value);

		boost::python::list results;
		for(k3d::uint_t i = 0; i != nodes.size(); ++i)
			results.append(nodes[i]);

		return results;
	}

	typedef std::map<k3d::string_t, k3d::double_t> profiler_task_records_t;
	typedef std::map<k3d::inode*, profiler_task_records_t> profiler_node_records_t;
	if(type == typeid(profiler_node_records_t))
	{
		boost::python::dict python_node_records;

		profiler_node_records_t node_records = boost::any_cast<profiler_node_records_t>(Value);
		for(profiler_node_records_t::const_iterator node_record = node_records.begin(); node_record != node_records.end(); ++node_record)
		{
			boost::python::dict python_task_records;

			k3d::inode* const node = node_record->first;
			profiler_task_records_t task_records = node_record->second;
			for(profiler_task_records_t::const_iterator task_record = task_records.begin(); task_record != task_records.end(); ++task_record)
				python_task_records[task_record->first] = task_record->second;

			python_node_records[k3d::python::node(node)] = python_task_records;
		}

		return python_node_records;
	}

	throw std::invalid_argument("can't convert unrecognized type [" + demangle(type) + "] to boost::python::object");
}

const boost::any python_to_any(const object& Value)
{
	PyObject* const value = Value.ptr();

	{
		extract<idocument_wrapper> value(Value);
		if(value.check())
		{
			return boost::any(value().wrapped_ptr());
		}
	}

	{
		extract<inode_wrapper> value(Value);
		if(value.check())
		{
			return boost::any(value().wrapped_ptr());
		}
	}

	throw std::invalid_argument("can't convert unrecognized python value");
}

const boost::any python_to_any(const object& Value, const std::type_info& TargetType)
{
	PyObject* const value = Value.ptr();

	if(TargetType == typeid(bool))
		return boost::any(PyObject_IsTrue(value) ? true : false);

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
		return boost::any(extract<unsigned long>(Value)());

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
		return boost::any(extract<k3d::filesystem::path>(Value)());

	if(TargetType == typeid(k3d::color))
		return boost::any(extract<k3d::color>(Value)());

	if(TargetType == typeid(k3d::point3))
		return boost::any(extract<k3d::point3>(Value)());

	if(TargetType == typeid(k3d::point4))
		return boost::any(extract<k3d::point4>(Value)());

	if(TargetType == typeid(k3d::normal3))
		return boost::any(extract<k3d::normal3>(Value)());

	if(TargetType == typeid(k3d::vector3))
		return boost::any(extract<k3d::vector3>(Value)());

	if(TargetType == typeid(k3d::texture3))
		return boost::any(extract<k3d::texture3>(Value)());

	if(TargetType == typeid(k3d::matrix4))
		return boost::any(extract<k3d::matrix4>(Value)());

	if(TargetType == typeid(k3d::angle_axis))
		return boost::any(extract<angle_axis>(Value)());

	if(TargetType == typeid(k3d::mesh_selection))
		return boost::any(extract<k3d::mesh_selection>(Value)());

	if(TargetType == typeid(k3d::bounding_box3))
		return boost::any(extract<k3d::bounding_box3>(Value)());

	if(TargetType == typeid(k3d::inode*))
	{
		if(Value == boost::python::object())
			return boost::any(static_cast<k3d::inode*>(0));

		extract<inode_wrapper> inode(Value);
		if(inode.check())
			return boost::any(inode().wrapped_ptr());

		extract<node> node(Value);
		if(node.check())
			return boost::any(node().inode_wrapper::wrapped_ptr());

		return boost::any(static_cast<k3d::inode*>(0));
	}

	if(TargetType == typeid(const k3d::bitmap*))
		return boost::any(extract<const_bitmap_wrapper>(Value)().wrapped_ptr());

	if(TargetType == typeid(k3d::bitmap*))
		return boost::any(extract<bitmap_wrapper>(Value)().wrapped_ptr());

	if(TargetType == typeid(std::vector<k3d::inode*>))
	{
		std::vector<k3d::inode*> results;

		boost::python::list nodes = extract<boost::python::list>(Value);
		const k3d::uint_t count = boost::python::len(nodes);
		results.resize(count);
		for(k3d::uint_t i = 0; i != count; ++i)
			results[i] = extract<inode_wrapper>(nodes[i])().wrapped_ptr();

		return boost::any(results);
	}

	if(TargetType == typeid(k3d::typed_array<k3d::double_t>))
	{
		k3d::typed_array<k3d::double_t> results;

		boost::python::list values = extract<boost::python::list>(Value);
		const k3d::uint_t count = boost::python::len(values);
		results.resize(count);
		for(k3d::uint_t i = 0; i != count; ++i)
		{
		    results[i] = extract<k3d::double_t>(values[i])();
		}

		return boost::any(results);
	}

	throw std::invalid_argument("can't convert python value to unrecognized type [" + demangle(TargetType) + "]");
}

} // namespace python

} // namespace k3d

