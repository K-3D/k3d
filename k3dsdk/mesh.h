#ifndef K3DSDK_MESH_H
#define K3DSDK_MESH_H

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

#include "named_arrays.h"
#include "point2.h"
#include "point3.h"
#include "normal3.h"
#include "vector3.h"
#include "typed_array.h"

namespace k3d
{

class imaterial;
class mesh_selection;

namespace legacy { class mesh; }

////////////////////////////////////////////////////////////////////////////////
// mesh

/// Encapsulates a hetereogenous collection of geometric primitives
class mesh
{
public:
	mesh();

	/// Defines storage for a generic collection of indices
	typedef typed_array<uint_t> indices_t;
	/// Defines storage for a generic collection of counts
	typedef typed_array<uint_t> counts_t;
	/// Defines storage for a generic collection of orders
	typedef typed_array<uint_t> orders_t;
	/// Defines storage for a generic collection of booleans
	typedef typed_array<bool_t> bools_t;
	/// Defines storage for a generic collection of weights
	typedef typed_array<double_t> weights_t;
	/// Defines storage for a generic collection of knot vectors
	typedef typed_array<double_t> knots_t;
	/// Defines storage for gprim selection state
	typedef typed_array<double_t> selection_t;
	/// Defines storage for gprim materials
	typedef typed_array<imaterial*> materials_t;
	/// Defines a heterogeneous collection of named, shared arrays
	typedef k3d::named_arrays named_arrays;
	/// Defines storage for a collection of 3D points
	typedef typed_array<point3> points_t;
	/// Defines storage for a collection of 3D normals
	typedef typed_array<normal3> normals_t;
	/// Defines storage for a collection of 3D vectors
	typedef typed_array<vector3> vectors_t;
	/// Defines storage for a collection of 2D points
	typedef typed_array<point2> points_2d_t;
	/// Defines storage for a collection of doubles
	typedef typed_array<double_t> doubles_t;

	/// Defines storage for point groups (particle clouds)
	class point_groups_t
	{
	public:
		/// Stores the set of per-point-group first points
		boost::shared_ptr<const indices_t> first_points;
		/// Stores the set of per-point-group point counts
		boost::shared_ptr<const counts_t> point_counts;
		/// Stores the set of per-point-group materials
		boost::shared_ptr<const materials_t> materials;
		/// Stores user-defined per-point-group data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores per-point-group point lists
		boost::shared_ptr<const indices_t> points;
		/// Stores user-defined per-point-group-point data (maps to RenderMan varying data)
		named_arrays varying_data;
	};

	/// Defines storage for linear curve groups
	class linear_curve_groups_t
	{
	public:
		/// Stores the set of per-curve-group first points
		boost::shared_ptr<const indices_t> first_curves;
		/// Stores the set of per-curve-group curve counts
		boost::shared_ptr<const counts_t> curve_counts;
		/// Stores the set of per-curve-group periodic state
		boost::shared_ptr<const bools_t> periodic_curves;
		/// Stores the set of per-curve-group materials
		boost::shared_ptr<const materials_t> materials;
		/// Stores user-defined per-curve-group data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores the set of per-curve first points
		boost::shared_ptr<const indices_t> curve_first_points;
		/// Stores the set of per-curve point counts
		boost::shared_ptr<const counts_t> curve_point_counts;
		/// Stores per-curve selection state
		boost::shared_ptr<const selection_t> curve_selection;
		/// Stores user-defined per-curve data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores per-curve point lists
		boost::shared_ptr<const indices_t> curve_points;
		/// Stores user-defined per-curve control point data (maps to RenderMan varying data)
		named_arrays varying_data;
	};

	/// Defines storage for cubic curve groups
	class cubic_curve_groups_t
	{
	public:
		/// Stores the set of per-curve-group first points
		boost::shared_ptr<const indices_t> first_curves;
		/// Stores the set of per-curve-group curve counts
		boost::shared_ptr<const counts_t> curve_counts;
		/// Stores the set of per-curve-group periodic state
		boost::shared_ptr<const bools_t> periodic_curves;
		/// Stores the set of per-curve-group materials
		boost::shared_ptr<const materials_t> materials;
		/// Stores user-defined per-curve-group data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores the set of per-curve first points
		boost::shared_ptr<const indices_t> curve_first_points;
		/// Stores the set of per-curve point counts
		boost::shared_ptr<const counts_t> curve_point_counts;
		/// Stores per-curve selection state
		boost::shared_ptr<const selection_t> curve_selection;
		/// Stores user-defined per-curve data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores per-curve point lists
		boost::shared_ptr<const indices_t> curve_points;
		/// Stores user-defined per-curve control point data (maps to RenderMan varying data)
		named_arrays varying_data;
	};

