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

#include "iselectable_python.h"

#include <k3dsdk/iselectable.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

iselectable::iselectable() :
	base()
{
}

iselectable::iselectable(k3d::iselectable* Selectable) :
	base(Selectable)
{
}

const double iselectable::get_selection_weight()
{
	return wrapped().get_selection_weight();
}

void iselectable::set_selection_weight(const double Weight)
{
	wrapped().set_selection_weight(Weight);
}

void export_iselectable()
{
	class_<iselectable>("iselectable")
		.add_property("selection_weight", &iselectable::get_selection_weight, &iselectable::set_selection_weight);
}

} // namespace python

} // namespace k3d

