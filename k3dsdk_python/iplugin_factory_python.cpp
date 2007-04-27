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

#include "iplugin_factory_python.h"

#include <k3dsdk/iapplication_plugin_factory.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/log.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

iplugin_factory::iplugin_factory() :
	base()
{
}

iplugin_factory::iplugin_factory(k3d::iplugin_factory* PluginFactory) :
	base(PluginFactory)
{
}

iplugin_factory::iplugin_factory(k3d::iplugin_factory& PluginFactory) :
	base(PluginFactory)
{
}

const k3d::uuid iplugin_factory::class_id()
{
	return wrapped().class_id();
}

const std::string iplugin_factory::name()
{
	return wrapped().name();
}

const std::string iplugin_factory::short_description()
{
	return wrapped().short_description();
}

const bool iplugin_factory::is_application_plugin()
{
	return dynamic_cast<k3d::iapplication_plugin_factory*>(&wrapped()) ? true : false;
}

const bool iplugin_factory::is_document_plugin()
{
	return dynamic_cast<k3d::idocument_plugin_factory*>(&wrapped()) ? true : false;
}

const list iplugin_factory::categories()
{
	list results;
	
	const k3d::iplugin_factory::categories_t& categories = wrapped().categories();
	for(k3d::iplugin_factory::categories_t::const_iterator category = categories.begin(); category != categories.end(); ++category)
		results.append(*category);

	return results;
}

const std::string iplugin_factory::quality()
{
	switch(wrapped().quality())
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

void export_iplugin_factory()
{
	class_<iplugin_factory>("iplugin_factory")
		.add_property("class_id", &iplugin_factory::class_id)
		.add_property("name", &iplugin_factory::name)
		.add_property("short_description", &iplugin_factory::short_description)
		.add_property("is_application_plugin", &iplugin_factory::is_application_plugin)
		.add_property("is_document_plugin", &iplugin_factory::is_document_plugin)
		.add_property("categories", &iplugin_factory::categories)
		.add_property("quality", &iplugin_factory::quality);
}

} // namespace python

} // namespace k3d

