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

#include "uuid_python.h"

#include <k3dsdk/uuid.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

int uuid_len(const k3d::uuid& Value)
{
	return 4;
}

unsigned long uuid_getitem(const k3d::uuid& LHS, const int Item)
{
	switch(Item)
	{
		case 0:
			return LHS.data1;
		case 1:
			return LHS.data2;
		case 2:
			return LHS.data3;
		case 3:
			return LHS.data4;
	}

	throw std::out_of_range("index out of range");
}

void uuid_setitem(k3d::uuid& LHS, const int Item, unsigned long Value)
{
	switch(Item)
	{
		case 0:
			LHS.data1 = Value;
			return;
		case 1:
			LHS.data2 = Value;
			return;
		case 2:
			LHS.data3 = Value;
			return;
		case 3:
			LHS.data4 = Value;
			return;
	}

	throw std::out_of_range("index out of range");
}

void export_uuid()
{
	class_<k3d::uuid>("uuid")
		.def(init<unsigned long, unsigned long, unsigned long, unsigned long>())
		.def("null", &k3d::uuid::null)
		.staticmethod("null")
		.def("__len__", uuid_len)
		.def("__getitem__", uuid_getitem)
		.def("__setitem__", uuid_setitem)
		.def(self < self)
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

