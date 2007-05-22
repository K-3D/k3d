#ifndef K3DSDK_CONST_ARRAY_PYTHON_H
#define K3DSDK_CONST_ARRAY_PYTHON_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3dsdk/array.h>
#include <boost/python.hpp>

namespace k3d
{

namespace python
{

/////////////////////////////////////////////////////////////////////////////////////////
// const_array

template<typename array_type>
class const_array
{
public:
	typedef typename array_type::value_type value_type;

	const_array() :
		m_wrapped(0)
	{
	}

	const_array(array_type& Array) :
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

private:
	array_type& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped array is null");

		return *m_wrapped;
	}

	array_type* const m_wrapped;
};

/////////////////////////////////////////////////////////////////////////////////////////
// const_array

template<>
class const_array<const typed_array<k3d::imaterial*> >
{
public:
	typedef const typed_array<k3d::imaterial*> array_type;

	const_array() :
		m_wrapped(0)
	{
	}

	const_array(array_type& Array) :
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
		return result ? boost::python::object(k3d::python::imaterial(result)) : boost::python::object();
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

//////////////////////////////////////////////////////////////////////////////////////////////
// export_const_array

template<typename array_type>
void export_const_array(const char* const ClassName, const char* const DocString)
{
	boost::python::class_<const_array<array_type> >(ClassName, DocString)
		.def("__len__", &const_array<array_type>::len)
		.def("__getitem__", &const_array<array_type>::get_item);
}

void export_const_arrays();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_CONST_ARRAY_PYTHON_H

