#ifndef K3DSDK_LEGACY_MESH_H
#define K3DSDK_LEGACY_MESH_H

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

#include "algebra.h"
#include "bounding_box3.h"
#include "multi_array.h"
#include "selectable.h"
#include "utility.h"

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>

#include <map>
#include <string>
#include <vector>

namespace k3d
{

class imaterial;
class mesh;

namespace legacy
{

/////////////////////////////////////////////////////////////////////////////
// parameters_t

/// Encapsulates a set of name-value pairs
typedef std::map<std::string, boost::any> parameters_t;

/////////////////////////////////////////////////////////////////////////////
// point

/// Encapsulates a point in 3D space
class point :
	public selectable
{
public:
	point(const point3& Position);
	point(const double X, const double Y, const double Z);

	/// Stores the position of the point in 3D space
	point3 position;
	/// Stores vertex data for the point
	parameters_t vertex_data;
	/// Stores tag (SDS) data for the point
	parameters_t tags;
};

/////////////////////////////////////////////////////////////////////////////
// point_group

/// Encapsulates a group of rendered points
class point_group :
	public selectable
{
public:
	point_group();

	typedef std::vector<point*> points_t;
	points_t points;

	/// Stores a reference to the material to use rendering the point group
	imaterial* material;
	/// Stores constant data for the group
	parameters_t constant_data;
};

/////////////////////////////////////////////////////////////////////////////
// split_edge

/// Encapsulates a split-edge data structure for representing topology information in a polygon mesh
class split_edge :
	public selectable
{
public:
	split_edge(point* Vertex, split_edge* FaceClockwise = 0, split_edge* Companion = 0) :
		vertex(Vertex),
		face_clockwise(FaceClockwise),
		companion(Companion)
	{
	}
	~split_edge();

	/// Stores a reference to the edge vertex
	point* vertex;
	/// Stores a reference to the next edge in clockwise order around the face
	split_edge* face_clockwise;
	/// Stores a reference to the "companion" edge associated with the face on the opposite side of this edge
	split_edge* companion;
	/// Stores facevarying data for this edge (specifically, its vertex)
	parameters_t facevarying_data;
	/// Stores tag (SDS) data for the edge
	parameters_t tags;
};

/// Convenience function for setting two edges as companions
inline void join_edges(split_edge& Edge1, split_edge& Edge2)
{
	Edge1.companion = &Edge2;
	Edge2.companion = &Edge1;
}

/// Convenience function for joining a collection of edges into a closed loop
template<typename IteratorType>
inline void loop_edges(IteratorType Begin, IteratorType End)
{
	if(Begin == End)
		return;

	IteratorType i = Begin;
	IteratorType j = i;
	while(++j != End)
	{
		(*i)->face_clockwise = *j;

		j = ++i;
	}

	(*i)->face_clockwise = *Begin;
}

/// Convenience function that returns face anticlockwise edge
inline split_edge* face_anticlockwise(split_edge* Edge)
{
	split_edge* current_edge = Edge;
	while(current_edge && current_edge->face_clockwise != Edge)
		current_edge = current_edge->face_clockwise;

	return current_edge;
}

/////////////////////////////////////////////////////////////////////////////
// face

/// Encapsulates a polygonal face
class face :
	public selectable
{
public:
	face(split_edge* FirstEdge, imaterial* Material);
	~face();

	/// Points to the first edge in the loop of edges that define the polygon
	split_edge* first_edge;
	/// Defines a collection of holes in the polygon face (each element points to the first edge in a loop that defines a hole)
	typedef std::vector<split_edge*> holes_t;
	/// Contains any holes in the polygon face
	holes_t holes;
	/// Stores a reference to the (optional) material for the face
	imaterial* material;
	/// Stores uniform data for the face
	parameters_t uniform_data;
	/// Stores tag (SDS) data for the face
	parameters_t tags;
};

/// Calculates the normal for an edge loop (returns a zero-length normal for degenerate cases)
normal3 normal(const split_edge* const Loop);
/// Calculates the normal for a face (returns a zero-length normal for degenerate cases)
normal3 normal(const face& Face);

/////////////////////////////////////////////////////////////////////////////
// polyhedron

/// Encapsulates a manifold polyhedron composed of polygon faces
class polyhedron :
	public selectable
{
public:
	polyhedron();
	~polyhedron();

	typedef std::vector<face*> faces_t;
	faces_t faces;

	typedef enum
	{
		POLYGONS,
		CATMULL_CLARK_SUBDIVISION_MESH,
	} type_t;

	/// Stores the polyhedron type
	type_t type;
	/// Stores constant data for the polyhedron
	parameters_t constant_data;
	/// Stores tag (SDS) data for the polyhedron
	parameters_t tags;

	friend std::ostream& operator<<(std::ostream&, const type_t&);
	friend std::istream& operator>>(std::istream&, type_t&);
};

/// Convenience function for setting all companions of a raw polygon edges
void set_companions(polyhedron& Polyhedron);

/////////////////////////////////////////////////////////////////////////////
// linear_curve

/// Encapsulates a linear curve
class linear_curve :
	public selectable
{
public:
	/// Defines storage for the set of curve control points
	typedef std::vector<point*> control_points_t;
	/// Stores the set of curve control points
	control_points_t control_points;
	/// Stores uniform data
	parameters_t uniform_data;
	/// Defines storage for varying data
	typedef std::vector<parameters_t> varying_t;
	/// Stores varying data
	varying_t varying_data;
};

/////////////////////////////////////////////////////////////////////////////
// linear_curve_group

/// Encapsulates a collection of linear curves
class linear_curve_group :
	public selectable
{
public:
	linear_curve_group();
	~linear_curve_group();

	/// Defines storage for a collection of linear curves
	typedef std::vector<linear_curve*> curves_t;
	/// Stores the collection of linear curves
	curves_t curves;
	/// Set to true iff the curves should wrap in the V direction
	bool wrap;
	/// Stores constant data
	parameters_t constant_data;
	/// Stores a reference to the optional curves material
	imaterial* material;
};

/////////////////////////////////////////////////////////////////////////////
// cubic_curve

/// Encapsulates a cubic curve
class cubic_curve :
	public selectable
{
public:
	/// Defines storage for the set of curve control points
	typedef std::vector<point*> control_points_t;
	/// Stores the set of curve control points
	control_points_t control_points;
	/// Stores uniform data
	parameters_t uniform_data;
	/// Defines storage for varying data
	typedef std::vector<parameters_t> varying_t;
	/// Stores varying data
	varying_t varying_data;
};

/////////////////////////////////////////////////////////////////////////////
// cubic_curve_group

/// Encapsulates a collection of cubic curves
class cubic_curve_group :
	public selectable
{
public:
	cubic_curve_group();
	~cubic_curve_group();

	/// Defines storage for a collection of cubic curves
	typedef std::vector<cubic_curve*> curves_t;
	/// Stores the collection of cubic curves
	curves_t curves;
	/// Set to true iff the curves should wrap in the V direction
	bool wrap;
	/// Stores constant data
	parameters_t constant_data;
	/// Stores a reference to the optional curves material
	imaterial* material;
};

/////////////////////////////////////////////////////////////////////////////
// nucurve

/// Encapsulates a NURBS curve - note: there is no equivalent RenderMan primitive, so these can't be rendered - take a look at cubic_curve, instead.
class nucurve :
	public selectable
{
public:
	nucurve();

	/// Stores the curve order (note - the curve order is defined as the curve degree plus 1)
	unsigned int order;
	/// Defines a collection of knots
	typedef std::vector<double> knots_t;
	/// The set of knots that define the curve's knot vector
	knots_t knots;

	/// Defines a control point - a combination of a point and a weight
	struct control_point
	{
		control_point(point* Position, const double Weight = 1.0) : position(Position), weight(Weight) { }

		point* position;
		double weight;
	};
	/// Defines a collection of control vertices
	typedef std::vector<control_point> control_points_t;
	/// The set of control vertices that define this curve
	control_points_t control_points;
};

/// Encapsulates a collection of NURBS curves - note: there is no equivalent RenderMan primitive, so these can't be rendered - take a look at cubic_curve_group, instead
class nucurve_group :
	public selectable
{
public:
	nucurve_group();
	~nucurve_group();

	/// Defines storage for a collection of NURBS curves
	typedef std::vector<nucurve*> curves_t;
	/// Stores the collection of NURBS curves
	curves_t curves;
	/// Stores a reference to the optional curve material
	imaterial* material;
};

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch

/// Encapsulates a bilinear patch
class bilinear_patch :
	public selectable
{
public:
	bilinear_patch();

	/// Defines control points for the patch
	typedef boost::array<point*, 4> control_points_t;
	/// Stores the patch control points
	control_points_t control_points;
	/// Defines storage for varying data
	typedef boost::array<parameters_t, 4> varying_t;
	/// Stores varying data
	varying_t varying_data;
	/// Stores uniform data
	parameters_t uniform_data;
	/// Stores a reference to the optional patch material
	imaterial* material;
};


/////////////////////////////////////////////////////////////////////////////
// bicubic_patch

/// Encapsulates a bicubic patch
class bicubic_patch :
	public selectable
{
public:
	bicubic_patch();

	/// Defines storage for the patch control-points
	typedef boost::array<point*, 16> control_points_t;
	/// Stores the patch control-points
	control_points_t control_points;
	/// Defines storage for varying data
	typedef boost::array<parameters_t, 4> varying_t;
	/// Stores varying data
	varying_t varying_data;
	/// Stores uniform data
	parameters_t uniform_data;
	/// Stores a reference to the (optional) patch material
	imaterial* material;
};

/////////////////////////////////////////////////////////////////////////////
// nupatch

/// Encapsulates a NURBS patch
class nupatch :
	public selectable
{
public:
	nupatch();

	/// Stores the patch order for u parametric direction (note: order is defined as degree + 1)
	unsigned int u_order;
	/// Stores the patch order for v parametric direction (note: order is defined as degree + 1)
	unsigned int v_order;
	/// Defines a collection of knots
	typedef std::vector<double> knots_t;
	/// The set of knots that define the patch's knot vector for u parametric direction
	knots_t u_knots;
	/// The set of knots that define the patch's knot vector for v parametric direction
	knots_t v_knots;

	/// Defines a control point - a combination of a point and a weight
	struct control_point
	{
		control_point(point* Position, const double Weight = 1.0) : position(Position), weight(Weight) { }

		point* position;
		double weight;
	};
	/// Defines a collection of control vertices
	typedef std::vector<control_point> control_points_t;
	/// The set of control vertices that define this curve
	control_points_t control_points;

	/// Stores a reference to the (optional) patch material
	imaterial* material;
};

/////////////////////////////////////////////////////////////////////////////
// blobby

/// Encapsulates a RenderMan blobby (implicit surface) primitive as a tree
class blobby
{
public:
	// Forward declarations
	class opcode;
	class visitor;

