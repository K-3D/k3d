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

#include "iuser_interface_python.h"
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/path.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

iuser_interface::iuser_interface() :
	base()
{
}

iuser_interface::iuser_interface(k3d::iuser_interface* UserInterface) :
	base(UserInterface)
{
}

void iuser_interface::message(const std::string& Message)
{
	wrapped().message(Message);
}

void iuser_interface::warning_message(const std::string& Message)
{
	wrapped().warning_message(Message);
}

void iuser_interface::error_message(const std::string& Message)
{
	wrapped().error_message(Message);
}

unsigned long iuser_interface::query_message(const std::string& Message, const list& Buttons)
{
	std::vector<std::string> buttons;
	for(long i = 0; i != len(Buttons); ++i)
		buttons.push_back(extract<std::string>(Buttons[i]));
	return wrapped().query_message(Message, 0, buttons);
}

const filesystem::path iuser_interface::get_file_path(const std::string& Direction, const std::string& Type, const std::string& Message, const std::string& StartPath)
{
	const filesystem::path old_path = filesystem::native_path(ustring::from_utf8(StartPath));
	filesystem::path new_path;
	if(Direction == "r" || Direction == "read")
		wrapped().get_file_path(k3d::ipath_property::READ, Type, Message, old_path, new_path);
	else if(Direction == "w" || Direction == "write")
		wrapped().get_file_path(k3d::ipath_property::WRITE, Type, Message, old_path, new_path);
	else
		throw std::invalid_argument("unknown file path operation, should be \"read\" or \"write\"");

	return new_path;
}

void export_iuser_interface()
{
	class_<iuser_interface>("iuser_interface")
		.def("message", &iuser_interface::message)
		.def("warning_message", &iuser_interface::warning_message)
		.def("error_message", &iuser_interface::error_message)
		.def("query_message", &iuser_interface::query_message)
		.def("get_file_path", &iuser_interface::get_file_path);
}

} // namespace python

} // namespace k3d

