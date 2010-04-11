#ifndef MODULES_NURBS_NURBS_PATCHES_H
#define MODULES_NURBS_NURBS_PATCHES_H

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

#include <k3dsdk/axis.h>
#include <k3dsdk/types.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/table.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>
#include <stdexcept>

namespace module
{

namespace nurbs
{

// forward declarations
class curve_arrays;

/// Encapsulates the control point positions, weights and attributes for a single patch
struct patch_point_data
{
	patch_point_data() {}
	patch_point_data(const k3d::mesh& Mesh, const k3d::nurbs_patch::const_primitive& Primitive, const k3d::uint_t Patch);
	patch_point_data(const k3d::uint_t Size);
	const k3d::bool_t validate() const;

	k3d::mesh::points_t points;
	k3d::mesh::weights_t weights;
	k3d::table point_attributes;
	k3d::table vertex_attributes;
};

/// Copy structure and attributes between patch point data structures
/**
 * Convenient copying class that treats points, weights point attributes and vertex attributes in one go.
 */
class point_data_copier
{
public:
	point_data_copier(const patch_point_data& Source, patch_point_data& Destination);
	/// Appends the given index value from each source array to each corresponding target array.
	void push_back(const k3d::uint_t Index);
	/// Computes a weighted sum of N values from each source array and appends the result to the corresponding target array.
	void push_back(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights);
	/// Copies the given source index value from each source array to the TargetIndex in each corresponding target array.
	void copy(const k3d::uint_t SourceIndex, const k3d::uint_t TargetIndex);
	/// Computes a weighted sum of N values from each source array and copies the result to the corresponding target array at the given TargetIndex.
	void copy(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights, const k3d::uint_t TargetIndex);
private:
	const patch_point_data& m_source;
	patch_point_data& m_destination;
	boost::scoped_ptr<k3d::table_copier> m_point_attribute_copier;
	boost::scoped_ptr<k3d::table_copier> m_vertex_attribute_copier;
};

/// Copy from curve data to patch data
class curve_point_data_copier
{
public:
	curve_point_data_copier(const curve_arrays& Source, patch_point_data& Destination);
	/// Appends the given index value from each source array to each corresponding target array.
	void push_back(const k3d::uint_t Index);
	/// Computes a weighted sum of N values from each source array and appends the result to the corresponding target array.
	void push_back(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights);
	/// Copies the given source index value from each source array to the TargetIndex in each corresponding target array.
	void copy(const k3d::uint_t SourceIndex, const k3d::uint_t TargetIndex);
	/// Computes a weighted sum of N values from each source array and copies the result to the corresponding target array at the given TargetIndex.
	void copy(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights, const k3d::uint_t TargetIndex);
private:
	const curve_arrays& m_source;
	patch_point_data& m_destination;
	boost::scoped_ptr<k3d::table_copier> m_point_attribute_copier;
	boost::scoped_ptr<k3d::table_copier> m_vertex_attribute_copier;
};

/// Extract and store patch and parameter attributes for a single patch
struct patch_attribute_tables
{
	patch_attribute_tables(const k3d::nurbs_patch::const_primitive& Primitive, const k3d::uint_t Patch);
	k3d::table patch_attributes;
	k3d::table parameter_attributes;
};

/// Adds a patch with the given points, weights and knots
void add_patch(k3d::mesh& Mesh,
		k3d::nurbs_patch::primitive& NurbsPatches,
		const patch_point_data& PointData,
		const k3d::mesh::knots_t& UKnots,
		const k3d::mesh::knots_t& VKnots,
		const k3d::uint_t UOrder,
		const k3d::uint_t VOrder,
		const k3d::table& PatchAttributes = k3d::table(),
		const k3d::table& ParameterAttributes = k3d::table());

/// Copies a patch from one primitive to another
void copy_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch);

void add_trim_curve(k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::mesh::points_2d_t& Points, const k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& Knots, const k3d::uint_t Order, const k3d::double_t UOffset = 0, const k3d::double_t VOffset = 0, const k3d::double_t UScale = 1, const k3d::double_t VScale = 1);

/// Creates a bilinear patch between the given corner points (in counter-clockwise order)
void create_bilinear_patch(k3d::mesh& OutputMesh,
		k3d::nurbs_patch::primitive& OutputPatches,
		const patch_point_data& PointData,
		const k3d::table& PatchAttributes,
		const k3d::table& ParameterAttributes);

/// Traverse each selected curve in SourceCurves along each selected curve in CurvesToTraverse
void traverse_curve(const k3d::mesh& SourceCurves, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh);

/// Traverse curves in the source mesh stored in the primitive given by SourcePrimIdx, storing the resulting patches in the given patch primitives
void traverse_curve(const k3d::mesh& SourceCurves, const k3d::uint_t SourcePrimIdx, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches);

/// Extract a curve by curve number from the given patch, appending it to the output structures.
/**
 * \param Curve The number of the curve to extract, where the numbering goes by the control point count in the specified direction
 */
void extract_patch_curve_by_number(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Curve, const k3d::bool_t UDirection);

/// Extract a curve by parameter from the given patch
/**
 * \param U The parameter value in the normalized interval [0, 1], U or V depending on the specified direction
 */
void extract_patch_curve_by_parameter(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::double_t U, const k3d::bool_t UDirection);

/// Elevate patch degree by the requested number of elevations
void elevate_patch_degree(const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::uint_t Elevations, const k3d::bool_t UDirection);

/// Insert a knot in one directrion of the patch
void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::uint_t r, const k3d::bool_t UDirection);

