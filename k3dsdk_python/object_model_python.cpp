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
#include "bounding_box3_python.h"
#include "color_python.h"
#include "euler_angles_python.h"
#include "iapplication_python.h"
#include "icommand_node_python.h"
#include "idocument_python.h"
#include "imaterial_python.h"
#include "inode_python.h"
#include "iplugin_factory_python.h"
#include "iproperty_python.h"
#include "iproperty_collection_python.h"
#include "iselectable_python.h"
#include "iunknown_python.h"
#include "iuser_interface_python.h"
#include "matrix4_python.h"
#include "mesh_python.h"
#include "mesh_selection_python.h"
#include "node_python.h"
#include "normal3_python.h"
#include "object_model_python.h"
#include "point3_python.h"
#include "point4_python.h"
#include "render_state_ri_python.h"
#include "uuid_python.h"
#include "vector3_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/new_mesh.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/render_state_ri.h>
#include <k3dsdk/scripting.h>
#include <k3dsdk/share.h>
#include <k3dsdk/types.h>
#include <k3dsdk/user_interface.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

////////////////////////////////////////////////////////////////////////////////////
// Conversion to python

/// Prevent implicit conversions - a separate python_wrap() implementation should be created for every required type
template<typename T>
struct python_wrap
{
	static PyObject* convert(const T&)
	{
		BOOST_STATIC_ASSERT(sizeof(T) == 0);
		k3d::log() << critical << __PRETTY_FUNCTION__ << " should not be reached" << std::endl;
		return 0;
	}
};

template<>
struct python_wrap<unsigned long>
{
	static PyObject* convert(const unsigned long Value)
	{
		return PyLong_FromUnsignedLong(Value);
	}
};

template<>
struct python_wrap<double>
{
	static PyObject* convert(const double Value)
	{
		return Py_BuildValue("d", Value);
	}
};

template<>
struct python_wrap<filesystem::path>
{
	static PyObject* convert(const filesystem::path& Value)
	{
		return Py_BuildValue("s", const_cast<char*>(Value.native_filesystem_string().c_str()));
	}
};

template<>
struct python_wrap<mesh_selection::records_t>
{
	static PyObject* convert(const k3d::mesh_selection::records_t& Value)
	{
		PyObject* const py_records = PyList_New(Value.size());
		return_val_if_fail(py_records, 0);

		size_t index = 0;
		for(k3d::mesh_selection::records_t::const_iterator record = Value.begin(); record != Value.end(); ++record, ++index)
		{
			PyObject* const py_record = PyTuple_New(3);
			return_val_if_fail(py_record, 0);

			PyTuple_SetItem(py_record, 0, python_wrap<unsigned long>::convert(static_cast<unsigned long>(record->begin)));
			PyTuple_SetItem(py_record, 1, python_wrap<unsigned long>::convert(static_cast<unsigned long>(record->end)));
			PyTuple_SetItem(py_record, 2, python_wrap<double>::convert(record->weight));

			PyList_SetItem(py_records, index, py_record);
		}

		return py_records;
	}
};

////////////////////////////////////////////////////////////////////////////////////
// k3d module helpers

const list module_command_nodes()
{
	list nodes;

	k3d::icommand_tree::nodes_t children = k3d::command_tree().children(0);
	for(k3d::icommand_tree::nodes_t::iterator child = children.begin(); child != children.end(); ++child)
		nodes.append(icommand_node(*child));

	return nodes;
}

iunknown module_create_plugin(const std::string& Type)
{
	const k3d::factories_t plugin_factories = k3d::plugins(Type);
	if(1 != plugin_factories.size())
		throw std::invalid_argument("unknown plugin type: " + Type);

	return iunknown(k3d::create_plugin(**plugin_factories.begin()));
}

void module_execute_script(const std::string& Script)
{
	k3d::iscript_engine::context_t context;
	bool recognized = false;
	bool executed = false;
	k3d::script::execute(k3d::script::code(Script), "Python Text", context, recognized, executed);
	if(!recognized)
		throw std::invalid_argument("Unrecognized scripting language");
	if(!executed)
		throw std::runtime_error("Error executing script");
}

const double module_length(const object& Value)
{
	extract<k3d::vector3> vector3(Value);
	if(vector3.check())
		return k3d::length(vector3());

	throw std::invalid_argument("can't calculate length for this type");
}

void module_log_critical(const std::string& Message)
{
	k3d::log() << critical << Message << std::endl;
}

void module_log_debug(const std::string& Message)
{
	k3d::log() << debug << Message << std::endl;
}

void module_log_error(const std::string& Message)
{
	k3d::log() << error << Message << std::endl;
}

void module_log_info(const std::string& Message)
{
	k3d::log() << info << Message << std::endl;
}

void module_log_warning(const std::string& Message)
{
	k3d::log() << warning << Message << std::endl;
}

