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

#include <k3dsdk/types.h>
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
	iproperty get_property(const string_t& Name);
	iproperty create_property(const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description);
	iproperty create_renderman_attribute(const string_t& Type, const string_t& List, const string_t& Name, const string_t& Label, const string_t& Description);
	iproperty create_renderman_option(const string_t& Type, const string_t& List, const string_t& Name, const string_t& Label, const string_t& Description);
	bool has_property(const string_t& Name);

	boost::python::object getattr(const string_t& Name);
	void setattr(const string_t& Name, const boost::python::object& Value);

	static void define_class();
};

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IPROPERTY_COLLECTION_PYTHON_H

