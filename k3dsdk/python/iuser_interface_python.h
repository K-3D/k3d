#ifndef K3DSDK_IUSER_INTERFACE_PYTHON_H
#define K3DSDK_IUSER_INTERFACE_PYTHON_H

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
#include <k3dsdk/path.h>
#include <boost/python/list.hpp>

namespace k3d
{

class iuser_interface;

namespace python
{

class iuser_interface :
	public interface_wrapper<k3d::iuser_interface>
{
	typedef interface_wrapper<k3d::iuser_interface> base;

public:
	iuser_interface();
	iuser_interface(k3d::iuser_interface& UserInterface);

	void open_uri(const std::string& URI);
	void message(const std::string& Message);
	void warning_message(const std::string& Message);
	void error_message(const std::string& Message);
	unsigned long query_message(const std::string& Message, const boost::python::list& Buttons);
	const filesystem::path get_file_path(const std::string& Direction, const std::string& Type, const std::string& Message, const std::string& StartPath);

	static void define_class();
};

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IUSER_INTERFACE_PYTHON_H

