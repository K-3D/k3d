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

primitive::primitive(const typed_array<matrix4>& Matrices, const typed_array<imaterial*>& Materials, const attribute_arrays& UniformData) :
	matrices(Matrices),
	materials(Materials),
	uniform_data(UniformData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// writable_primitive

writable_primitive::writable_primitive(typed_array<matrix4>& Matrices, typed_array<imaterial*>& Materials, attribute_arrays& UniformData) :
	matrices(Matrices),
	materials(Materials),
	uniform_data(UniformData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

writable_primitive create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("teapots");

	writable_primitive result(
		generic_primitive.topology.create<typed_array<matrix4> >("matrices"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.attributes["uniform"]
		);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

primitive* validate(const mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "teapots")
		return 0;

	const typed_array<matrix4>* const matrices = GenericPrimitive.topology.lookup<typed_array<matrix4> >("matrices");
	if(!matrices)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing matrices array" << std::endl;
		return 0;
	}
	const typed_array<imaterial*>* const materials = GenericPrimitive.topology.lookup<typed_array<imaterial*> >("materials");
	if(!materials)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing materials array" << std::endl;
		return 0;
	}
	const attribute_arrays* const uniform_data = GenericPrimitive.attributes.lookup("uniform");
	if(!uniform_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive uniform attribute data" << std::endl;
		return 0;
	}

	if(matrices->size() != materials->size())
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	return new primitive(*matrices, *materials, *uniform_data);
}

writable_primitive* validate(mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "teapots")
		return 0;

	typed_array<matrix4>* const matrices = GenericPrimitive.topology.writable<typed_array<matrix4> >("matrices");
	if(!matrices)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing matrices array" << std::endl;
		return 0;
	}
	typed_array<imaterial*>* const materials = GenericPrimitive.topology.writable<typed_array<imaterial*> >("materials");
	if(!materials)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing materials array" << std::endl;
		return 0;
	}
	attribute_arrays* const uniform_data = GenericPrimitive.attributes.writable("uniform");
	if(!uniform_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive uniform attribute data" << std::endl;
		return 0;
	}

	if(matrices->size() != materials->size())
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	return new writable_primitive(*matrices, *materials, *uniform_data);
}

} // namespace teapots

} // namespace k3d

