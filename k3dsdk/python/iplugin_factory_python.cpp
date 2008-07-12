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

#include "iplugin_factory_python.h"
#include "interface_wrapper_python.h"

#include <k3dsdk/iapplication_plugin_factory.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

typedef interface_wrapper<k3d::iplugin_factory> iplugin_factory_wrapper;

static const k3d::uuid factory_id(iplugin_factory_wrapper& Self)
{
	return Self.wrapped().factory_id();
}

static const string_t name(iplugin_factory_wrapper& Self)
{
	return Self.wrapped().name();
}

static const string_t short_description(iplugin_factory_wrapper& Self)
{
	return Self.wrapped().short_description();
}

static const bool_t is_application_plugin(iplugin_factory_wrapper& Self)
{
	return dynamic_cast<k3d::iapplication_plugin_factory*>(&Self.wrapped()) ? true : false;
}

static const bool_t is_document_plugin(iplugin_factory_wrapper& Self)
{
	return dynamic_cast<k3d::idocument_plugin_factory*>(&Self.wrapped()) ? true : false;
}

static const list categories(iplugin_factory_wrapper& Self)
{
	list results;
	
	const k3d::iplugin_factory::categories_t& categories = Self.wrapped().categories();
	for(k3d::iplugin_factory::categories_t::const_iterator category = categories.begin(); category != categories.end(); ++category)
		results.append(*category);

	return results;
}

static const string_t quality(iplugin_factory_wrapper& Self)
{
	switch(Self.wrapped().quality())
	{
		case k3d::iplugin_factory::STABLE:
			return "stable";
		case k3d::iplugin_factory::EXPERIMENTAL:
			return "experimental";
		case k3d::iplugin_factory::DEPRECATED:
			return "deprecated";
	}

	k3d::log() << warning << "Unknown plugin factory quality enumeration" << std::endl;
	return "unknown";
}

static boost::python::dict metadata(iplugin_factory_wrapper& Self)
{
	boost::python::dict result;

	const iplugin_factory::metadata_t metadata = Self.wrapped().metadata();
	for(iplugin_factory::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		result[pair->first] = pair->second;

	return result;
}

void define_class_iplugin_factory()
{
	class_<iplugin_factory_wrapper>("iplugin_factory",
		"Encapsulates a K-3D plugin factory, which stores metadata describing a plugin type.")
		.def("factory_id", &factory_id,
			"Returns a universally-unique identifier for this factory.")
		.def("name", &name,
			"Returns the human-readable plugin name, which is displayed in the user interface and can be used to instantiate plugins.")
		.def("short_description", &short_description,
			"Returns a short human-readable description of the plugin's purpose.")
		.def("is_application_plugin", &is_application_plugin,
			"Returns true if the plugin is an application plugin.")
		.def("is_document_plugin", &is_document_plugin,
			"Returns true if the plugin is a document plugin.")
		.def("categories", &categories,
			"Returns an arbitrary collection of human-readable categories used to organize the list of plugins in the user interface.")
		.def("quality", &quality,
			"Returns the string \"stable\", \"experimental\", or \"deprecated\".")
		.def("metadata", &metadata,
			"Returns plugin metadata as a dict containing name-value pairs.");
}

} // namespace python

} // namespace k3d

