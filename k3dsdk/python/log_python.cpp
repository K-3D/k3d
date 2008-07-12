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

#include "log_python.h"

#include <k3dsdk/log.h>
#include <k3dsdk/types.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class log
{
public:
	static void critical(const string_t& Message)
	{
		k3d::log() << k3d::critical << Message << std::endl;
	}

	static void debug(const string_t& Message)
	{
		k3d::log() << k3d::debug << Message << std::endl;
	}

	static void error(const string_t& Message)
	{
		k3d::log() << k3d::error << Message << std::endl;
	}

	static void info(const string_t& Message)
	{
		k3d::log() << k3d::info << Message << std::endl;
	}

	static void warning(const string_t& Message)
	{
		k3d::log() << k3d::warning << Message << std::endl;
	}
};

void define_namespace_log()
{
	scope outer = class_<log>("log", no_init)
		.def("critical", &log::critical,
			"Sends a critical message to the K-3D log.")
		.def("debug", &log::debug,
			"Sends a debug message to the K-3D log.")
		.def("error", &log::error,
			"Sends an error message to the K-3D log.")
		.def("info", &log::info,
			"Sends an informational message to the K-3D log.")
		.def("warning", &log::warning,
			"Sends a warning message to the K-3D log.")
		.staticmethod("critical")
		.staticmethod("debug")
		.staticmethod("error")
		.staticmethod("info")
		.staticmethod("warning");
}

} // namespace python

} // namespace k3d

