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
#include "iunknown_python.h"
#include "utility_python.h"

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

static const k3d::uuid factory_id(iunknown_wrapper& Self)
{
	return dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).factory_id();
}

static const string_t name(iunknown_wrapper& Self)
{
	return dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).name();
}

static const string_t short_description(iunknown_wrapper& Self)
{
	return dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).short_description();
}

static const bool_t is_application_plugin(iunknown_wrapper& Self)
{
	return dynamic_cast<k3d::iapplication_plugin_factory*>(&Self.wrapped()) ? true : false;
}

static const bool_t is_document_plugin(iunknown_wrapper& Self)
{
	return dynamic_cast<k3d::idocument_plugin_factory*>(&Self.wrapped()) ? true : false;
}

static const list categories(iunknown_wrapper& Self)
{
	list results;
	
	const k3d::iplugin_factory::categories_t& categories = dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).categories();
	for(k3d::iplugin_factory::categories_t::const_iterator category = categories.begin(); category != categories.end(); ++category)
		results.append(*category);

	return results;
}

static const string_t quality(iunknown_wrapper& Self)
{
	switch(dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).quality())
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

static boost::python::dict metadata(iunknown_wrapper& Self)
{
	boost::python::dict result;

	const iplugin_factory::metadata_t metadata = dynamic_cast<k3d::iplugin_factory&>(Self.wrapped()).metadata();
	for(iplugin_factory::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
		result[pair->first] = pair->second;

	return result;
}

void define_methods_iplugin_factory(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<iplugin_factory*>(&Interface))
		return;

	utility::add_method(utility::make_function(&factory_id, "Returns a universally-unique identifier for this factory."), "factory_id", Instance);
	utility::add_method(utility::make_function(&name, "Returns the human-readable plugin name, which is displayed in the user interface and can be used to instantiate plugins."), "name", Instance);
	utility::add_method(utility::make_function(&short_description, "Returns a short human-readable description of the plugin's purpose."), "short_description", Instance);
	utility::add_method(utility::make_function(&is_application_plugin, "Returns true if the plugin is an application plugin."), "is_application_function", Instance);
	utility::add_method(utility::make_function(&is_document_plugin, "Returns true if the plugin is a document plugin."), "is_document_plugin", Instance);
	utility::add_method(utility::make_function(&categories, "Returns an arbitrary collection of human-readable categories used to organize the list of plugins in the user interface."), "categories", Instance);
	utility::add_method(utility::make_function(&quality, "Returns the string \"stable\", \"experimental\", or \"deprecated\"."), "quality", Instance);
	utility::add_method(utility::make_function(&metadata, "Returns plugin metadata as a dict containing name-value pairs."), "metadata", Instance);
}

} // namespace python

} // namespace k3d

