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

#include "mime_python.h"

#include <k3dsdk/path.h>
#include <k3dsdk/mime_types.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class mime
{
public:

	class type
	{
	public:
		static const k3d::mime::type lookup_file(const filesystem::path& File)
		{
			return k3d::mime::type::lookup(File);
		}

		static const k3d::mime::type lookup_data(const string_t& Data)
		{
			return k3d::mime::type::lookup(Data);
		}

		static const string_t str(k3d::mime::type& lhs)
		{
			return lhs.str();
		}
	};
};

void define_mime_namespace()
{
	scope outer = class_<mime>("mime", no_init);

	class_<k3d::mime::type>("type")
		.def("empty", &k3d::mime::type::empty,
			"Returns true if the MIME type is empty (unknown).")
		.def("lookup", mime::type::lookup_file,
			"Returns the MIME type of a file (could return an empty type if the file cannot be identified).")
		.def("lookup", mime::type::lookup_data,
			"Returns the MIME type of a data buffer (could return an empty type if the data cannot be identified).")
		.def("str", &k3d::mime::type::str,
			"Returns the MIME type as a string.")
		.def("__str__", mime::type::str,
			"")
		.def(self == self)
		.staticmethod("lookup");
}

} // namespace python

} // namespace k3d

