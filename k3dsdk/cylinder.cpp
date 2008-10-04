// K-3D
// Copyright (c) 1995-200 7, Timothy M. Shead
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

#include "cylinder.h"

namespace k3d
{

namespace cylinder
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const typed_array<matrix4>& Matrices,
	const typed_array<imaterial*>& Materials,
	const typed_array<double_t>& Radii,
	const typed_array<double_t>& ZMin,
	const typed_array<double_t>& ZMax,
	const typed_array<double_t>& SweepAngles,
	const attribute_arrays& ConstantData,
	const attribute_arrays& UniformData,
	const attribute_arrays& VaryingData) :
	matrices(Matrices),
	materials(Materials),
	radii(Radii),
	z_min(ZMin),
	z_max(ZMax),
	sweep_angles(SweepAngles),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	typed_array<matrix4>& Matrices,
	typed_array<imaterial*>& Materials,
	typed_array<double_t>& Radii,
	typed_array<double_t>& ZMin,
	typed_array<double_t>& ZMax,
	typed_array<double_t>& SweepAngles,
	attribute_arrays& ConstantData,
	attribute_arrays& UniformData,
	attribute_arrays& VaryingData) :
	matrices(Matrices),
	materials(Materials),
	radii(Radii),
	z_min(ZMin),
	z_max(ZMax),
	sweep_angles(SweepAngles),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("cylinder");

	return new primitive(
		generic_primitive.topology.create<typed_array<matrix4> >("matrices"),
		generic_primitive.topology.create<typed_array<imaterial*> >("materials"),
		generic_primitive.topology.create<typed_array<double_t> >("radii"),
		generic_primitive.topology.create<typed_array<double_t> >("z_min"),
		generic_primitive.topology.create<typed_array<double_t> >("z_max"),
		generic_primitive.topology.create<typed_array<double_t> >("sweep_angles"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "cylinder")
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
	const typed_array<double_t>* const radii = GenericPrimitive.topology.lookup<typed_array<double_t> >("radii");
	if(!radii)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing radii array" << std::endl;
		return 0;
	}
	const typed_array<double_t>* const z_min = GenericPrimitive.topology.lookup<typed_array<double_t> >("z_min");
	if(!z_min)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing z_min array" << std::endl;
		return 0;
	}
	const typed_array<double_t>* const z_max = GenericPrimitive.topology.lookup<typed_array<double_t> >("z_max");
	if(!z_max)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing z_max array" << std::endl;
		return 0;
	}
	const typed_array<double_t>* const sweep_angles = GenericPrimitive.topology.lookup<typed_array<double_t> >("sweep_angles");
	if(!sweep_angles)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing sweep_angles array" << std::endl;
		return 0;
	}
	const attribute_arrays* const constant_data = GenericPrimitive.attributes.lookup("constant");
	if(!constant_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing constant attribute data" << std::endl;
		return 0;
	}
	const attribute_arrays* const uniform_data = GenericPrimitive.attributes.lookup("uniform");
	if(!uniform_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing uniform attribute data" << std::endl;
		return 0;
	}
	const attribute_arrays* const varying_data = GenericPrimitive.attributes.lookup("varying");
	if(!varying_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing varying attribute data" << std::endl;
		return 0;
	}

	if(!(
		matrices->size() == materials->size()
		&& matrices->size() == radii->size()
		&& matrices->size() == z_min->size()
		&& matrices->size() == z_max->size()
		&& matrices->size() == sweep_angles->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per cylinder" << std::endl;
		return 0;
	}

	if(!uniform_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] uniform attributes must contain one value per cylinder" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(matrices->size() * 4))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain four values per cylinder" << std::endl;
		return 0;
	}

	return new const_primitive(*matrices, *materials, *radii, *z_min, *z_max, *sweep_angles, *constant_data, *uniform_data, *varying_data);
}

primitive* validate(mesh::primitive& GenericPrimitive)
{
	if(GenericPrimitive.type != "cylinder")
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
	typed_array<double_t>* const radii = GenericPrimitive.topology.writable<typed_array<double_t> >("radii");
	if(!radii)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing radii array" << std::endl;
		return 0;
	}
	typed_array<double_t>* const z_min = GenericPrimitive.topology.writable<typed_array<double_t> >("z_min");
	if(!z_min)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing z_min array" << std::endl;
		return 0;
	}
	typed_array<double_t>* const z_max = GenericPrimitive.topology.writable<typed_array<double_t> >("z_max");
	if(!z_max)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing z_max array" << std::endl;
		return 0;
	}
	typed_array<double_t>* const sweep_angles = GenericPrimitive.topology.writable<typed_array<double_t> >("sweep_angles");
	if(!sweep_angles)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing sweep_angles array" << std::endl;
		return 0;
	}
	attribute_arrays* const constant_data = GenericPrimitive.attributes.writable("constant");
	if(!constant_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing constant attribute data" << std::endl;
		return 0;
	}
	attribute_arrays* const uniform_data = GenericPrimitive.attributes.writable("uniform");
	if(!uniform_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing uniform attribute data" << std::endl;
		return 0;
	}
	attribute_arrays* const varying_data = GenericPrimitive.attributes.writable("varying");
	if(!varying_data)
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive missing varying attribute data" << std::endl;
		return 0;
	}

	if(!(
		matrices->size() == materials->size()
		&& matrices->size() == radii->size()
		&& matrices->size() == z_min->size()
		&& matrices->size() == z_max->size()
		&& matrices->size() == sweep_angles->size()
		))
	{
		log() << error << "[" << GenericPrimitive.type << "] primitive array-length mismatch" << std::endl;
		return 0;
	}

	if(!constant_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] constant attributes must contain one value per cylinder" << std::endl;
		return 0;
	}

	if(!uniform_data->match_size(matrices->size()))
	{
		log() << error << "[" << GenericPrimitive.type << "] uniform attributes must contain one value per cylinder" << std::endl;
		return 0;
	}

	if(!varying_data->match_size(matrices->size() * 4))
	{
		log() << error << "[" << GenericPrimitive.type << "] varying attributes must contain four values per cylinder" << std::endl;
		return 0;
	}

	return new primitive(*matrices, *materials, *radii, *z_min, *z_max, *sweep_angles, *constant_data, *uniform_data, *varying_data);
}

} // namespace cylinder

} // namespace k3d

