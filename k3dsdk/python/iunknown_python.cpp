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

#include <k3dsdk/python/any_python.h>
#include <k3dsdk/python/icommand_node_python.h>
#include <k3dsdk/python/offscreen_context_factory_gl_python.h>
#include <k3dsdk/python/idocument_exporter_python.h>
#include <k3dsdk/python/idocument_importer_python.h>
#include <k3dsdk/python/ifile_change_notifier_python.h>
#include <k3dsdk/python/ikeyframer_python.h>
#include <k3dsdk/python/imesh_storage_python.h>
#include <k3dsdk/python/imetadata_python.h>
#include <k3dsdk/python/inode_python.h>
#include <k3dsdk/python/inode_selection_python.h>
#include <k3dsdk/python/iplugin_factory_python.h>
#include <k3dsdk/python/iproperty_python.h>
#include <k3dsdk/python/iproperty_collection_python.h>
#include <k3dsdk/python/irender_camera_frame_python.h>
#include <k3dsdk/python/irender_camera_preview_python.h>
#include <k3dsdk/python/isnappable_python.h>
#include <k3dsdk/python/iunknown_python.h>
#include <k3dsdk/python/iuser_interface_python.h>

#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/log.h>
#include <k3dsdk/property.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

object wrap_unknown(iunknown* Unknown)
{
	return Unknown ? wrap_unknown(*Unknown) : object();
}

object wrap_unknown(iunknown& Unknown)
{
	object result = object(iunknown_wrapper(Unknown));

	define_methods_icommand_node(Unknown, result);
	define_methods_offscreen_context_factory_gl(Unknown, result);
	define_methods_idocument_exporter(Unknown, result);
	define_methods_idocument_importer(Unknown, result);
	define_methods_ifile_change_notifier(Unknown, result);
	define_methods_ikeyframer(Unknown, result);
	define_methods_imesh_storage(Unknown, result);
	define_methods_imetadata(Unknown, result);
	define_methods_inode(Unknown, result);
	define_methods_inode_selection(Unknown, result);
	define_methods_iplugin_factory(Unknown, result);
	define_methods_iproperty(Unknown, result);
	define_methods_iproperty_collection(Unknown, result);
	define_methods_irender_camera_frame(Unknown, result);
	define_methods_irender_camera_preview(Unknown, result);
	define_methods_isnappable(Unknown, result);
	define_methods_iuser_interface(Unknown, result);
	
	return result;
}

// This rightfully belongs with the rest of the iproperty_collection code, but for some reason __getattr__
// doesn't seem to work with our dynamically-added instance methods.
static object getattr(iunknown_wrapper& Self, const string_t& Name)
{
	// Return K-3D properties as attributes whenever they're available ...
	if(k3d::iproperty_collection* const property_collection = Self.wrapped_ptr<k3d::iproperty_collection>())
	{
		if(k3d::iproperty* property = k3d::property::get(*property_collection, Name))
			return any_to_python(k3d::property::pipeline_value(*property));

		throw std::invalid_argument("unknown property: " + Name);
	}

	return object();
}

// This rightfully belongs with the rest of the iproperty_collection code, but for some reason __getattr__
// doesn't seem to work with our dynamically-added instance methods.
static void setattr(object& Self, const string_t& Name, const object& Value)
{
	// If this is a K-3D property, set its internal value ...
	extract<iunknown_wrapper> unknown(Self);
	if(unknown.check())
	{
		if(k3d::iproperty_collection* const property_collection = unknown().wrapped_ptr<k3d::iproperty_collection>())
		{
			if(k3d::iproperty* const property = k3d::property::get(*property_collection, Name))
			{
				if(k3d::iwritable_property* const writable = dynamic_cast<k3d::iwritable_property*>(property))
				{
					writable->property_set_value(python_to_any(Value, property->property_type()));
					return;
				}

				throw std::invalid_argument("read-only property: " + Name);
			}
		}
	}

	// Otherwise, fallback on default behavior ...
	Self.attr("__dict__")[Name] = Value;
}

static bool eq(const object& Self, const object& Other)
{
	if(Other == boost::python::object())
		return false;

	extract<iunknown_wrapper> self(Self);
	extract<iunknown_wrapper> other(Other);

	return self == other;
}

static bool ne(const object& Self, const object& Other)
{
	if(Other == boost::python::object())
		return false;

	extract<iunknown_wrapper> self(Self);
	extract<iunknown_wrapper> other(Other);

	return !(self == other);
}

void define_class_iunknown()
{
	class_<iunknown_wrapper>("iunknown", 
		"Abstract interface that represents an object with unknown capabilities.\n\n"
		"Methods for other implemented interfaces are added dynamically at runtime.",
		no_init)
		.def("__getattr__", getattr)
		.def("__setattr__", setattr)
		.def("__eq__", eq)
		.def("__ne__", ne)
		;
}

} // namespace python

} // namespace k3d

