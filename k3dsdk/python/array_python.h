#ifndef K3DSDK_ARRAY_PYTHON_H
#define K3DSDK_ARRAY_PYTHON_H

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

#include "imaterial_python.h"
#include "inode_python.h"

#include <k3dsdk/typed_array.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////
// array

template<typename array_type>
class array
{
public:
	typedef typename array_type::value_type value_type;

	array() :
		m_wrapped(0)
	{
	}

	array(array_type& Array) :
		m_wrapped(&Array)
	{
	}

	int len()
	{
		return wrapped().size();
	}

	value_type get_item(int item)
	{
		if(item < 0 || item >= wrapped().size())
			throw std::out_of_range("index out-of-range");

		return wrapped().at(item);
	}

	void set_item(int item, const value_type& value)
	{
		if(item < 0)
			throw std::out_of_range("index out-of-range");

		if(static_cast<size_t>(item) >= wrapped().size())
			wrapped().resize(item + 1);

		wrapped()[item] = value;
	}

	void append(const value_type& Value)
	{
		wrapped().push_back(Value);
	}

	void assign(const boost::python::list& Value)
	{
		array_type& storage = wrapped();

		const size_t count = boost::python::len(Value);
		storage.resize(count);
		for(size_t i = 0; i != count; ++i)
			storage[i] = boost::python::extract<typename array_type::value_type>(Value[i]);
	}

private:
	array_type& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped array is null");

		return *m_wrapped;
	}

	array_type* const m_wrapped;
};

///////////////////////////////////////////////////////////////////////////////////////
// array

template<>
class array<typed_array<k3d::imaterial*> >
{
public:
	typedef typed_array<k3d::imaterial*> array_type;

	array() :
		m_wrapped(0)
	{
	}

	array(array_type& Array) :
		m_wrapped(&Array)
	{
	}

	int len()
	{
		return wrapped().size();
	}

	boost::python::object get_item(int item)
	{
		if(item < 0 || item >= wrapped().size())
			throw std::out_of_range("index out-of-range");

		k3d::imaterial* const result = wrapped().at(item);
		return result ? boost::python::object(imaterial_wrapper(result)) : boost::python::object();
	}

	void set_item(int item, const boost::python::object& value)
	{
		if(item < 0)
			throw std::out_of_range("index out-of-range");

		if(static_cast<size_t>(item) >= wrapped().size())
			wrapped().resize(item + 1);

		if(value)
		{
			imaterial_wrapper material = boost::python::extract<imaterial_wrapper >(value);
			wrapped()[item] = &material.wrapped();
		}
		else
		{
			wrapped()[item] = static_cast<k3d::imaterial*>(0);
		}
	}

	void append(const boost::python::object& Value)
	{
		if(Value)
		{
			imaterial_wrapper material = boost::python::extract<imaterial_wrapper >(Value);
			wrapped().push_back(&material.wrapped());
		}
		else
		{
			wrapped().push_back(static_cast<k3d::imaterial*>(0));
		}
	}

	void assign(const boost::python::list& Value)
	{
		array_type& storage = wrapped();

		const size_t count = boost::python::len(Value);
		storage.resize(count);
		for(size_t i = 0; i != count; ++i)
		{
			if(Value[i])
			{
				imaterial_wrapper material = boost::python::extract<imaterial_wrapper >(Value[i]);
				storage[i] = &material.wrapped();
			}
			else
			{
				storage[i] = static_cast<k3d::imaterial*>(0);
			}
		}
	}

private:
	array_type& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped array is null");

		return *m_wrapped;
	}

	array_type* const m_wrapped;
};

///////////////////////////////////////////////////////////////////////////////////////
// array

template<>
class array<typed_array<k3d::inode*> >
{
public:
	typedef typed_array<k3d::inode*> array_type;

	array() :
		m_wrapped(0)
	{
	}

	array(array_type& Array) :
		m_wrapped(&Array)
	{
	}

	int len()
	{
		return wrapped().size();
	}

	boost::python::object get_item(int item)
	{
		if(item < 0 || item >= wrapped().size())
			throw std::out_of_range("index out-of-range");

		return wrap(wrapped().at(item));
	}

	void set_item(int item, const boost::python::object& value)
	{
		if(item < 0)
			throw std::out_of_range("index out-of-range");

		if(static_cast<size_t>(item) >= wrapped().size())
			wrapped().resize(item + 1);

		if(value)
		{
			inode_wrapper node = boost::python::extract<inode_wrapper >(value);
			wrapped()[item] = &node.wrapped();
		}
		else
		{
			wrapped()[item] = static_cast<k3d::inode*>(0);
		}
	}

	void append(const boost::python::object& Value)
	{
		if(Value)
		{
			inode_wrapper node = boost::python::extract<inode_wrapper >(Value);
			wrapped().push_back(&node.wrapped());
		}
		else
		{
			wrapped().push_back(static_cast<k3d::inode*>(0));
		}
	}

	void assign(const boost::python::list& Value)
	{
		array_type& storage = wrapped();

		const size_t count = boost::python::len(Value);
		storage.resize(count);
		for(size_t i = 0; i != count; ++i)
		{
			if(Value[i])
			{
				inode_wrapper node = boost::python::extract<inode_wrapper >(Value[i]);
				storage[i] = &node.wrapped();
			}
			else
			{
				storage[i] = static_cast<k3d::inode*>(0);
			}
		}
	}

private:
	array_type& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped array is null");

		return *m_wrapped;
	}

	array_type* const m_wrapped;
};

////////////////////////////////////////////////////////////////////////////////////////
// export_array

template<typename array_type>
void export_array(const char* const ClassName, const char* const DocString)
{
	boost::python::class_<array<array_type> >(ClassName, DocString, boost::python::no_init)
		.def("__len__", &array<array_type>::len)
		.def("__getitem__", &array<array_type>::get_item)
		.def("__setitem__", &array<array_type>::set_item)
		.def("append", &array<array_type>::append,
			"Append a value to the end of the array, growing its size by one.")
		.def("assign", &array<array_type>::assign,
			"Replace the contents of the array with a list of values.");
}

void export_arrays();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_ARRAY_PYTHON_H

