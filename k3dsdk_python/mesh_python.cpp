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
#include "interface_wrapper_python.h"
#include "mesh_python.h"

#include <k3dsdk/color.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////
// wrap_const_object

template<typename return_type, typename pointer_type>
object wrap_const_object(const pointer_type& Pointer)
{
	if(!Pointer)
		return object();

	return object(return_type(*Pointer));
}

////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_object

template<typename return_type, typename pointer_type>
object wrap_non_const_object(pointer_type& Pointer)
{
	if(!Pointer)
		return object();

	return object(return_type(*k3d::make_unique(Pointer)));
}

////////////////////////////////////////////////////////////////////////////////////////
// create_object

template<typename return_type, typename new_type, typename pointer_type>
object create_object(pointer_type& Pointer)
{
	new_type* const new_object = new new_type();
	Pointer.reset(new_object);
	return object(return_type(*new_object));
}

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
		return wrapped().at(item);
	}

	void set_item(int item, const value_type& value)
	{
		array_type& storage = wrapped();

		if(static_cast<size_t>(item) >= storage.size())
			storage.resize(item + 1);

		storage[item] = value;
	}

	void append(const value_type& Value)
	{
		wrapped().push_back(Value);
	}

	void assign(const list& Value)
	{
		array_type& storage = wrapped();

		const size_t count = boost::python::len(Value);
		storage.resize(count);
		for(size_t i = 0; i != count; ++i)
			storage[i] = extract<typename array_type::value_type>(Value[i]);
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

	object get_item(int item)
	{
		k3d::imaterial* const result = wrapped().at(item);
		return result ? object(k3d::python::imaterial(result)) : object();
	}

	void set_item(int item, const object& value)
	{
		array_type& storage = wrapped();

		if(static_cast<size_t>(item) >= storage.size())
			storage.resize(item + 1);

		if(value)
		{
			k3d::python::imaterial material = extract<k3d::python::imaterial>(value);
			storage[item] = &material.wrapped();
		}
		else
		{
			storage[item] = static_cast<k3d::imaterial*>(0);
		}
	}

	void append(const object& Value)
	{
		if(Value)
		{
			k3d::python::imaterial material = extract<k3d::python::imaterial>(Value);
			wrapped().push_back(&material.wrapped());
		}
		else
		{
			wrapped().push_back(static_cast<k3d::imaterial*>(0));
		}
	}

	void assign(const list& Value)
	{
		array_type& storage = wrapped();

		const size_t count = boost::python::len(Value);
		storage.resize(count);
		for(size_t i = 0; i != count; ++i)
		{
			if(Value[i])
			{
				k3d::python::imaterial material = extract<k3d::python::imaterial>(Value[i]);
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

////////////////////////////////////////////////////////////////////////////////////////
// export_array

template<typename array_type>
void export_array(const char* const ClassName)
{
	class_<array<array_type> >(ClassName)
		.def("__len__", &array<array_type>::len)
		.def("__getitem__", &array<array_type>::get_item)
		.def("__setitem__", &array<array_type>::set_item)
		.def("append", &array<array_type>::append)
		.def("assign", &array<array_type>::assign);
}

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

	object get_item(int item)
	{
		k3d::imaterial* const result = wrapped().at(item);
		return result ? object(k3d::python::imaterial(result)) : object();
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
void export_const_array(const char* const ClassName)
{
	class_<const_array<array_type> >(ClassName)
		.def("__len__", &const_array<array_type>::len)
		.def("__getitem__", &const_array<array_type>::get_item);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_const_array

template<typename pointer_type>
object wrap_const_array(pointer_type& Pointer)
{
	if(!Pointer)
		return object();

	return object(const_array<typename pointer_type::element_type>(*Pointer));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_array

template<typename pointer_type>
object wrap_non_const_array(pointer_type& Pointer)
{
	typedef typename boost::remove_const<typename pointer_type::element_type>::type array_type;

	if(!Pointer)
		return object();

	return object(array<array_type>(*k3d::make_unique(Pointer)));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// create_array

template<typename pointer_type>
object create_array(pointer_type& Pointer)
{
	typedef typename boost::remove_const<typename pointer_type::element_type>::type array_type;

	array_type* const new_array = new array_type();
	Pointer.reset(new_array);
	return object(array<array_type>(*new_array));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// const_named_arrays

class const_named_arrays
{
public:
	const_named_arrays() :
		m_wrapped(0)
	{
	}

	const_named_arrays(const k3d::dev::mesh::named_arrays& NamedArrays) :
		m_wrapped(&NamedArrays)
	{
	}

	list array_names()
	{
		list results;

		for(k3d::dev::mesh::named_arrays::const_iterator array = wrapped().begin(); array != wrapped().end(); ++array)
			results.append(array->first);

		return results;
	}

	object array(const std::string& Name)
	{
		if(!wrapped().count(Name))
			throw std::runtime_error("Unknown array name: " + Name);

		return wrap_array(wrapped().find(Name)->second.get());
	}

	int len()
	{
		return wrapped().size();
	}

	object get_item(int item)
	{
		k3d::dev::mesh::named_arrays::const_iterator array_iterator = wrapped().begin();
		std::advance(array_iterator, item);

		return wrap_array(array_iterator->second.get());
	}

private:
	object wrap_array(const k3d::array* const Array)
	{
		return object();
	}

	const k3d::dev::mesh::named_arrays& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped object is null");

		return *m_wrapped;
	}

	const k3d::dev::mesh::named_arrays* const m_wrapped;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// export_const_named_arrays

void export_const_named_arrays()
{
	class_<const_named_arrays>("const_named_arrays")
		.add_property("array_names", &const_named_arrays::array_names)
		.def("array", &const_named_arrays::array)
		.def("__len__", &const_named_arrays::len)
		.def("__getitem__", &const_named_arrays::get_item);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// named_arrays

class named_arrays
{
public:
	named_arrays() :
		m_wrapped(0)
	{
	}

	named_arrays(k3d::dev::mesh::named_arrays& NamedArrays) :
		m_wrapped(&NamedArrays)
	{
	}

	list array_names()
	{
		list results;

		for(k3d::dev::mesh::named_arrays::const_iterator array = wrapped().begin(); array != wrapped().end(); ++array)
			results.append(array->first);

		return results;
	}

	object array(const std::string& Name)
	{
		if(!wrapped().count(Name))
			throw std::runtime_error("Unknown array name: " + Name);

		return wrap_array(wrapped().find(Name)->second.get());
	}

    template<typename array_type>
    object create_typed_array(const std::string& Name)
    {
        k3d::typed_array<array_type>* const new_array = new k3d::typed_array<array_type>();
        wrapped()[Name].reset(new_array);
        return object(k3d::python::detail::array<k3d::typed_array<array_type> >(*new_array));
    }

	object create_array(const std::string& Name, const std::string& Type)
	{
	    if(Name.empty())
            throw std::runtime_error("Empty array name");

        if(Type == k3d::type_string<double>())
            return create_typed_array<double>(Name);
        else if(Type == k3d::type_string<k3d::color>())
            return create_typed_array<k3d::color>(Name);
        else if(Type == k3d::type_string<k3d::point3>())
            return create_typed_array<k3d::point3>(Name);
        else if(Type == k3d::type_string<k3d::vector3>())
            return create_typed_array<k3d::vector3>(Name);
        else if(Type == k3d::type_string<k3d::normal3>())
            return create_typed_array<k3d::normal3>(Name);
        else if(Type == k3d::type_string<k3d::matrix4>())
            return create_typed_array<k3d::matrix4>(Name);
        else if(Type == k3d::type_string<k3d::point4>())
            return create_typed_array<k3d::point4>(Name);

        throw std::runtime_error("Unknown type[" + Type + "] for array [" + Name + "]");
	}

	int len()
	{
		return wrapped().size();
	}

	object get_item(int item)
	{
		k3d::dev::mesh::named_arrays::const_iterator array_iterator = wrapped().begin();
		std::advance(array_iterator, item);

		return wrap_array(array_iterator->second.get());
	}

private:
	object wrap_array(const k3d::array* const Array)
	{
		return object();
	}

	k3d::dev::mesh::named_arrays& wrapped()
	{
		if(!m_wrapped)
			throw std::runtime_error("wrapped object is null");

		return *m_wrapped;
	}

	k3d::dev::mesh::named_arrays* const m_wrapped;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// export_named_arrays

void export_named_arrays()
{
	class_<named_arrays>("named_arrays")
		.add_property("array_names", &named_arrays::array_names)
		.def("array", &named_arrays::array)
		.def("create_array", &named_arrays::create_array)
		.def("__len__", &named_arrays::len)
		.def("__getitem__", &named_arrays::get_item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// wrap_const_named_arrays

object wrap_const_named_arrays(const k3d::dev::mesh::named_arrays& NamedArrays)
{
	return object(const_named_arrays(NamedArrays));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_named_arrays

object wrap_non_const_named_arrays(k3d::dev::mesh::named_arrays& NamedArrays)
{
	return object(named_arrays(NamedArrays));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_point_groups

class const_point_groups :
	public interface_wrapper<const k3d::dev::mesh::point_groups_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::point_groups_t> base;
public:
	const_point_groups() :
		base()
	{
	}

	const_point_groups(const k3d::dev::mesh::point_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	const_point_groups(const k3d::dev::mesh::point_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_points() { return wrap_const_array(wrapped().first_points); }
	object point_counts() { return wrap_const_array(wrapped().point_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object points() { return wrap_const_array(wrapped().points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// point_groups

class point_groups :
	public interface_wrapper<k3d::dev::mesh::point_groups_t>
{
	typedef interface_wrapper<k3d::dev::mesh::point_groups_t> base;
public:
	point_groups() :
		base()
	{
	}

	point_groups(k3d::dev::mesh::point_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	point_groups(k3d::dev::mesh::point_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_points() { return wrap_const_array(wrapped().first_points); }
	object point_counts() { return wrap_const_array(wrapped().point_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object points() { return wrap_const_array(wrapped().points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }

	object writable_first_points() { return wrap_non_const_array(wrapped().first_points); }
	object writable_point_counts() { return wrap_non_const_array(wrapped().point_counts); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_points() { return wrap_non_const_array(wrapped().points); }
	object writable_varying_data() { return wrap_non_const_named_arrays(wrapped().varying_data); }

	object create_first_points() { return create_array(wrapped().first_points); }
	object create_point_counts() { return create_array(wrapped().point_counts); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_points() { return create_array(wrapped().points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_linear_curve_groups

class const_linear_curve_groups :
	public interface_wrapper<const k3d::dev::mesh::linear_curve_groups_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::linear_curve_groups_t> base;
public:
	const_linear_curve_groups() :
		base()
	{
	}

	const_linear_curve_groups(const k3d::dev::mesh::linear_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	const_linear_curve_groups(const k3d::dev::mesh::linear_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object periodic_curves() { return wrap_const_array(wrapped().periodic_curves); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// linear_curve_groups

class linear_curve_groups :
	public interface_wrapper<k3d::dev::mesh::linear_curve_groups_t>
{
	typedef interface_wrapper<k3d::dev::mesh::linear_curve_groups_t> base;
public:
	linear_curve_groups() :
		base()
	{
	}

	linear_curve_groups(k3d::dev::mesh::linear_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	linear_curve_groups(k3d::dev::mesh::linear_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object periodic_curves() { return wrap_const_array(wrapped().periodic_curves); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }

	object writable_first_curves() { return wrap_non_const_array(wrapped().first_curves); }
	object writable_curve_counts() { return wrap_non_const_array(wrapped().curve_counts); }
	object writable_periodic_curves() { return wrap_non_const_array(wrapped().periodic_curves); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_curve_first_points() { return wrap_non_const_array(wrapped().curve_first_points); }
	object writable_curve_point_counts() { return wrap_non_const_array(wrapped().curve_point_counts); }
	object writable_curve_selection() { return wrap_non_const_array(wrapped().curve_selection); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_curve_points() { return wrap_non_const_array(wrapped().curve_points); }

	object create_first_curves() { return create_array(wrapped().first_curves); }
	object create_curve_counts() { return create_array(wrapped().curve_counts); }
	object create_periodic_curves() { return create_array(wrapped().periodic_curves); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_curve_first_points() { return create_array(wrapped().curve_first_points); }
	object create_curve_point_counts() { return create_array(wrapped().curve_point_counts); }
	object create_curve_selection() { return create_array(wrapped().curve_selection); }
	object create_curve_points() { return create_array(wrapped().curve_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_cubic_curve_groups

class const_cubic_curve_groups :
	public interface_wrapper<const k3d::dev::mesh::cubic_curve_groups_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::cubic_curve_groups_t> base;
public:
	const_cubic_curve_groups() :
		base()
	{
	}

	const_cubic_curve_groups(const k3d::dev::mesh::cubic_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	const_cubic_curve_groups(const k3d::dev::mesh::cubic_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object periodic_curves() { return wrap_const_array(wrapped().periodic_curves); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// cubic_curve_groups

class cubic_curve_groups :
	public interface_wrapper<k3d::dev::mesh::cubic_curve_groups_t>
{
	typedef interface_wrapper<k3d::dev::mesh::cubic_curve_groups_t> base;
public:
	cubic_curve_groups() :
		base()
	{
	}

	cubic_curve_groups(k3d::dev::mesh::cubic_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	cubic_curve_groups(k3d::dev::mesh::cubic_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object periodic_curves() { return wrap_const_array(wrapped().periodic_curves); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }

	object writable_first_curves() { return wrap_non_const_array(wrapped().first_curves); }
	object writable_curve_counts() { return wrap_non_const_array(wrapped().curve_counts); }
	object writable_periodic_curves() { return wrap_non_const_array(wrapped().periodic_curves); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_curve_first_points() { return wrap_non_const_array(wrapped().curve_first_points); }
	object writable_curve_point_counts() { return wrap_non_const_array(wrapped().curve_point_counts); }
	object writable_curve_selection() { return wrap_non_const_array(wrapped().curve_selection); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_curve_points() { return wrap_non_const_array(wrapped().curve_points); }

	object create_first_curves() { return create_array(wrapped().first_curves); }
	object create_curve_counts() { return create_array(wrapped().curve_counts); }
	object create_periodic_curves() { return create_array(wrapped().periodic_curves); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_curve_first_points() { return create_array(wrapped().curve_first_points); }
	object create_curve_point_counts() { return create_array(wrapped().curve_point_counts); }
	object create_curve_selection() { return create_array(wrapped().curve_selection); }
	object create_curve_points() { return create_array(wrapped().curve_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_nurbs_curve_groups

class const_nurbs_curve_groups :
	public interface_wrapper<const k3d::dev::mesh::nurbs_curve_groups_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::nurbs_curve_groups_t> base;
public:
	const_nurbs_curve_groups() :
		base()
	{
	}

	const_nurbs_curve_groups(const k3d::dev::mesh::nurbs_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	const_nurbs_curve_groups(const k3d::dev::mesh::nurbs_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_orders() { return wrap_const_array(wrapped().curve_orders); }
	object curve_first_knots() { return wrap_const_array(wrapped().curve_first_knots); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
	object curve_point_weights() { return wrap_const_array(wrapped().curve_point_weights); }
	object curve_knots() { return wrap_const_array(wrapped().curve_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// nurbs_curve_groups

class nurbs_curve_groups :
	public interface_wrapper<k3d::dev::mesh::nurbs_curve_groups_t>
{
	typedef interface_wrapper<k3d::dev::mesh::nurbs_curve_groups_t> base;
public:
	nurbs_curve_groups() :
		base()
	{
	}

	nurbs_curve_groups(k3d::dev::mesh::nurbs_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	nurbs_curve_groups(k3d::dev::mesh::nurbs_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_orders() { return wrap_const_array(wrapped().curve_orders); }
	object curve_first_knots() { return wrap_const_array(wrapped().curve_first_knots); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
	object curve_point_weights() { return wrap_const_array(wrapped().curve_point_weights); }
	object curve_knots() { return wrap_const_array(wrapped().curve_knots); }

	object writable_first_curves() { return wrap_non_const_array(wrapped().first_curves); }
	object writable_curve_counts() { return wrap_non_const_array(wrapped().curve_counts); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_curve_first_points() { return wrap_non_const_array(wrapped().curve_first_points); }
	object writable_curve_point_counts() { return wrap_non_const_array(wrapped().curve_point_counts); }
	object writable_curve_orders() { return wrap_non_const_array(wrapped().curve_orders); }
	object writable_curve_first_knots() { return wrap_non_const_array(wrapped().curve_first_knots); }
	object writable_curve_selection() { return wrap_non_const_array(wrapped().curve_selection); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_curve_points() { return wrap_non_const_array(wrapped().curve_points); }
	object writable_curve_point_weights() { return wrap_non_const_array(wrapped().curve_point_weights); }
	object writable_curve_knots() { return wrap_non_const_array(wrapped().curve_knots); }

	object create_first_curves() { return create_array(wrapped().first_curves); }
	object create_curve_counts() { return create_array(wrapped().curve_counts); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_curve_first_points() { return create_array(wrapped().curve_first_points); }
	object create_curve_point_counts() { return create_array(wrapped().curve_point_counts); }
	object create_curve_orders() { return create_array(wrapped().curve_orders); }
	object create_curve_first_knots() { return create_array(wrapped().curve_first_knots); }
	object create_curve_selection() { return create_array(wrapped().curve_selection); }
	object create_curve_points() { return create_array(wrapped().curve_points); }
	object create_curve_point_weights() { return create_array(wrapped().curve_point_weights); }
	object create_curve_knots() { return create_array(wrapped().curve_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_bilinear_patches

class const_bilinear_patches :
	public interface_wrapper<const k3d::dev::mesh::bilinear_patches_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::bilinear_patches_t> base;
public:
	const_bilinear_patches() :
		base()
	{
	}

	const_bilinear_patches(const k3d::dev::mesh::bilinear_patches_t* Patches) :
		base(Patches)
	{
	}

	const_bilinear_patches(const k3d::dev::mesh::bilinear_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// bilinear_patches

class bilinear_patches :
	public interface_wrapper<k3d::dev::mesh::bilinear_patches_t>
{
	typedef interface_wrapper<k3d::dev::mesh::bilinear_patches_t> base;
public:
	bilinear_patches() :
		base()
	{
	}

	bilinear_patches(k3d::dev::mesh::bilinear_patches_t* Patches) :
		base(Patches)
	{
	}

	bilinear_patches(k3d::dev::mesh::bilinear_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }

	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_varying_data() { return wrap_non_const_named_arrays(wrapped().varying_data); }

	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_bicubic_patches

class const_bicubic_patches :
	public interface_wrapper<const k3d::dev::mesh::bicubic_patches_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::bicubic_patches_t> base;
public:
	const_bicubic_patches() :
		base()
	{
	}

	const_bicubic_patches(const k3d::dev::mesh::bicubic_patches_t* Patches) :
		base(Patches)
	{
	}

	const_bicubic_patches(const k3d::dev::mesh::bicubic_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// bicubic_patches

class bicubic_patches :
	public interface_wrapper<k3d::dev::mesh::bicubic_patches_t>
{
	typedef interface_wrapper<k3d::dev::mesh::bicubic_patches_t> base;
public:
	bicubic_patches() :
		base()
	{
	}

	bicubic_patches(k3d::dev::mesh::bicubic_patches_t* Patches) :
		base(Patches)
	{
	}

	bicubic_patches(k3d::dev::mesh::bicubic_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }

	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_varying_data() { return wrap_non_const_named_arrays(wrapped().varying_data); }

	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_nurbs_patches

class const_nurbs_patches :
	public interface_wrapper<const k3d::dev::mesh::nurbs_patches_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::nurbs_patches_t> base;
public:
	const_nurbs_patches() :
		base()
	{
	}

	const_nurbs_patches(const k3d::dev::mesh::nurbs_patches_t* Patches) :
		base(Patches)
	{
	}

	const_nurbs_patches(const k3d::dev::mesh::nurbs_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_first_points() { return wrap_const_array(wrapped().patch_first_points); }
	object patch_u_point_counts() { return wrap_const_array(wrapped().patch_u_point_counts); }
	object patch_v_point_counts() { return wrap_const_array(wrapped().patch_v_point_counts); }
	object patch_u_orders() { return wrap_const_array(wrapped().patch_u_orders); }
	object patch_v_orders() { return wrap_const_array(wrapped().patch_v_orders); }
	object patch_u_first_knots() { return wrap_const_array(wrapped().patch_u_first_knots); }
	object patch_v_first_knots() { return wrap_const_array(wrapped().patch_v_first_knots); }
	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object patch_point_weights() { return wrap_const_array(wrapped().patch_point_weights); }
	object patch_u_knots() { return wrap_const_array(wrapped().patch_u_knots); }
	object patch_v_knots() { return wrap_const_array(wrapped().patch_v_knots); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// nurbs_patches

class nurbs_patches :
	public interface_wrapper<k3d::dev::mesh::nurbs_patches_t>
{
	typedef interface_wrapper<k3d::dev::mesh::nurbs_patches_t> base;
public:
	nurbs_patches() :
		base()
	{
	}

	nurbs_patches(k3d::dev::mesh::nurbs_patches_t* Patches) :
		base(Patches)
	{
	}

	nurbs_patches(k3d::dev::mesh::nurbs_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_first_points() { return wrap_const_array(wrapped().patch_first_points); }
	object patch_u_point_counts() { return wrap_const_array(wrapped().patch_u_point_counts); }
	object patch_v_point_counts() { return wrap_const_array(wrapped().patch_v_point_counts); }
	object patch_u_orders() { return wrap_const_array(wrapped().patch_u_orders); }
	object patch_v_orders() { return wrap_const_array(wrapped().patch_v_orders); }
	object patch_u_first_knots() { return wrap_const_array(wrapped().patch_u_first_knots); }
	object patch_v_first_knots() { return wrap_const_array(wrapped().patch_v_first_knots); }
	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object patch_point_weights() { return wrap_const_array(wrapped().patch_point_weights); }
	object patch_u_knots() { return wrap_const_array(wrapped().patch_u_knots); }
	object patch_v_knots() { return wrap_const_array(wrapped().patch_v_knots); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }

	object writable_patch_first_points() { return wrap_non_const_array(wrapped().patch_first_points); }
	object writable_patch_u_point_counts() { return wrap_non_const_array(wrapped().patch_u_point_counts); }
	object writable_patch_v_point_counts() { return wrap_non_const_array(wrapped().patch_v_point_counts); }
	object writable_patch_u_orders() { return wrap_non_const_array(wrapped().patch_u_orders); }
	object writable_patch_v_orders() { return wrap_non_const_array(wrapped().patch_v_orders); }
	object writable_patch_u_first_knots() { return wrap_non_const_array(wrapped().patch_u_first_knots); }
	object writable_patch_v_first_knots() { return wrap_non_const_array(wrapped().patch_v_first_knots); }
	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_patch_point_weights() { return wrap_non_const_array(wrapped().patch_point_weights); }
	object writable_patch_u_knots() { return wrap_non_const_array(wrapped().patch_u_knots); }
	object writable_patch_v_knots() { return wrap_non_const_array(wrapped().patch_v_knots); }
	object writable_varying_data() { return wrap_non_const_named_arrays(wrapped().varying_data); }

	object create_patch_first_points() { return create_array(wrapped().patch_first_points); }
	object create_patch_u_point_counts() { return create_array(wrapped().patch_u_point_counts); }
	object create_patch_v_point_counts() { return create_array(wrapped().patch_v_point_counts); }
	object create_patch_u_orders() { return create_array(wrapped().patch_u_orders); }
	object create_patch_v_orders() { return create_array(wrapped().patch_v_orders); }
	object create_patch_u_first_knots() { return create_array(wrapped().patch_u_first_knots); }
	object create_patch_v_first_knots() { return create_array(wrapped().patch_v_first_knots); }
	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
	object create_patch_point_weights() { return create_array(wrapped().patch_point_weights); }
	object create_patch_u_knots() { return create_array(wrapped().patch_u_knots); }
	object create_patch_v_knots() { return create_array(wrapped().patch_v_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_polyhedra

class const_polyhedra :
	public interface_wrapper<const k3d::dev::mesh::polyhedra_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::polyhedra_t> base;
public:
	const_polyhedra() :
		base()
	{
	}

	const_polyhedra(const k3d::dev::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	const_polyhedra(const k3d::dev::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap_const_named_arrays(wrapped().face_varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// polyhedra

class polyhedra :
	public interface_wrapper<k3d::dev::mesh::polyhedra_t>
{
	typedef interface_wrapper<k3d::dev::mesh::polyhedra_t> base;
public:
	polyhedra() :
		base()
	{
	}

	polyhedra(k3d::dev::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	polyhedra(k3d::dev::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap_const_named_arrays(wrapped().face_varying_data); }

	object writable_first_faces() { return wrap_non_const_array(wrapped().first_faces); }
	object writable_face_counts() { return wrap_non_const_array(wrapped().face_counts); }
	object writable_types() { return wrap_non_const_array(wrapped().types); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_face_first_loops() { return wrap_non_const_array(wrapped().face_first_loops); }
	object writable_face_loop_counts() { return wrap_non_const_array(wrapped().face_loop_counts); }
	object writable_face_selection() { return wrap_non_const_array(wrapped().face_selection); }
	object writable_face_materials() { return wrap_non_const_array(wrapped().face_materials); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_loop_first_edges() { return wrap_non_const_array(wrapped().loop_first_edges); }
	object writable_edge_points() { return wrap_non_const_array(wrapped().edge_points); }
	object writable_clockwise_edges() { return wrap_non_const_array(wrapped().clockwise_edges); }
	object writable_edge_selection() { return wrap_non_const_array(wrapped().edge_selection); }
	object writable_face_varying_data() { return wrap_non_const_named_arrays(wrapped().face_varying_data); }

	object create_first_faces() { return create_array(wrapped().first_faces); }
	object create_face_counts() { return create_array(wrapped().face_counts); }
	object create_types() { return create_array(wrapped().types); }
	object create_face_first_loops() { return create_array(wrapped().face_first_loops); }
	object create_face_loop_counts() { return create_array(wrapped().face_loop_counts); }
	object create_face_selection() { return create_array(wrapped().face_selection); }
	object create_face_materials() { return create_array(wrapped().face_materials); }
	object create_loop_first_edges() { return create_array(wrapped().loop_first_edges); }
	object create_edge_points() { return create_array(wrapped().edge_points); }
	object create_clockwise_edges() { return create_array(wrapped().clockwise_edges); }
	object create_edge_selection() { return create_array(wrapped().edge_selection); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_blobbies

class const_blobbies :
	public interface_wrapper<const k3d::dev::mesh::blobbies_t>
{
	typedef interface_wrapper<const k3d::dev::mesh::blobbies_t> base;
public:
	const_blobbies() :
		base()
	{
	}

	const_blobbies(const k3d::dev::mesh::blobbies_t* Blobbies) :
		base(Blobbies)
	{
	}

	const_blobbies(const k3d::dev::mesh::blobbies_t& Blobbies) :
		base(Blobbies)
	{
	}

	object first_primitives() { return wrap_const_array(wrapped().first_primitives); }
	object primitive_counts() { return wrap_const_array(wrapped().primitive_counts); }
	object first_operators() { return wrap_const_array(wrapped().first_operators); }
	object operator_counts() { return wrap_const_array(wrapped().operator_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object primitives() { return wrap_const_array(wrapped().primitives); }
	object primitive_first_floats() { return wrap_const_array(wrapped().primitive_first_floats); }
	object primitive_float_counts() { return wrap_const_array(wrapped().primitive_float_counts); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
	object vertex_data() { return wrap_const_named_arrays(wrapped().vertex_data); }
	object operators() { return wrap_const_array(wrapped().operators); }
	object operator_first_operands() { return wrap_const_array(wrapped().operator_first_operands); }
	object operator_operand_counts() { return wrap_const_array(wrapped().operator_operand_counts); }
	object floats() { return wrap_const_array(wrapped().floats); }
	object operands() { return wrap_const_array(wrapped().operands); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//blobbies 

class blobbies :
	public interface_wrapper<k3d::dev::mesh::blobbies_t>
{
	typedef interface_wrapper<k3d::dev::mesh::blobbies_t> base;
public:
	blobbies() :
		base()
	{
	}

	blobbies(k3d::dev::mesh::blobbies_t* Blobbies) :
		base(Blobbies)
	{
	}

	blobbies(k3d::dev::mesh::blobbies_t& Blobbies) :
		base(Blobbies)
	{
	}

	object first_primitives() { return wrap_const_array(wrapped().first_primitives); }
	object primitive_counts() { return wrap_const_array(wrapped().primitive_counts); }
	object first_operators() { return wrap_const_array(wrapped().first_operators); }
	object operator_counts() { return wrap_const_array(wrapped().operator_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap_const_named_arrays(wrapped().constant_data); }
	object uniform_data() { return wrap_const_named_arrays(wrapped().uniform_data); }
	object primitives() { return wrap_const_array(wrapped().primitives); }
	object primitive_first_floats() { return wrap_const_array(wrapped().primitive_first_floats); }
	object primitive_float_counts() { return wrap_const_array(wrapped().primitive_float_counts); }
	object varying_data() { return wrap_const_named_arrays(wrapped().varying_data); }
	object vertex_data() { return wrap_const_named_arrays(wrapped().vertex_data); }
	object operators() { return wrap_const_array(wrapped().operators); }
	object operator_first_operands() { return wrap_const_array(wrapped().operator_first_operands); }
	object operator_operand_counts() { return wrap_const_array(wrapped().operator_operand_counts); }
	object floats() { return wrap_const_array(wrapped().floats); }
	object operands() { return wrap_const_array(wrapped().operands); }

	object writable_first_primitives() { return wrap_non_const_array(wrapped().first_primitives); }
	object writable_primitive_counts() { return wrap_non_const_array(wrapped().primitive_counts); }
	object writable_first_operators() { return wrap_non_const_array(wrapped().first_operators); }
	object writable_operator_counts() { return wrap_non_const_array(wrapped().operator_counts); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap_non_const_named_arrays(wrapped().constant_data); }
	object writable_uniform_data() { return wrap_non_const_named_arrays(wrapped().uniform_data); }
	object writable_primitives() { return wrap_non_const_array(wrapped().primitives); }
	object writable_primitive_first_floats() { return wrap_non_const_array(wrapped().primitive_first_floats); }
	object writable_primitive_float_counts() { return wrap_non_const_array(wrapped().primitive_float_counts); }
	object writable_varying_data() { return wrap_non_const_named_arrays(wrapped().varying_data); }
	object writable_vertex_data() { return wrap_non_const_named_arrays(wrapped().vertex_data); }
	object writable_operators() { return wrap_non_const_array(wrapped().operators); }
	object writable_operator_first_operands() { return wrap_non_const_array(wrapped().operator_first_operands); }
	object writable_operator_operand_counts() { return wrap_non_const_array(wrapped().operator_operand_counts); }
	object writable_floats() { return wrap_non_const_array(wrapped().floats); }
	object writable_operands() { return wrap_non_const_array(wrapped().operands); }

	object create_first_primitives() { return create_array(wrapped().first_primitives); }
	object create_primitive_counts() { return create_array(wrapped().primitive_counts); }
	object create_first_operators() { return create_array(wrapped().first_operators); }
	object create_operator_counts() { return create_array(wrapped().operator_counts); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_primitives() { return create_array(wrapped().primitives); }
	object create_primitive_first_floats() { return create_array(wrapped().primitive_first_floats); }
	object create_primitive_float_counts() { return create_array(wrapped().primitive_float_counts); }
	object create_operators() { return create_array(wrapped().operators); }
	object create_operator_first_operands() { return create_array(wrapped().operator_first_operands); }
	object create_operator_operand_counts() { return create_array(wrapped().operator_operand_counts); }
	object create_floats() { return create_array(wrapped().floats); }
	object create_operands() { return create_array(wrapped().operands); }
};

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh() :
	base()
{
}

mesh::mesh(k3d::dev::mesh* Mesh) :
	base(Mesh)
{
}

void mesh::copy(const mesh& RHS)
{
	wrapped() = RHS.wrapped();
}

object mesh::bicubic_patches() { return detail::wrap_const_object<detail::const_bicubic_patches>(wrapped().bicubic_patches); } 
object mesh::bilinear_patches() { return detail::wrap_const_object<detail::const_bilinear_patches>(wrapped().bilinear_patches); } 
object mesh::blobbies() { return detail::wrap_const_object<detail::const_blobbies>(wrapped().blobbies); }
object mesh::create_bicubic_patches() { return detail::create_object<detail::bicubic_patches, k3d::dev::mesh::bicubic_patches_t>(wrapped().bicubic_patches); }
object mesh::create_bilinear_patches() { return detail::create_object<detail::bilinear_patches, k3d::dev::mesh::bilinear_patches_t>(wrapped().bilinear_patches); }
object mesh::create_blobbies() { return detail::create_object<detail::blobbies, k3d::dev::mesh::blobbies_t>(wrapped().blobbies); }
object mesh::create_cubic_curve_groups() { return detail::create_object<detail::cubic_curve_groups, k3d::dev::mesh::cubic_curve_groups_t>(wrapped().cubic_curve_groups); }
object mesh::create_linear_curve_groups() { return detail::create_object<detail::linear_curve_groups, k3d::dev::mesh::linear_curve_groups_t>(wrapped().linear_curve_groups); }
object mesh::create_nurbs_curve_groups() { return detail::create_object<detail::nurbs_curve_groups, k3d::dev::mesh::nurbs_curve_groups_t>(wrapped().nurbs_curve_groups); }
object mesh::create_nurbs_patches() { return detail::create_object<detail::nurbs_patches, k3d::dev::mesh::nurbs_patches_t>(wrapped().nurbs_patches); }
object mesh::create_point_groups() { return detail::create_object<detail::point_groups, k3d::dev::mesh::point_groups_t>(wrapped().point_groups); } 
object mesh::create_point_selection() { return detail::create_array(wrapped().point_selection); } 
object mesh::create_points() { return detail::create_array(wrapped().points); }
object mesh::create_polyhedra() { return detail::create_object<detail::polyhedra, k3d::dev::mesh::polyhedra_t>(wrapped().polyhedra); } 
object mesh::cubic_curve_groups() { return detail::wrap_const_object<detail::const_cubic_curve_groups>(wrapped().cubic_curve_groups); }
object mesh::linear_curve_groups() { return detail::wrap_const_object<detail::const_linear_curve_groups>(wrapped().linear_curve_groups); } 
object mesh::nurbs_curve_groups() { return detail::wrap_const_object<detail::const_nurbs_curve_groups>(wrapped().nurbs_curve_groups); } 
object mesh::nurbs_patches() { return detail::wrap_const_object<detail::const_nurbs_patches>(wrapped().nurbs_patches); } 
object mesh::point_groups() { return detail::wrap_const_object<detail::const_point_groups>(wrapped().point_groups); } 
object mesh::point_selection() { return detail::wrap_const_array(wrapped().point_selection); } 
object mesh::points() { return detail::wrap_const_array(wrapped().points); } 
object mesh::polyhedra() { return detail::wrap_const_object<detail::const_polyhedra>(wrapped().polyhedra); } 
object mesh::vertex_data() { return detail::wrap_const_named_arrays(wrapped().vertex_data); } 
object mesh::writable_bicubic_patches() { return detail::wrap_non_const_object<detail::bicubic_patches>(wrapped().bicubic_patches); } 
object mesh::writable_bilinear_patches() { return detail::wrap_non_const_object<detail::bilinear_patches>(wrapped().bilinear_patches); } 
object mesh::writable_blobbies() { return detail::wrap_non_const_object<detail::blobbies>(wrapped().blobbies); }
object mesh::writable_cubic_curve_groups() { return detail::wrap_non_const_object<detail::cubic_curve_groups>(wrapped().cubic_curve_groups); } 
object mesh::writable_linear_curve_groups() { return detail::wrap_non_const_object<detail::linear_curve_groups>(wrapped().linear_curve_groups); } 
object mesh::writable_nurbs_curve_groups() { return detail::wrap_non_const_object<detail::nurbs_curve_groups>(wrapped().nurbs_curve_groups); } 
object mesh::writable_nurbs_patches() { return detail::wrap_non_const_object<detail::nurbs_patches>(wrapped().nurbs_patches); } 
object mesh::writable_point_groups() { return detail::wrap_non_const_object<detail::point_groups>(wrapped().point_groups); } 
object mesh::writable_point_selection() { return detail::wrap_non_const_array(wrapped().point_selection); } 
object mesh::writable_points() { return detail::wrap_non_const_array(wrapped().points); } 
object mesh::writable_polyhedra() { return detail::wrap_non_const_object<detail::polyhedra>(wrapped().polyhedra); } 
object mesh::writable_vertex_data() { return detail::wrap_non_const_named_arrays(wrapped().vertex_data); } 

const std::string mesh::repr()
{
	std::ostringstream buffer;
	buffer << wrapped();
	return buffer.str();
}

const std::string mesh::str()
{
	std::ostringstream buffer;
	buffer << "<k3d.mesh object wrapping mesh " << &wrapped() << ">";
	return buffer.str();
}

const bool operator==(const mesh& LHS, const mesh& RHS)
{
	return LHS.wrapped() == RHS.wrapped();
}

const bool operator!=(const mesh& LHS, const mesh& RHS)
{
	return LHS.wrapped() != RHS.wrapped();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// export_mesh

void export_mesh()
{
	class_<mesh>("mesh")
		.add_property("bicubic_patches", &mesh::bicubic_patches)
		.add_property("bilinear_patches", &mesh::bilinear_patches)
		.add_property("blobbies", &mesh::blobbies)
		.add_property("cubic_curve_groups", &mesh::cubic_curve_groups)
		.add_property("linear_curve_groups", &mesh::linear_curve_groups)
		.add_property("nurbs_curve_groups", &mesh::nurbs_curve_groups)
		.add_property("nurbs_patches", &mesh::nurbs_patches)
		.add_property("vertex_data", &mesh::vertex_data)
		.add_property("point_groups", &mesh::point_groups)
		.add_property("point_selection", &mesh::point_selection)
		.add_property("points", &mesh::points)
		.add_property("polyhedra", &mesh::polyhedra)
		.add_property("writable_bicubic_patches", &mesh::writable_bicubic_patches)
		.add_property("writable_bilinear_patches", &mesh::writable_bilinear_patches)
		.add_property("writable_blobbies", &mesh::writable_blobbies)
		.add_property("writable_cubic_curve_groups", &mesh::writable_cubic_curve_groups)
		.add_property("writable_linear_curve_groups", &mesh::writable_linear_curve_groups)
		.add_property("writable_nurbs_curve_groups", &mesh::writable_nurbs_curve_groups)
		.add_property("writable_nurbs_patches", &mesh::writable_nurbs_patches)
		.add_property("writable_vertex_data", &mesh::writable_vertex_data)
		.add_property("writable_point_groups", &mesh::writable_point_groups)
		.add_property("writable_point_selection", &mesh::writable_point_selection)
		.add_property("writable_points", &mesh::writable_points)
		.add_property("writable_polyhedra", &mesh::writable_polyhedra)
		.def("copy", &mesh::copy)
		.def("create_bicubic_patches", &mesh::create_bicubic_patches)
		.def("create_bilinear_patches", &mesh::create_bilinear_patches)
		.def("create_blobbies", &mesh::create_blobbies)
		.def("create_cubic_curve_groups", &mesh::create_cubic_curve_groups)
		.def("create_linear_curve_groups", &mesh::create_linear_curve_groups)
		.def("create_nurbs_curve_groups", &mesh::create_nurbs_curve_groups)
		.def("create_nurbs_patches", &mesh::create_nurbs_patches)
		.def("create_point_groups", &mesh::create_point_groups)
		.def("create_point_selection", &mesh::create_point_selection)
		.def("create_points", &mesh::create_points)
		.def("create_polyhedra", &mesh::create_polyhedra)
		.def(self != self)
		.def(self == self)
		.def("__repr__", &mesh::repr)
		.def("__str__", &mesh::str);

	class_<detail::const_point_groups>("const_point_groups")
		.add_property("first_points", &detail::const_point_groups::first_points)
		.add_property("point_counts", &detail::const_point_groups::point_counts)
		.add_property("materials", &detail::const_point_groups::materials)
		.add_property("constant_data", &detail::const_point_groups::constant_data)
		.add_property("points", &detail::const_point_groups::points)
		.add_property("varying_data", &detail::const_point_groups::varying_data);

	class_<detail::point_groups>("point_groups")
		.add_property("first_points", &detail::point_groups::first_points)
		.add_property("point_counts", &detail::point_groups::point_counts)
		.add_property("materials", &detail::point_groups::materials)
		.add_property("constant_data", &detail::point_groups::constant_data)
		.add_property("points", &detail::point_groups::points)
		.add_property("varying_data", &detail::point_groups::varying_data)

		.add_property("writable_first_points", &detail::point_groups::writable_first_points)
		.add_property("writable_point_counts", &detail::point_groups::writable_point_counts)
		.add_property("writable_materials", &detail::point_groups::writable_materials)
		.add_property("writable_constant_data", &detail::point_groups::writable_constant_data)
		.add_property("writable_points", &detail::point_groups::writable_points)
		.add_property("writable_varying_data", &detail::point_groups::writable_varying_data)

		.def("create_first_points", &detail::point_groups::create_first_points)
		.def("create_point_counts", &detail::point_groups::create_point_counts)
		.def("create_materials", &detail::point_groups::create_materials)
		.def("create_points", &detail::point_groups::create_points);

	class_<detail::const_linear_curve_groups>("const_linear_curve_groups")
		.add_property("first_curves", &detail::const_linear_curve_groups::first_curves)
		.add_property("curve_counts", &detail::const_linear_curve_groups::curve_counts)
		.add_property("periodic_curves", &detail::const_linear_curve_groups::periodic_curves)
		.add_property("materials", &detail::const_linear_curve_groups::materials)
		.add_property("constant_data", &detail::const_linear_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::const_linear_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::const_linear_curve_groups::curve_point_counts)
		.add_property("curve_selection", &detail::const_linear_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::const_linear_curve_groups::uniform_data)
		.add_property("curve_points", &detail::const_linear_curve_groups::curve_points);

	class_<detail::linear_curve_groups>("linear_curve_groups")
		.add_property("first_curves", &detail::linear_curve_groups::first_curves)
		.add_property("curve_counts", &detail::linear_curve_groups::curve_counts)
		.add_property("periodic_curves", &detail::linear_curve_groups::periodic_curves)
		.add_property("materials", &detail::linear_curve_groups::materials)
		.add_property("constant_data", &detail::linear_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::linear_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::linear_curve_groups::curve_point_counts)
		.add_property("curve_selection", &detail::linear_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::linear_curve_groups::uniform_data)
		.add_property("curve_points", &detail::linear_curve_groups::curve_points)

		.add_property("writable_first_curves", &detail::linear_curve_groups::writable_first_curves)
		.add_property("writable_curve_counts", &detail::linear_curve_groups::writable_curve_counts)
		.add_property("writable_periodic_curves", &detail::linear_curve_groups::writable_periodic_curves)
		.add_property("writable_materials", &detail::linear_curve_groups::writable_materials)
		.add_property("writable_constant_data", &detail::linear_curve_groups::writable_constant_data)
		.add_property("writable_curve_first_points", &detail::linear_curve_groups::writable_curve_first_points)
		.add_property("writable_curve_point_counts", &detail::linear_curve_groups::writable_curve_point_counts)
		.add_property("writable_curve_selection", &detail::linear_curve_groups::writable_curve_selection)
		.add_property("writable_uniform_data", &detail::linear_curve_groups::writable_uniform_data)
		.add_property("writable_curve_points", &detail::linear_curve_groups::writable_curve_points)

		.def("create_first_curves", &detail::linear_curve_groups::create_first_curves)
		.def("create_curve_counts", &detail::linear_curve_groups::create_curve_counts)
		.def("create_periodic_curves", &detail::linear_curve_groups::create_periodic_curves)
		.def("create_materials", &detail::linear_curve_groups::create_materials)
		.def("create_curve_first_points", &detail::linear_curve_groups::create_curve_first_points)
		.def("create_curve_point_counts", &detail::linear_curve_groups::create_curve_point_counts)
		.def("create_curve_selection", &detail::linear_curve_groups::create_curve_selection)
		.def("create_curve_points", &detail::linear_curve_groups::create_curve_points);

	class_<detail::const_cubic_curve_groups>("const_cubic_curve_groups")
		.add_property("first_curves", &detail::const_cubic_curve_groups::first_curves)
		.add_property("curve_counts", &detail::const_cubic_curve_groups::curve_counts)
		.add_property("periodic_curves", &detail::const_cubic_curve_groups::periodic_curves)
		.add_property("materials", &detail::const_cubic_curve_groups::materials)
		.add_property("constant_data", &detail::const_cubic_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::const_cubic_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::const_cubic_curve_groups::curve_point_counts)
		.add_property("curve_selection", &detail::const_cubic_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::const_cubic_curve_groups::uniform_data)
		.add_property("curve_points", &detail::const_cubic_curve_groups::curve_points);

	class_<detail::cubic_curve_groups>("cubic_curve_groups")
		.add_property("first_curves", &detail::cubic_curve_groups::first_curves)
		.add_property("curve_counts", &detail::cubic_curve_groups::curve_counts)
		.add_property("periodic_curves", &detail::cubic_curve_groups::periodic_curves)
		.add_property("materials", &detail::cubic_curve_groups::materials)
		.add_property("constant_data", &detail::cubic_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::cubic_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::cubic_curve_groups::curve_point_counts)
		.add_property("curve_selection", &detail::cubic_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::cubic_curve_groups::uniform_data)
		.add_property("curve_points", &detail::cubic_curve_groups::curve_points)

		.add_property("writable_first_curves", &detail::cubic_curve_groups::writable_first_curves)
		.add_property("writable_curve_counts", &detail::cubic_curve_groups::writable_curve_counts)
		.add_property("writable_periodic_curves", &detail::cubic_curve_groups::writable_periodic_curves)
		.add_property("writable_materials", &detail::cubic_curve_groups::writable_materials)
		.add_property("writable_constant_data", &detail::cubic_curve_groups::writable_constant_data)
		.add_property("writable_curve_first_points", &detail::cubic_curve_groups::writable_curve_first_points)
		.add_property("writable_curve_point_counts", &detail::cubic_curve_groups::writable_curve_point_counts)
		.add_property("writable_curve_selection", &detail::cubic_curve_groups::writable_curve_selection)
		.add_property("writable_uniform_data", &detail::cubic_curve_groups::writable_uniform_data)
		.add_property("writable_curve_points", &detail::cubic_curve_groups::writable_curve_points)

		.def("create_first_curves", &detail::cubic_curve_groups::create_first_curves)
		.def("create_curve_counts", &detail::cubic_curve_groups::create_curve_counts)
		.def("create_periodic_curves", &detail::cubic_curve_groups::create_periodic_curves)
		.def("create_materials", &detail::cubic_curve_groups::create_materials)
		.def("create_curve_first_points", &detail::cubic_curve_groups::create_curve_first_points)
		.def("create_curve_point_counts", &detail::cubic_curve_groups::create_curve_point_counts)
		.def("create_curve_selection", &detail::cubic_curve_groups::create_curve_selection)
		.def("create_curve_points", &detail::cubic_curve_groups::create_curve_points);

	class_<detail::const_nurbs_curve_groups>("const_nurbs_curve_groups")
		.add_property("first_curves", &detail::const_nurbs_curve_groups::first_curves)
		.add_property("curve_counts", &detail::const_nurbs_curve_groups::curve_counts)
		.add_property("materials", &detail::const_nurbs_curve_groups::materials)
		.add_property("constant_data", &detail::const_nurbs_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::const_nurbs_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::const_nurbs_curve_groups::curve_point_counts)
		.add_property("curve_orders", &detail::const_nurbs_curve_groups::curve_orders)
		.add_property("curve_first_knots", &detail::const_nurbs_curve_groups::curve_first_knots)
		.add_property("curve_selection", &detail::const_nurbs_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::const_nurbs_curve_groups::uniform_data)
		.add_property("curve_points", &detail::const_nurbs_curve_groups::curve_points)
		.add_property("curve_point_weights", &detail::const_nurbs_curve_groups::curve_point_weights)
		.add_property("curve_knots", &detail::const_nurbs_curve_groups::curve_knots);

	class_<detail::nurbs_curve_groups>("nurbs_curve_groups")
		.add_property("first_curves", &detail::nurbs_curve_groups::first_curves)
		.add_property("curve_counts", &detail::nurbs_curve_groups::curve_counts)
		.add_property("materials", &detail::nurbs_curve_groups::materials)
		.add_property("constant_data", &detail::nurbs_curve_groups::constant_data)
		.add_property("curve_first_points", &detail::nurbs_curve_groups::curve_first_points)
		.add_property("curve_point_counts", &detail::nurbs_curve_groups::curve_point_counts)
		.add_property("curve_orders", &detail::nurbs_curve_groups::curve_orders)
		.add_property("curve_first_knots", &detail::nurbs_curve_groups::curve_first_knots)
		.add_property("curve_selection", &detail::nurbs_curve_groups::curve_selection)
		.add_property("uniform_data", &detail::nurbs_curve_groups::uniform_data)
		.add_property("curve_points", &detail::nurbs_curve_groups::curve_points)
		.add_property("curve_point_weights", &detail::nurbs_curve_groups::curve_point_weights)
		.add_property("curve_knots", &detail::nurbs_curve_groups::curve_knots)

		.add_property("writable_first_curves", &detail::nurbs_curve_groups::writable_first_curves)
		.add_property("writable_curve_counts", &detail::nurbs_curve_groups::writable_curve_counts)
		.add_property("writable_materials", &detail::nurbs_curve_groups::writable_materials)
		.add_property("writable_constant_data", &detail::nurbs_curve_groups::writable_constant_data)
		.add_property("writable_curve_first_points", &detail::nurbs_curve_groups::writable_curve_first_points)
		.add_property("writable_curve_point_counts", &detail::nurbs_curve_groups::writable_curve_point_counts)
		.add_property("writable_curve_orders", &detail::nurbs_curve_groups::writable_curve_orders)
		.add_property("writable_curve_first_knots", &detail::nurbs_curve_groups::writable_curve_first_knots)
		.add_property("writable_curve_selection", &detail::nurbs_curve_groups::writable_curve_selection)
		.add_property("writable_uniform_data", &detail::nurbs_curve_groups::writable_uniform_data)
		.add_property("writable_curve_points", &detail::nurbs_curve_groups::writable_curve_points)
		.add_property("writable_curve_point_weights", &detail::nurbs_curve_groups::writable_curve_point_weights)
		.add_property("writable_curve_knots", &detail::nurbs_curve_groups::writable_curve_knots)

		.def("create_first_curves", &detail::nurbs_curve_groups::create_first_curves)
		.def("create_curve_counts", &detail::nurbs_curve_groups::create_curve_counts)
		.def("create_materials", &detail::nurbs_curve_groups::create_materials)
		.def("create_curve_first_points", &detail::nurbs_curve_groups::create_curve_first_points)
		.def("create_curve_point_counts", &detail::nurbs_curve_groups::create_curve_point_counts)
		.def("create_curve_orders", &detail::nurbs_curve_groups::create_curve_orders)
		.def("create_curve_first_knots", &detail::nurbs_curve_groups::create_curve_first_knots)
		.def("create_curve_selection", &detail::nurbs_curve_groups::create_curve_selection)
		.def("create_curve_points", &detail::nurbs_curve_groups::create_curve_points)
		.def("create_curve_point_weights", &detail::nurbs_curve_groups::create_curve_point_weights)
		.def("create_curve_knots", &detail::nurbs_curve_groups::create_curve_knots);

	class_<detail::const_bilinear_patches>("const_bilinear_patches")
		.add_property("patch_selection", &detail::const_bilinear_patches::patch_selection)
		.add_property("patch_materials", &detail::const_bilinear_patches::patch_materials)
		.add_property("constant_data", &detail::const_bilinear_patches::constant_data)
		.add_property("uniform_data", &detail::const_bilinear_patches::uniform_data)
		.add_property("patch_points", &detail::const_bilinear_patches::patch_points)
		.add_property("varying_data", &detail::const_bilinear_patches::varying_data);

	class_<detail::bilinear_patches>("bilinear_patches")
		.add_property("patch_selection", &detail::bilinear_patches::patch_selection)
		.add_property("patch_materials", &detail::bilinear_patches::patch_materials)
		.add_property("constant_data", &detail::bilinear_patches::constant_data)
		.add_property("uniform_data", &detail::bilinear_patches::uniform_data)
		.add_property("patch_points", &detail::bilinear_patches::patch_points)
		.add_property("varying_data", &detail::bilinear_patches::varying_data)

		.add_property("writable_patch_selection", &detail::bilinear_patches::writable_patch_selection)
		.add_property("writable_patch_materials", &detail::bilinear_patches::writable_patch_materials)
		.add_property("writable_constant_data", &detail::bilinear_patches::writable_constant_data)
		.add_property("writable_uniform_data", &detail::bilinear_patches::writable_uniform_data)
		.add_property("writable_patch_points", &detail::bilinear_patches::writable_patch_points)
		.add_property("writable_varying_data", &detail::bilinear_patches::writable_varying_data)

		.def("create_patch_selection", &detail::bilinear_patches::create_patch_selection)
		.def("create_patch_materials", &detail::bilinear_patches::create_patch_materials)
		.def("create_patch_points", &detail::bilinear_patches::create_patch_points);

	class_<detail::const_bicubic_patches>("const_bicubic_patches")
		.add_property("patch_selection", &detail::const_bicubic_patches::patch_selection)
		.add_property("patch_materials", &detail::const_bicubic_patches::patch_materials)
		.add_property("constant_data", &detail::const_bicubic_patches::constant_data)
		.add_property("uniform_data", &detail::const_bicubic_patches::uniform_data)
		.add_property("patch_points", &detail::const_bicubic_patches::patch_points)
		.add_property("varying_data", &detail::const_bicubic_patches::varying_data);

	class_<detail::bicubic_patches>("bicubic_patches")
		.add_property("patch_selection", &detail::bicubic_patches::patch_selection)
		.add_property("patch_materials", &detail::bicubic_patches::patch_materials)
		.add_property("constant_data", &detail::bicubic_patches::constant_data)
		.add_property("uniform_data", &detail::bicubic_patches::uniform_data)
		.add_property("patch_points", &detail::bicubic_patches::patch_points)
		.add_property("varying_data", &detail::bicubic_patches::varying_data)

		.add_property("writable_patch_selection", &detail::bicubic_patches::writable_patch_selection)
		.add_property("writable_patch_materials", &detail::bicubic_patches::writable_patch_materials)
		.add_property("writable_constant_data", &detail::bicubic_patches::writable_constant_data)
		.add_property("writable_uniform_data", &detail::bicubic_patches::writable_uniform_data)
		.add_property("writable_patch_points", &detail::bicubic_patches::writable_patch_points)
		.add_property("writable_varying_data", &detail::bicubic_patches::writable_varying_data)

		.def("create_patch_selection", &detail::bicubic_patches::create_patch_selection)
		.def("create_patch_materials", &detail::bicubic_patches::create_patch_materials)
		.def("create_patch_points", &detail::bicubic_patches::create_patch_points);

	class_<detail::const_nurbs_patches>("const_nurbs_patches")
		.add_property("patch_first_points", &detail::const_nurbs_patches::patch_first_points)
		.add_property("patch_u_point_counts", &detail::const_nurbs_patches::patch_u_point_counts)
		.add_property("patch_v_point_counts", &detail::const_nurbs_patches::patch_v_point_counts)
		.add_property("patch_u_orders", &detail::const_nurbs_patches::patch_u_orders)
		.add_property("patch_v_orders", &detail::const_nurbs_patches::patch_v_orders)
		.add_property("patch_u_first_knots", &detail::const_nurbs_patches::patch_u_first_knots)
		.add_property("patch_v_first_knots", &detail::const_nurbs_patches::patch_v_first_knots)
		.add_property("patch_selection", &detail::const_nurbs_patches::patch_selection)
		.add_property("patch_materials", &detail::const_nurbs_patches::patch_materials)
		.add_property("constant_data", &detail::const_nurbs_patches::constant_data)
		.add_property("uniform_data", &detail::const_nurbs_patches::uniform_data)
		.add_property("patch_points", &detail::const_nurbs_patches::patch_points)
		.add_property("patch_point_weights", &detail::const_nurbs_patches::patch_point_weights)
		.add_property("patch_u_knots", &detail::const_nurbs_patches::patch_u_knots)
		.add_property("patch_v_knots", &detail::const_nurbs_patches::patch_v_knots)
		.add_property("varying_data", &detail::const_nurbs_patches::varying_data);

	class_<detail::nurbs_patches>("nurbs_patches")
		.add_property("patch_first_points", &detail::nurbs_patches::patch_first_points)
		.add_property("patch_u_point_counts", &detail::nurbs_patches::patch_u_point_counts)
		.add_property("patch_v_point_counts", &detail::nurbs_patches::patch_v_point_counts)
		.add_property("patch_u_orders", &detail::nurbs_patches::patch_u_orders)
		.add_property("patch_v_orders", &detail::nurbs_patches::patch_v_orders)
		.add_property("patch_u_first_knots", &detail::nurbs_patches::patch_u_first_knots)
		.add_property("patch_v_first_knots", &detail::nurbs_patches::patch_v_first_knots)
		.add_property("patch_selection", &detail::nurbs_patches::patch_selection)
		.add_property("patch_materials", &detail::nurbs_patches::patch_materials)
		.add_property("constant_data", &detail::nurbs_patches::constant_data)
		.add_property("uniform_data", &detail::nurbs_patches::uniform_data)
		.add_property("patch_points", &detail::nurbs_patches::patch_points)
		.add_property("patch_point_weights", &detail::nurbs_patches::patch_point_weights)
		.add_property("patch_u_knots", &detail::nurbs_patches::patch_u_knots)
		.add_property("patch_v_knots", &detail::nurbs_patches::patch_v_knots)
		.add_property("varying_data", &detail::nurbs_patches::varying_data)

		.add_property("writable_patch_first_points", &detail::nurbs_patches::writable_patch_first_points)
		.add_property("writable_patch_u_point_counts", &detail::nurbs_patches::writable_patch_u_point_counts)
		.add_property("writable_patch_v_point_counts", &detail::nurbs_patches::writable_patch_v_point_counts)
		.add_property("writable_patch_u_orders", &detail::nurbs_patches::writable_patch_u_orders)
		.add_property("writable_patch_v_orders", &detail::nurbs_patches::writable_patch_v_orders)
		.add_property("writable_patch_u_first_knots", &detail::nurbs_patches::writable_patch_u_first_knots)
		.add_property("writable_patch_v_first_knots", &detail::nurbs_patches::writable_patch_v_first_knots)
		.add_property("writable_patch_selection", &detail::nurbs_patches::writable_patch_selection)
		.add_property("writable_patch_materials", &detail::nurbs_patches::writable_patch_materials)
		.add_property("writable_constant_data", &detail::nurbs_patches::writable_constant_data)
		.add_property("writable_uniform_data", &detail::nurbs_patches::writable_uniform_data)
		.add_property("writable_patch_points", &detail::nurbs_patches::writable_patch_points)
		.add_property("writable_patch_point_weights", &detail::nurbs_patches::writable_patch_point_weights)
		.add_property("writable_patch_u_knots", &detail::nurbs_patches::writable_patch_u_knots)
		.add_property("writable_patch_v_knots", &detail::nurbs_patches::writable_patch_v_knots)
		.add_property("writable_varying_data", &detail::nurbs_patches::writable_varying_data)

		.def("create_patch_first_points", &detail::nurbs_patches::create_patch_first_points)
		.def("create_patch_u_point_counts", &detail::nurbs_patches::create_patch_u_point_counts)
		.def("create_patch_v_point_counts", &detail::nurbs_patches::create_patch_v_point_counts)
		.def("create_patch_u_orders", &detail::nurbs_patches::create_patch_u_orders)
		.def("create_patch_v_orders", &detail::nurbs_patches::create_patch_v_orders)
		.def("create_patch_u_first_knots", &detail::nurbs_patches::create_patch_u_first_knots)
		.def("create_patch_v_first_knots", &detail::nurbs_patches::create_patch_v_first_knots)
		.def("create_patch_selection", &detail::nurbs_patches::create_patch_selection)
		.def("create_patch_materials", &detail::nurbs_patches::create_patch_materials)
		.def("create_patch_points", &detail::nurbs_patches::create_patch_points)
		.def("create_patch_point_weights", &detail::nurbs_patches::create_patch_point_weights)
		.def("create_patch_u_knots", &detail::nurbs_patches::create_patch_u_knots)
		.def("create_patch_v_knots", &detail::nurbs_patches::create_patch_v_knots);

	class_<detail::const_polyhedra>("const_polyhedra")
		.add_property("first_faces", &detail::const_polyhedra::first_faces)
		.add_property("face_counts", &detail::const_polyhedra::face_counts)
		.add_property("types", &detail::const_polyhedra::types)
		.add_property("constant_data", &detail::const_polyhedra::constant_data)
		.add_property("face_first_loops", &detail::const_polyhedra::face_first_loops)
		.add_property("face_loop_counts", &detail::const_polyhedra::face_loop_counts)
		.add_property("face_selection", &detail::const_polyhedra::face_selection)
		.add_property("face_materials", &detail::const_polyhedra::face_materials)
		.add_property("uniform_data", &detail::const_polyhedra::uniform_data)
		.add_property("loop_first_edges", &detail::const_polyhedra::loop_first_edges)
		.add_property("edge_points", &detail::const_polyhedra::edge_points)
		.add_property("clockwise_edges", &detail::const_polyhedra::clockwise_edges)
		.add_property("edge_selection", &detail::const_polyhedra::edge_selection)
		.add_property("face_varying_data", &detail::const_polyhedra::face_varying_data);

	class_<detail::polyhedra>("polyhedra")
		.add_property("first_faces", &detail::polyhedra::first_faces)
		.add_property("face_counts", &detail::polyhedra::face_counts)
		.add_property("types", &detail::polyhedra::types)
		.add_property("constant_data", &detail::polyhedra::constant_data)
		.add_property("face_first_loops", &detail::polyhedra::face_first_loops)
		.add_property("face_loop_counts", &detail::polyhedra::face_loop_counts)
		.add_property("face_selection", &detail::polyhedra::face_selection)
		.add_property("face_materials", &detail::polyhedra::face_materials)
		.add_property("uniform_data", &detail::polyhedra::uniform_data)
		.add_property("loop_first_edges", &detail::polyhedra::loop_first_edges)
		.add_property("edge_points", &detail::polyhedra::edge_points)
		.add_property("clockwise_edges", &detail::polyhedra::clockwise_edges)
		.add_property("edge_selection", &detail::polyhedra::edge_selection)
		.add_property("face_varying_data", &detail::polyhedra::face_varying_data)

		.add_property("writable_first_faces", &detail::polyhedra::writable_first_faces)
		.add_property("writable_face_counts", &detail::polyhedra::writable_face_counts)
		.add_property("writable_types", &detail::polyhedra::writable_types)
		.add_property("writable_constant_data", &detail::polyhedra::writable_constant_data)
		.add_property("writable_face_first_loops", &detail::polyhedra::writable_face_first_loops)
		.add_property("writable_face_loop_counts", &detail::polyhedra::writable_face_loop_counts)
		.add_property("writable_face_selection", &detail::polyhedra::writable_face_selection)
		.add_property("writable_face_materials", &detail::polyhedra::writable_face_materials)
		.add_property("writable_uniform_data", &detail::polyhedra::writable_uniform_data)
		.add_property("writable_loop_first_edges", &detail::polyhedra::writable_loop_first_edges)
		.add_property("writable_edge_points", &detail::polyhedra::writable_edge_points)
		.add_property("writable_clockwise_edges", &detail::polyhedra::writable_clockwise_edges)
		.add_property("writable_edge_selection", &detail::polyhedra::writable_edge_selection)
		.add_property("writable_face_varying_data", &detail::polyhedra::writable_face_varying_data)

		.def("create_first_faces", &detail::polyhedra::create_first_faces)
		.def("create_face_counts", &detail::polyhedra::create_face_counts)
		.def("create_types", &detail::polyhedra::create_types)
		.def("create_face_first_loops", &detail::polyhedra::create_face_first_loops)
		.def("create_face_loop_counts", &detail::polyhedra::create_face_loop_counts)
		.def("create_face_selection", &detail::polyhedra::create_face_selection)
		.def("create_face_materials", &detail::polyhedra::create_face_materials)
		.def("create_loop_first_edges", &detail::polyhedra::create_loop_first_edges)
		.def("create_edge_points", &detail::polyhedra::create_edge_points)
		.def("create_clockwise_edges", &detail::polyhedra::create_clockwise_edges)
		.def("create_edge_selection", &detail::polyhedra::create_edge_selection);

	class_<detail::const_blobbies>("const_blobbies")
		.add_property("first_primitives", &detail::const_blobbies::first_primitives)
		.add_property("primitive_counts", &detail::const_blobbies::primitive_counts)
		.add_property("first_operators", &detail::const_blobbies::first_operators)
		.add_property("operator_counts", &detail::const_blobbies::operator_counts)
		.add_property("materials", &detail::const_blobbies::materials)
		.add_property("constant_data", &detail::const_blobbies::constant_data)
		.add_property("uniform_data", &detail::const_blobbies::uniform_data)
		.add_property("primitives", &detail::const_blobbies::primitives)
		.add_property("primitive_first_floats", &detail::const_blobbies::primitive_first_floats)
		.add_property("primitive_float_counts", &detail::const_blobbies::primitive_float_counts)
		.add_property("varying_data", &detail::const_blobbies::varying_data)
		.add_property("vertex_data", &detail::const_blobbies::vertex_data)
		.add_property("operators", &detail::const_blobbies::operators)
		.add_property("operator_first_operands", &detail::const_blobbies::operator_first_operands)
		.add_property("operator_operand_counts", &detail::const_blobbies::operator_operand_counts)
		.add_property("operands", &detail::const_blobbies::operands)
		.add_property("floats", &detail::const_blobbies::floats);

	class_<detail::blobbies>("blobbies")
		.add_property("first_primitives", &detail::blobbies::first_primitives)
		.add_property("primitive_counts", &detail::blobbies::primitive_counts)
		.add_property("first_operators", &detail::blobbies::first_operators)
		.add_property("operator_counts", &detail::blobbies::operator_counts)
		.add_property("materials", &detail::blobbies::materials)
		.add_property("constant_data", &detail::blobbies::constant_data)
		.add_property("uniform_data", &detail::blobbies::uniform_data)
		.add_property("primitives", &detail::blobbies::primitives)
		.add_property("primitive_first_floats", &detail::blobbies::primitive_first_floats)
		.add_property("primitive_float_counts", &detail::blobbies::primitive_float_counts)
		.add_property("varying_data", &detail::blobbies::varying_data)
		.add_property("vertex_data", &detail::blobbies::vertex_data)
		.add_property("operators", &detail::blobbies::operators)
		.add_property("operator_first_operands", &detail::blobbies::operator_first_operands)
		.add_property("operator_operand_counts", &detail::blobbies::operator_operand_counts)
		.add_property("operands", &detail::blobbies::operands)
		.add_property("floats", &detail::blobbies::floats)

		.add_property("writable_first_primitives", &detail::blobbies::writable_first_primitives)
		.add_property("writable_primitive_counts", &detail::blobbies::writable_primitive_counts)
		.add_property("writable_first_operators", &detail::blobbies::writable_first_operators)
		.add_property("writable_operator_counts", &detail::blobbies::writable_operator_counts)
		.add_property("writable_materials", &detail::blobbies::writable_materials)
		.add_property("writable_constant_data", &detail::blobbies::writable_constant_data)
		.add_property("writable_uniform_data", &detail::blobbies::writable_uniform_data)
		.add_property("writable_primitives", &detail::blobbies::writable_primitives)
		.add_property("writable_primitive_first_floats", &detail::blobbies::writable_primitive_first_floats)
		.add_property("writable_primitive_float_counts", &detail::blobbies::writable_primitive_float_counts)
		.add_property("writable_varying_data", &detail::blobbies::writable_varying_data)
		.add_property("writable_vertex_data", &detail::blobbies::writable_vertex_data)
		.add_property("writable_operators", &detail::blobbies::writable_operators)
		.add_property("writable_operator_first_operands", &detail::blobbies::writable_operator_first_operands)
		.add_property("writable_operator_operand_counts", &detail::blobbies::writable_operator_operand_counts)
		.add_property("writable_operands", &detail::blobbies::writable_operands)
		.add_property("writable_floats", &detail::blobbies::writable_floats)

		.def("create_first_primitives", &detail::blobbies::create_first_primitives)
		.def("create_primitive_counts", &detail::blobbies::create_primitive_counts)
		.def("create_first_operators", &detail::blobbies::create_first_operators)
		.def("create_operator_counts", &detail::blobbies::create_operator_counts)
		.def("create_materials", &detail::blobbies::create_materials)
		.def("create_primitives", &detail::blobbies::create_primitives)
		.def("create_primitive_first_floats", &detail::blobbies::create_primitive_first_floats)
		.def("create_primitive_float_counts", &detail::blobbies::create_primitive_float_counts)
		.def("create_operators", &detail::blobbies::create_operators)
		.def("create_operator_first_operands", &detail::blobbies::create_operator_first_operands)
		.def("create_operator_operand_counts", &detail::blobbies::create_operator_operand_counts)
		.def("create_operands", &detail::blobbies::create_operands)
		.def("create_floats", &detail::blobbies::create_floats);

	enum_<k3d::dev::mesh::polyhedra_t::polyhedron_type>("polyhedron_type")
		.value("polygons", k3d::dev::mesh::polyhedra_t::POLYGONS)
		.value("catmull_clark", k3d::dev::mesh::polyhedra_t::CATMULL_CLARK)
		.attr("__module__") = "k3d";

	enum_<k3d::dev::mesh::blobbies_t::primitive_type>("primitive_type")
		.value("constant", k3d::dev::mesh::blobbies_t::CONSTANT)
		.value("ellipsoid", k3d::dev::mesh::blobbies_t::ELLIPSOID)
		.value("segment", k3d::dev::mesh::blobbies_t::SEGMENT)
		.attr("__module__") = "k3d";

	enum_<k3d::dev::mesh::blobbies_t::operator_type>("operator_type")
		.value("add", k3d::dev::mesh::blobbies_t::ADD)
		.value("multiply", k3d::dev::mesh::blobbies_t::MULTIPLY)
		.value("maximum", k3d::dev::mesh::blobbies_t::MAXIMUM)
		.value("minimum", k3d::dev::mesh::blobbies_t::MINIMUM)
		.value("divide", k3d::dev::mesh::blobbies_t::DIVIDE)
		.value("subtract", k3d::dev::mesh::blobbies_t::SUBTRACT)
		.value("negate", k3d::dev::mesh::blobbies_t::NEGATE)
		.value("identity", k3d::dev::mesh::blobbies_t::IDENTITY)
		.attr("__module__") = "k3d";

	detail::export_const_array<const k3d::typed_array<bool> >("const_bool_array");
	detail::export_const_array<const k3d::typed_array<double> >("const_double_array");
	detail::export_const_array<const k3d::typed_array<k3d::dev::mesh::polyhedra_t::polyhedron_type> >("const_polyhedron_type_array");
	detail::export_const_array<const k3d::typed_array<k3d::dev::mesh::blobbies_t::operator_type> >("const_blobby_operator_type_array");
	detail::export_const_array<const k3d::typed_array<k3d::dev::mesh::blobbies_t::primitive_type> >("const_blobby_primitive_type_array");
	detail::export_const_array<const k3d::typed_array<k3d::imaterial*> >("const_material_array");
	detail::export_const_array<const k3d::typed_array<k3d::color> >("const_color_array");
	detail::export_const_array<const k3d::typed_array<k3d::matrix4> >("const_matrix4_array");
	detail::export_const_array<const k3d::typed_array<k3d::normal3> >("const_normal3_array");
	detail::export_const_array<const k3d::typed_array<k3d::point3> >("const_point3_array");
	detail::export_const_array<const k3d::typed_array<k3d::point4> >("const_point4_array");
	detail::export_const_array<const k3d::typed_array<size_t> >("const_size_t_array");
	detail::export_const_array<const k3d::typed_array<k3d::vector3> >("const_vector3_array");

	detail::export_array<k3d::typed_array<bool> >("bool_array");
	detail::export_array<k3d::typed_array<double> >("double_array");
	detail::export_array<k3d::typed_array<k3d::dev::mesh::polyhedra_t::polyhedron_type> >("polyhedron_type_array");
	detail::export_array<k3d::typed_array<k3d::dev::mesh::blobbies_t::operator_type> >("blobby_operator_type_array");
	detail::export_array<k3d::typed_array<k3d::dev::mesh::blobbies_t::primitive_type> >("blobby_primitive_type_array");
	detail::export_array<k3d::typed_array<k3d::imaterial*> >("material_array");
	detail::export_array<k3d::typed_array<k3d::matrix4> >("matrix_array");
	detail::export_array<k3d::typed_array<k3d::color> >("color_array");
	detail::export_array<k3d::typed_array<k3d::normal3> >("normal3_array");
	detail::export_array<k3d::typed_array<k3d::point3> >("point3_array");
	detail::export_array<k3d::typed_array<k3d::point4> >("point4_array");
	detail::export_array<k3d::typed_array<size_t> >("size_t_array");
	detail::export_array<k3d::typed_array<k3d::vector3> >("vector3_array");

	detail::export_const_named_arrays();
	detail::export_named_arrays();
}

} // namespace python

} // namespace k3d

