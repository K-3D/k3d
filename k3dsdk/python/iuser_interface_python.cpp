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
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/path.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static void open_uri(iunknown_wrapper& Self, const string_t& URI)
{
	dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).open_uri(URI);
}

static void message(iunknown_wrapper& Self, const string_t& Message)
{
	dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).message(Message);
}

static void warning_message(iunknown_wrapper& Self, const string_t& Message)
{
	dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).warning_message(Message);
}

static void error_message(iunknown_wrapper& Self, const string_t& Message)
{
	dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).error_message(Message);
}

static unsigned long query_message(iunknown_wrapper& Self, const string_t& Message, const list& Buttons)
{
	std::vector<string_t> buttons;
	for(long i = 0; i != len(Buttons); ++i)
		buttons.push_back(extract<string_t>(Buttons[i]));
	return dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).query_message(Message, 0, buttons);
}

static const filesystem::path get_file_path(iunknown_wrapper& Self, const string_t& Direction, const string_t& Type, const string_t& Message, const string_t& StartPath)
{
	const filesystem::path old_path = filesystem::native_path(ustring::from_utf8(StartPath));
	filesystem::path new_path;
	if(Direction == "r" || Direction == "read")
		dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).get_file_path(k3d::ipath_property::READ, Type, Message, old_path, new_path);
	else if(Direction == "w" || Direction == "write")
		dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).get_file_path(k3d::ipath_property::WRITE, Type, Message, old_path, new_path);
	else
		throw std::invalid_argument("unknown file path operation, should be \"read\" or \"write\"");

	return new_path;
}

static void synchronize(iunknown_wrapper& Self)
{
	dynamic_cast<k3d::iuser_interface&>(Self.wrapped()).synchronize();
}

void define_methods_iuser_interface(k3d::iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<k3d::iuser_interface*>(&Interface))
		return;

	utility::add_method(utility::make_function(&open_uri,
		"Opens a Uniform Resource Identifier (URI) in the user's preferred application.\n"
		"@param URI: The URI string to be opened.\n"
		"@note: Depending on the user interface plugin, this might open an application, "
		"print a message to the console, display a dialog box, or do nothing."), "open_uri", Instance);
	utility::add_method(utility::make_function(&message,
		"Displays an information message to the user.\n"
		"@param message: The message string to be displayed.\n" 
		"@note: Depending on the user interface plugin, this might print a message "
		"to the console, display a dialog box, or do nothing."), "message", Instance);
	utility::add_method(utility::make_function(&warning_message,
		"Displays a warning message to the user.\n"
		"@param message: The warning message string to be displayed.\n" 
		"@note: Depending on the user interface plugin, this might print a message "
		"to the console, display a dialog box, or do nothing."), "warning_message", Instance);
	utility::add_method(utility::make_function(&error_message,
		"Displays a warning message to the user.\n"
		"@param message: The error message string to be displayed.\n" 
		"@note: Depending on the user interface plugin, this might print a message "
		"to the console, display a dialog box, or do nothing."), "error_message", Instance);
	utility::add_method(utility::make_function(&query_message,
		"Query the user to choose from among a set of fixed options.\n"
		"@param message: The error message string to be displayed.\n" 
		"@param options: A list of strings where each string is an option to be displayed.\n" 
		"@return: Returns the one-based index of the option chosen by the user, or zero if "
		"the user cancelled the query.\n" 
		"@note: Depending on the user interface plugin, this might print a message "
		"to the console, display a dialog box, or do nothing."), "query_message", Instance);
	utility::add_method(utility::make_function(&get_file_path,
		"Query the user to choose a filesystem path.\n"
		"@param direction: Either \"read\" or \"write\".\n"
		"@param type: The type of path to be accessed (used to group most-recently-used paths.\n"
		"@param message: Message string displayed to the user.\n" 
		"@param start: The initial path to be displayed to the user.\n"
		"@return: Returns the path chosen by the as a string, empty string if the user cancels.\n"
		"@note: Depending on the user interface plugin, this might print a message "
		"to the console, display a dialog box, or do nothing."), "get_file_path", Instance);
	utility::add_method(utility::make_function(&synchronize,
		"Gives the user interface a chance to synchronize itself with the current application state."), "synchronize", Instance);
}

} // namespace python

} // namespace k3d

