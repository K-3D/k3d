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

#include "bilinear_patch.h"
#include "metadata_keys.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace bilinear_patch
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::selection_t& PatchSelections,
	const mesh::materials_t& PatchMaterials,
	const mesh::indices_t& PatchPoints,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::selection_t& PatchSelections,
	mesh::materials_t& PatchMaterials,
	mesh::indices_t& PatchPoints,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("bilinear_patch");

	primitive* const result = new primitive(
		generic_primitive.topology.create<mesh::selection_t >("patch_selections"),
		generic_primitive.topology.create<mesh::materials_t >("patch_materials"),
		generic_primitive.topology.create<mesh::indices_t >("patch_points"),
		generic_primitive.attributes["constant"],
		generic_primitive.attributes["uniform"],
		generic_primitive.attributes["varying"]
		);

	result->patch_points.set_metadata_value(metadata::key::domain(), metadata::value::mesh_point_indices_domain());

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// validate

const_primitive* validate(const mesh::primitive& Primitive)
{
	if(Primitive.type != "bilinear_patch")
		return 0;

	try
	{
		const mesh::selection_t& patch_selections = require_const_array<mesh::selection_t >(Primitive, "patch_selections");
		const mesh::materials_t& patch_materials = require_const_array<mesh::materials_t >(Primitive, "patch_materials");
		const mesh::indices_t& patch_points = require_const_array<mesh::indices_t >(Primitive, "patch_points");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, patch_materials, "patch_materials", patch_selections.size());
		require_array_size(Primitive, patch_points, "patch_points", patch_selections.size() * 4);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", patch_selections.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", patch_selections.size() * 4);

		return new const_primitive(patch_selections, patch_materials, patch_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "bilinear_patch")
		return 0;

	try
	{
		mesh::selection_t& patch_selections = require_array<mesh::selection_t >(Primitive, "patch_selections");
		mesh::materials_t& patch_materials = require_array<mesh::materials_t >(Primitive, "patch_materials");
		mesh::indices_t& patch_points = require_array<mesh::indices_t >(Primitive, "patch_points");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& uniform_data = require_attribute_arrays(Primitive, "uniform");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		require_array_size(Primitive, patch_materials, "patch_materials", patch_selections.size());
		require_array_size(Primitive, patch_points, "patch_points", patch_selections.size() * 4);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", patch_selections.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", patch_selections.size() * 4);

		return new primitive(patch_selections, patch_materials, patch_points, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace bilinear_patch

} // namespace k3d

