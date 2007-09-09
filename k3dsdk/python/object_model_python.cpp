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
#include "array_python.h"
#include "bitmap_python.h"
#include "bounding_box3_python.h"
#include "color_python.h"
#include "const_array_python.h"
#include "const_named_arrays_python.h"
#include "dynamic_cast_python.h"
#include "euler_angles_python.h"
#include "icommand_node_python.h"
#include "idocument_python.h"
#include "imaterial_python.h"
#include "imesh_storage_python.h"
#include "inode_python.h"
#include "iplugin_factory_python.h"
#include "iproperty_python.h"
#include "iproperty_collection_python.h"
#include "isnappable_python.h"
#include "iunknown_python.h"
#include "iuser_interface_python.h"
#include "matrix4_python.h"
#include "mesh_python.h"
#include "mesh_selection_python.h"
#include "named_arrays_python.h"
#include "node_python.h"
#include "normal3_python.h"
#include "object_model_python.h"
#include "point3_python.h"
#include "point4_python.h"
#include "ri_render_state_python.h"
#include "uuid_python.h"
#include "vector3_python.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/batch_mode.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/mesh_diff.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/scripting.h>
#include <k3dsdk/share.h>
#include <k3dsdk/type_registry.h>
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
		k3d::log() << critical << k3d_file_reference << " should not be reached" << std::endl;
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

void module_check_node_environment(const boost::python::dict& Locals, const std::string& PluginType)
{
	if(Locals.has_key("Node"))
	{
		boost::python::object object = Locals.get("Node");
		boost::python::extract<node> node(object);
		if(node.check())
		{
			if(node().inode::wrapped().factory().name() == PluginType)
			{
				return;
			}
		}
	}

	k3d::user_interface().error_message(k3d::string_cast(boost::format("This script can only be used from within a %1% plugin.") % PluginType));
	throw std::runtime_error("script can only be run from " + PluginType);
}

