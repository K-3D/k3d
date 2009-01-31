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

#include "idocument_importer_python.h"
#include "idocument_python.h"

#include <k3dsdk/path.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////
// idocument_importer

static bool_t read_file(idocument_importer_wrapper& Self, idocument_wrapper& Document, const filesystem::path& Path)
{
	return Self.wrapped().read_file(Document.wrapped(), Path);
}

void define_class_idocument_importer()
{
	class_<idocument_importer_wrapper>("idocument_importer",
		"Abstract interface for objects that can import file data into a document.", no_init)
		.def("read_file", &read_file,
			"Import a file's contents into a document.")
		;
}

} // namespace python

} // namespace k3d