	blobby(opcode* Opcode);
	~blobby();

	/// Visitor design pattern
	void accept(visitor& Visitor);

	/// Stores the root "instruction" for the implicit surface function
	opcode* root;
	/// Stores a reference to the (optional) blobby material
	imaterial* material;
	/// Stores constant data
	parameters_t constant_data;

	/// Defines an "instruction" used to define the implicit surface function
	class opcode :
		public selectable
	{
	public:
		virtual ~opcode() {}

		/// Virtual copy constructor design pattern
		virtual opcode* clone() = 0;
		/// Visitor design pattern
		virtual void accept(visitor& Visitor) = 0;
	};

	/// Inserts a constant value into the implicit surface function
	class constant :
		public opcode
	{
	public:
		constant(double Value);
		opcode* clone();
		void accept(visitor& Visitor);

		/// Stores the value to be inserted
		double value;
	};

	/// Inserts an ellipsoid primitive into the implicit surface function
	class ellipsoid :
		public opcode
	{
	public:
		ellipsoid(point* Origin, const matrix4& Transformation);
		opcode* clone();
		void accept(visitor& Visitor);

		/// Stores the origin of the unit sphere that underlies the ellipsoid
		point* origin;
		/// Stores a matrix used to transform the unit sphere into an ellipsoid in mesh coordinates
		matrix4 transformation;
		/// Stores vertex data for this primitive
		parameters_t vertex_data;
	};