	/// Defines storage for NURBS curve groups
	class nurbs_curve_groups_t
	{
	public:
		/// Stores the set of per-curve-group first points
		boost::shared_ptr<const indices_t> first_curves;
		/// Stores the set of per-curve-group curve counts
		boost::shared_ptr<const counts_t> curve_counts;
		/// Stores the set of per-curve-group materials
		boost::shared_ptr<const materials_t> materials;
		/// Stores user-defined per-curve-group data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores the set of per-curve first points
		boost::shared_ptr<const indices_t> curve_first_points;
		/// Stores the set of per-curve point counts
		boost::shared_ptr<const counts_t> curve_point_counts;
		/// Stores the set of per-curve orders
		boost::shared_ptr<const orders_t> curve_orders;
		/// Stores the set of per-curve first knots
		boost::shared_ptr<const indices_t> curve_first_knots;
		/// Stores per-curve selection state
		boost::shared_ptr<const selection_t> curve_selection;
		/// Stores user-defined per-curve data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores per-curve control points
		boost::shared_ptr<const indices_t> curve_points;
		/// Stores user-defined per-curve control point data (maps to RenderMan varying data)
		named_arrays varying_data;
		/// Stores per-curve control point weights
		boost::shared_ptr<const weights_t> curve_point_weights;
		/// Stores per-curve knot vectors
		boost::shared_ptr<const knots_t> curve_knots;
	};

	/// Defines storage for bilinear patches
	class bilinear_patches_t
	{
	public:
		/// Stores per-patch selection state
		boost::shared_ptr<const selection_t> patch_selection;
		/// Stores the set of per-patch materials
		boost::shared_ptr<const materials_t> patch_materials;
		/// Stores user-defined per-patch data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores user-defined per-patch data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores the set of per-patch points
		boost::shared_ptr<const indices_t> patch_points;
		/// Stores user-defined per-parametric-corner data (maps to RenderMan varying data)
		named_arrays varying_data;
	};
	
	/// Defines storage for bicubic patches
	class bicubic_patches_t
	{
	public:
		/// Stores per-patch selection state
		boost::shared_ptr<const selection_t> patch_selection;
		/// Stores the set of per-patch materials
		boost::shared_ptr<const materials_t> patch_materials;
		/// Stores user-defined per-patch data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores user-defined per-patch data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores the set of per-patch points
		boost::shared_ptr<const indices_t> patch_points;
		/// Stores the set of per-parametric-corner data (maps to RenderMan varying data)
		named_arrays varying_data;
	};
	
