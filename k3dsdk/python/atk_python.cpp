// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include "atk_python.h"
#include "utility_python.h"

#include <k3dsdk/result.h>

#include <boost/algorithm/string.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/python.hpp>

#include <k3dsdk/type_registry.h> // for demangle

using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

boost::python::object wrap(AtkObject* Object);

/// Converts the given string to a name suitable as Python function name
/**
 * TODO: Move this to the SDK, so the conversion is always consistent?
 */
const k3d::string_t script_name(const k3d::string_t& Name)
{
	return boost::replace_all_copy(Name, " ", "_");
}

const k3d::int32_t action_index(AtkAction* Action, const k3d::string_t& ActionName)
{
	return_val_if_fail(Action, -1);
	for(k3d::uint_t i = 0; i != atk_action_get_n_actions(Action); ++i)
	{
		if(std::string(atk_action_get_name(Action, i)) == ActionName)
			return i;
	}
	return -1;
}

/// Performs the action with the given name. Requires Self to be an AtkAction and the action must exist (caller needs to check this)
k3d::bool_t do_named_action(atk_object_wrapper& Self, const k3d::string_t& ActionName)
{
	AtkObject* atk_object = Self.wrapped_ptr();
	return_val_if_fail(ATK_IS_ACTION(atk_object), false);
	AtkAction* action = ATK_ACTION(atk_object);
	k3d::int32_t action_idx = action_index(action, ActionName);
	return_val_if_fail(action_idx > -1, false);
	return atk_action_do_action(action, action_idx);
}

/// Proxy for click action
static void click(atk_object_wrapper& Self)
{
	do_named_action(Self, "click");
}

k3d::bool_t has_action(AtkObject* Object, const k3d::string_t& ActionName)
{
	if(!ATK_IS_ACTION(Object))
		return false;
	return action_index(ATK_ACTION(Object), ActionName) > -1;
}

void define_action_methods(AtkObject* Object, boost::python::object& Instance)
{
	if(ATK_IS_ACTION(Object))
	{
		if(has_action(Object, "click"))
		{
			utility::add_method(utility::make_function(&click, ""), "click", Instance);
		}
	}
}

/// Get the item with role Role and name Key
static object get_item_name(atk_object_wrapper& Self, const string_t& Key, const AtkRole Role)
{
	AtkObject* atk_obj = Self.wrapped_ptr();
	AtkObject* found = 0;
	for(k3d::uint_t i = 0; i != atk_object_get_n_accessible_children(atk_obj); ++i)
	{
		AtkObject* child = atk_object_ref_accessible_child(atk_obj, i);
		const gchar* name_ptr = atk_object_get_name(child);
		const k3d::string_t name_str(name_ptr ? name_ptr : "");
		if(name_str == Key && atk_object_get_role(child) == Role)
		{
			if(found)
				throw std::runtime_error("Duplicate value for key: " + Key);
			found = child;
		}
	}

	if(!found)
		throw std::runtime_error("unknown key: " + Key);

	return wrap(found);
}

/// Get the item with index Key and role Role, with indices counted within children of the same type
static object get_item_idx(atk_object_wrapper& Self, const k3d::uint_t Key, const AtkRole Role)
{
	AtkObject* atk_obj = Self.wrapped_ptr();
	const k3d::uint_t child_count = atk_object_get_n_accessible_children(atk_obj);
	if(Key >= child_count)
		throw std::runtime_error("index out of range: " + Key);
	k3d::uint_t idx = 0;
	for(k3d::uint_t i = 0; i != child_count; ++i)
	{
		AtkObject* child = atk_object_ref_accessible_child(atk_obj, i);
		if(atk_object_get_role(child) == Role)
		{
			if(idx == Key)
				return wrap(child);
			++idx;
		}
	}
	std::stringstream error;
	error << "No child with role " << atk_role_get_name(Role) << " at index " << Key;
	throw std::runtime_error(error.str());
}

template<typename Role>
static object get_item(atk_object_wrapper& Self, object Key)
{
	if(PyString_Check(Key.ptr()))
		return get_item_name(Self, boost::python::extract<string_t>(Key), static_cast<AtkRole>(Role::value));
	else if(PyInt_Check(Key.ptr()))
		return get_item_idx(Self, boost::python::extract<int32_t>(Key), static_cast<AtkRole>(Role::value));
	else
		throw std::runtime_error("Item lookup key must be a string or an integer");
}

class role_method_creator
{
public:
	role_method_creator(boost::python::object& Instance) : m_instance(Instance)
	{
	}

	template<typename T> void operator()(T RoleIdx)
	{
		const string_t role_name(atk_role_get_name(static_cast<AtkRole>(RoleIdx.value)));
		const string_t function_name(script_name(role_name));
		const string_t doc1 = "Get a " + role_name + " by name.";
		const string_t doc2 = "Get a " + role_name + " by index.";
		utility::add_method(utility::make_function(&get_item<T>, doc1.c_str()), function_name, m_instance);
	}
private:
	boost::python::object& m_instance;
};

/// Wrap an AtkObject, adding additional dynamic methods for any actions it may have
boost::python::object wrap(AtkObject* Object)
{
	boost::python::object wrapped = k3d::python::wrap(Object);

	role_method_creator creator(wrapped);
	boost::mpl::for_each<boost::mpl::range_c<int32_t,0,ATK_ROLE_LAST_DEFINED> >(creator);

	define_action_methods(Object, wrapped);
	return wrapped;
}

} // namespace detail

static k3d::string_t name(atk_object_wrapper& Self)
{
	const gchar* the_name = atk_object_get_name(Self.wrapped_ptr());
	if(!the_name)
		return "";
	return k3d::string_t(the_name);
}

class atk
{
public:
	static object root()
	{
		return detail::wrap(atk_get_root());
	}
};

void define_class_atk_object()
{

	scope outer = class_<atk>("atk", no_init)
		.def("root", &atk::root, "Returns the root ATK object for the K-3D application")
		.staticmethod("root");

	class_<atk_object_wrapper>("object",
		"Wraps an ATK object", no_init)
		.def("name", &name, "Get the name of the ATK object");
}

} // namespace python

} // namespace k3d