	/// Inserts a segment blob primitive into the implicit surface function
	class segment :
		public opcode
	{
	public:
		segment(point* Start, point* End, double Radius, const matrix4& Transformation);
		opcode* clone();
		void accept(visitor& Visitor);

		/// Stores the first end point that defines the segment blob
		point* start;
		/// Stores the second end point that defines the segment blob
		point* end;
		/// Stores the radius of the segment blob
		double radius;
		/// Stores a matrix used to transform the segment blob into mesh coordinates
		matrix4 transformation;
		/// Stores vertex data for this primitive
		parameters_t vertex_data;
	};

	/// Inserts a subtraction operation into the implicit surface function
	class subtract :
		public opcode
	{
	public:
		subtract(opcode* Subtrahend, opcode* Minuend);
		~subtract();
		opcode* clone();
		void accept(visitor& Visitor);

		opcode* subtrahend;
		opcode* minuend;
	};

	/// Inserts a division operation into the implicit surface function
	class divide :
		public opcode
	{
	public:
		divide(opcode* Dividend, opcode* Divisor);
		~divide();
		opcode* clone();
		void accept(visitor& Visitor);

		opcode* dividend;
		opcode* divisor;
	};

	/// Base class for opcodes that act on a variable number of arguments
	class variable_operands :
		public opcode
	{
	public:
		void add_operand(opcode* Operand);
		void operands_accept(visitor& Visitor);

		typedef std::vector<opcode*> operands_t;
		operands_t operands;

	protected:
		~variable_operands();
		void clone_operands();
	};

