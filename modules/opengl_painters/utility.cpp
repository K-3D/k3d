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

/** \file utility.cpp
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\created May 4, 2009
*/

#include "utility.h"

namespace module
{

namespace opengl
{

namespace painters
{

k3d::bool_t has_non_empty_polyhedra(const k3d::mesh& Mesh)
{
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(polyhedron.get() && polyhedron->face_first_loops.size())
		{
			return true;
		}
	}
	return false;
}

k3d::bool_t has_non_sds_polyhedra(const k3d::mesh& Mesh)
{
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(polyhedron.get() && !k3d::polyhedron::is_sds(*polyhedron) && polyhedron->face_first_loops.size())
		{
			return true;
		}
	}
	return false;
}

k3d::bool_t has_sds_polyhedra(const k3d::mesh& Mesh)
{
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(polyhedron.get() && k3d::polyhedron::is_sds(*polyhedron) && polyhedron->face_first_loops.size())
		{
			return true;
		}
	}
	return false;
}

} // namespace painters

} // namespace opengl

} // namespace module
