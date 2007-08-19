// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "isnappable_python.h"

#include <k3dsdk/explicit_snap_source.h>
#include <k3dsdk/explicit_snap_target.h>
#include <k3dsdk/isnappable.h>
#include <k3dsdk/result.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

isnappable::isnappable() :
	base()
{
}

isnappable::isnappable(k3d::isnappable* Node) :
	base(Node)
{
}

void isnappable::add_snap_source(const std::string& Label, const k3d::point3& Position)
{
	wrapped().add_snap_source(new k3d::explicit_snap_source(Label, Position));
}

void isnappable::add_snap_target(const std::string& Label, const k3d::point3& Position)
{
	wrapped().add_snap_target(new k3d::explicit_snap_target(Label, Position));
}

void isnappable::define_class()
{
	class_<isnappable>("isnappable",
		"Abstract interface implemented by nodes that can be interactive \"snapped\" to one another.", no_init)
		.def("add_snap_source", &isnappable::add_snap_source,
			"Adds a new snap source to the object.\n\n")
		.def("add_snap_target", &isnappable::add_snap_target,
			"Adds a new snap target to the object.\n\n")
		;
}

} // namespace python

} // namespace k3d

