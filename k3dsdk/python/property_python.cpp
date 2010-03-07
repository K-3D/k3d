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

#include <k3dsdk/property.h>
#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iunknown_python.h>

#include <boost/python.hpp>

namespace k3d
{

namespace python
{

class property
{
public:
	static void connect(idocument_wrapper& Document, iunknown_wrapper& From, iunknown_wrapper& To)
	{
		k3d::iproperty* const from = From.wrapped_ptr<k3d::iproperty>();
		if(!from)
			throw std::invalid_argument("From argument must be a valid property object.");

		k3d::iproperty* const to = To.wrapped_ptr<k3d::iproperty>();
		if(!to)
			throw std::invalid_argument("To argument must be a valid property object.");

		k3d::property::connect(Document.wrapped(), *from, *to);
	}

	static void disconnect(idocument_wrapper& Document, iunknown_wrapper& Property)
	{
		k3d::iproperty* const property = Property.wrapped_ptr<k3d::iproperty>();
		if(!property)
			throw std::invalid_argument("Argument must be a valid property.");

		k3d::property::disconnect(Document.wrapped(), *property);
	}
};

void define_namespace_property()
{
	boost::python::scope outer = boost::python::class_<property>("property", boost::python::no_init)

		.def("connect", property::connect,
			"Makes a pipeline connection between two properties.")
		.staticmethod("connect")

		.def("disconnect", property::disconnect,
			"Breaks the pipeline connection (if any) to the given property.")
		.staticmethod("connect")
		;
}

} // namespace python

} // namespace k3d

