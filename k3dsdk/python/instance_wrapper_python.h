#ifndef K3DSDK_PYTHON_INSTANCE_WRAPPER_PYTHON_H
#define K3DSDK_PYTHON_INSTANCE_WRAPPER_PYTHON_H

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

#include <k3dsdk/iunknown.h>

#include <boost/python/object.hpp>
#include <stdexcept>

namespace k3d
{

namespace python
{

////////////////////////////////////////////////////////////////////////////////////
// instance_wrapper

template<typename T>
class instance_wrapper
{
public:
	typedef T wrapped_type;

	instance_wrapper() :
		m_wrapped(0)
	{
	}

	instance_wrapper(T* Wrapped) :
		m_wrapped(Wrapped)
	{
	}

	instance_wrapper(T& Wrapped) :
		m_wrapped(&Wrapped)
	{
	}

	friend const bool operator==(const instance_wrapper& lhs, const instance_wrapper& rhs)
	{
		return lhs.m_wrapped == rhs.m_wrapped;
	}

	T& wrapped() const
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped interface is null");

		return *m_wrapped;
	}

	T* wrapped_ptr() const
	{
		return m_wrapped;
	}
	
private:
	T* m_wrapped;
};

template<typename T>
boost::python::object wrap(T* Wrapped)
{
	return Wrapped ? boost::python::object(instance_wrapper<T>(*Wrapped)) : boost::python::object();
}

template<typename T>
boost::python::object wrap(T& Wrapped)
{
	return boost::python::object(instance_wrapper<T>(Wrapped));
}

boost::python::object wrap(k3d::iunknown* Wrapped);
boost::python::object wrap(k3d::iunknown& Wrapped);

} // namespace python

} // namespace k3d

#endif // !K3DSDK_PYTHON_INSTANCE_WRAPPER_PYTHON_H

