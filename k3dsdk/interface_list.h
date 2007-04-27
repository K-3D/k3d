#ifndef K3DSDK_INTERFACE_LIST_H
#define K3DSDK_INTERFACE_LIST_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iapplication_plugin_factory.h"
#include "plugin_factory.h"

namespace k3d
{

/// Defines a null interface type for marking the ends of an interface list
class null_interface
{
};

/// Used to create compile-time lists of interfaces (i.e. typelists - see Alexandrescu "Modern C++ Design")
template<typename head_t, typename tail_t = null_interface>
struct interface_list
{
	typedef head_t head;
	typedef tail_t tail;
};

/////////////////////////////////////////////////////////////////////////////
// implements_interface

/// Generates code at compile-time that test whether an interface_list contains a specific interface type
template<class interface_type>
struct implements_interface;

/// Generates code at compile-time that test whether an interface_list contains a specific interface type
template<>
struct implements_interface<null_interface>
{
	bool operator()(const std::type_info& InterfaceType)
	{
		return false;
	}
};

/// Generates code at compile-time that test whether an interface_list contains a specific interface type
template<typename head_t, typename tail_t>
struct implements_interface<interface_list<head_t, tail_t> >
{
	bool operator()(const std::type_info& InterfaceType)
	{
		if(typeid(head_t) == InterfaceType)
			return true;

		return implements_interface<tail_t>()(InterfaceType);
	}
};

/// Generates code at compile-time that returns a list of interface types
template<class interface_type>
struct get_interfaces;

/// Generates code at compile-time that returns a list of interface types
template<>
struct get_interfaces<null_interface>
{
	void operator()(iplugin_factory::interfaces_t& Interfaces)
	{
	}
};

/// Generates code at compile-time that returns a list of interface types
template<typename head_t, typename tail_t>
struct get_interfaces<interface_list<head_t, tail_t> >
{
	void operator()(iplugin_factory::interfaces_t& Interfaces)
	{
		Interfaces.push_back(&typeid(head_t));
		get_interfaces<tail_t>()(Interfaces);
	}
};

} // namespace k3d

#endif // !K3DSDK_INTERFACE_LIST_H

