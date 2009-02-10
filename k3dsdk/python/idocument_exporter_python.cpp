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

#include "idocument_exporter_python.h"
#include "idocument_python.h"
#include "iunknown_python.h"
#include "utility_python.h"

#include <k3dsdk/idocument_exporter.h>
#include <k3dsdk/path.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////
// idocument_exporter

static bool_t write_file(iunknown_wrapper& Self, idocument_wrapper& Document, const filesystem::path& Path)
{
	return dynamic_cast<idocument_exporter&>(Self.wrapped()).write_file(Document.wrapped(), Path);
}

void define_methods_idocument_exporter(iunknown& Interface, boost::python::object& Instance)
{
	if(!dynamic_cast<idocument_exporter*>(&Interface))
		return;

	utility::add_method(make_function(write_file), "write_file", Instance);
}

} // namespace python

} // namespace k3d

