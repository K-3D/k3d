#ifndef K3DSDK_IPROPERTY_COLLECTION_PYTHON_H
#define K3DSDK_IPROPERTY_COLLECTION_PYTHON_H

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

#include "iproperty_python.h"
#include <boost/python/list.hpp>

namespace k3d
{
	
class iproperty_collection;

namespace python
{

class iproperty_collection :
	public interface_wrapper<k3d::iproperty_collection>
{
	typedef interface_wrapper<k3d::iproperty_collection> base;
public:
	iproperty_collection();
	iproperty_collection(k3d::iproperty_collection* CommandNode);

	boost::python::list properties();
	iproperty get_property(const std::string& Name);
	iproperty add_user_property(const std::string& Type, const std::string& Name, const std::string& Label, const std::string& Description);
	iproperty add_ri_attribute(const std::string& Type, const std::string& List, const std::string& Name, const std::string& Label, const std::string& Description);
	iproperty add_ri_option(const std::string& Type, const std::string& List, const std::string& Name, const std::string& Label, const std::string& Description);

	boost::python::object getattr(const std::string& Name);
	void setattr(const std::string& Name, const boost::python::object& Value);
};

void export_iproperty_collection();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IPROPERTY_COLLECTION_PYTHON_H

