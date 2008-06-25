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

#include "parallel_python.h"

#include <k3dsdk/parallel/threads.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

class parallel
{
public:
};


void define_parallel_namespace()
{
	class_<parallel>("parallel", no_init)
		.def("grain_size", k3d::parallel::grain_size,
			"Returns the global grain size to be used for parallel computation.")
		.def("set_grain_size", k3d::parallel::set_grain_size,
			"Sets the global grain size to be used for parallel computation.")
		.def("set_thread_count", k3d::parallel::set_thread_count,
			"Sets the number of threads to be used for parallel computation (quietly ignored if parallel computation wasn't enabled in the build.")
		.staticmethod("grain_size")
		.staticmethod("set_grain_size")
		.staticmethod("set_thread_count");

}

} // namespace python

} // namespace k3d

