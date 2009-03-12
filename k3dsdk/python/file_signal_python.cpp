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

#include "file_signal_python.h"

#include <k3dsdk/log.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

//////////////////////////////////////////////////////////////////////////////
// file_signal

sigc::connection file_signal::connect_output_signal(const sigc::slot<void, const string_t&>& Slot)
{
	return signal.connect(Slot);
}

void write(file_signal& Self, const string_t& Output)
{
	Self.signal.emit(Output);
}

//////////////////////////////////////////////////////////////////////////////
// define_class_file_signal

void define_class_file_signal()
{
	class_<file_signal>("file_signal", "Converts file output to a C++ signal.", no_init)
		.def("write", &write)
		;
}

} // namespace python

} // namespace k3d

