#ifndef K3DSDK_MESH_H
#define K3DSDK_MESH_H

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

#include "attribute_arrays.h"
#include "bounding_box3.h"
#include "named_attribute_arrays.h"
#include "named_arrays.h"
#include "named_array_types.h"
#include "pipeline_data.h"
#include "typed_array.h"
#include "uint_t_array.h"

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

	/// Defines storage for a collection of indices
	typedef uint_t_array indices_t;
	/// Defines storage for a collection of counts
	typedef uint_t_array counts_t;
	/// Defines storage for a collection of orders
	typedef uint_t_array orders_t;
	/// Defines storage for a collection of weights
	typedef typed_array<double_t> weights_t;
	/// Defines storage for a collection of knot vectors
	typedef typed_array<double_t> knots_t;
	/// Defines storage for gprim selection state
	typedef typed_array<double_t> selection_t;
	/// Defines storage for gprim materials
	typedef typed_array<imaterial*> materials_t;
	/// Defines storage for a generic collection of boolean values
	typedef typed_array<bool_t> bools_t;
	/// Defines storage for a generic collection of colors
	typedef typed_array<color> colors_t;
	/// Defines storage for a generic collection of floating-point values
	typedef typed_array<double_t> doubles_t;
	/// Defines storage for a generic collection of 4x4 matrices
	typedef typed_array<matrix4> matrices_t;
	/// Defines storage for a generic collection of 3D normals
	typedef typed_array<normal3> normals_t;
	/// Defines storage for a generic collection of 2D points
	typedef typed_array<point2> points_2d_t;
	/// Defines storage for a generic collection of 3D points
	typedef typed_array<point3> points_t;
	/// Defines storage for a generic collection of 3D texture coordinates
	typedef typed_array<texture3> texture_coordinates_t;
	/// Defines storage for a generic collection of 3D vectors
	typedef typed_array<vector3> vectors_t;
	/// Defines a heterogeneous collection of named, shared arrays with varying lengths
	typedef k3d::named_arrays named_arrays_t;
	/// Defines a heterogeneous collection of named, shared arrays with identical length
	typedef k3d::attribute_arrays attribute_arrays_t;
	/// Defines a named collection of attribute arrays
	typedef k3d::named_attribute_arrays named_attribute_arrays_t;

	/// Defines storage for a generic mesh primitive
	class primitive
	{
	public:
		primitive(const string_t& Type);

		/// Stores the primitive type ("point_groups", "polyhedra", "teapot", etc.)
		string_t type;
		/// Stores array data that defines the primitive's structure (topology and geometry)
		named_arrays_t structure;
		/// Stores array data that defines the primitive's attributes
		named_attribute_arrays_t attributes;

		/// Compares two primitives for equality using the fuzzy semantics of almost_equal
		const bool_t almost_equal(const primitive& Other, const uint64_t Threshold) const;
	};

	/// Defines storage for a collection of primitives
	class primitives_t :
		public std::vector<pipeline_data<primitive> >
	{
	public:
		primitive& create(const string_t& Type);
	};

	/// Defines storage for NURBS curve groups
	class nurbs_curve_groups_t
	{
	public:
		/// Stores the set of per-curve-group first points
		pipeline_data<indices_t> first_curves;
		/// Stores the set of per-curve-group curve counts
		pipeline_data<counts_t> curve_counts;
		/// Stores the set of per-curve-group materials
		pipeline_data<materials_t> materials;
		/// Stores user-defined per-curve-group data (maps to RenderMan constant data)
		attribute_arrays_t constant_data;
		/// Stores the set of per-curve first points
		pipeline_data<indices_t> curve_first_points;
		/// Stores the set of per-curve point counts
		pipeline_data<counts_t> curve_point_counts;
		/// Stores the set of per-curve orders
		pipeline_data<orders_t> curve_orders;
		/// Stores the set of per-curve first knots
		pipeline_data<indices_t> curve_first_knots;
		/// Stores per-curve selection state
		pipeline_data<selection_t> curve_selection;
		/// Stores user-defined per-curve data (maps to RenderMan uniform data)
		attribute_arrays_t uniform_data;
		/// Stores per-curve control points
		pipeline_data<indices_t> curve_points;
		/// Stores user-defined per-curve control point data (maps to RenderMan varying data)
		attribute_arrays_t varying_data;
		/// Stores per-curve control point weights
		pipeline_data<weights_t> curve_point_weights;
		/// Stores per-curve knot vectors
		pipeline_data<knots_t> curve_knots;
	};

	/// Defines storage for NURBS patches
	class nurbs_patches_t
	{
	public:
		/// Stores the set of per-patch first points
		pipeline_data<indices_t> patch_first_points;
		/// Stores the set of per-patch point counts in the U parametric direction
		pipeline_data<counts_t> patch_u_point_counts;
		/// Stores the set of per-patch point counts in the V parametric direction
		pipeline_data<counts_t> patch_v_point_counts;
		/// Stores the set of per-patch orders in the U parametric direction
		pipeline_data<orders_t> patch_u_orders;
		/// Stores the set of per-patch orders in the V parametric direction
		pipeline_data<orders_t> patch_v_orders;
		/// Stores the set of per-patch first knots in the U parametric direction
		pipeline_data<indices_t> patch_u_first_knots;
		/// Stores the set of per-patch first knots in the V parametric direction
		pipeline_data<indices_t> patch_v_first_knots;
		/// Stores per-patch selection state
		pipeline_data<selection_t> patch_selection;
		/// Stores per-patch materials
		pipeline_data<materials_t> patch_materials;
		/// Stores user-defined per-patch data (maps to RenderMan constant data)
		attribute_arrays_t constant_data;
		/// Stores user-defined per-patch data (maps to RenderMan uniform data)
		attribute_arrays_t uniform_data;
		/// Stores per-patch control points
		pipeline_data<indices_t> patch_points;
		/// Stores per-patch control point weights
		pipeline_data<weights_t> patch_point_weights;
		/// Stores per-patch knot vectors in the U parametric direction
		pipeline_data<knots_t> patch_u_knots;
		/// Stores per-patch knot vectors in the V parametric direction
		pipeline_data<knots_t> patch_v_knots;
		/// Stores user-defined per-parametric-corner data (maps to RenderMan varying data)
		attribute_arrays_t varying_data;
		/// Stores the number of trim curve loops for each patch
		pipeline_data<counts_t> patch_trim_curve_loop_counts;
		/// Stores the first trim curve loop (index into first_trim_curves) for each patch
		pipeline_data<indices_t> patch_first_trim_curve_loops;
		/// Stores the trim curve control points, expressed in parameter space
		pipeline_data<points_2d_t> trim_points;
		/// Stores the trim curve control point selection
		pipeline_data<selection_t> trim_point_selection;
		/// Stores the set of per-curve-loop first curves (index into trim_curve_first_points)
		pipeline_data<indices_t> first_trim_curves;
		/// Stores the set of per-curve-loop curve counts
		pipeline_data<counts_t> trim_curve_counts;
		/// Stores per-curve-loop selection
		pipeline_data<selection_t> trim_curve_loop_selection;
		/// Stores the set of per-curve first points
		pipeline_data<indices_t> trim_curve_first_points;
		/// Stores the set of per-curve point counts
		pipeline_data<counts_t> trim_curve_point_counts;
		/// Stores the set of per-curve orders
		pipeline_data<orders_t> trim_curve_orders;
		/// Stores the set of per-curve first knots
		pipeline_data<indices_t> trim_curve_first_knots;
		/// Stores per-curve selection state
		pipeline_data<selection_t> trim_curve_selection;
		/// Stores per-curve control points
		pipeline_data<indices_t> trim_curve_points;
		/// Stores per-curve control point weights
		pipeline_data<weights_t> trim_curve_point_weights;
		/// Stores per-curve knot vectors
		pipeline_data<knots_t> trim_curve_knots;
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
		pipeline_data<indices_t> first_faces;
		/// Stores per-polyhedron face counts
		pipeline_data<counts_t> face_counts;
		/// Stores per-polyhedron types
		pipeline_data<types_t> types;
		/// Stores user-defined per-polyhedron data (maps to RenderMan constant data)
		attribute_arrays_t constant_data;
		/// Stores per-face first loops
		pipeline_data<indices_t> face_first_loops;
		/// Stores per-face loop counts
		pipeline_data<counts_t> face_loop_counts;
		/// Stores per-face selection state
		pipeline_data<selection_t> face_selection;
		/// Stores per-face materials
		pipeline_data<materials_t> face_materials;
		/// Stores user-defined per-face data (maps to RenderMan uniform data)
		attribute_arrays_t uniform_data;
		/// Stores per-loop first edges
		pipeline_data<indices_t> loop_first_edges;
		/// Stores the start point of each edge
		pipeline_data<indices_t> edge_points;
		/// Stores the next edge in clockwise direction
		pipeline_data<indices_t> clockwise_edges;
		/// Stores per-edge selection state
		pipeline_data<selection_t> edge_selection;
		/// Stores user-defined per-edge data (maps to RenderMan facevarying data)
		attribute_arrays_t face_varying_data;
	};

	/// Stores the set of mesh points
	pipeline_data<points_t> points;
	/// Stores per-point selection state
	pipeline_data<selection_t> point_selection;
	/// Stores user-defined per-point data (maps to RenderMan vertex data)
	attribute_arrays_t vertex_data;
	/// Stores mesh primitives
	primitives_t primitives;

	/// Stores nurbs curve groups
	pipeline_data<nurbs_curve_groups_t> nurbs_curve_groups;
	/// Stores nurbs patches
	pipeline_data<nurbs_patches_t> nurbs_patches;
	/// Stores polyhedra
	pipeline_data<polyhedra_t> polyhedra;

	/// Compares two meshes for equality using the fuzzy semantics of almost_equal
	const bool_t almost_equal(const mesh& Other, const uint64_t Threshold) const;

	/// Conversion from a legacy mesh to a new mesh
	mesh& operator=(const k3d::legacy::mesh& RHS);

	/// Returns a bounding-box containing every point in the given mesh.
	static const bounding_box3 bounds(const mesh& Mesh);
	/// Returns a bounding-box containing every point in the given array.
	static const bounding_box3 bounds(const points_t& Points);
	/// Initialize an array to mark unused mesh points (points not used by any primitive).
	static void lookup_unused_points(const mesh& Mesh, mesh::bools_t& UnusedPoints);
	/// Remove unused points from a mesh, adjusting point indices in all remaining primitives.
	static void delete_unused_points(mesh& Mesh);
	/// Performs a deep-copy from one mesh to another (the new mesh doesn't share any memory with the old).
	static void deep_copy(const mesh& From, mesh& To);

	/// Iterates over every array in a generic mesh primitive, passing the array name and array to a functor.
	template<typename FunctorT>
	static void visit_arrays(const mesh::primitive& Primitive, FunctorT Functor)
	{
		for(mesh::named_arrays_t::const_iterator array = Primitive.structure.begin(); array != Primitive.structure.end(); ++array)
			Functor(array->first, array->second);

		for(mesh::named_attribute_arrays_t::const_iterator attributes = Primitive.attributes.begin(); attributes != Primitive.attributes.end(); ++attributes)
		{
			for(mesh::attribute_arrays_t::const_iterator array = attributes->second.begin(); array != attributes->second.end(); ++array)
				Functor(array->first, array->second);
		}
	}

	/// Iterates over every array in a generic mesh primitive, passing the array name and array to a functor.
	template<typename FunctorT>
	static void visit_arrays(mesh::primitive& Primitive, FunctorT Functor)
	{
		for(mesh::named_arrays_t::iterator array = Primitive.structure.begin(); array != Primitive.structure.end(); ++array)
			Functor(array->first, array->second);

		for(mesh::named_attribute_arrays_t::iterator attributes = Primitive.attributes.begin(); attributes != Primitive.attributes.end(); ++attributes)
		{
			for(mesh::attribute_arrays_t::iterator array = attributes->second.begin(); array != attributes->second.end(); ++array)
				Functor(array->first, array->second);
		}
	}

	/// Iterates over every array in every generic primitive in the given mesh, passing the array name and array to a functor.
	template<typename FunctorT>
	static void visit_arrays(const mesh& Mesh, FunctorT Functor)
	{
		for(mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
			visit_arrays(**p, Functor);
	}

	/// Iterates over every array in every generic primitive in the given mesh, passing the array name and array to a functor.
	template<typename FunctorT>
	static void visit_arrays(mesh& Mesh, FunctorT Functor)
	{
		for(mesh::primitives_t::iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p)
			visit_arrays(p->writable(), Functor);
	}
};

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh::polyhedra_t::polyhedron_type& RHS);
/// Stream extraction
std::istream& operator>>(std::istream& Stream, mesh::polyhedra_t::polyhedron_type& RHS);

/// Stream serialization
std::ostream& operator<<(std::ostream& Stream, const mesh& RHS);

template<>
class almost_equal<mesh::polyhedra_t::polyhedron_type>
{
public:
	almost_equal(const uint64_t) { } 
	inline const bool_t operator()(const mesh::polyhedra_t::polyhedron_type A, const mesh::polyhedra_t::polyhedron_type B) const { return A == B; }
};

/// Specialization of almost_equal that tests k3d::mesh for equality
template<>
class almost_equal<mesh>
{
	typedef mesh T;

public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B) const
	{
		return A.almost_equal(B, threshold);
	}

	const uint64_t threshold;
};

/// Specialization of almost_equal that tests k3d::mesh::primitive for equality
template<>
class almost_equal<mesh::primitive>
{
	typedef mesh::primitive T;

public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B) const
	{
		return A.almost_equal(B, threshold);
	}

	const uint64_t threshold;
};

} // namespace k3d

#endif // !K3DSDK_MESH_H