const list module_plugins()
{
	list plugins;

	const k3d::iplugin_factory_collection::factories_t& factories = k3d::application().plugins();
	for(k3d::iplugin_factory_collection::factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
		plugins.append(iplugin_factory(*factory));

	return plugins;
}

const k3d::matrix4 module_rotate3(const object& Value)
{
	extract<angle_axis> angle_axis_value(Value);
	if(angle_axis_value.check())
		return k3d::rotation3D(angle_axis_value());

	extract<k3d::euler_angles> euler_angles_value(Value);
	if(euler_angles_value.check())
		return k3d::rotation3D(k3d::quaternion(euler_angles_value()));

	throw std::invalid_argument("can't generate rotation matrix from given type");
}

const k3d::matrix4 module_scale3(const double X, const double Y, const double Z)
{
	return k3d::scaling3D(k3d::point3(X, Y, Z));
}

const k3d::matrix4 module_translate3(const object& Value)
{
	extract<k3d::vector3> vector3(Value);
	if(vector3.check())
		return k3d::translation3D(vector3());

	extract<k3d::point3> point3(Value);
	if(point3.check())
		return k3d::translation3D(point3());

	throw std::invalid_argument("cannot generate translation matrix from given type");
}

////////////////////////////////////////////////////////////////////////////////////
// k3d module

BOOST_PYTHON_MODULE(k3d)
{
	to_python_converter<k3d::filesystem::path, python_wrap<k3d::filesystem::path> >();
	to_python_converter<k3d::mesh_selection::records_t, python_wrap<k3d::mesh_selection::records_t> >();

	angle_axis::define_class();
	export_bitmap();
	export_bounding_box3();
	export_color();
	export_euler_angles();
	export_iapplication();
	export_icommand_node();
	export_idocument();
	export_imaterial();
	export_inode();
	export_iplugin_factory();
	export_iproperty();
	export_iproperty_collection();
	export_iselectable();
	export_iunknown();
	export_iuser_interface();
	export_matrix4();
	export_mesh();
	export_mesh_selection();
	export_node();
	export_normal3();
	export_point3();
	export_point4();
	export_render_state_ri();
	export_uuid();
	export_vector3();

	def("command_nodes", module_command_nodes, "Returns the root(s) of the command node hierarchy.");
	def("component_deselect_all", k3d::mesh_selection::component_deselect_all);
	def("component_select_all", k3d::mesh_selection::component_select_all);
	def("create_plugin", module_create_plugin, "Creates an application plugin instance by name (fails if there is no application plugin factory with the given name).");
	def("deselect_all", k3d::mesh_selection::deselect_all, "Returns a L{mesh_selection} that explicitly deselects every component.");
	def("euler_angles", euler_angles_init); // Special-case the euler_angles ctor to handle the degrees-to-radians conversion
	def("execute_script", module_execute_script, "Executes a script (which does not have to be written in Python).");
	def("identity3", k3d::identity3D, "Returns a L{matrix4} containing a three-dimensional identity matrix.");
	def("length", module_length, "Returns the length of a L{vector3}.");
	def("log_critical", module_log_critical, "Sends a critical message to the K-3D log.");
	def("log_debug", module_log_debug, "Sends a debug message to the K-3D log.");
	def("log_error", module_log_error, "Sends an error message to the K-3D log.");
	def("log_info", module_log_info, "Sends an informational message to the K-3D log.");
	def("log_warning", module_log_warning, "Sends a warning message to the K-3D log.");
	def("plugins", module_plugins, "Returns a list containing the set of all plugin factories.");
	def("rotate3", module_rotate3, "Returns a L{matrix4} containing a three-dimensional rotation matrix.");
	def("scale3", module_scale3, "Returns a L{matrix4} containing a three-dimensional scaling matrix.");
	def("select_all", k3d::mesh_selection::select_all, "Returns a L{mesh_selection} that explicitly selects every component.");
	def("select_null", k3d::mesh_selection::select_null, "Returns a L{mesh_selection} that does not select or deselect any components.");
	def("translate3", module_translate3);

	scope().attr("application") = iapplication(&k3d::application());
	scope().attr("share_path") = k3d::share_path();
	scope().attr("ui") = iuser_interface(&k3d::user_interface());

	scope().attr("__doc__") = "Main module exporting the K-3D API";
}

/////////////////////////////////////////////////////////////////////////////
// set_context

void set_context(const k3d::iscript_engine::context_t& Context, boost::python::dict& Dictionary)
{
	for(k3d::iscript_engine::context_t::const_iterator context = Context.begin(); context != Context.end(); ++context)
	{
		try
		{
			k3d::iunknown* const unknown = context->second.type() == typeid(k3d::iunknown*) ? boost::any_cast<k3d::iunknown*>(context->second) : 0;
			if(k3d::idocument* const k3d_document = dynamic_cast<k3d::idocument*>(unknown))
			{
				Dictionary[context->first] = idocument(k3d_document);
			}
			else if(k3d::inode* const k3d_node = dynamic_cast<k3d::inode*>(unknown))
			{
				Dictionary[context->first] = node(k3d_node);
			}
			else if(context->second.type() == typeid(k3d::dev::mesh*))
			{
				Dictionary[context->first] = mesh(boost::any_cast<k3d::dev::mesh*>(context->second));
			}
			else if(context->second.type() == typeid(const k3d::ri::render_state*))
			{
				Dictionary[context->first] = render_state_ri(boost::any_cast<const k3d::ri::render_state*>(context->second));
			}
			else
			{
				Dictionary[context->first] = any_to_python(context->second);
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "error marshalling [" << context->first << "] to Python: " << e.what() << std::endl;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// get_context

void get_context(boost::python::dict& Dictionary, k3d::iscript_engine::context_t& Context)
{
	for(k3d::iscript_engine::context_t::iterator context = Context.begin(); context != Context.end(); ++context)
	{
		k3d::iunknown* const unknown = context->second.type() == typeid(k3d::iunknown*) ? boost::any_cast<k3d::iunknown*>(context->second) : 0;

		if(dynamic_cast<k3d::idocument*>(unknown))
			continue;
		else if(dynamic_cast<k3d::inode*>(unknown))
			continue;
		else if(context->second.type() == typeid(k3d::dev::mesh*))
			continue;
		else if(context->second.type() == typeid(const k3d::ri::render_state*))
			continue;
		else
		{
			context->second = python_to_any(Dictionary[context->first], context->second.type());
		}
	}
}

} // namespace python

} // namespace k3d