void module_execute_script_context(const std::string& Script, const boost::python::dict& PythonContext)
{
	k3d::iscript_engine::context_t context;

	boost::python::dict python_context = PythonContext;
	while(boost::python::len(python_context))
	{
		boost::python::tuple python_item = python_context.popitem();
		boost::python::object python_key = python_item[0];
		boost::python::object python_value = python_item[1];

		const std::string key = PyString_AsString(python_key.ptr());
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

void module_execute_script(const std::string& Script)
{
	module_execute_script_context(Script, boost::python::dict());
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

iuser_interface module_ui()
{
	return iuser_interface(k3d::user_interface());
}

void module_exit()
{
	k3d::application().exit();
}

idocument module_new_document()
{
	return idocument(k3d::application().create_document());
}

void module_close_document(idocument& Document)
{
	k3d::application().close_document(Document.wrapped());
}

object module_get_command_node(const std::string& Path)
{
	if(k3d::icommand_node* const command_node = k3d::command_node::lookup(Path))
		return object(icommand_node(command_node));

	return object();
}

idocument module_open_document(const std::string& Path)
{
	const filesystem::path document_path = filesystem::native_path(ustring::from_utf8(Path));

	k3d::auto_ptr<k3d::idocument_importer> filter(k3d::create_plugin<k3d::idocument_importer>(k3d::classes::DocumentImporter()));
	if(!filter.get())
		throw std::runtime_error("no importer plugin available");

	k3d::idocument* const document = k3d::application().create_document();
	if(!document)
		throw std::runtime_error("couldn't create empty document");

	if(!filter->read_file(*document, document_path))
		throw std::runtime_error("error loading document");

	return idocument(document);
}

const std::string module_print_diff(const object& A, const object& B, const object& Threshold)
{
	extract<mesh> a(A);
	extract<mesh> b(B);
	extract<boost::uint64_t> threshold(Threshold);
	if(a.check() && b.check() && threshold.check())
	{
		std::ostringstream buffer;
		k3d::print_diff(buffer, a().wrapped(), b().wrapped(), threshold());
		return buffer.str();
	}

	throw std::invalid_argument("cannot diff given objects");
}

////////////////////////////////////////////////////////////////////////////////////
// k3d module

BOOST_PYTHON_MODULE(k3d)
{
	to_python_converter<k3d::filesystem::path, python_wrap<k3d::filesystem::path> >();
	to_python_converter<k3d::mesh_selection::records_t, python_wrap<k3d::mesh_selection::records_t> >();

	angle_axis::define_class();
	const_named_arrays::define_class();
	euler_angles::define_class();
	export_arrays();
	export_bitmap();
	export_bounding_box3();
	export_color();
	export_const_arrays();
	export_matrix4();
	export_mesh_selection();
	export_normal3();
	export_point3();
	export_point4();
	export_uuid();
	export_vector3();
	icommand_node::define_class();
	idocument::define_class();
	imaterial::define_class();
	imesh_storage::define_class();
	inode::define_class();
	iplugin_factory::define_class();
	iproperty::define_class();
	iproperty_collection::define_class();
	isnappable::define_class();
	iunknown::define_class();
	iuser_interface::define_class();
	mesh::define_class();
	named_arrays::define_class();
	node::define_class();
	ri_render_state::define_class();

	def("batch_mode", k3d::batch_mode,
		"Returns True if batch (no user intervention) mode is enabled for the user interface.\n"
		"@note: Well-behaved scripts should not prompt the user for input if batch mode is enabled.");
	def("check_node_environment", module_check_node_environment,
		"Checks to see whether the current script is running from within the given node type.");
	def("close_document", module_close_document,
		"Closes an open document.");
	def("command_nodes", module_command_nodes,
		"Returns the root(s) of the command node hierarchy.");
	def("component_deselect_all", k3d::mesh_selection::component_deselect_all,
		"Returns a list for use with L{mesh_selection} that deselects an entire range of mesh components.");
	def("component_select_all", k3d::mesh_selection::component_select_all,
		"Returns a list for use with L{mesh_selection} that selects an entire range of mesh components.");
	def("create_plugin", module_create_plugin,
		"Creates an application plugin instance by name (fails if there is no application plugin factory with the given name).");
	def("deselect_all", k3d::mesh_selection::deselect_all,
		"Returns a L{mesh_selection} that explicitly deselects every component.");
	def("dynamic_cast", do_dynamic_cast,
		"Attempts to coerce an object from one type to another.");
	def("print_diff", module_print_diff,
		"Returns the difference of two L{mesh} objects as a string.");
	def("execute_script", module_execute_script,
		"Executes a script (which does not have to be written in Python).");
	def("execute_script", module_execute_script_context,
		"Executes a script (which does not have to be written in Python).");
	def("exit", module_exit,
		"Request program exit (may be overridden by user input).");
	def("get_command_node", module_get_command_node,
		"Returns a command node by path.");
	def("identity3", k3d::identity3D,
		"Returns a L{matrix4} containing a three-dimensional identity matrix.");
	def("length", module_length,
		"Returns the length of a L{vector3}.");
	def("log_critical", module_log_critical,
		"Sends a critical message to the K-3D log.");
	def("log_debug", module_log_debug,
		"Sends a debug message to the K-3D log.");
	def("log_error", module_log_error,
		"Sends an error message to the K-3D log.");
	def("log_info", module_log_info,
		"Sends an informational message to the K-3D log.");
	def("log_warning", module_log_warning,
		"Sends a warning message to the K-3D log.");
	def("new_document", module_new_document,
		"Returns a new (completely empty) document.");
	def("open_document", module_open_document,
		"Opens an existing document stored on disk.");
	def("plugins", module_plugins,
		"Returns a list containing the set of all plugin factories.");
	def("rotate3", module_rotate3,
		"Returns a L{matrix4} containing a three-dimensional rotation matrix.");
	def("scale3", module_scale3,
		"Returns a L{matrix4} containing a three-dimensional scaling matrix.");
	def("select_all", k3d::mesh_selection::select_all,
		"Returns a L{mesh_selection} that explicitly selects every component.");
	def("select_null", k3d::mesh_selection::select_null,
		"Returns a L{mesh_selection} that does not select or deselect any components.");
	def("share_path", k3d::share_path,
		"Returns the runtime path to shared data.");
	def("translate3", module_translate3,
		"Returns a L{matrix4} containing a three-dimensional translation matrix.");
	def("ui", module_ui,
		"Returns the singleton runtime L{iuser_interface} plugin instance.");

	scope().attr("__doc__") = "Provides access to the K-3D API";
}

/////////////////////////////////////////////////////////////////////////////
// set_context

void set_context(const k3d::iscript_engine::context_t& Context, boost::python::dict& Dictionary)
{
	for(k3d::iscript_engine::context_t::const_iterator context = Context.begin(); context != Context.end(); ++context)
	{
		try
		{
			Dictionary[context->first] = any_to_python(context->second);
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
		const std::type_info& type = context->second.type();

		if(type == typeid(k3d::idocument*))
			continue;
		else if(type == typeid(k3d::inode*))
			continue;
		else if(type == typeid(k3d::mesh*))
			continue;
		else if(type == typeid(const k3d::ri::render_state*))
			continue;
		else
		{
			context->second = python_to_any(Dictionary[context->first], type);
		}
	}
}

} // namespace python

} // namespace k3d