	/// Defines storage for NURBS patches
	class nurbs_patches_t
	{
	public:
		/// Stores the set of per-patch first points
		boost::shared_ptr<const indices_t> patch_first_points;
		/// Stores the set of per-patch point counts in the U parametric direction
		boost::shared_ptr<const counts_t> patch_u_point_counts;
		/// Stores the set of per-patch point counts in the V parametric direction
		boost::shared_ptr<const counts_t> patch_v_point_counts;
		/// Stores the set of per-patch orders in the U parametric direction
		boost::shared_ptr<const orders_t> patch_u_orders;
		/// Stores the set of per-patch orders in the V parametric direction
		boost::shared_ptr<const orders_t> patch_v_orders;
		/// Stores the set of per-patch first knots in the U parametric direction
		boost::shared_ptr<const indices_t> patch_u_first_knots;
		/// Stores the set of per-patch first knots in the V parametric direction
		boost::shared_ptr<const indices_t> patch_v_first_knots;
		/// Stores per-patch selection state
		boost::shared_ptr<const selection_t> patch_selection;
		/// Stores per-patch materials
		boost::shared_ptr<const materials_t> patch_materials;
		/// Stores user-defined per-patch data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores user-defined per-patch data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores per-patch control points
		boost::shared_ptr<const indices_t> patch_points;
		/// Stores per-patch control point weights
		boost::shared_ptr<const weights_t> patch_point_weights;
		/// Stores per-patch knot vectors in the U parametric direction
		boost::shared_ptr<const knots_t> patch_u_knots;
		/// Stores per-patch knot vectors in the V parametric direction
		boost::shared_ptr<const knots_t> patch_v_knots;
		/// Stores user-defined per-parametric-corner data (maps to RenderMan varying data)
		named_arrays varying_data;
		/// Stores the number of trim curve loops for each patch
		boost::shared_ptr<const counts_t> patch_trim_curve_loop_counts;
		/// Stores the first trim curve loop (index into first_trim_curves) for each patch
		boost::shared_ptr<const indices_t> patch_first_trim_curve_loops;
		/// Stores the trim curve control points, expressed in parameter space
		boost::shared_ptr<const points_2d_t> trim_points;
		/// Stores the trim curve control point selection
		boost::shared_ptr<const selection_t> trim_point_selection;
		/// Stores the set of per-curve-loop first curves (index into trim_curve_first_points)
		boost::shared_ptr<const indices_t> first_trim_curves;
		/// Stores the set of per-curve-loop curve counts
		boost::shared_ptr<const counts_t> trim_curve_counts;
		/// Stores per-curve-loop selection
		boost::shared_ptr<const selection_t> trim_curve_loop_selection;
		/// Stores the set of per-curve first points
		boost::shared_ptr<const indices_t> trim_curve_first_points;
		/// Stores the set of per-curve point counts
		boost::shared_ptr<const counts_t> trim_curve_point_counts;
		/// Stores the set of per-curve orders
		boost::shared_ptr<const orders_t> trim_curve_orders;
		/// Stores the set of per-curve first knots
		boost::shared_ptr<const indices_t> trim_curve_first_knots;
		/// Stores per-curve selection state
		boost::shared_ptr<const selection_t> trim_curve_selection;
		/// Stores per-curve control points
		boost::shared_ptr<const indices_t> trim_curve_points;
		/// Stores per-curve control point weights
		boost::shared_ptr<const weights_t> trim_curve_point_weights;
		/// Stores per-curve knot vectors
		boost::shared_ptr<const knots_t> trim_curve_knots;
	};

	/// Defines storage for polyhedra (polygons and subdivision surfaces)
	class polyhedra_t
	{
	public:
		/// Defines allowable polyhedron types
		typedef enum
		{
			POLYGONS,
			CATMULL_CLARK,
		} polyhedron_type;

		/// Defines storage for per-polyhedron types
		typedef typed_array<polyhedron_type> types_t;

		/// Stores per-polyhedron first faces
		boost::shared_ptr<const indices_t> first_faces;
		/// Stores per-polyhedron face counts
		boost::shared_ptr<const counts_t> face_counts;
		/// Stores per-polyhedron types
		boost::shared_ptr<const types_t> types;
		/// Stores user-defined per-polyhedron data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores per-face first loops
		boost::shared_ptr<const indices_t> face_first_loops;
		/// Stores per-face loop counts
		boost::shared_ptr<const counts_t> face_loop_counts;
		/// Stores per-face selection state
		boost::shared_ptr<const selection_t> face_selection;
		/// Stores per-face materials
		boost::shared_ptr<const materials_t> face_materials;
		/// Stores user-defined per-face data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores per-loop first edges
		boost::shared_ptr<const indices_t> loop_first_edges;
		/// Stores the start point of each edge
		boost::shared_ptr<const indices_t> edge_points;
		/// Stores the next edge in clockwise direction
		boost::shared_ptr<const indices_t> clockwise_edges;
		/// Stores per-edge selection state
		boost::shared_ptr<const selection_t> edge_selection;
		/// Stores user-defined per-edge data (maps to RenderMan facevarying data)
		named_arrays face_varying_data;
	};

	/// Defines storage for blobbies (implicit surfaces)
	class blobbies_t
	{
	public:
		/// Defines allowable opcode types
		typedef enum
		{
			CONSTANT = 1000,
			ELLIPSOID = 1001,
			SEGMENT = 1002,
		} primitive_type;

		typedef enum
		{
			ADD = 0,
			MULTIPLY = 1,
			MAXIMUM = 2,
			MINIMUM = 3,
			SUBTRACT = 4,
			DIVIDE = 5,
			NEGATE = 6,
			IDENTITY = 7

		} operator_type;

		/// Defines storage for blobby primitives
		typedef typed_array<primitive_type> primitives_t;
		/// Defines storage for blobby operators
		typedef typed_array<operator_type> operators_t;
		/// Defines storage for primitive floating-point values
		typedef typed_array<double_t> floats_t;
		/// Defines storage for operator operands
		typedef typed_array<uint_t> operands_t;

