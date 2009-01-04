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

/** \file
		\author Ashish Myles (marcianx@gmail.com)
*/

#include "bezier_triangle_patch.h"
#include "metadata_keys.h"
#include "primitive_detail.h"

#include <numeric>

namespace k3d
{

namespace bezier_triangle_patch
{

/////////////////////////////////////////////////////////////////////////////////////////////
// const_primitive

const_primitive::const_primitive(
	const mesh::indices_t& PatchFirstPoints,
	const mesh::orders_t& PatchOrders,
	const mesh::selection_t& PatchSelections,
	const mesh::materials_t& PatchMaterials,
	const mesh::indices_t& PatchPoints,
	const mesh::weights_t& PatchPointWeights,
	const mesh::attribute_arrays_t& ConstantData,
	const mesh::attribute_arrays_t& UniformData,
	const mesh::attribute_arrays_t& VaryingData
		) :
	patch_first_points(PatchFirstPoints),
	patch_orders(PatchOrders),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// primitive

primitive::primitive(
	mesh::indices_t& PatchFirstPoints,
	mesh::orders_t& PatchOrders,
	mesh::selection_t& PatchSelections,
	mesh::materials_t& PatchMaterials,
	mesh::indices_t& PatchPoints,
	mesh::weights_t& PatchPointWeights,
	mesh::attribute_arrays_t& ConstantData,
	mesh::attribute_arrays_t& UniformData,
	mesh::attribute_arrays_t& VaryingData
		) :
	patch_first_points(PatchFirstPoints),
	patch_orders(PatchOrders),
	patch_selections(PatchSelections),
	patch_materials(PatchMaterials),
	patch_points(PatchPoints),
	patch_point_weights(PatchPointWeights),
	constant_data(ConstantData),
	uniform_data(UniformData),
	varying_data(VaryingData)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
// create

primitive* create(mesh& Mesh)
{
	mesh::primitive& generic_primitive = Mesh.primitives.create("bezier_triangle_patch");

	primitive* const result = new primitive(
		generic_primitive.structure.create<mesh::indices_t >("patch_first_points"),
		generic_primitive.structure.create<mesh::orders_t >("patch_orders"),
		generic_primitive.structure.create<mesh::selection_t >("patch_selections"),
		generic_primitive.structure.create<mesh::materials_t >("patch_materials"),
		generic_primitive.structure.create<mesh::indices_t >("patch_points"),
		generic_primitive.structure.create<mesh::weights_t >("patch_point_weights"),
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
	if(Primitive.type != "bezier_triangle_patch")
		return 0;

	try
	{
		const mesh::indices_t& patch_first_points = require_const_array<mesh::indices_t >(Primitive, "patch_first_points");
		const mesh::orders_t& patch_orders = require_const_array<mesh::orders_t >(Primitive, "patch_orders");
		const mesh::selection_t& patch_selections = require_const_array<mesh::selection_t >(Primitive, "patch_selections");
		const mesh::materials_t& patch_materials = require_const_array<mesh::materials_t >(Primitive, "patch_materials");
		const mesh::indices_t& patch_points = require_const_array<mesh::indices_t >(Primitive, "patch_points");
		const mesh::weights_t& patch_point_weights = require_const_array<mesh::weights_t >(Primitive, "patch_point_weights");

		const attribute_arrays& constant_data = require_const_attribute_arrays(Primitive, "constant");
		const attribute_arrays& uniform_data = require_const_attribute_arrays(Primitive, "uniform");
		const attribute_arrays& varying_data = require_const_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());
		//require_metadata(Primitive, patch_first_points, "patch_first_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		const k3d::uint_t num_patches = patch_selections.size();
		k3d::uint_t num_control_points = 0;

		require_array_size(Primitive, patch_materials, "patch_materials", num_patches);
		require_array_size(Primitive, patch_orders, "patch_orders", num_patches);
		require_array_size(Primitive, patch_first_points, "patch_first_points", num_patches);

		for(k3d::uint_t patch = 0; patch < num_patches; ++patch)
		{
			const k3d::uint_t patch_begin = patch_first_points[patch];
			const k3d::uint_t patch_end = patch_begin + patch_orders[patch];
			require_array_size(Primitive, patch_points, "patch_points", patch_selections.size() * 16);

			const k3d::uint_t patch_size = (patch_orders[patch] * (patch_orders[patch] + 1)) / 2;
			num_control_points += patch_size;
			if (patch < num_patches-1 && patch_first_points[patch] + patch_size != patch_first_points[patch+1])
			{
				std::ostringstream buffer;
				buffer << "[" << Primitive.type << "] primitive [patch_first_points[" << (patch+1)
				       << "]] incorrect value [" << patch_first_points[patch+1]
				       << "], expected [" << (patch_first_points[patch] + patch_size) << "]";
				throw std::runtime_error(buffer.str());
			}
		}
		require_array_size(Primitive, patch_points, "patch_points", num_control_points);
		require_array_size(Primitive, patch_point_weights, "patch_point_weights", num_control_points);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", patch_selections.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", patch_selections.size() * 4);

		return new const_primitive(patch_first_points, patch_orders, patch_selections, patch_materials, patch_points, patch_point_weights, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

primitive* validate(mesh::primitive& Primitive)
{
	if(Primitive.type != "bezier_triangle_patch")
		return 0;

	try
	{
		mesh::indices_t& patch_first_points = require_array<mesh::indices_t >(Primitive, "patch_first_points");
		mesh::orders_t& patch_orders = require_array<mesh::orders_t >(Primitive, "patch_orders");
		mesh::selection_t& patch_selections = require_array<mesh::selection_t >(Primitive, "patch_selections");
		mesh::materials_t& patch_materials = require_array<mesh::materials_t >(Primitive, "patch_materials");
		mesh::indices_t& patch_points = require_array<mesh::indices_t >(Primitive, "patch_points");
		mesh::weights_t& patch_point_weights = require_array<mesh::weights_t >(Primitive, "patch_point_weights");

		attribute_arrays& constant_data = require_attribute_arrays(Primitive, "constant");
		attribute_arrays& uniform_data = require_attribute_arrays(Primitive, "uniform");
		attribute_arrays& varying_data = require_attribute_arrays(Primitive, "varying");

		require_metadata(Primitive, patch_points, "patch_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());
		//require_metadata(Primitive, patch_first_points, "patch_first_points", metadata::key::domain(), metadata::value::mesh_point_indices_domain());

		const k3d::uint_t num_patches = patch_selections.size();
		k3d::uint_t num_control_points = 0;

		require_array_size(Primitive, patch_materials, "patch_materials", patch_selections.size());
		require_array_size(Primitive, patch_orders, "patch_orders", num_patches);
		require_array_size(Primitive, patch_first_points, "patch_first_points", num_patches);

		for(k3d::uint_t patch = 0; patch < num_patches; ++patch)
		{
			const k3d::uint_t patch_begin = patch_first_points[patch];
			const k3d::uint_t patch_end = patch_begin + patch_orders[patch];
			require_array_size(Primitive, patch_points, "patch_points", patch_selections.size() * 16);

			const k3d::uint_t patch_size = (patch_orders[patch] * (patch_orders[patch] + 1)) / 2;
			num_control_points += patch_size;
			if (patch < num_patches-1 && patch_first_points[patch] + patch_size != patch_first_points[patch+1])
			{
				std::ostringstream buffer;
				buffer << "[" << Primitive.type << "] primitive [patch_first_points[" << (patch+1)
				       << "]] incorrect value [" << patch_first_points[patch+1]
				       << "], expected [" << (patch_first_points[patch] + patch_size) << "]";
				throw std::runtime_error(buffer.str());
			}
		}
		require_array_size(Primitive, patch_points, "patch_points", num_control_points);
		require_array_size(Primitive, patch_point_weights, "patch_point_weights", num_control_points);

		require_attribute_arrays_size(Primitive, constant_data, "constant", 1);
		require_attribute_arrays_size(Primitive, uniform_data, "uniform", patch_selections.size());
		require_attribute_arrays_size(Primitive, varying_data, "varying", patch_selections.size() * 4);

		return new primitive(patch_first_points, patch_orders, patch_selections, patch_materials, patch_points, patch_point_weights, constant_data, uniform_data, varying_data);
	}
	catch(std::exception& e)
	{
		log() << error << e.what() << std::endl;
	}

	return 0;
}

} // namespace bezier_triangle_patch

} // namespace k3d

