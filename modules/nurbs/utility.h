#ifndef MODULES_NURBS_UTILITY_H
#define MODULES_NURBS_UTILITY_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

/** \file Defines some functions to visit curves and patches in a mesh
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/types.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>
#include <stdexcept>

#include "nurbs_curves.h"
#include "nurbs_patches.h"

namespace module
{

namespace nurbs
{

/// TODO: Move to SDK
void delete_empty_primitives(k3d::mesh& Mesh);

/// Visitor to get the selected components
struct selected_component_extractor
{
	selected_component_extractor(k3d::mesh::indices_t& SelectedCurves, const k3d::string_t& ComponentName) : selected_curves(SelectedCurves), component_name(ComponentName) {}
	void operator()(const k3d::string_t& StructureName, const k3d::table& Structure, const k3d::string_t& ArrayName, const k3d::pipeline_data<k3d::array>& Array)
	{
		if(StructureName == component_name && Array->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
		{
			const k3d::mesh::selection_t* curve_selections = dynamic_cast<const k3d::mesh::selection_t*>(Array.get());
			for(k3d::uint_t curve = 0; curve != curve_selections->size(); ++curve)
			{
				if(curve_selections->at(curve))
					selected_curves.push_back(curve);
			}
		}
	}
	k3d::mesh::indices_t& selected_curves;
	const k3d::string_t component_name;
};

//////////////////////////////////////////////////////////////////
// Curve visiting functions
//////////////////////////////////////////////////////////////////

/// Visit all selected curves
template <typename FunctorT>
void visit_selected_curves(const k3d::mesh& Mesh, FunctorT Modifier)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != Mesh.primitives.size(); ++prim_idx)
	{
		k3d::mesh::indices_t selected_curves;
		k3d::mesh::visit_arrays(*Mesh.primitives[prim_idx], selected_component_extractor(selected_curves, "curve"));
		if(selected_curves.size())
		{
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves(k3d::nurbs_curve::validate(Mesh, *Mesh.primitives[prim_idx]));
			for(k3d::uint_t i = 0; i != selected_curves.size(); ++i)
			{
				const k3d::uint_t curve = selected_curves[i];
				try
				{
					Modifier(Mesh, *curves, curve);
				}
				catch(std::runtime_error& E)
				{
					k3d::log() << error << "Error visiting curve " << curve << " of primitive " << prim_idx << ": " << E.what() << std::endl;
				}
			}
		}
	}
}

/// Apply a modifier to the selected curves in OutputMesh
template <typename FunctorT>
void modify_selected_curves(const k3d::mesh& InputMesh, k3d::mesh& OutputMesh, FunctorT Modifier)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != InputMesh.primitives.size(); ++prim_idx)
	{
		k3d::mesh::indices_t selected_curves;
		k3d::mesh::visit_arrays(*OutputMesh.primitives[prim_idx], selected_component_extractor(selected_curves, "curve"));
		if(selected_curves.size())
		{
			boost::scoped_ptr<k3d::nurbs_curve::const_primitive> input_curves(k3d::nurbs_curve::validate(InputMesh, *InputMesh.primitives[prim_idx]));
			const k3d::uint_t curve_count = input_curves->curve_first_points.size();
			k3d::mesh::bools_t curve_selections(curve_count, false);
			for(k3d::uint_t i = 0; i != selected_curves.size(); ++i) curve_selections[selected_curves[i]] = true;
			boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves(k3d::nurbs_curve::create(OutputMesh.primitives[prim_idx].create(new k3d::mesh::primitive("nurbs_curve"))));
			for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
			{
				if(curve_selections[curve])
				{
					try
					{
						Modifier(OutputMesh, *output_curves, InputMesh, *input_curves, curve);
					}
					catch(std::runtime_error& E)
					{
						k3d::log() << error << "Error modifiying curve " << curve << " of primitive " << prim_idx << ": " << E.what() << std::endl;
						add_curve(OutputMesh, *output_curves, InputMesh, *input_curves, curve);
					}
				}
				else
				{
					add_curve(OutputMesh, *output_curves, InputMesh, *input_curves, curve);
				}
				output_curves->curve_selections.back() = curve_selections[curve];
			}
			if(output_curves->material.empty())
				output_curves->material = input_curves->material;
		}
	}
	if(OutputMesh.points)
		k3d::mesh::delete_unused_points(OutputMesh);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// shared functors
//////////////////////////////////////////////////////////////////////////////////////////////

/// Calculates the maximum order of all visited curves
struct max_order_calculator
{
	max_order_calculator(k3d::double_t& Order) : order(Order)
	{
		order = 0;
	}
	void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
	{
		order = Curves.curve_orders[Curve] > order ? Curves.curve_orders[Curve] : order;
	}
	/// The maximum order of all curves visited
	k3d::double_t& order;
};

/// Calculates a common knot vector among the visited curves
struct knot_vector_calculator
{
	knot_vector_calculator(k3d::mesh::knots_t& Knots) : knots(Knots) {}
	void operator()(const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t& Curve)
	{
		append_common_knot_vector(knots, Curves, Curve);
	}
	/// The common knot vector for the visited curves
	k3d::mesh::knots_t& knots;
};

/// Applies NURBS curve degree elevation to reach the given order
struct degree_elevator
{
	degree_elevator(const k3d::uint_t Order) : order(Order) {}
	const k3d::double_t order;
	void operator()(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t& Curve)
	{
		elevate_curve_degree(OutputMesh, OutputCurves, InputMesh, InputCurves, Curve, order - InputCurves.curve_orders[Curve]);
	}
};

/// Makes UnifiedKnots the knot vector of all given curves
struct knot_vector_merger
{
	knot_vector_merger(const k3d::mesh::knots_t& UnifiedKnots, const k3d::uint_t Order);
	void operator()(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t& Curve);
	k3d::mesh::knots_t unified_knots;
	const k3d::uint_t order;
};

//////////////////////////////////////////////////////////////////
// Patch visiting functions
//////////////////////////////////////////////////////////////////

/// Apply a modifier to the selected patches in OutputMesh
template <typename FunctorT>
void modify_selected_patches(const k3d::mesh& InputMesh, k3d::mesh& OutputMesh, FunctorT Modifier)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != InputMesh.primitives.size(); ++prim_idx)
	{
		k3d::mesh::indices_t selected_patches;
		k3d::mesh::visit_arrays(*OutputMesh.primitives[prim_idx], selected_component_extractor(selected_patches, "patch"));
		if(selected_patches.size())
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> input_patches(k3d::nurbs_patch::validate(InputMesh, *InputMesh.primitives[prim_idx]));
			const k3d::uint_t patch_count = input_patches->patch_first_points.size();
			k3d::mesh::bools_t patch_selections(patch_count, false);
			for(k3d::uint_t i = 0; i != selected_patches.size(); ++i) patch_selections[selected_patches[i]] = true;
			boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(OutputMesh.primitives[prim_idx].create(new k3d::mesh::primitive("nurbs_patch"))));
			for(k3d::uint_t patch = 0; patch != patch_count; ++patch)
			{
				if(patch_selections[patch])
				{
					try
					{
						Modifier(OutputMesh, *output_patches, InputMesh, *input_patches, patch);
						output_patches->patch_selections.back() = patch_selections[patch];
					}
					catch(std::runtime_error& E)
					{
						k3d::log() << error << "Error modifying patch " << patch << " of primitive " << prim_idx << ": " << E.what() << std::endl;
						add_patch(OutputMesh, *output_patches, InputMesh, *input_patches, patch);
						output_patches->patch_materials[patch] = input_patches->patch_materials[patch];
					}
				}
				else
				{
					add_patch(OutputMesh, *output_patches, InputMesh, *input_patches, patch);
					output_patches->patch_materials[patch] = input_patches->patch_materials[patch];
				}
			}
		}
	}
	if(OutputMesh.points)
		k3d::mesh::delete_unused_points(OutputMesh);
}

/// Use with std::transform to normalize a knot vector
struct knot_normalizer
{
	knot_normalizer(const k3d::double_t Min, const k3d::double_t Max) : min(Min), max(Max) {}
	k3d::double_t min, max;
	const k3d::double_t operator()(const k3d::double_t Knot)
	{
		return (Knot - min) / (max - min);
	}
};

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_CURVES_H
