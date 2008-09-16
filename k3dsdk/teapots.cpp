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

#include "teapots.h"

namespace k3d
{

namespace teapots
{

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive() :
	matrices(0),
	materials(0),
	uniform_data(0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

void create(mesh& Mesh, primitive& Primitive)
{
	k3d::mesh::primitive& generic_primitive = Mesh.primitives.create("teapots");
	Primitive.matrices = &generic_primitive.topology.create<k3d::typed_array<k3d::matrix4> >("matrices");
	Primitive.materials = &generic_primitive.topology.create<k3d::typed_array<k3d::imaterial*> >("materials");
	Primitive.uniform_data = &generic_primitive.attributes["uniform"];
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const bool_t validate(const mesh::primitive& GenericPrimitive, primitive& Primitive)
{
	if(GenericPrimitive.type != "teapots")
		return false;

	Primitive.matrices = GenericPrimitive.topology.lookup<k3d::typed_array<k3d::matrix4> >("matrices");
	if(!Primitive.matrices)
	{
		k3d::log() << error << "[" << GenericPrimitive.type << "] primitive missing matrices array" << std::endl;
		return false;
	}
	Primitive.materials = GenericPrimitive.topology.lookup<k3d::typed_array<k3d::imaterial*> >("materials");
	if(!Primitive.materials)
	{
		k3d::log() << error << "[" << GenericPrimitive.type << "] primitive missing materials array" << std::endl;
		return false;
	}
	Primitive.uniform_data = GenericPrimitive.attributes.lookup("uniform");
	if(!Primitive.uniform_data)
	{
		k3d::log() << error << "[" << GenericPrimitive.type << "] primitive uniform attribute data" << std::endl;
		return false;
	}

	if(Primitive.matrices->size() != Primitive.materials->size())
	{
		k3d::log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return false;
	}

	return true;
}

} // namespace teapots

} // namespace k3d