		/// Stores per-blobby primitive offsets
		boost::shared_ptr<const indices_t> first_primitives;
		/// Stores per-blobby primitive counts
		boost::shared_ptr<const counts_t> primitive_counts;
		/// Stores per-blobby operator offsets
		boost::shared_ptr<const indices_t> first_operators;
		/// Stores per-blobby operator counts
		boost::shared_ptr<const counts_t> operator_counts;
		/// Stores per-blobby materials
		boost::shared_ptr<const materials_t> materials;
		/// Stores user-defined per-blobby data (maps to RenderMan constant data)
		named_arrays constant_data;
		/// Stores user-defined per-blobby data (maps to RenderMan uniform data)
		named_arrays uniform_data;
		/// Stores blobby primitives
		boost::shared_ptr<const primitives_t> primitives;
		/// Stores per-primitive floating-point value offsets
		boost::shared_ptr<const indices_t> primitive_first_floats;
		/// Stores per-primitive floating-point value counts
		boost::shared_ptr<const counts_t> primitive_float_counts;
		/// Stores user-defined per-primitive data (maps to RenderMan varying data)
		named_arrays varying_data;
		/// Stores user-defined per-primitive data (maps to RenderMan vertex data)
		named_arrays vertex_data;
		/// Stores blobby operators
		boost::shared_ptr<const operators_t> operators;
		/// Stores operator operand offsets
		boost::shared_ptr<const indices_t> operator_first_operands;
		/// Stores operator operand counts
		boost::shared_ptr<const counts_t> operator_operand_counts;
		/// Stores primitive floating-point values
		boost::shared_ptr<const floats_t> floats;
		/// Stores operator operands
		boost::shared_ptr<const operands_t> operands;
	};

	/// Stores the set of mesh points
	boost::shared_ptr<const points_t> points;
	/// Stores per-point selection state
	boost::shared_ptr<const selection_t> point_selection;
	/// Stores user-defined per-point data (maps to RenderMan vertex data)
	named_arrays vertex_data;

	/// Stores point groups
	boost::shared_ptr<const point_groups_t> point_groups;
	/// Stores linear curve groups
	boost::shared_ptr<const linear_curve_groups_t> linear_curve_groups;
	/// Stores cubic curve groups
	boost::shared_ptr<const cubic_curve_groups_t> cubic_curve_groups;
	/// Stores nurbs curve groups
	boost::shared_ptr<const nurbs_curve_groups_t> nurbs_curve_groups;
	/// Stores bilinear patches
	boost::shared_ptr<const bilinear_patches_t> bilinear_patches;
	/// Stores bicubic patches
	boost::shared_ptr<const bicubic_patches_t> bicubic_patches;
	/// Stores nurbs patches
	boost::shared_ptr<const nurbs_patches_t> nurbs_patches;
	/// Stores polyhedra
	boost::shared_ptr<const polyhedra_t> polyhedra;
	/// Stores blobbies (implicit surfaces)
	boost::shared_ptr<const blobbies_t> blobbies;
	
	/// Conversion from a legacy mesh to a new mesh
	mesh& operator=(const k3d::legacy::mesh& RHS);
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh::polyhedra_t::polyhedron_type& RHS);
/// Stream extraction
std::istream& operator>>(std::istream& Stream, mesh::polyhedra_t::polyhedron_type& RHS);

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh::blobbies_t::primitive_type& RHS);
/// Stream extraction
std::istream& operator>>(std::istream& Stream, mesh::blobbies_t::primitive_type& RHS);

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh::blobbies_t::operator_type& RHS);
/// Stream extraction
std::istream& operator>>(std::istream& Stream, mesh::blobbies_t::operator_type& RHS);

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh& RHS);

template<>
class almost_equal<mesh::polyhedra_t::polyhedron_type>
{
public:
	almost_equal(const boost::uint64_t) { } 
	inline const bool_t operator()(const mesh::polyhedra_t::polyhedron_type A, const mesh::polyhedra_t::polyhedron_type B) const { return A == B; }
};

template<>
class almost_equal<mesh::blobbies_t::primitive_type>
{
public:
	almost_equal(const boost::uint64_t) { } 
	inline const bool_t operator()(const mesh::blobbies_t::primitive_type A, const mesh::blobbies_t::primitive_type B) const { return A == B; }
};

template<>
class almost_equal<mesh::blobbies_t::operator_type>
{
public:
	almost_equal(const boost::uint64_t) { } 
	inline const bool_t operator()(const mesh::blobbies_t::operator_type A, const mesh::blobbies_t::operator_type B) const { return A == B; }
};

} // namespace k3d

#endif // K3DSDK_MESH_H

