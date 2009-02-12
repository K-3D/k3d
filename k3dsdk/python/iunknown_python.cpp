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

#include "iunknown_python.h"

#include "idocument_exporter_python.h"
#include "idocument_importer_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

object wrap(iunknown* Unknown)
{
	return Unknown ? wrap(*Unknown) : object();
}

object wrap(iunknown& Unknown)
{
	object result = object(iunknown_wrapper(Unknown));

	define_methods_idocument_exporter(Unknown, result);
	define_methods_idocument_importer(Unknown, result);
	
	return result;
}

void define_class_iunknown()
{
	class_<iunknown_wrapper>("iunknown", 
		"Abstract interface that represents an object with unknown capabilities.\n\n"
		"Methods for other implemented interfaces are added dynamically at runtime.",
		no_init);
}

} // namespace python

} // namespace k3d

