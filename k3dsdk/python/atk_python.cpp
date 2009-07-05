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

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

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

/// Wrap an AtkObject, adding additional dynamic methods for any actions it may have
boost::python::object wrap(AtkObject* Object)
{
	boost::python::object wrapped = k3d::python::wrap(Object);
	define_action_methods(Object, wrapped);
	return wrapped;
}

}

static object get_item_name(atk_object_wrapper& Self, const string_t& Key)
{
	AtkObject* atk_obj = Self.wrapped_ptr();
	for(k3d::uint_t i = 0; i != atk_object_get_n_accessible_children(atk_obj); ++i)
	{
		AtkObject* child = atk_object_ref_accessible_child(atk_obj, i);
		const gchar* name_ptr = atk_object_get_name(child);
		const k3d::string_t name_str(name_ptr ? name_ptr : "");
		if(name_str == Key)
		{
			return detail::wrap(child);
		}
	}

	throw std::runtime_error("unknown key: " + Key);
}

static object get_item_idx(atk_object_wrapper& Self, const k3d::uint_t Key)
{
	AtkObject* atk_obj = Self.wrapped_ptr();
	if(Key >= atk_object_get_n_accessible_children(atk_obj))
		throw std::runtime_error("index out of range: " + Key);
	return detail::wrap(atk_object_ref_accessible_child(atk_obj, Key));
}

static int len(atk_object_wrapper& Self)
{
	return atk_object_get_n_accessible_children(Self.wrapped_ptr());
}

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
			.def("name", &name, "Get the name of the ATK object")
			.def("__len__", &len)
			.def("__getitem__", &get_item_name)
			.def("__getitem__", &get_item_idx);
}

} // namespace python

} // namespace k3d
