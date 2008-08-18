#ifndef MODULE_NURBS_CURVE_MODIFIER_H
#define MODULE_NURBS_CURVE_MODIFIER_H
// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
*/

#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/point4.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/gprim_factory.h>
#include <deque>
#include "math.h"

#include "nurbs_patch_modifier.h"

#define MODULE_NURBS_DEBUG 1
#define nurbs_debug __FILE__ << ": " << __LINE__ << " "
#define MY_DEBUG if(MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug

namespace module{
	namespace nurbs{

        struct nurbs_curve;
        struct nurbs_trim_curve;
        struct nurbs_patch;
	    class nurbs_patch_modifier;

        ///All NURBS curve editing methods and algorithms are packaged here
        class nurbs_curve_modifier{
            public:
                ///Create an instance of nurbs_curve_modifier working on the given mesh
                ///\param input The mesh to work on
                nurbs_curve_modifier(k3d::mesh& input);

                ///Add a curve to this mesh - gprims add_nurbs_curve doesnt handle groups
                ///\param curve The curve to add
                ///\param shared Set this to true to make use of existing vertices in the mesh if these coincidence with the curve's control points
                int add_curve(nurbs_curve& curve, bool shared);

                ///Returns the index of the selected curve
                int selected_curve();

                ///Returns the indices of the selected curves
                std::vector<k3d::uint_t> selected_curves();

                ///Print the knot vector of the selected curve to k3d::log
                ///\param curve The index of the curve
                void print_knot_vector(k3d::uint_t curve);

                ///Replace a point on the curve with a new one (by changing the index)
                ///\param newIndex the index of the point which shall be used from now on
                ///\param curve The curve we're operating on
                ///\param point The index of the point which shall be replaced
                ///\param continuous If this is set to true, the curve's knot vector is changed so that the curve intersects the point
                void replace_point(k3d::uint_t newIndex, k3d::uint_t curve, k3d::uint_t point, bool continuous);

                ///Flips a curve in memory, so swaps the order of points and inverts the knot vector
                ///\param curve The curve which shall be flipped
                void flip_curve(k3d::uint_t curve);

                ///Normalize the knot vector of the given curve to a range of [0,1]
                ///\param curve The curve whose knot vector will be normalized
                void normalize_knot_vector(k3d::uint_t curve);

                ///Returns the number of nurbs curves in this complete mesh
                int count_all_curves_in_groups();

                ///Returns the curve group in which this curve is located
                ///\param curve The curve to search for
                int get_curve_group(k3d::uint_t curve);

                ///Removes all empty groups
                void remove_empty_groups();

                ///Removes all points which are never referenced
                void remove_unused_points();

                ///Delete the selected curve and all points which become unused
                ///\param curve The curve to delete
                void delete_curve(k3d::uint_t curve);

                ///Returns true if the start and endpoint reference to the same point
                bool is_closed(k3d::uint_t curve);

                ///Opens the selected curve, meaning the end point will no longer be shared as start&end point
                void open_up_curve(k3d::uint_t curve);

                ///Join (connect) the two curves at their selected end points, one of the curve gets deleted
                ///\param point1 The end point of the first curve (needs to be an end point!)
                ///\param curve1 One of the 2 curves to join
                ///\param point2 The end point of the 2nd curve
                ///\param curve2 The curve to join with the other
                void join_curves(k3d::uint_t point1, k3d::uint_t curve1, k3d::uint_t point2, k3d::uint_t curve2);

                ///Returns true if the two points are equal or don't vary more than the given threshold in any component
                ///\param p1 The points to compare
                ///\param p2 The points to compare
                ///\param threshold The maximum distance in each component
                static bool point3_float_equal(const k3d::point3& p1, const k3d::point3& p2, float threshold);

                ///Returns the homogenous point evaluated on the curve at the given u-value
                ///\param curve The curve
                ///\param u The u-value where to evaluate the point
                k3d::point4 curve_point(k3d::uint_t curve,double u);

