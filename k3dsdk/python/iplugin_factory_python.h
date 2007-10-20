#ifndef K3DSDK_IPLUGIN_FACTORY_PYTHON_H
#define K3DSDK_IPLUGIN_FACTORY_PYTHON_H

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

#include "interface_wrapper_python.h"

#include <k3dsdk/uuid.h>
#include <boost/python/list.hpp>

namespace k3d
{

class iplugin_factory;

namespace python
{

class iplugin_factory :
	public interface_wrapper<k3d::iplugin_factory>
{
	typedef interface_wrapper<k3d::iplugin_factory> base;

public:
	iplugin_factory();
	iplugin_factory(k3d::iplugin_factory* PluginFactory);
	iplugin_factory(k3d::iplugin_factory& PluginFactory);

	const uuid factory_id();
	const std::string name();
	const std::string short_description();
	const bool is_application_plugin();
	const bool is_document_plugin();
	const boost::python::list categories();
	const std::string quality();

	static void define_class();
};

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IPLUGIN_FACTORY_PYTHON_H