	/// Inserts an addition operation into the implicit surface function
	class add :
		public variable_operands
	{
	public:
		opcode* clone();
		void accept(visitor& Visitor);
	};

	/// Inserts a multiplication operation into the implicit surface function
	class multiply :
		public variable_operands
	{
	public:
		opcode* clone();
		void accept(visitor& Visitor);
	};

	/// Inserts a maximum operation into the implicit surface function
	class max :
		public variable_operands
	{
	public:
		opcode* clone();
		void accept(visitor& Visitor);
	};

	/// Inserts a minimum operation into the implicit surface function
	class min :
		public variable_operands
	{
	public:
		opcode* clone();
		void accept(visitor& Visitor);
	};

	/// Visitor design pattern
	class visitor
	{
	public:
		virtual void visit_constant(constant&) = 0;
		virtual void visit_ellipsoid(ellipsoid&) = 0;
		virtual void visit_segment(segment&) = 0;
		virtual void visit_subtract(subtract&) = 0;
		virtual void visit_divide(divide&) = 0;
		virtual void visit_add(add&) = 0;
		virtual void visit_multiply(multiply&) = 0;
		virtual void visit_min(min&) = 0;
		virtual void visit_max(max&) = 0;

	protected:
		visitor() {}
		virtual ~visitor() {}
		visitor(const visitor&) {}
		visitor& operator=(const visitor&) { return *this; }
	};
};

/////////////////////////////////////////////////////////////////////////////
// mesh

/// Encapsulates a collection of geometry
class mesh :
	public selectable
{
public:
	mesh();
	virtual ~mesh();

	/// Defines a collection of points
	typedef std::vector<point*> points_t;
	/// Stores the set of points within the mesh that are shared among the rest of the mesh geometry
	points_t points;

	/// Defines a collection of rendered point groups
	typedef std::vector<point_group*> point_groups_t;
	/// Stores a collection of rendered point groups
	point_groups_t point_groups;

	/// Defines a collection of manifold polyhedra
	typedef std::vector<polyhedron*> polyhedra_t;
	/// Stores a collection of manifold polyhedra
	polyhedra_t polyhedra;

	/// Defines a collection of linear curve groups
	typedef std::vector<linear_curve_group*> linear_curve_groups_t;
	/// Stores a collection of linear curve groups
	linear_curve_groups_t linear_curve_groups;

	/// Defines a collection of cubic curve groups
	typedef std::vector<cubic_curve_group*> cubic_curve_groups_t;
	/// Stores a collection of cubic curve groups
	cubic_curve_groups_t cubic_curve_groups;

	/// Defines a collection of NURBS curve groups
	typedef std::vector<nucurve_group*> nucurve_groups_t;
	/// Stores a collection of nucurves
	nucurve_groups_t nucurve_groups;

	/// Defines a collection of bilinear patches
	typedef std::vector<bilinear_patch*> bilinear_patches_t;
	/// Stores a collection of bilinear patches
	bilinear_patches_t bilinear_patches;

	/// Defines a collection of bicubic patches
	typedef std::vector<bicubic_patch*> bicubic_patches_t;
	/// Stores a collection of bicubic patches
	bicubic_patches_t bicubic_patches;

	/// Defines a collection of nupatches
	typedef std::vector<nupatch*> nupatches_t;
	/// Stores a collection of nupatches
	nupatches_t nupatches;

	/// Defines a collection of blobbies
	typedef std::vector<blobby*> blobbies_t;
	/// Stores a collection of blobbies
	blobbies_t blobbies;

	/// Conversion from a new mesh to a legacy mesh
	mesh& operator=(const k3d::mesh& RHS);
	
private:
	mesh(const mesh& RHS);
	mesh& operator=(const mesh& RHS);
};

/// Function template in the spirit of std::for_each that applies a functor to every point in a mesh
template<typename T>
void for_each_point(mesh& Mesh, T Functor)
{
	for(mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
		Functor(**point);
}

/// Function template in the spirit of std::for_each that applies a functor to every edge in a mesh
template<typename T>
void for_each_edge(mesh& Mesh, T Functor)
{
	for(mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		for(polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			for(split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
			{
				Functor(*edge);

				if(edge->face_clockwise == (*face)->first_edge)
					break;
			}

			// Face holes
			for(face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
			{
				for(split_edge* edge = *hole; edge; edge = edge->face_clockwise)
				{
					Functor(*edge);

					if(edge->face_clockwise == (*hole))
						break;
				}
			}
		}
	}
}

/// Function template in the spirit of std::for_each that applies a functor to every face in a mesh
template<typename T>
void for_each_face(mesh& Mesh, T Functor)
{
	for(mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		Functor(**polyhedron);
		for(polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			Functor(**face);
	}
}

/// Function template in the spirit of std::for_each that applies a functor to every linear curve in a mesh
template<typename T>
void for_each_linear_curve(mesh& Mesh, T Functor)
{
	for(mesh::linear_curve_groups_t::iterator group = Mesh.linear_curve_groups.begin(); group != Mesh.linear_curve_groups.end(); ++group)
	{
		Functor(**group);
		for(linear_curve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}
}

/// Function template in the spirit of std::for_each that applies a functor to every cubic curve in a mesh
template<typename T>
void for_each_cubic_curve(mesh& Mesh, T Functor)
{
	for(mesh::cubic_curve_groups_t::iterator group = Mesh.cubic_curve_groups.begin(); group != Mesh.cubic_curve_groups.end(); ++group)
	{
		Functor(**group);
		for(cubic_curve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}
}

/// Function template in the spirit of std::for_each that applies a functor to every nucurve in a mesh
template<typename T>
void for_each_nucurve(mesh& Mesh, T Functor)
{
	for(mesh::nucurve_groups_t::iterator group = Mesh.nucurve_groups.begin(); group != Mesh.nucurve_groups.end(); ++group)
	{
		Functor(**group);
		for(nucurve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}
}

/// Function template in the spirit of std::for_each that applies a functor to every bilinear patch in a mesh
template<typename T>
void for_each_bilinear_patch(mesh& Mesh, T Functor)
{
	for(mesh::bilinear_patches_t::iterator patch = Mesh.bilinear_patches.begin(); patch != Mesh.bilinear_patches.end(); ++patch)
		Functor(**patch);
}

/// Function template in the spirit of std::for_each that applies a functor to every bicubic patch in a mesh
template<typename T>
void for_each_bicubic_patch(mesh& Mesh, T Functor)
{
	for(mesh::bicubic_patches_t::iterator patch = Mesh.bicubic_patches.begin(); patch != Mesh.bicubic_patches.end(); ++patch)
		Functor(**patch);
}

/// Function template in the spirit of std::for_each that applies a functor to every bilinear patch in a mesh
template<typename T>
void for_each_nupatch(mesh& Mesh, T Functor)
{
	for(mesh::nupatches_t::iterator patch = Mesh.nupatches.begin(); patch != Mesh.nupatches.end(); ++patch)
		Functor(**patch);
}

/// Function template in the spirit of std::for_each that applies a functor to every part of a mesh
template<typename T>
T for_each_component(mesh& Mesh, T Functor)
{
	Functor(Mesh);

	for(mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
		Functor(**point);

	for(mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		Functor(**polyhedron);

		for(polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			Functor(**face);

			for(split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
			{
				Functor(*edge);

				if(edge->face_clockwise == (*face)->first_edge)
					break;
			}

			// Face holes
			for(face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
			{
				for(split_edge* edge = *hole; edge; edge = edge->face_clockwise)
				{
					Functor(*edge);

					if(edge->face_clockwise == (*hole))
						break;
				}
			}
		}
	}

	for(mesh::linear_curve_groups_t::iterator group = Mesh.linear_curve_groups.begin(); group != Mesh.linear_curve_groups.end(); ++group)
	{
		Functor(**group);
		for(linear_curve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}

	for(mesh::cubic_curve_groups_t::iterator group = Mesh.cubic_curve_groups.begin(); group != Mesh.cubic_curve_groups.end(); ++group)
	{
		Functor(**group);
		for(cubic_curve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}

	for(mesh::nucurve_groups_t::iterator group = Mesh.nucurve_groups.begin(); group != Mesh.nucurve_groups.end(); ++group)
	{
		Functor(**group);
		for(nucurve_group::curves_t::iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			Functor(**curve);
	}

	for(mesh::bilinear_patches_t::iterator patch = Mesh.bilinear_patches.begin(); patch != Mesh.bilinear_patches.end(); ++patch)
		Functor(**patch);

	for(mesh::bicubic_patches_t::iterator patch = Mesh.bicubic_patches.begin(); patch != Mesh.bicubic_patches.end(); ++patch)
		Functor(**patch);

	for(mesh::nupatches_t::iterator patch = Mesh.nupatches.begin(); patch != Mesh.nupatches.end(); ++patch)
		Functor(**patch);

	return Functor;
}

/// Decomposes a collection of faces into triangles
void triangulate(const polyhedron::faces_t& Faces, polyhedron::faces_t& NewFaces, mesh::points_t& NewPoints);

/// Creates data for a unit cube and appends it to the given Mesh & Polyhedron
void add_unit_cube(mesh& Mesh, polyhedron& Polyhedron, imaterial* const Material);
/// Defines a reference to polygonal grid data that maintains the topology in a convenient form
typedef boost::tuple<boost::multi_array<point*, 2>, boost::multi_array<split_edge*, 3>, boost::multi_array<face*, 2> > grid_results_t;
/// Creates data for a grid, optionally stitched along either or both axes (e.g. a cylinder or a torus)
grid_results_t add_grid(mesh& Mesh, polyhedron& Polyhedron, const unsigned long Rows, const unsigned long Columns, const bool StitchTop, const bool StitchSide, imaterial* const Material);
/// Copies the input mesh into the output mesh
void deep_copy(const mesh& Input, mesh& Output);

/// Returns the number of segments in a linear curve, based on the control point count and wrap state
const unsigned long segment_count(const linear_curve& Curve, const bool Wrap);
/// Returns the number of segments in a cubic curve, based on the control point count, basis, and wrap state
const unsigned long segment_count(const cubic_curve& Curve, const bool Wrap);

/// Returns the number of varying parameters in a linear curve, based on the control point count and wrap state
const unsigned long varying_count(const linear_curve& Curve, const bool Wrap);
/// Returns the number of varying parameters in a cubic curve, based on the control point count, basis, and wrap state
const unsigned long varying_count(const cubic_curve& Curve, const bool Wrap);

/// Returns true iff the given polyhedron contains valid data
bool is_valid(const polyhedron& Polyhedron);
/// Returns true iff the given NURBS curve contains valid data
bool is_valid(const nucurve& Curve);
/// Returns true iff the given NURBS patch contains valid data
bool is_valid(const nupatch& Patch);

/// Returns true iff the given polyhedron is a solid volume (no holes!)
bool is_solid(const polyhedron& Polyhedron);

/// Returns a bounding-box containing every point in the given mesh
const bounding_box3 bounds(const mesh& Mesh);

} // namespace legacy

} // namespace k3d

#endif // K3DSDK_LEGACY_MESH_H