                ///Insert a knot into a curve, makes use of the algorithm in "The NURBS book" by A. Piegl and W. Tiller
                ///\param curve The curve
                ///\param u The u-value where to insert the knot
                ///\param r The multiplicity of the new knot
                bool curve_knot_insertion(k3d::uint_t curve,double u, k3d::uint_t r);

                ///Close the selected curve either by adding a new point or by replacing the former end points by their average
                ///\param curve The curve to close
                ///\param keep_ends If this is true then we're going to add a new point, otherwise the old end points get discarded
                void close_curve(k3d::uint_t curve,bool keep_ends);

                ///DegreeElevation increases the degree and order of the curve with a step of 1
                ///Follows the implementation of A. Piegl and W. Tiller in "The NURBS book"
                ///\param curve The curve we want to degree elevate
                int curve_degree_elevate(k3d::uint_t curve);

                ///Returns the homogenous point with index point
                ///\param point The index to the point in curve_points and curve_point_weights
                k3d::point4 get_homogenous_point(k3d::uint_t point);

                ///Split the selected curve at the knot value u
                ///Follows the implementation of A. Piegl and W. Tiller in "The NURBS book"
                ///\param curve The curve to śplit
                ///\param u The u-value at which we're going to split
                ///\param reconnect If the curve was closed, connect it at the initial start/end point to have one single curve
                void split_curve_at(k3d::uint_t curve, double u, bool reconnect);

                ///Returns a nurbs_curve struct which is a copy of the chosen curve
                ///\param curve The curve to extract
                nurbs_curve extract_curve(k3d::uint_t curve);

                ///A common knot vector of the given list of curves will be
                ///generated and then curve_knot_insertion is called so that all curves have this knot vector
                ///Furthermore they all have the same degree after this operation
                void knot_vector_adaption(std::vector<k3d::uint_t> curves);

                ///Moves curve2 along curve1 and creates a NURBS surface representing the area the curve has moved over
                ///It doesnt matter which curve is curve1 and which is curve2 as it results in the same shape
                ///\param curve1 one curve
                ///\param curve2 the other curve
                ///\param create_caps Whether or not to create caps at the ends - only if one of the curves is a loop
                void traverse_curve(k3d::uint_t curve1, k3d::uint_t curve2, bool create_caps);

                ///Revolves the given curve around the selected axis and adds the patch to this mesh
                ///\param curve The curve which we're going to revolve
                ///\param axis The axis to revolve around
                ///\param angle the angle around which we do revolve (360 means a circle)
                ///\param segments The number of segments our circle should have
                ///\param caps Whether or not to create caps at the ends - just has influence if angle = 2*pi
                void revolve_curve(k3d::uint_t curve, k3d::axis axis, double angle, int segments, bool caps);

                ///Spans up a NURBS surface between the 2 selected curves
                ///\param curve1
                ///\param curve2
                void ruled_surface(k3d::uint_t curve1, k3d::uint_t curve2);

                ///Creates a cap for the given curve, the curve needs to be closed
                ///The surface will fill out the whole area whithin the curve
                ///\param curve
                bool create_cap(k3d::uint_t curve);

                ///This generates a surface which will use the given curves as "skeleton", as control shape.
                ///The curves get ordered by their y-value, so make sure you take care of that when creating these
                ///Your resulting surface will have order 3 in y-direction
                ///\param curves A vector of curves which shall be used as skeleton for the skin
                void skinned_surface(std::vector<k3d::uint_t> curves, k3d::axis axis);

                ///Moves curve2 along curve1 and adjusts is along the tangent of curve1 at this position
                ///creates a NURBS surface representing the area the curve has moved over
                ///It doesnt matter which curve is curve1 and which is curve2 as it results in the same shape
                ///\param curve1 one curve
                ///\param curve2 the other curve
                ///\param segments how many segments the swept curve should have
                ///\param create_caps Whether or not to create caps at the ends - only if one of the curves is a loop
                void sweep_surface(k3d::uint_t curve1, k3d::uint_t curve2, k3d::uint_t segments, bool create_caps);

                ///Selects this curve and deselects all others
                void select_curve(k3d::uint_t curve);

