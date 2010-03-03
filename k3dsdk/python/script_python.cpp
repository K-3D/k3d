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

#include <k3dsdk/scripting.h>
#include <k3dsdk/python/any_python.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class script
{
public:
	static void execute_with_context(const string_t& Script, const dict& PythonContext)
	{
		k3d::iscript_engine::context_t context;

		dict python_context = PythonContext;
		while(len(python_context))
		{
			tuple python_item = python_context.popitem();
			object python_key = python_item[0];
			object python_value = python_item[1];

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
		execute_with_context(Script, dict());
	}

};

void define_namespace_script()
{
	scope outer = class_<script>("script", no_init)
		.def("execute", script::execute,
			"Executes a script (which does not have to be written in Python).")
		.def("execute", script::execute_with_context,
			"Executes a script (which does not have to be written in Python).")
		.staticmethod("execute")
		;
}

} // namespace python

} // namespace k3d