/// Splits the patch at the requested parameter value u
void split_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::bool_t UDirection);

/// Revolves a curve around an axis
void revolve_curve(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::axis Axis, const k3d::double_t Angle, const k3d::uint_t Segments);

/// Creates a ruled surface between two curves.
/**
 * Prerequisites: - NurbsCurves must contain exactly two curves
 *                - The curves must be compatible, i.e. have the same knot vector and order
 */
void ruled_surface(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Order, const k3d::uint_t Segments);

/// Creates a Coons surface between 4 curves
/**
 * InputCurves must contain 4 curves, and the parameters U1, U2, V1, V2 indicate which curves
 * span the patch in the U and V direction. Each U and V curve must have a common endpoint
 */
void coons_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t U1 = 0, const k3d::uint_t V1 = 1, const k3d::uint_t U2 = 2, const k3d::uint_t V2 = 3);

/// Extracts a coordinate system and the points parametric coordinates from a point cloud, based on the "mean" normal of the points
void extract_plane(k3d::point3& Origin, k3d::normal3& Normal, k3d::vector3& U, k3d::vector3& V, k3d::mesh::points_2d_t& ParameterPoints, const k3d::mesh::points_t& Points, const k3d::double_t UOffset = 0.0);

/// Helper function to extract a 2D bounding box
void bbox2(k3d::point2& Min, k3d::point2& Max, const k3d::mesh::points_2d_t& Points);

/// Skinned surface between the curves in the given primitive
void skin_curves(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::primitive& InputCurves, const k3d::mesh::knots_t VKnots, const k3d::uint_t VOrder);

/// Sweeps the curves in SweepedCurves along the curves in Paths
/**
 * Algorithm: Each control point on the curve to sweep is traced along the convex hull of the path.
 * This is done by projecting the control points on planes that bisect the convex hull of the path,
 * or planes normal to the path in the case of the start and end points.
 */
void sweep(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& SweptCurves, const k3d::nurbs_curve::const_primitive& Paths, const k3d::uint_t Samples, const k3d::bool_t AlignNormal);

/// Store a polygon apprimation of a NURBS patch
void polygonize(k3d::mesh::points_t& Vertices, k3d::mesh::counts_t& VertexCounts, k3d::mesh::indices_t& VertexIndices, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t USamples, const k3d::uint_t VSamples);

/// Extract the trim curve with the given number into the supplied arrays, where the 2D UV points are stored in the X and Y coordinates of the output point array
void extract_trim_curve(k3d::mesh::points_t& Points, k3d::mesh::weights_t& Weights, k3d::mesh::knots_t& Knots, const k3d::nurbs_patch::const_primitive& NurbsPatches, const k3d::uint_t Patch, const k3d::uint_t Curve);

/// Converts the trim curves in a patch to 3D NURBS curves
void trim_to_nurbs(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Samples);

} //namespace nurbs

} //namespace module

#endif // !MODULES_NURBS_NURBS_PATCHES_H
