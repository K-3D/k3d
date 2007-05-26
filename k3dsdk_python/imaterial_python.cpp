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

#include "imaterial_python.h"

#include <k3dsdk/imaterial.h>
#include <boost/python.hpp>
using namespace boost::python;

namespace k3d 
{

namespace python
{

imaterial::imaterial() :
	base()
{
}

imaterial::imaterial(k3d::imaterial* Material) :
	base(Material)
{
}

void imaterial::define_class()
{
	class_<imaterial>("imaterial", no_init);
}

} // namespace python

} // namespace k3d

