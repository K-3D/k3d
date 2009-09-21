#ifndef MODULES_NURBS_NURBS_CURVES_H
#define MODULES_NURBS_NURBS_CURVES_H
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

/** \file
	\author Carsten Haubold (CarstenHaubold@web.de)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/types.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace nurbs
{

/// Adds the given curve to the other primitive and mesh
void add_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve);

/// Deletes a curve
void delete_curve(k3d::nurbs_curve::primitive& Curves, const k3d::uint_t Curve);

/// Normalizes the knot vector of the given curve number in the given NURBS curve primitive
void normalize_knot_vector(k3d::nurbs_curve::primitive& NurbsCurve, const k3d::uint_t Curve);

///Replace a point on the curve with a new one (by changing the index)
/**\param newIndex the index of the point which shall be used from now on
 * \param curve The curve we're operating on
 * \param point The index of the point which shall be replaced
 * \param continuous If this is set to true, the curve's knot vector is changed so that the curve intersects the point
 **/
void replace_point(k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t newIndex, k3d::uint_t curve, k3d::uint_t point, bool continuous);

/// Replace duplicate point indices in the mesh
void replace_duplicate_points(k3d::mesh& Mesh);

///Close the selected curve either by adding a new point or by replacing the former end points by their average
/** \param curve The curve to close
 *  \param keep_ends If this is true then we're going to add a new point, otherwise the old end points get discarded
 **/
void close_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::bool_t keep_ends);

/// Elevate the degree of the curves listed in Curves, storing the resulting mesh in OutputMesh and OutputCurves
void elevate_curve_degree(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::uint_t Elevations = 1);

/// Connect the two input curves at their selected end points, storing the result in Output
void connect_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::uint_t Primitive1Index, const k3d::uint_t Curve1Index, const k3d::bool_t Curve1FirstPointSelection, const k3d::uint_t Primitive2Index, const k3d::uint_t Curve2Index, const k3d::bool_t Curve2FirstPointSelection);

/// Visitor to get the selected curves
struct selected_curves_extractor
{
	selected_curves_extractor(k3d::mesh::indices_t& SelectedCurves) : selected_curves(SelectedCurves) {}
	void operator()(const k3d::string_t& StructureName, const k3d::table& Structure, const k3d::string_t& ArrayName, const k3d::pipeline_data<k3d::array>& Array)
	{
		if(StructureName == "curve" && Array->get_metadata_value(k3d::metadata::key::role()) == k3d::metadata::value::selection_role())
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
};

/// Apply a modifier to the selected curves in OutputMesh
template <typename FunctorT>
void modifiy_selected_curves(const k3d::mesh& InputMesh, k3d::mesh& OutputMesh, FunctorT Modifier)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != InputMesh.primitives.size(); ++prim_idx)
	{
		k3d::mesh::indices_t selected_curves;
		k3d::mesh::visit_arrays(*OutputMesh.primitives[prim_idx], selected_curves_extractor(selected_curves));
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
					Modifier(OutputMesh, *output_curves, InputMesh, *input_curves, curve);
				else
					add_curve(OutputMesh, *output_curves, InputMesh, *input_curves, curve);
			}
		}
	}
	replace_duplicate_points(OutputMesh);
	k3d::mesh::delete_unused_points(OutputMesh);
}

/// TODO: Move to SDK
void delete_empty_primitives(k3d::mesh& Mesh);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_CURVES_H
