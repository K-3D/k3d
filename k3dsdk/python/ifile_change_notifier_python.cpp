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

#include "ifile_change_notifier_python.h"
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/ifile_change_notifier.h>
#include <k3dsdk/path.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////
// file_change_receiver

class file_change_receiver
{
public:
	void file_changed(const filesystem::path& Path)
	{
		changed_files.push_back(Path);
	}

	boost::python::list changed()
	{
		boost::python::list result;
		for(k3d::uint_t i = 0; i != changed_files.size(); ++i)
			result.append(changed_files[i]);
		return result;
	}

	void clear()
	{
		changed_files.clear();
	}

private:
	std::vector<filesystem::path> changed_files;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// ifile_change_notifier

static uint_t watch_file(iunknown_wrapper& Self, const filesystem::path& Path, file_change_receiver& Receiver)
{
	return dynamic_cast<ifile_change_notifier&>(Self.wrapped()).watch_file(Path, sigc::bind(sigc::mem_fun(Receiver, &file_change_receiver::file_changed), Path));
}

static void unwatch_file(iunknown_wrapper& Self, const uint_t WatchID)
{
	dynamic_cast<ifile_change_notifier&>(Self.wrapped()).unwatch_file(WatchID);
}

static void wait_for_changes(iunknown_wrapper& Self)
{
	dynamic_cast<ifile_change_notifier&>(Self.wrapped()).wait_for_changes();
}

static uint_t change_count(iunknown_wrapper& Self)
{
	return dynamic_cast<ifile_change_notifier&>(Self.wrapped()).change_count();
}

static void signal_change(iunknown_wrapper& Self)
{
	dynamic_cast<ifile_change_notifier&>(Self.wrapped()).signal_change();
}

void define_class_file_change_receiver()
{
	class_<file_change_receiver>("file_change_receiver",
		"Helper class that caches file-change notifications for later retrieval.")
		.def("changed", &file_change_receiver::changed,
			"Retrieves the list of files that have changed.")
		.def("clear", &file_change_receiver::clear,
			"Clears the list of files that have changed.")
		;
}

void define_methods_ifile_change_notifier(k3d::iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<ifile_change_notifier*>(&Interface))
		return;

	utility::add_method(utility::make_function(&watch_file, "Watch a file for changes.\n\n" "@rtype: integer watch identifier\n"), "watch_file", Instance);
	utility::add_method(utility::make_function(&unwatch_file, "Stops watching a file for changes."), "unwatch_file", Instance);
	utility::add_method(utility::make_function(&wait_for_changes, "Blocks indefinitely until at least one file change has been received."), "wait_for_changes", Instance);
	utility::add_method(utility::make_function(&change_count, "Returns the number of file changes that are pending, ready to be signalled.  This method never blocks."), "change_count", Instance);
	utility::add_method(utility::make_function(&signal_change, "Signals the next file change that is pending, if any.  This method never blocks."), "signal_change", Instance);
}

} // namespace python

} // namespace k3d