                ///Creates a linear_curve_groups representation of the given curve and deletes the original curve if thats selected
                ///\param curve The curve we're going to poligonize
                ///\param segments How many segments the new curve representation shall have
                ///\param delete_curve Whether or not to delete the original NURBS curve
                void polygonize_curve(k3d::uint_t curve, k3d::uint_t segments, bool delete_curve);

                ///Creates a trimming curve from the given curve, discards the Z component and uses the first point as end point
                ///so the result is a closed curve
                ///\param curve The curve to be converted. The original will still exist
                nurbs_trim_curve create_trim_curve(k3d::uint_t curve);

                ///Returns the span in which the knot value u lies
                ///\param curve The curve to examine
                ///\param u We want the span which contains this value
                int find_span(k3d::uint_t curve,double u);

                ///Returns an array of the basis function coefficients which influence the knot at the given value and in this span
                ///\param curve The curve
                ///\param u The u-value where we want to know the basis functions
                ///\param span The span in which u lies
                std::vector<double> basis_functions(k3d::uint_t curve, double u, k3d::uint_t span);

            private:

                ///Adds this point to the mesh's points_t instance, and returns the index to its position
                ///\param point The point to add
                ///\param shared If this is true then we'll try to use an existing point at the same position
                k3d::uint_t insert_point(k3d::point3& point, bool shared);

                ///returns the factorial of n
                int factorial(int n);

                ///Returns the binomial coefficient n over k = n! / ((n-k)! * k!)
                double binomial_coefficient(int n, int k);

                ///Return the minimum of two integers
                int Min(int a, int b);
                ///Returns the maximum of the two ints
                int Max(int a, int b);

                ///Fills the "bezalfs" vector - bezier curve alpha coefficients, used by degee_elevate
                void fill_bezalfs(std::vector<std::vector<double> >& bezalfs, int power, int t);

                ///Print a point to the debug output
                std::string output_point(const k3d::point4& point);

                ///Print a point to the debug output
                std::string output_point(const k3d::point3& point);

                ///Given a set of homogenous points this puts the points into the mesh and returns a list of indices to the inserted mesh points
                ///\param points The homogenous points
                k3d::mesh::indices_t create_curve_points(std::vector<k3d::point4>& points);

                ///Given a set of homogenous points this returns an array of the weights of these
                ///\param points the homogenous points
                k3d::mesh::weights_t create_curve_point_weights(std::vector<k3d::point4>& points);

                ///Returns true if index was inside the list of points
                ///\param points The indices array to search through
                ///\param index The index to look for
                bool find_point_inside(k3d::mesh::indices_t *points, k3d::uint_t index);

                ///Offset all indices in "points" greater than "first" to the amount of offset
                ///\param points The indices to operate on
                ///\param first the index which is the last one not offsetted
                ///\param offset The amount to which all indices greater than first will be increased
                void offset_all_after(k3d::mesh::indices_t *points, k3d::uint_t first, int offset);

                ///Calculates a tangent vector on this curve at knot value u (in 0,1)
                ///This might give unpredictable results on non-deriveable curves
                ///because it works with the difference quotient
                k3d::point3 calculate_curve_tangent(k3d::uint_t curve, double u);

                k3d::mesh *m_instance;
                k3d::mesh::nurbs_curve_groups_t *groups;
                k3d::mesh::knots_t *curve_knots;
                k3d::mesh::indices_t *curve_points;
                k3d::mesh::weights_t *curve_point_weights;
                k3d::mesh::counts_t *curve_counts;
                k3d::mesh::orders_t *curve_orders;
                k3d::mesh::counts_t *curve_point_counts;
                k3d::mesh::indices_t *curve_first_points;
                k3d::mesh::indices_t *curve_first_knots;
                k3d::mesh::indices_t *first_curves;
                k3d::mesh::selection_t *curve_selection;
                k3d::mesh::selection_t *point_selection;
                k3d::mesh::points_t *mesh_points;
                k3d::mesh::materials_t *materials;
        };
	}//namespace nurbs
}//namespace module

#endif // MODULE_NURBS_CURVE_OPERATIONS_H
