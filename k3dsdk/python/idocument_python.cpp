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

#include "idocument_python.h"
#include "iproperty_python.h"
#include "iunknown_python.h"
#include "dynamic_cast_python.h"
#include "node_python.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/idocument_exporter.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/utility_gl.h>

#include <boost/python.hpp>
using namespace boost::python;

#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

static const bool save(idocument_wrapper& Self, const std::string& Path)
{
	boost::scoped_ptr<k3d::idocument_exporter> exporter(k3d::plugin::create<k3d::idocument_exporter>(k3d::classes::DocumentExporter()));
	if(!exporter)
		throw std::runtime_error("no exporter plugin available");

	return exporter->write_file(Self.wrapped(), filesystem::native_path(ustring::from_utf8(Path)));
}

static void start_change_set(idocument_wrapper& Self)
{
	k3d::start_state_change_set(Self.wrapped(), K3D_CHANGE_SET_CONTEXT);
}

static void cancel_change_set(idocument_wrapper& Self)
{
	k3d::cancel_state_change_set(Self.wrapped(), K3D_CHANGE_SET_CONTEXT);
}

static void finish_change_set(idocument_wrapper& Self, const std::string& Label)
{
	k3d::finish_state_change_set(Self.wrapped(), Label, K3D_CHANGE_SET_CONTEXT);
}

static void redraw_all(idocument_wrapper& Self)
{
	k3d::gl::redraw_all(Self.wrapped(), k3d::gl::irender_viewport::ASYNCHRONOUS);
}

static const list nodes(idocument_wrapper& Self)
{
	list results;

	const k3d::inode_collection::nodes_t nodes = Self.wrapped().nodes().collection();
	for(k3d::inode_collection::nodes_t::const_iterator n = nodes.begin(); n != nodes.end(); ++n)
		results.append(node(*n));

	return results;
}

static const object new_node(idocument_wrapper& Self, const object& Type)
{
	extract<std::string> plugin_name(Type);
	if(plugin_name.check())
	{
		k3d::iplugin_factory* const plugin_factory = k3d::plugin::factory::lookup(plugin_name());
		if(!plugin_factory)
			throw std::runtime_error("no factory for plugin type " + plugin_name());

		return object(node(k3d::plugin::create<k3d::iunknown>(*plugin_factory, Self.wrapped(), k3d::unique_name(Self.wrapped().nodes(), plugin_name()))));
	}

	extract<iunknown_wrapper> plugin_factory(Type);
	if(plugin_factory.check())
	{
		return object(node(k3d::plugin::create<k3d::iunknown>(dynamic_cast<k3d::iplugin_factory&>(plugin_factory().wrapped()), Self.wrapped())));
	}

	throw std::invalid_argument("can't create new node from given argument");
}

static const object get_node(idocument_wrapper& Self, const std::string& Name)
{
	return object(node(k3d::find_node(Self.wrapped().nodes(), Name)));
}

static const object get_node_by_metadata(idocument_wrapper& Self, const std::string& Type, const std::string& MetaName, const std::string& MetaValue)
{
	const k3d::inode_collection::nodes_t nodes = k3d::find_nodes<k3d::inode>(Self.wrapped().nodes(), MetaName, MetaValue);
	
	if(nodes.size() > 1)
		throw std::runtime_error("multiple nodes exist with the given metadata");
	
	if(nodes.empty())
		return object(node(0));
	
	return do_dynamic_cast(object(node(nodes.back())), Type);
}

static const bool has_node(idocument_wrapper& Self, const std::string& Name)
{
	if (k3d::find_node(Self.wrapped().nodes(), Name) != 0)
		return true;
	return false;
}

static void delete_node(idocument_wrapper& Self, object& Node)
{
	extract<node> node(Node);
	if(!node.check())
		throw std::invalid_argument("argument isn't a node");

	k3d::delete_nodes(Self.wrapped(), k3d::make_collection<k3d::nodes_t>(node().inode_wrapper::wrapped_ptr()));
}

static object get_dependency(idocument_wrapper& Self, iproperty_wrapper& Property)
{
	k3d::iproperty* const property = Property.wrapped_ptr();
	if(!property)
		throw std::invalid_argument("property cannot be null");

	return wrap(Self.wrapped().pipeline().dependency(*property));
}

static void set_dependency(idocument_wrapper& Self, iproperty_wrapper& From, boost::python::object& To)
{
	k3d::iproperty* to = 0;

	extract<iproperty_wrapper> iproperty_value(To);
	if(iproperty_value.check())
	{
		to = iproperty_value().wrapped_ptr();
	}
	else if(To.ptr() == boost::python::object().ptr())
	{
		to = 0;
	}
	else
	{
		throw std::invalid_argument("to property must be an iproperty instance or None");
	}

	k3d::iproperty* const from = From.wrapped_ptr();
	if(!from)
		throw std::invalid_argument("from property cannot be null");

	if(from && to && from->property_type() != to->property_type())
		throw std::invalid_argument("property types do not match");

	k3d::ipipeline::dependencies_t dependencies;
	dependencies[from] = to;
	Self.wrapped().pipeline().set_dependencies(dependencies);
}

void define_class_idocument()
{
	class_<idocument_wrapper>("idocument")
		.def("save", &save)
		.def("start_change_set", &start_change_set)
		.def("cancel_change_set", &cancel_change_set)
		.def("finish_change_set", &finish_change_set)
		.def("redraw_all", &redraw_all)
		.def("nodes", &nodes)
		.def("new_node", &new_node)
		.def("get_node", &get_node)
		.def("get_node_by_metadata", &get_node_by_metadata)
		.def("has_node", &has_node)
		.def("delete_node", &delete_node)
		.def("get_dependency", &get_dependency)
		.def("set_dependency", &set_dependency);
}

} // namespace python

} // namespace k3d

