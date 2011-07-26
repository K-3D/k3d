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

#include <boost/python.hpp>

#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/python/any_python.h>
#include <k3dsdk/scripting.h>


namespace k3d
{

namespace python
{

class script
{
public:
	static void execute_with_context(const string_t& Script, const boost::python::dict& PythonContext)
	{
		k3d::iscript_engine::context context;

		boost::python::dict python_context = PythonContext;
		while(len(python_context))
		{
			boost::python::tuple python_item = python_context.popitem();
			boost::python::object python_key = python_item[0];
			boost::python::object python_value = python_item[1];

			const string_t key = PyString_AsString(python_key.ptr());
			boost::any value = python_to_any(python_value);

			context.insert(std::make_pair(key, value));
		}

		bool recognized = false;
		bool executed = false;
		k3d::script::execute(k3d::script::code(Script), "Python Text", context, recognized, executed);
		if(!recognized)
			throw std::invalid_argument("Unrecognized scripting language");
		if(!executed)
			throw std::runtime_error("Error executing script");
	}

	static void execute(const string_t& Script)
	{
		execute_with_context(Script, boost::python::dict());
	}

};

static int context_len(const k3d::iscript_engine::context& Self)
{
	return Self.size();
}

static boost::python::object context_get_item(const k3d::iscript_engine::context& Self, const k3d::string_t& Key)
{
	k3d::iscript_engine::context::const_iterator item = Self.find(Key);
	if(item == Self.end())
		throw std::out_of_range("unknown key");

	return any_to_python(item->second);
}

static void context_set_item(k3d::iscript_engine::context& Self, const k3d::string_t& Key, const boost::python::object& Value)
{
	Self[Key] = python_to_any(Value);
}

static void context_del_item(k3d::iscript_engine::context& Self, const k3d::string_t& Key)
{
	k3d::iscript_engine::context::iterator item = Self.find(Key);
	if(item == Self.end())
		throw std::out_of_range("unknown key");

	Self.erase(item);
}

static bool_t context_has_key(const k3d::iscript_engine::context& Self, const k3d::string_t& Key)
{
	return Self.count(Key);
}

static boost::python::list context_keys(const k3d::iscript_engine::context& Self)
{
	boost::python::list results;
	for(k3d::iscript_engine::context::const_iterator pair = Self.begin(); pair != Self.end(); ++pair)
		results.append(pair->first);
	return results;
}

static boost::python::list context_values(const k3d::iscript_engine::context& Self)
{
	boost::python::list results;
	for(k3d::iscript_engine::context::const_iterator pair = Self.begin(); pair != Self.end(); ++pair)
		results.append(any_to_python(pair->second));
	return results;
}

void define_namespace_script()
{
	boost::python::scope outer = boost::python::class_<script>("script", boost::python::no_init)
		.def("execute", script::execute,
			"Executes a script (which does not have to be written in Python).")
		.def("execute", script::execute_with_context,
			"Executes a script (which does not have to be written in Python).")
		.staticmethod("execute")
		;

	boost::python::class_<k3d::iscript_engine::context>("context", "")
		.def("__len__", &context_len)
		.def("__getattr__", &context_get_item)
		.def("__setattr__", &context_set_item)
		.def("__delattr__", &context_del_item)
		.def("__getitem__", &context_get_item)
		.def("__setitem__", &context_set_item)
		.def("__delitem__", &context_del_item)
		.def("has_key", &context_has_key)
		.def("keys", &context_keys)
		.def("values", &context_values)
		;
}

} // namespace python

} // namespace k3d

