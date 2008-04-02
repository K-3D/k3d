// K-3D
// Copyright (c) 2004-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Remeshing tool based on Harmonic Functions for Quadrilateral Remeshing of Arbitrary Manifolds, S. Dong, S. Kircher, M. Garland
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include <set>
#include <set>
#include <map>
#include <stack>

namespace module
{

namespace quadrilateral_remeshing
{

namespace detail
{

extern "C"
{
#include <superlu/dsp_defs.h>
}

const k3d::normal3 normal(k3d::legacy::split_edge* Edge)
{
	// Calculates the normal for an edge loop using the summation method, which is more robust than the three-point methods (handles zero-length edges)
	k3d::normal3 result(0, 0, 0);

	for(k3d::legacy::split_edge* edge = Edge; ; )
	{
		const k3d::point3& i = edge->vertex->position;
		const k3d::point3& j = edge->face_clockwise->vertex->position;

		result[0] += (i[1] + j[1]) * (j[2] - i[2]);
		result[1] += (i[2] + j[2]) * (j[0] - i[0]);
		result[2] += (i[0] + j[0]) * (j[1] - i[1]);

		edge = edge->face_clockwise;
		if(edge == Edge)
			break;
	}

	return 0.5 * result;
}

double cotangent(const double cosine)
{
	const double sine = std::sqrt(1 - cosine*cosine);
	if(sine != 0)
		return cosine / sine;

	k3d::log() << debug << "Null triangle area" << std::endl;
	return 0;
}

/// Gauss-Jordan elimination for a 3x3 matrix based system
// Original code by Stephen R. Schmitt
class GaussJordan_3x3
{
public:
	std::vector<double> M;

	double get(const unsigned long row, const unsigned long col)
	{
		return M[(col-1) + 4*(row-1)];
	}

	void set(const unsigned long row, const unsigned long col, const double val)
	{
		M[(col-1) + 4*(row-1)] = val;
	}

	// Row operation to zero non-diagonal element A(j,i)
	// A(j) = A(j) - m * A(i)
	// NOTE: (j != i)
	void row_operation(const unsigned long j, const unsigned long i)
	{
		// Choose m so that A(j,i) --> 0
		const double m = get(j,i) / get(i,i);

		for(unsigned long k = 4; k >= i; --k)
		{
			const double t = get(j,k) - m * get(i,k);
			set(j, k, t);
		}
	}

	// Find first row below A(i,i) with non-zero element in column i
	bool swap(const unsigned long i)
	{
		unsigned long first = 0;

		for(unsigned long j = i; j <= 3; ++j)
		{
			if(get(j,i) == 0)
				continue;

			first = j;
			break;
		}

		// There's no solution
		if(!first)
			return false;

		// A row is available, swap with row i
		for(unsigned long k = 1; k <= 4; ++k)
		{
			// Swap M[i, k] and M[first, k]
			const double t = get(i,k);
			set(i, k, get(first,k));
			set(first, k, t);
		}

		return true;
	}

	// Multiplies each element of a row by the inverse of A(i,i)
	void normalize(const unsigned long i)
	{
		double m = get(i, i);
		if(m == 0)
			return;

		m = 1 / m;
		for(unsigned long j = i; j <= 4; ++j)
			set(i, j, m * get(i, j));
	}

	// Returns false if the linear system is singular
	bool solve()
	{
		// Check A11
		if(get(1,1) == 0)
		{
			if(!swap(1))
				return false;
		}

		// Zero A21
		row_operation(2, 1);
		// Zero A31
		row_operation(3, 1);

		// Check A22
		if(get(2,2) == 0.0)
		{
			if(!swap(2))
				return false;
		}

		// Zero A32
		row_operation(3, 2);

		// Check A33
		if(get(3,3) == 0.0)
			return false;

		// Zero A23
		row_operation(2, 3);
		// Zero A13
		row_operation(1, 3);
		// Zero A12
		row_operation(1, 2);

		// A11 --> 1
		normalize(1);
		// A22 --> 1
		normalize(2);
		// A33 --> 1
		normalize(3);

		return true;
	}
};

bool segment_intersection(const double s1x1, const double s1y1, const double s1x2, const double s1y2,
	const double s2x1, const double s2y1, const double s2x2, const double s2y2,
	double& r, double &s)
{
	double denominator = (s1x2 - s1x1)*(s2y2 - s2y1) - (s1y2 - s1y1)*(s2x2 - s2x1);
	if(denominator == 0)
		// Parallel
		return false;

	r = ((s1y1 - s2y1)*(s2x2 - s2x1) - (s1x1 - s2x1)*(s2y2 - s2y1)) / denominator;
	s = ((s1y1 - s2y1)*(s1x2 - s1x1) - (s1x1 - s2x1)*(s1y2 - s1y1)) / denominator;

	return true;
}

bool segment_intersection(const k3d::point3& A, const k3d::point3& B, const k3d::point3& C, const k3d::point3& D, double& r, double& s)
{
	// Cross product
	k3d::vector3 dot = k3d::normalize((B - A) ^ (D - C));

	// Find best axis projection
	double x = std::fabs(dot * k3d::vector3(1, 0, 0));
	double y = std::fabs(dot * k3d::vector3(0, 1, 0));
	double z = std::fabs(dot * k3d::vector3(0, 0, 1));

	if(x > y)
	{
		if(x > z)
			return segment_intersection(A[1], A[2], B[1], B[2], C[1], C[2], D[1], D[2], r, s);
		else
			return segment_intersection(A[0], A[1], B[0], B[1], C[0], C[1], D[0], D[1], r, s);
	}

	if(y > z)
		return segment_intersection(A[0], A[2], B[0], B[2], C[0], C[2], D[0], D[2], r, s);

	return segment_intersection(A[0], A[1], B[0], B[1], C[0], C[1], D[0], D[1], r, s);
}

bool is_point_inside_triangle(const k3d::point3& point, const k3d::point3& p1, const k3d::point3& p2, const k3d::point3& p3)
{
	// Compute area when projected on Z plane
	double area = 0;
	area += (p1[0] * p2[1]) - (p2[0] * p1[1]);
	area += (p2[0] * p3[1]) - (p3[0] * p2[1]);
	area += (p3[0] * p1[1]) - (p1[0] * p3[1]);
	if(area != 0)
	{
		const double a = (p2[0] - p1[0]) * (point[1] - p1[1]) - (p2[1] - p1[1]) * (point[0] - p1[0]);
		const double b = (p3[0] - p2[0]) * (point[1] - p2[1]) - (p3[1] - p2[1]) * (point[0] - p2[0]);
		const double c = (p1[0] - p3[0]) * (point[1] - p3[1]) - (p1[1] - p3[1]) * (point[0] - p3[0]);
		if(a >= 0 && b >= 0 && c >= 0)
			return true;
		if(a <= 0 && b <= 0 && c <= 0)
			return true;

		// Check for very small values
		const double epsilon = 1e-6;
		if(std::fabs(a) < epsilon)
		{
			if(b >= 0 && c >= 0)
				return true;
			if(b <= 0 && c <= 0)
				return true;
		}
		if(std::fabs(b) < epsilon)
		{
			if(a >= 0 && c >= 0)
				return true;
			if(a <= 0 && c <= 0)
				return true;
		}
		if(std::fabs(c) < epsilon)
		{
			if(a >= 0 && b >= 0)
				return true;
			if(a <= 0 && b <= 0)
				return true;
		}

		return false;
	}
	else
		assert_not_reached();

	return false;
}

// Point to segment distance
double distance_to_segment(const k3d::point3& Point, const k3d::point3& S1, const k3d::point3& S2)
{
	const k3d::vector3 vector = S2 - S1;
	const k3d::vector3 w = Point - S1;

	const double c1 = w * vector;
	if(c1 <= 0)
		return k3d::distance(Point, S1);

	const double c2 = vector * vector;
	if(c2 <= c1)
		return k3d::distance(Point, S2);

	const double b = c1 / c2;
	const k3d::point3 middlepoint = S1 + b * vector;
	return k3d::distance(Point, middlepoint);
}
double distance2_to_segment(const k3d::point3& Point, const k3d::point3& S1, const k3d::point3& S2)
{
	const k3d::vector3 vector = S2 - S1;
	const k3d::vector3 w = Point - S1;

	const double c1 = w * vector;
	if(c1 <= 0)
		return (Point - S1).length2();

	const double c2 = vector * vector;
	if(c2 <= c1)
		return (Point - S2).length2();

	const double b = c1 / c2;
	const k3d::point3 middlepoint = S1 + b * vector;
	return (Point - middlepoint).length2();
}

/// Robust vector comparison
bool equal_vectors(const k3d::point3& v1, const k3d::point3& v2)
{
	if(fabs(v1[0] - v2[0]) < 1e-10
			&& fabs(v1[1] - v2[1]) < 1e-10
			&& fabs(v1[2] - v2[2]) < 1e-10)
		return true;

	return false;
}

}

/////////////////////////////////////////////////////////////////////////////
// quadrilateral_remeshing

class quadrilateral_remeshing :
	public k3d::material_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

	typedef unsigned long index_t;

public:
	quadrilateral_remeshing(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_minima_selection(init_owner(*this) + init_name("minima_selection") + init_label(_("Minima selection")) + init_description(_("Minima Selection")) + init_value(k3d::mesh_selection())),
		m_maxima_selection(init_owner(*this) + init_name("maxima_selection") + init_label(_("Maxima selection")) + init_description(_("Maxima Selection")) + init_value(k3d::mesh_selection())),
		m_min_index(init_owner(*this) + init_name("min_index") + init_label(_("Min index")) + init_description(_("Min index")) + init_value(0) + init_constraint(constraint::minimum(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_max_index(init_owner(*this) + init_name("max_index") + init_label(_("Max index")) + init_description(_("Max index")) + init_value(0) + init_constraint(constraint::minimum(0)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_h1(init_owner(*this) + init_name("gradient_spacing") + init_label(_("Gradient spacing")) + init_description(_("Gradient spacing")) + init_value(0.4) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_h2(init_owner(*this) + init_name("iso_spacing") + init_label(_("Iso spacing")) + init_description(_("Iso spacing")) + init_value(0.4) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_minima_value(init_owner(*this) + init_name("minima_value") + init_label(_("Minima value")) + init_description(_("Minima value")) + init_value(-1000) + init_step_increment(10) + init_units(typeid(k3d::measurement::scalar))),
		m_maxima_value(init_owner(*this) + init_name("maxima_value") + init_label(_("Maxima value")) + init_description(_("Maxima value")) + init_value(1000) + init_step_increment(10) + init_units(typeid(k3d::measurement::scalar)))//,
		//m_curvature_sensibility(init_owner(*this) + init_name("curvature_sensibility") + init_label(_("Curvature sensibility")) + init_description(_("Curvature sensibility")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{

		m_minima_selection.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		m_maxima_selection.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		m_min_index.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		m_max_index.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		m_h1.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		m_h2.changed_signal().connect(sigc::mem_fun(*this, &quadrilateral_remeshing::on_reset_all));
		//m_curvature_sensibility.changed_signal().connect(make_reset_mesh_slot());

		m_material.changed_signal().connect(make_reset_mesh_slot());

		m_input_changed = false;
	}

	void on_reset_geometry()
	{
		m_output_mesh.reset();
	}

	void on_reset_all(k3d::iunknown*)
	{
		on_input_change();
		on_reset_geometry();
	}

	void on_input_change()
	{
		m_input_changed = true;
	}

	// Mesh preprocessing : computed only once
	bool preprocess_geometry(const k3d::legacy::mesh& input)
	{
		// Create a mapping of points-to-zero-based-indices, store points and their parameters
		std::map<k3d::legacy::point*, unsigned long> point_map;
		std::vector<k3d::legacy::point*> points;
		points.reserve(input.points.size());

		m_extrema.clear();

		unsigned long index = 0;
		for(k3d::legacy::mesh::points_t::const_iterator point = input.points.begin(); point != input.points.end(); ++point)
		{
			const k3d::point3 p = (*point)->position;
			points.push_back(*point);
			point_map[*point] = index;

			// Get flow start and end point values
			k3d::legacy::parameters_t::const_iterator value = (*point)->vertex_data.find("field_value");
			if(value != (*point)->vertex_data.end())
			{
				const double u = boost::any_cast<double>(value->second);
				if(u != 0)
					m_extrema.insert(std::make_pair(*point, extremum_t(u)));
			}

			++index;
		}

		// Check m_extrema list consistency
		bool valid_extrema = false;
		if(m_extrema.size() >= 2)
		{
			extrema_t::const_iterator extremum = m_extrema.begin();
			double extremum_value = extremum->second.u;
			for(++extremum; extremum != m_extrema.end(); ++extremum)
			{
				if(extremum->second.u != extremum_value)
				{
					valid_extrema = true;
					break;
				}
			}
		}

		// If there's no parameter value defined, check mesh selections
		const k3d::mesh_selection& minima_selection = m_minima_selection.pipeline_value();
		const k3d::mesh_selection& maxima_selection = m_maxima_selection.pipeline_value();
		if(!valid_extrema && minima_selection.points.size() && maxima_selection.points.size())
		{
			k3d::mesh_selection::records_t::const_iterator selection_item;
			const unsigned long max_point_index = points.size();

			unsigned long minima = 0;
			const double min_value = m_minima_value.pipeline_value();
			for(selection_item = minima_selection.points.begin(); selection_item != minima_selection.points.end(); ++selection_item)
			{
				const unsigned long selection_index = selection_item->begin;
				if(selection_index < max_point_index && selection_item->weight != 0)
				{
					k3d::log() << debug << "Adding minimum " << selection_index << std::endl;
					m_extrema.insert(std::make_pair(points[selection_index], extremum_t(min_value)));

					++minima;
				}
			}

			unsigned long maxima = 0;
			const double max_value = m_maxima_value.pipeline_value();
			for(selection_item = maxima_selection.points.begin(); selection_item != maxima_selection.points.end(); ++selection_item)
			{
				const unsigned long selection_index = selection_item->begin;
				if(selection_index < max_point_index && selection_item->weight != 0)
				{
					k3d::log() << debug << "Adding maximum " << selection_index << std::endl;
					m_extrema.insert(std::make_pair(points[selection_index], extremum_t(max_value)));

					++maxima;
				}
			}

			// Validate input
			if(minima && maxima)
				valid_extrema = true;
		}

		// Fall back on min and max values
		if(!valid_extrema)
		{
			// Get valid flow start and stop values
			const unsigned long flow_min = m_min_index.pipeline_value();
			const unsigned long flow_max = m_max_index.pipeline_value();

			// Make sure flow start and stop values are valid
			if(flow_min == flow_max || flow_min >= input.points.size() || flow_max >= input.points.size())
				return true;

			const double min_value = m_minima_value.pipeline_value();
			const double max_value = m_maxima_value.pipeline_value();

			// Save extremal points
			m_extrema.insert(std::make_pair(points[flow_min], extremum_t(min_value)));
			m_extrema.insert(std::make_pair(points[flow_max], extremum_t(max_value)));
		}

		// Construct piecewise-linear scalar field over vertices
		typedef std::vector<index_t> polygon_t;

		typedef std::set<index_t> index_t_set_t;
		typedef std::vector<index_t_set_t> adjacency_t;
		adjacency_t adjacency_sets;
		adjacency_sets.resize(points.size());
		std::vector< std::map<index_t, double> > cotangents;
		cotangents.resize(points.size());

		// Collect triangles information, make sure the input is a closed manifold made of triangles only ...
		unsigned long triangle_number = 0;
		m_triangles.clear();
		m_edge_triangle.clear();
		m_edge_triangle[0] = 0;
		k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = input.polyhedra.begin();
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			k3d::legacy::split_edge* first = (*face)->first_edge;
			return_val_if_fail(first, false);

			points_t coords;
			k3d::legacy::split_edge* current_edge = first;
			do
			{
				coords.push_back(current_edge->vertex->position);
				current_edge = current_edge->face_clockwise;

				// Make sure it's a closed manifold
				return_val_if_fail(current_edge->companion, false);
			}
			while(current_edge && current_edge != first);

			// Make sure there are only triangles
			return_val_if_fail(coords.size() == 3, false);

			// Compute discrete harmonic weights for edges
			k3d::vector3 v21 = k3d::normalize((coords[1] - coords[2]));
			k3d::vector3 v20 = k3d::normalize((coords[0] - coords[2]));
			const double cot12 = detail::cotangent(v21 * v20);

			k3d::vector3 v02 = k3d::normalize((coords[2] - coords[0]));
			k3d::vector3 v01 = k3d::normalize((coords[1] - coords[0]));
			const double cot23 = detail::cotangent(v02 * v01);

			k3d::vector3 v10 = k3d::normalize((coords[0] - coords[1]));
			k3d::vector3 v12 = k3d::normalize((coords[2] - coords[1]));
			const double cot31 = detail::cotangent(v10 * v12);

			k3d::normal3 normal = k3d::normalize(detail::normal((*face)->first_edge));

			k3d::legacy::split_edge* e1 = first;
			k3d::legacy::split_edge* e2 = first->face_clockwise;
			k3d::legacy::split_edge* e3 = first->face_clockwise->face_clockwise;

			const index_t v1 = point_map[e1->vertex];
			const index_t v2 = point_map[e2->vertex];
			const index_t v3 = point_map[e3->vertex];

			adjacency_sets[v1].insert(v2);
			adjacency_sets[v2].insert(v1);
			cotangents[v1].insert(std::pair<index_t, double>(v2, cot12));

			adjacency_sets[v2].insert(v3);
			adjacency_sets[v3].insert(v2);
			cotangents[v2].insert(std::pair<index_t, double>(v3, cot23));

			adjacency_sets[v3].insert(v1);
			adjacency_sets[v1].insert(v3);
			cotangents[v3].insert(std::pair<index_t, double>(v1, cot31));

			// Save normal, vertex numbers and edges
			triangle_t triangle_data = new triangle_data_t(triangle_number);
			triangle_data->normal = k3d::to_vector(normal);
			triangle_data->e1 = first;
			triangle_data->e2 = first->face_clockwise;
			triangle_data->e3 = first->face_clockwise->face_clockwise;
			m_triangles.push_back(triangle_data);

			m_edge_triangle.insert(std::make_pair(e1, triangle_data));
			m_edge_triangle.insert(std::make_pair(e2, triangle_data));
			m_edge_triangle.insert(std::make_pair(e3, triangle_data));

			++triangle_number;
		}

		std::vector<double> non_zero_coefficients;
		std::vector<index_t> row_indices;
		std::vector<index_t> coefficient_row_indices;
		coefficient_row_indices.push_back(0);

		// Build adjacency matrix
		unsigned long vn = 0;
		for(adjacency_t::const_iterator va = adjacency_sets.begin(); va != adjacency_sets.end(); ++va)
		{
			unsigned long row_values = row_indices.size();

			k3d::legacy::point* current_point = points[vn];
			extrema_t::iterator extremum = m_extrema.find(current_point);
			if(extremum != m_extrema.end())
			{
				extremum->second.index = vn;

				// Aij = deltaij if i is a constraint vertex
				non_zero_coefficients.push_back(1);
				row_indices.push_back(vn);

				row_values += 1;
			}
			else
			{
				polygon_t row;
				for(index_t_set_t::const_iterator sa = va->begin(); sa != va->end(); ++sa)
					row.push_back(*sa);
				assert_warning(row.size());
				std::sort(row.begin(), row.end());

				polygon_t::const_iterator row_item = row.begin();
				polygon_t::const_iterator row_end = row.end();
				bool passed_diagonal = false;
				double w_sum = 0;
				unsigned long diagonal_pos = 0;
				for(; row_item != row_end; ++row_item)
				{
					index_t adjacent_vertex = *row_item;

					// Lij = 1 if i == j
					if(adjacent_vertex > vn && !passed_diagonal)
					{
						diagonal_pos = non_zero_coefficients.size();
						non_zero_coefficients.push_back(1);
						row_indices.push_back(vn);

						passed_diagonal = true;
					}

					// Opposite angle cotangent
					double cot1 = cotangents[vn][adjacent_vertex];
					// Opposite angle cotangent in other triangle
					double cot2 = cotangents[adjacent_vertex][vn];
					double w = cot1 + cot2;
					w_sum += w;

					non_zero_coefficients.push_back(w);
					row_indices.push_back(adjacent_vertex);
				}

				// Lij = 1 if i == j
				w_sum = -w_sum;
				if(!passed_diagonal)
				{
					non_zero_coefficients.push_back(w_sum);
					row_indices.push_back(vn);
				}
				else
				{
					non_zero_coefficients[diagonal_pos] = w_sum;
				}

				row_values += row.size() + 1;
			}

			coefficient_row_indices.push_back(row_values);

			++vn;
		}

		// Create SuperLU matrices
		detail::SuperMatrix A, L, U, B;
		double	*a, *rhs;
		int	*asub, *xa;
		int	*perm_r; // row permutations from partial pivoting
		int	*perm_c; // column permutation vector
		int	info, i, m, n, nnz;
		detail::superlu_options_t options;
		detail::SuperLUStat_t stat;

		// Initialize matrix A
		const unsigned long points_size = points.size();
		m = n = points_size;
		nnz = non_zero_coefficients.size();
		return_val_if_fail(a = detail::doubleMalloc(nnz), false);
		return_val_if_fail(asub = detail::intMalloc(nnz), false);
		return_val_if_fail(xa = detail::intMalloc(n+1), false);

		assert_warning(coefficient_row_indices.size() == points_size + 1);


		std::vector<double>::const_iterator nz = non_zero_coefficients.begin();
		std::vector<double>::const_iterator nz_end = non_zero_coefficients.end();
		double* a_p = a;
		for(; nz != nz_end; ++nz)
			*a_p++ = *nz;

		std::vector<index_t>::const_iterator ri = row_indices.begin();
		std::vector<index_t>::const_iterator ri_end = row_indices.end();
		int* asub_p = asub;
		for(; ri != ri_end; ++ri)
			*asub_p++ = *ri;

		std::vector<index_t>::const_iterator cr = coefficient_row_indices.begin();
		std::vector<index_t>::const_iterator cr_end = coefficient_row_indices.end();
		int* xa_p = xa;
		for(; cr != cr_end; ++cr)
			*xa_p++ = *cr;

		// Create matrix A in the format expected by SuperLU
		detail::dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, detail::SLU_NR, detail::SLU_D, detail::SLU_GE);

		// Create right-hand side matrix B
		int nrhs = 1;
		return_val_if_fail(rhs = detail::doubleMalloc(m * nrhs), false);
		for (i = 0; i < m; ++i) rhs[i] = 0.0;
		for(extrema_t::const_iterator extremum = m_extrema.begin(); extremum != m_extrema.end(); ++extremum)
		{
			rhs[extremum->second.index] = extremum->second.u;
		}
		detail::dCreate_Dense_Matrix(&B, m, nrhs, rhs, m, detail::SLU_DN, detail::SLU_D, detail::SLU_GE);

		return_val_if_fail(perm_r = detail::intMalloc(m), false);
		return_val_if_fail(perm_c = detail::intMalloc(n), false);

		// Set the default input options
		detail::set_default_options(&options);
		options.ColPerm = detail::NATURAL;

		// Initialize the statistics variables
		detail::StatInit(&stat);

		// Run actual sparse matrix solver
		detail::dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);

		// Save scalar field values
		m_scalar_field.clear();
		for(i = 0; i < m; ++i)
		{
			m_scalar_field.insert(std::make_pair(points[i], rhs[i]));
		}

		// De-allocate SuperLU storage
		detail::SUPERLU_FREE (rhs);
		detail::SUPERLU_FREE (perm_r);
		detail::SUPERLU_FREE (perm_c);
		detail::Destroy_CompCol_Matrix(&A);
		detail::Destroy_SuperMatrix_Store(&B);
		detail::Destroy_SuperNode_Matrix(&L);
		detail::Destroy_CompCol_Matrix(&U);
		detail::StatFree(&stat);

		// List triangle neighbours, compute orthogonal and tangent fields (g1 and g2)
		triangles_t::const_iterator triangle_i = m_triangles.begin();
		triangles_t::const_iterator triangles_end = m_triangles.end();
		for(; triangle_i != triangles_end; ++triangle_i)
		{
			triangle_t triangle = (*triangle_i);

			// Gradient vector g1 (see paper for explanation)
			const k3d::point3 xi = triangle->e1->vertex->position;
			const k3d::point3 xj = triangle->e2->vertex->position;
			const k3d::point3 xk = triangle->e3->vertex->position;
			const k3d::vector3 normal = triangle->normal;

			const double ui = m_scalar_field[triangle->e1->vertex];
			const double uj = m_scalar_field[triangle->e2->vertex];
			const double uk = m_scalar_field[triangle->e3->vertex];

			const double xji = xj[0] - xi[0];
			const double yji = xj[1] - xi[1];
			const double zji = xj[2] - xi[2];
			const double xkj = xk[0] - xj[0];
			const double ykj = xk[1] - xj[1];
			const double zkj = xk[2] - xj[2];

			detail::GaussJordan_3x3 matrix;

			matrix.M.push_back(xji);
			matrix.M.push_back(yji);
			matrix.M.push_back(zji);
			matrix.M.push_back(uj - ui);

			matrix.M.push_back(xkj);
			matrix.M.push_back(ykj);
			matrix.M.push_back(zkj);
			matrix.M.push_back(uk - uj);

			matrix.M.push_back(normal[0]);
			matrix.M.push_back(normal[1]);
			matrix.M.push_back(normal[2]);
			matrix.M.push_back(0);

			// Well formed input mesh should never yield an unsolvable system
			assert_warning(matrix.solve());

			k3d::vector3 g1 = k3d::normalize(k3d::vector3(matrix.M[3], matrix.M[7], matrix.M[11]));
			triangle->orthogonal_vector = g1;

			// Orthogonal vector g2
			k3d::vector3 g2 = k3d::normalize(normal ^ g1);
			triangle->tangent_vector = g2;
		}

		// Find starting point for drawing flows (a minimal extremum)
		k3d::legacy::point* minimal_extremum = 0;
		double minimal_u;
		for(extrema_t::const_iterator extremum = m_extrema.begin(); extremum != m_extrema.end(); ++extremum)
		{
			if(!minimal_extremum)
			{
				minimal_extremum = extremum->first;
				minimal_u = extremum->second.u;

				continue;
			}

			const double current_u = extremum->second.u;
			if(current_u < minimal_u)
			{
				minimal_extremum = extremum->first;
				minimal_u = current_u;
			}
		}
		return_val_if_fail(minimal_extremum, false);

		m_starting_point = minimal_extremum->position;
		m_starting_u = minimal_u;

		m_starting_triangle = 0;
		for(triangles_t::const_iterator triangle = m_triangles.begin(); triangle != triangles_end; ++triangle)
		{
			if((*triangle)->e1->vertex == minimal_extremum ||
				(*triangle)->e2->vertex == minimal_extremum ||
				(*triangle)->e3->vertex == minimal_extremum)
			{
				m_starting_triangle = *triangle;
				break;
			}
		}

		on_reset_geometry();

		return true;
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Check for new mesh
		if(m_input_changed)
		{
			return_if_fail(preprocess_geometry(InputMesh));
			m_input_changed = false;
		}

		// Sanity checks
		if(!m_triangles.size() || !m_starting_triangle)
			return;
		return_if_fail(m_extrema.size() >= 2);

		// Create isoparametric flows
		segments_t iso_flows;
		m_iso_triangle_segments.clear();
		m_iso_triangle_segments.resize(m_triangles.size());
		if(!draw_isoparametric_flows(iso_flows))
			return;

		// Create gradient flows
		segments_t gradient_flows;
		m_gradient_triangle_segments.clear();
		m_gradient_triangle_segments.resize(m_triangles.size());
		draw_gradient_flows(gradient_flows);

/** \todo discard too short flows */

/*
// Draw gradient flows
for(unsigned long g = 0; g < gradient_flows.size(); ++g)
{
	k3d::linear_curve_group* const group = new k3d::linear_curve_group();
	k3d::linear_curve* const curve = new k3d::linear_curve();

	segment_t* segment = gradient_flows[g];
	segment_t* previous_segment = segment;

	while(segment)
	{
		const k3d::point3 p = segment->p1;
		k3d::legacy::point* const p_p = new k3d::legacy::point(p);
		Mesh.points.push_back(p_p);

		curve->control_points.push_back(p_p);

		previous_segment = segment;
		segment = segment->next;
	}

	const k3d::point3 p = previous_segment->p2;
	k3d::legacy::point* const p_p = new k3d::legacy::point(p);
	Mesh.points.push_back(p_p);

	curve->control_points.push_back(p_p);

	group->curves.push_back(curve);
	Mesh.linear_curve_groups.push_back(group);
}
// Draw isoparametric flows
for(unsigned long t = 0; t < iso_flows.size(); ++t)
{
	k3d::linear_curve_group* const group = new k3d::linear_curve_group();
	k3d::linear_curve* const curve = new k3d::linear_curve();

	segment_t* segment = iso_flows[t];
	segment_t* first_segment = segment;

	while(segment)
	{
		const k3d::point3 p = segment->p1;
		k3d::legacy::point* const p_p = new k3d::legacy::point(p);
		Mesh.points.push_back(p_p);

		curve->control_points.push_back(p_p);

		segment = segment->next;
		if(segment == first_segment)
			break;
	}

	// Close loop
	curve->control_points.push_back(curve->control_points[0]);

	group->curves.push_back(curve);
	Mesh.linear_curve_groups.push_back(group);
}
*/

		// Extract the connectivity of the ouput graph
		point_segments_map_t point_segments_map;
		crossings_t gradient_crossings;
		crossings_t iso_crossings;
		connectivity_graph(gradient_flows, iso_flows, point_segments_map, gradient_crossings, iso_crossings);

		// Delete segment lists
		for(segments_t::iterator segment = gradient_flows.begin(); segment != gradient_flows.end(); ++segment)
		{
			segment_t* s = *segment;
			while(s)
			{
				segment_t* previous_segment = s;
				s = s->next;

				delete previous_segment;
			}
		}

		gradient_flows.clear();

		for(segments_t::iterator segment = iso_flows.begin(); segment != iso_flows.end(); ++segment)
		{
			segment_t* s = *segment;
			segment_t* first_segment = s;

			while(s)
			{
				segment_t* previous_segment = s;
				s = s->next;

				delete previous_segment;

				if(s == first_segment)
					break;
			}
		}

		iso_flows.clear();

		// Create faces ...
		k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
		return_if_fail(polyhedron);
		Mesh.polyhedra.push_back(polyhedron);
		connect_polygons(Mesh, *polyhedron, gradient_crossings, iso_crossings, point_segments_map);

		assert_warning(is_solid(*polyhedron));
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<
			quadrilateral_remeshing,
			k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
					k3d::uuid(0xde6494ab, 0x1c4d448d, 0xa6a2657e, 0x1751fc93),
					"QuadrilateralRemeshing",
					"Quadrilateral Remeshing for Closed Manifolds",
					"Polygons",
					k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	// Geometric data structures ...
	typedef std::vector<k3d::point3> points_t;
	typedef std::vector<index_t> indices_t;

	typedef k3d::legacy::split_edge* edge_t;

	struct triangle_data_t
	{
		triangle_data_t(const unsigned long Number) :
			number(Number)
		{
		}

		index_t number;

		edge_t e1;
		edge_t e2;
		edge_t e3;

		k3d::vector3 normal;
		k3d::vector3 orthogonal_vector;
		k3d::vector3 tangent_vector;
	};

	typedef triangle_data_t* triangle_t;
	typedef std::vector<triangle_data_t*> triangles_t;

	typedef enum
	{
		EDGE, // Intersection is on the edge (anywhere on the edge but at the end-points)
		POINT, // Intersection with one of the two end-points
		NONE
	} intersection_t;

	struct extremum_t
	{
		extremum_t(const double U) :
			u(U)
		{
		}

		double u;

		unsigned long index;
	};

	typedef std::map<k3d::legacy::point*, extremum_t> extrema_t;

	class segment_t
	{
	public:
		segment_t(const k3d::point3& P1, const k3d::point3& P2) :
			p1(P1), p2(P2),
			previous(0), next(0)
		{
		}

		void sort_points()
		{
			if(points.size() < 2)
				return;

			// Find best projection axis
			unsigned long axis = 0;
			double max = std::fabs(p1[0] - p2[0]);
			double max_y = std::fabs(p1[1] - p2[1]);
			double max_z = std::fabs(p1[2] - p2[2]);
			if(max_y > max)
			{
				max = max_y;
				axis = 1;
			}
			if(max_z > max)
				axis = 2;

			// Up or down ?
			bool forward = true;
			if(p1[axis] > p2[axis])
				forward = false;

			// Sort points along projection axis (bubble sort)
			for(unsigned long n = 0; n < points.size() - 1; ++n)
			{
				unsigned long min_index = n;
				double min_value = points[n]->position[axis];
				for(unsigned long p = 0; p < points.size(); ++p)
				{
					if(points[p]->position[axis] < min_value)
					{
						min_index = p;
						min_value = points[p]->position[axis];
					}
				}

				if(min_index > n)
					std::swap(points[n], points[min_index]);
			}

			if(!forward)
				std::reverse(points.begin(), points.end());
		}

		k3d::legacy::point* first_point() { return points.size() ? points.front() : 0; }
		k3d::legacy::point* last_point() { return points.size() ? points.back() : 0; }

		k3d::legacy::point* next_point(const k3d::legacy::point* point)
		{
			if(points.size() >= 2)
			{
				unsigned long n = 0;
				while(n < points.size() - 1)
				{
					if(points[n] == point)
						return points[n + 1];

					n++;
				}
			}

			return 0;
		}

		k3d::legacy::point* previous_point(const k3d::legacy::point* point)
		{
			if(points.size() >= 2)
			{
				unsigned long n = points.size() - 1;
				while(n > 0)
				{
					if(points[n] == point)
						return points[n - 1];

					n--;
				}
			}

			return 0;
		}

		void remove_point(k3d::legacy::point* point)
		{
			std::vector<k3d::legacy::point*>::iterator p = std::find(points.begin(), points.end(), point);
			if(p == points.end())
				return;

			points.erase(p);
		}

		k3d::point3 p1;
		k3d::point3 p2;

		// Doubly-linked list
		segment_t* previous;
		segment_t* next;

		std::vector<k3d::legacy::point*> points;
	};

	typedef std::vector<segment_t*> segments_t;
	typedef std::vector<segments_t> segment_lists_t;

	// Crossing
	struct crossing_t
	{
		crossing_t(k3d::legacy::point* p) :
			vertex(p),
			next(0), previous(0),
			up_edge(0), down_edge(0)
		{
		}

		k3d::legacy::point* vertex;

		crossing_t* next;
		crossing_t* previous;

		k3d::legacy::split_edge* up_edge;
		k3d::legacy::split_edge* down_edge;

		bool forward;
	};

	typedef std::vector<crossing_t*> crossings_t;

	// Flow intersection
	class flow_intersection_t
	{
	public:
		flow_intersection_t() :
			gradient_segment(0), isoparametric_segment(0), gradient_c(0), isoparametric_c(0)
		{
		}

		flow_intersection_t(segment_t* g_s, segment_t* i_s) :
			gradient_segment(g_s), isoparametric_segment(i_s), gradient_c(0), isoparametric_c(0)
		{
		}

		~flow_intersection_t()
		{
		}

		segment_t* gradient_segment;
		segment_t* isoparametric_segment;
		crossing_t* gradient_c;
		crossing_t* isoparametric_c;
	};

	typedef std::map<k3d::legacy::point*, flow_intersection_t> point_segments_map_t;


	/// Test how m_direction intersects with segment [v1, v2] from m_intersection
	// edge is v1-v2's edge index in m_current_triangle
	edge_t test_intersection(edge_t edge, intersection_t& intersection_type)
	{
		const k3d::point3 p1 = edge->vertex->position;
		const k3d::point3 p2 = edge->face_clockwise->vertex->position;
		const double u1 = m_scalar_field[edge->vertex];
		const double u2 = m_scalar_field[edge->face_clockwise->vertex];

		// Intersection position on line 1 : m_intersection (0.0) -> m_intersection + m_direction (1.0)
		double r;
		// Intersection position on line 2 : p1 (0.0) -> p2 (1.0)
		double s;
		if(detail::segment_intersection(m_intersection, m_intersection + m_direction, p1, p2, r, s))
		{
			m_iedge1 = edge;
			m_iedge2 = edge->face_clockwise;

			// Segment intersects with v1
			if(s == 0 || (r > 0 && std::fabs(s) < 1e-6))
			{
				m_intersection = p1;
				m_new_u = u1;
				intersection_type = POINT;
				return edge;
			}
			// Segment intersects with v2
			else if(s == 1 || (r > 0 && (1 - 1e-6 < s) && (s < 1 + 1e-6)))
			{
				m_intersection = p2;
				m_new_u = u2;
				intersection_type = POINT;
				return edge->face_clockwise;
			}
			// Segments don't intersect
			else if(r < -1e-6)
			{
				m_missed = false;
			}
			// Intersection ~ 0 : m_intersection point is on (v1;v2) line
			else if(r < 1e-6)
			{
				intersection_type = EDGE;
				return edge->companion;
			}
			// Intersection is on half-infinite line starting from m_intersection following m_direction
			else if(s > 0 && s < 1)
			{
				m_intersection = p1 + s * (p2 - p1);
				m_new_u = u1 + s*(u2 - u1);

				intersection_type = EDGE;
				return edge->companion;
			}
			// Intersection is on half-infinite line but outside [v1;v2] segment
			else
			{
				if(s < 0)
				{
					m_missed = true;
					m_missed_index = edge;
				}
				if(s > 1)
				{
					m_missed = true;
					m_missed_index = edge->face_clockwise;
				}
			}
		}

		intersection_type = NONE;
		return 0;
	}

	// Turn around v to find next triangle crossed by the flow
	bool vertex_case(k3d::legacy::split_edge* v, k3d::legacy::split_edge* v_start, const bool forward, intersection_t& intersection_type)
	{
		k3d::legacy::split_edge* v_start2 = v_start;
		k3d::legacy::split_edge* v_current = v_start;
		bool prev_missed = false;
		k3d::legacy::split_edge* prev_missed_index = v;

		do
		{
			k3d::legacy::split_edge* edge1 = m_current_triangle->e1;
			k3d::legacy::split_edge* edge2 = m_current_triangle->e2;
			k3d::legacy::split_edge* edge3 = m_current_triangle->e3;

			intersection_type = NONE;
			if(v->vertex == edge1->vertex)
			{
				triangle_t new_triangle = m_edge_triangle[test_intersection(m_current_triangle->e2, intersection_type)];
				if(intersection_type == NONE)
				{
					if(v_current == edge2)
						v_start2 = edge3;

					v_current = edge2;

					if(m_missed)
					{
						if(prev_missed)
						{
							if(m_missed_index->vertex == prev_missed_index->vertex)
							{
								m_intersection = m_missed_index->vertex->position;
								m_new_u = m_scalar_field[m_missed_index->vertex];
								return vertex_case(m_iedge1, m_iedge2, forward, intersection_type);
							}
						}

						prev_missed = true;
						prev_missed_index = m_missed_index;
					}

					m_current_triangle = m_edge_triangle[m_current_triangle->e1->companion];
					m_direction = forward ? m_current_triangle->orthogonal_vector : - m_current_triangle->orthogonal_vector;
				}
				else if(intersection_type == POINT)
				{
					m_through = m_current_triangle;
					return true;
				}
				else if(intersection_type == EDGE)
				{
					m_through = m_current_triangle;
					m_current_triangle = new_triangle;
					return true;
				}
			}
			else if(v->vertex == edge2->vertex)
			{
				triangle_t new_triangle = m_edge_triangle[test_intersection(m_current_triangle->e3, intersection_type)];
				if(intersection_type == NONE)
				{
					if(v_current == edge3)
						v_start2 = edge1;

					v_current = edge3;

					if(m_missed)
					{
						if(prev_missed)
						{
							if(m_missed_index->vertex == prev_missed_index->vertex)
							{
								m_intersection = m_missed_index->vertex->position;
								m_new_u = m_scalar_field[m_missed_index->vertex];
								return vertex_case(m_iedge1, m_iedge2, forward, intersection_type);
							}
						}

						prev_missed = true;
						prev_missed_index = m_missed_index;
					}

					m_current_triangle = m_edge_triangle[m_current_triangle->e2->companion];
					m_direction = forward ? m_current_triangle->orthogonal_vector : - m_current_triangle->orthogonal_vector;
				}
				else if(intersection_type == POINT)
				{
					m_through = m_current_triangle;
					return true;
				}
				else if(intersection_type == EDGE)
				{
					m_through = m_current_triangle;
					m_current_triangle = new_triangle;
					return true;
				}
			}
			else if(v->vertex == edge3->vertex)
			{
				triangle_t new_triangle = m_edge_triangle[test_intersection(m_current_triangle->e1, intersection_type)];
				if(intersection_type == NONE)
				{
					if(v_current == edge1)
						v_start2 = edge2;

					v_current = edge1;

					if(m_missed)
					{
						if(prev_missed)
						{
							if(m_missed_index->vertex == prev_missed_index->vertex)
							{
								m_intersection = m_missed_index->vertex->position;
								m_new_u = m_scalar_field[m_missed_index->vertex];
								return vertex_case(m_iedge1, m_iedge2, forward, intersection_type);
							}
						}

						prev_missed = true;
						prev_missed_index = m_missed_index;
					}

					m_current_triangle = m_edge_triangle[m_current_triangle->e3->companion];
					m_direction = forward ? m_current_triangle->orthogonal_vector : - m_current_triangle->orthogonal_vector;
				}
				else if(intersection_type == POINT)
				{
					m_through = m_current_triangle;
					return true;
				}
				else if(intersection_type == EDGE)
				{
					m_through = m_current_triangle;
					m_current_triangle = new_triangle;
					return true;
				}
			}
			else
{
assert_not_reached();
				return false;
}

		}
		while(v_current->vertex != v_start2->vertex);

		return true;
	}

	// Regular case: m_intersection lies on an edge (doesn't coincide with any of the three vertices)
	bool edge_intersection(intersection_t& intersection_type)
	{
		m_through = m_current_triangle;

		k3d::legacy::split_edge* edge1 = m_current_triangle->e1;
		k3d::legacy::split_edge* edge2 = m_current_triangle->e2;
		k3d::legacy::split_edge* edge3 = m_current_triangle->e3;

		unsigned long other_v_pos = 0;
		if(edge1->vertex == m_iedge1->vertex || edge1->vertex == m_iedge2->vertex)
		{
			if(edge2->vertex == m_iedge1->vertex || edge2->vertex == m_iedge2->vertex)
				other_v_pos = 2;
			else
				other_v_pos = 1;
		}

		edge_t new_edge;
		if(other_v_pos == 0)
		{
			// Check other 2 edges
			new_edge = test_intersection(m_current_triangle->e1, intersection_type);
			if(intersection_type == NONE)
				new_edge = test_intersection(m_current_triangle->e3, intersection_type);
		}
		else if(other_v_pos == 1)
		{
			// Check other 2 edges
			new_edge = test_intersection(m_current_triangle->e1, intersection_type);
			if(intersection_type == NONE)
				new_edge = test_intersection(m_current_triangle->e2, intersection_type);
		}
		else
		{
			// Check other 2 edges
			new_edge = test_intersection(m_current_triangle->e2, intersection_type);
			if(intersection_type == NONE)
				new_edge = test_intersection(m_current_triangle->e3, intersection_type);
		}

		if(intersection_type == EDGE)
		{
			// Intersection with another edge
			m_current_triangle = m_edge_triangle[new_edge];
			return true;
		}

		if(intersection_type == POINT)
		{
			// Intersection with one the two end-points (vertex case)
			return true;
		}

		// intersection_type == NONE :

		// Edge case
		// find which of the two end-points m_direction points to
		intersection_type = POINT;
		if(other_v_pos == 0)
		{
			k3d::point3 p2 = m_current_triangle->e2->vertex->position;
			k3d::point3 p3 = m_current_triangle->e3->vertex->position;
			const double dot_product = (p3 - p2) * m_direction;
			if(dot_product > 0)
			{
				m_intersection = p3;
				m_new_u = m_scalar_field[edge3->vertex];
				m_iedge1 = edge3;
				m_iedge2 = edge2;
			}
			else if(dot_product < 0)
			{
				m_intersection = p2;
				m_new_u = m_scalar_field[edge2->vertex];
				m_iedge1 = edge2;
				m_iedge2 = edge3;
			}
			else
			{
				assert_not_reached();
				return false;
			}
		}
		else if(other_v_pos == 1)
		{
			k3d::point3 p3 = m_current_triangle->e3->vertex->position;
			k3d::point3 p1 = m_current_triangle->e1->vertex->position;
			const double dot_product = (p1 - p3) * m_direction;
			if(dot_product > 0)
			{
				m_intersection = p1;
				m_new_u = m_scalar_field[edge1->vertex];
				m_iedge1 = edge1;
				m_iedge2 = edge3;
			}
			else if(dot_product < 0)
			{
				m_intersection = p3;
				m_new_u = m_scalar_field[edge3->vertex];
				m_iedge1 = edge3;
				m_iedge2 = edge1;
			}
			else
			{
				assert_not_reached();
				return false;
			}
		}
		else
		{
			k3d::point3 p1 = m_current_triangle->e1->vertex->position;
			k3d::point3 p2 = m_current_triangle->e2->vertex->position;
			const double dot_product = (p2 - p1) * m_direction;
			if(dot_product > 0)
			{
				m_intersection = p2;
				m_new_u = m_scalar_field[edge2->vertex];
				m_iedge1 = edge2;
				m_iedge2 = edge1;
			}
			else if(dot_product < 0)
			{
				m_intersection = p1;
				m_new_u = m_scalar_field[edge1->vertex];
				m_iedge1 = edge1;
				m_iedge2 = edge2;
			}
			else
			{
				assert_not_reached();
				return false;
			}
		}

		return true;
	}

	/// From m_intersection, find which triangle's edge or point intersects with m_direction
	edge_t find_triangle_intersection(triangle_t triangle, intersection_t& intersection_type)
	{
		m_through = m_current_triangle;

		// Get triangle's vertex indices
		k3d::legacy::split_edge* edge1 = triangle->e1;
		k3d::legacy::split_edge* edge2 = triangle->e2;
		k3d::legacy::split_edge* edge3 = triangle->e3;

		// Avoid current edge (when crossing this edge, avoid self-intersection check)
		unsigned long index = 3;
		if(m_iedge1 && m_iedge2)
		{
			if(m_iedge2->vertex == edge1->vertex && m_iedge1->vertex == edge2->vertex)
				index = 0;
			else if(m_iedge2->vertex == edge2->vertex && m_iedge1->vertex == edge3->vertex)
				index = 1;
			else if(m_iedge2->vertex == edge3->vertex && m_iedge1->vertex == edge1->vertex)
				index = 2;
		}

		// Temp values
		bool saved_values = false;
		intersection_t save_intersection_type;
		k3d::point3 save_intersection;
		k3d::legacy::split_edge* save_iedge1;
		k3d::legacy::split_edge* save_iedge2;
		edge_t save_neighbour;

		const double epsilon = 1e-6;
		const k3d::point3 previous_point = m_intersection;
		if(index != 0)
		{
			edge_t new_edge = test_intersection(triangle->e1, intersection_type);
			if(intersection_type != NONE)
			{
				if(k3d::distance(m_intersection, previous_point) >= epsilon)
					return new_edge;

				saved_values = true;
				save_intersection_type = intersection_type;
				save_intersection = m_intersection;
				save_iedge1 = m_iedge1;
				save_iedge2 = m_iedge2;
				save_neighbour = new_edge;
			}
		}

		if(index != 1)
		{
			edge_t new_edge = test_intersection(triangle->e2, intersection_type);
			if(intersection_type != NONE)
			{
				if(k3d::distance(m_intersection, previous_point) >= epsilon)
					return new_edge;

				saved_values = true;
				save_intersection_type = intersection_type;
				save_intersection = m_intersection;
				save_iedge1 = m_iedge1;
				save_iedge2 = m_iedge2;
				save_neighbour = new_edge;
			}
		}

		if(index != 2)
		{
			edge_t new_edge = test_intersection(triangle->e3, intersection_type);
			if(intersection_type != NONE)
			{
				if(k3d::distance(m_intersection, previous_point) < epsilon)
					return new_edge;

				saved_values = true;
				save_intersection_type = intersection_type;
				save_intersection = m_intersection;
				save_iedge1 = m_iedge1;
				save_iedge2 = m_iedge2;
				save_neighbour = new_edge;
			}
		}

		if(saved_values)
		{
			intersection_type = save_intersection_type;
			m_intersection = save_intersection;
			m_iedge1 = save_iedge1;
			m_iedge2 = save_iedge2;

			return save_neighbour;
		}

		// Check whether point is near an edge, then check neighbour
		const k3d::point3 p1 = m_current_triangle->e1->vertex->position;
		const k3d::point3 p2 = m_current_triangle->e2->vertex->position;
		const k3d::point3 p3 = m_current_triangle->e3->vertex->position;
		const double edge1_dist = detail::distance_to_segment(m_intersection, p1, p2);
		const double edge2_dist = detail::distance_to_segment(m_intersection, p2, p3);
		const double edge3_dist = detail::distance_to_segment(m_intersection, p3, p1);

		unsigned long edge_index = 0;
		double edge_distance = edge1_dist;
		if(edge2_dist < edge_distance)
		{
			edge_index = 1;
			edge_distance = edge2_dist;
		}
		if(edge3_dist < edge_distance)
		{
			edge_index = 2;
			edge_distance = edge3_dist;
		}

		// Continue along edge
		if(std::fabs(edge_distance) < 1e-6)
		{
			intersection_type = POINT;
			if(edge_index == 0)
			{
				if((p2 - p1) * m_direction > 0)
				{
					m_intersection = p2;
					return triangle->e2;
				}
				else
				{
					m_intersection = p1;
					return triangle->e1;
				}

			}
			if(edge_index == 1)
			{
				if((p3 - p2) * m_direction > 0)
				{
					m_intersection = p3;
					return triangle->e3;
				}
				else
				{
					m_intersection = p2;
					return triangle->e2;
				}

			}
			if(edge_index == 2)
			{
				if((p1 - p3) * m_direction > 0)
				{
					m_intersection = p1;
					return triangle->e1;
				}
				else
				{
					m_intersection = p3;
					return triangle->e3;
				}

			}
		}

		assert_not_reached();
		return 0;
	}

	// Returns true if no segment was found within distance
	bool flow_distance_test(const bool seed_point, const bool gradient)
	{
		double distance = m_gradient_distance;
		if(!gradient)
			distance = m_isoparametric_distance;
		double square_distance = distance * distance;

		triangle_t start_triangle = m_current_triangle;
		const k3d::point3 intersection = m_intersection;

		if(seed_point)
		{
			square_distance *= 0.9;
		}
		else
		{
			square_distance *= 0.25;

			// Decrease distance near extrema
			if(near_extremum(intersection, m_isoparametric_distance * 2))
				square_distance *= 0.01;
		}

		// Check neighbouring triangles
		std::set<triangle_t> done;
		std::stack<triangle_t> triangles;
		triangles.push(start_triangle);
		while(triangles.size())
		{
			triangle_t t = triangles.top();

			// Release triangle
			done.insert(t);
			triangles.pop();

			// Save neighbouring triangles if they're not too far
			const k3d::point3 p1 = t->e1->vertex->position;
			const k3d::point3 p2 = t->e2->vertex->position;
			const k3d::point3 p3 = t->e3->vertex->position;

			const bool d1 = (intersection - p1).length2() < square_distance;
			const bool d2 = (intersection - p2).length2() < square_distance;
			const bool d3 = (intersection - p3).length2() < square_distance;

			if(d1 || d2)
			{
				triangle_t new_triangle = m_edge_triangle[t->e1->companion];
				if(done.find(new_triangle) == done.end())
					triangles.push(new_triangle);
			}
			if(d2 || d3)
			{
				triangle_t new_triangle = m_edge_triangle[t->e2->companion];
				if(done.find(new_triangle) == done.end())
					triangles.push(new_triangle);
			}
			if(d3 || d1)
			{
				triangle_t new_triangle = m_edge_triangle[t->e3->companion];
				if(done.find(new_triangle) == done.end())
					triangles.push(new_triangle);
			}

			// Check distance against triangle's segments
			if(gradient)
			{
				for(unsigned long n = 0; n < m_gradient_triangle_segments[t->number].size(); ++n)
				{
					segment_t* segment = m_gradient_triangle_segments[t->number][n];
					if(detail::distance2_to_segment(intersection, segment->p1, segment->p2) < square_distance)
						// TODO : check distance along mesh surface for better accuracy
						return false;
				}
			}
			else
			{
				for(unsigned long n = 0; n < m_iso_triangle_segments[t->number].size(); ++n)
				{
					segment_t* segment = m_iso_triangle_segments[t->number][n];
					if(detail::distance2_to_segment(intersection, segment->p1, segment->p2) < square_distance)
						// TODO : check distance along mesh surface for better accuracy
						return false;
				}
			}
		}

		return true;
	}

	// Build a gradient flow line forward or backward
	void gradient_flow(const triangle_t start_triangle, const k3d::point3& start_point, const double start_u, const bool forward, points_t& point_list, triangles_t& triangle_list, std::vector<double>& u_list, segments_t& segments, triangles_t& segment_triangles)
	{
		m_current_triangle = start_triangle;
		m_intersection = start_point;
		m_new_u = start_u;

		m_missed = false;
		m_iedge1 = 0;
		m_iedge2 = 0;

		k3d::point3 previous_intersection = m_intersection;
		double previous_u = m_new_u;

		m_direction = m_current_triangle->orthogonal_vector;
		if(!forward)
			m_direction = - m_direction;
		intersection_t intersection_type = NONE;
		m_current_triangle = m_edge_triangle[find_triangle_intersection(start_triangle, intersection_type)];

		if(intersection_type == NONE)
			return;

		do
		{
			// Avoid loops that may happen in convoluted mesh areas
			if(m_intersection == previous_intersection)
				break;

			segment_t* segment = 0;
			if(forward)
				segment = new segment_t(previous_intersection, m_intersection);
			else
				segment = new segment_t(m_intersection, previous_intersection);

			segments.push_back(segment);
			segment_triangles.push_back(m_through);

			point_list.push_back(m_intersection);
			triangle_list.push_back(m_current_triangle);
			u_list.push_back(m_new_u);

			previous_intersection = m_intersection;
			previous_u = m_new_u;

			m_direction = m_current_triangle->orthogonal_vector;
			if(!forward)
				m_direction = - m_direction;

			if(intersection_type == EDGE)
				return_if_fail(edge_intersection(intersection_type));
			else if(intersection_type == POINT)
				vertex_case(m_iedge1, m_iedge2, forward, intersection_type);
		}
		while(intersection_type != NONE && flow_distance_test(false, true));
	}

	// Build a gradient flow line from either side of start_point
	void gradient_flow_line(triangle_t start_triangle, const k3d::point3& start_point, const double start_u, points_t& point_list, triangles_t& triangle_list, std::vector<double>& u_list, segments_t& gradient_flows)
	{
		// Store segments
		segments_t segments;
		triangles_t segment_triangles;

		// Down flow points
		gradient_flow(start_triangle, start_point, start_u, false, point_list, triangle_list, u_list, segments, segment_triangles);

		// Reverse down lists
		std::reverse(point_list.begin(), point_list.end());
		std::reverse(triangle_list.begin(), triangle_list.end());
		std::reverse(u_list.begin(), u_list.end());
		std::reverse(segments.begin(), segments.end());
		std::reverse(segment_triangles.begin(), segment_triangles.end());

		unsigned long middle_index = segments.size();

		// Up flow points
		gradient_flow(start_triangle, start_point, start_u, true, point_list, triangle_list, u_list, segments, segment_triangles);

		// Link and save segments
		segment_t* previous_segment = 0;
		for(unsigned long n = 0; n < segments.size(); ++n)
		{
			segment_t* segment = segments[n];
			triangle_t triangle = segment_triangles[n];

			// Save segment start
			if(n == 0)
				gradient_flows.push_back(segment);

			if(middle_index && (middle_index < segments.size()) && (middle_index == n)
					&& (segment_triangles[middle_index-1]->number == segment_triangles[middle_index]->number))
			{
				// Remove middle triangle point
				previous_segment->p2 = segment->p2;
				delete segment;

				continue;
			}

			if(previous_segment)
			{
				previous_segment->next = segment;
				segment->previous = previous_segment;
			}

			m_gradient_triangle_segments[triangle->number].push_back(segment);

			previous_segment = segment;
		}
	}

	void draw_gradient_flows(segments_t& gradient_flows)
	{
		// Get and save first gradient flow
		points_t first_flow;
		triangles_t first_triangles;
		std::vector<double> first_values;
		gradient_flow_line(m_starting_triangle, m_starting_point, m_starting_u, first_flow, first_triangles, first_values, gradient_flows);

		// Gradient flows
		std::vector<points_t> current_flows;
		std::vector<triangles_t> current_flows_triangles;
		std::vector< std::vector<double> > current_flows_values;

		current_flows.push_back(first_flow);
		current_flows_triangles.push_back(first_triangles);
		current_flows_values.push_back(first_values);

		// Trace other gradient flow lines using h1 as flow distance ...
		const double h1 = m_h1.pipeline_value();
		m_gradient_distance = h1;

		// Go through every flow we create
		unsigned long current_flow = 0;

		while(current_flow < current_flows.size())
		{
			points_t seed_points;
			triangles_t seed_triangles;
			std::vector<double> seed_values;

			// Find a seed point from either side for each flow point
			for(unsigned long n = 1; n < current_flows[current_flow].size() - 1; ++n)
			{
				const k3d::point3& intersection = current_flows[current_flow][n];
				triangle_t triangle = current_flows_triangles[current_flow][n];
				const double u = current_flows_values[current_flow][n];

				k3d::point3 new_point;
				triangle_t new_triangle;
				if(isoparametric_seed_point(triangle, intersection, u, h1, new_point, new_triangle, true))
				{
					seed_points.push_back(new_point);
					seed_triangles.push_back(new_triangle);
					seed_values.push_back(m_new_u);
				}

				m_intersection = current_flows[current_flow][n];
				m_current_triangle = current_flows_triangles[current_flow][n];
				m_new_u = current_flows_values[current_flow][n];
				if(isoparametric_seed_point(triangle, intersection, u, h1, new_point, new_triangle, false))
				{
					seed_points.push_back(new_point);
					seed_triangles.push_back(new_triangle);
					seed_values.push_back(m_new_u);
				}
			}

			// Draw new flows
			for(unsigned long n = 0; n < seed_points.size(); ++n)
			{
				m_intersection = seed_points[n];
				m_current_triangle = seed_triangles[n];
				m_new_u = seed_values[n];
				if(flow_distance_test(true, true))
				{
					points_t new_flow;
					triangles_t new_triangles;
					std::vector<double> new_values;
					gradient_flow_line(seed_triangles[n], seed_points[n], seed_values[n], new_flow, new_triangles, new_values, gradient_flows);
					if(new_flow.size())
					{
						current_flows.push_back(new_flow);
						current_flows_triangles.push_back(new_triangles);
						current_flows_values.push_back(new_values);
					}
				}
			}

			++current_flow;
		}
	}

	bool gradient_seed_point(const triangle_t start_triangle, const k3d::point3& start_point, const double start_u, const double distance, triangle_t& seed_triangle, k3d::point3& seed_point, double& seed_u, const bool forward)
	{
		m_current_triangle = start_triangle;
		m_intersection = start_point;
		m_new_u = start_u;

		m_missed = false;
		m_iedge1 = 0;
		m_iedge2 = 0;

		k3d::point3 previous_point = m_intersection;
		double previous_u = m_new_u;

		m_direction = m_current_triangle->orthogonal_vector;
		if(!forward)
			m_direction = - m_direction;
		intersection_t intersection_type;
		m_current_triangle = m_edge_triangle[find_triangle_intersection(m_current_triangle, intersection_type)];
		return_val_if_fail(intersection_type != NONE, false);

		double length = 0;
		do
		{
			// Avoid loops that may happen in convoluted mesh areas
			if(m_intersection == previous_point)
				break;

			const double current_length = k3d::distance(m_intersection, previous_point);
			if(length + current_length >= distance)
			{
				seed_triangle = m_through;
				seed_point = previous_point + (m_intersection - previous_point) * (distance - length) / current_length;
				seed_u = previous_u + (m_new_u - previous_u) * (distance - length) / current_length;

				return true;
			}

			length += current_length;

			previous_point = m_intersection;
			previous_u = m_new_u;

			m_direction = m_current_triangle->orthogonal_vector;
			if(!forward)
				m_direction = - m_direction;

			if(intersection_type == EDGE)
				return_val_if_fail(edge_intersection(intersection_type), false);
			else if(intersection_type == POINT)
				vertex_case(m_iedge1, m_iedge2, forward, intersection_type);
		}
		while(intersection_type != NONE);

		return false;
	}

	/// From m_intersection, find which triangle's edge or point intersects with m_direction
	triangle_t iso_find_triangle_intersection(const double u, intersection_t& intersection_type)
	{
		m_through = m_current_triangle;

		const k3d::point3 p1 = m_current_triangle->e1->vertex->position;
		const k3d::point3 p2 = m_current_triangle->e2->vertex->position;
		const k3d::point3 p3 = m_current_triangle->e3->vertex->position;
		k3d::legacy::split_edge* edge1 = m_current_triangle->e1;
		k3d::legacy::split_edge* edge2 = m_current_triangle->e2;
		k3d::legacy::split_edge* edge3 = m_current_triangle->e3;
		const double u1 = m_scalar_field[edge1->vertex];
		const double u2 = m_scalar_field[edge2->vertex];
		const double u3 = m_scalar_field[edge3->vertex];

		// Avoid current edge
		unsigned long index = 3;
		if(m_iedge1 && m_iedge2)
		{
			if(m_iedge2->vertex == edge1->vertex && m_iedge1->vertex == edge2->vertex)
				index = 0;
			else if(m_iedge2->vertex == edge2->vertex && m_iedge1->vertex == edge3->vertex)
				index = 1;
			else if(m_iedge2->vertex == edge3->vertex && m_iedge1->vertex == edge1->vertex)
				index = 2;
		}

		// Temp values
		bool saved_values = false;
		intersection_t save_intersection_type;
		k3d::point3 save_intersection;
		k3d::legacy::split_edge* save_iedge1;
		k3d::legacy::split_edge* save_iedge2;
		triangle_t save_neighbour;

		const double epsilon = 1e-6;
		intersection_type = NONE;
		if(index != 0)
		{
			// Intersection with v1
			if(u1 == u)
			{
				m_iedge1 = edge1;
				m_iedge2 = edge2;
				intersection_type = POINT;
				m_intersection = p1;
				return m_current_triangle;
			}

			// Intersection with v2
			if(u2 == u)
			{
				m_iedge1 = edge1;
				m_iedge2 = edge2;
				intersection_type = POINT;
				m_intersection = p2;
				return m_current_triangle;
			}

			// Intersection with edge ]v1;v2[
			if((u1 < u && u < u2) || (u2 < u && u < u1))
			{
				const double coef = (u - u1) / (u2 - u1);
				const k3d::point3 candidate = p1 + (p2 - p1) * coef;
				k3d::vector3 cdir = (candidate - m_intersection);

				// Normalize
				const double clen = cdir.length();
				if(clen < epsilon)
				{
					// Point on edge, check neighbouring triangle
					saved_values = true;
					save_intersection_type = EDGE;
					save_intersection = candidate;
					save_iedge1 = edge1;
					save_iedge2 = edge2;
					save_neighbour = m_edge_triangle[m_current_triangle->e1->companion];
				}
				else
				{
					cdir /= clen;
					const double cos = cdir * m_direction;
					if(((index != 3) && (cos > 0.9)) || cos > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_iedge1 = edge1;
						m_iedge2 = edge2;
						return m_edge_triangle[m_current_triangle->e1->companion];
					}
				}
			}
		}
		if(index != 1)
		{
			if(u2 == u)
			{
				m_iedge1 = edge2;
				m_iedge2 = edge3;
				intersection_type = POINT;
				m_intersection = p2;
				return m_current_triangle;
			}

			if(u3 == u)
			{
				m_iedge1 = edge2;
				m_iedge2 = edge3;
				intersection_type = POINT;
				m_intersection = p3;
				return m_current_triangle;
			}

			if((u2 < u && u < u3) || (u3 < u && u < u2))
			{
				const double coef = (u - u2) / (u3 - u2);
				const k3d::point3 candidate = p2 + (p3 - p2) * coef;
				k3d::vector3 cdir = (candidate - m_intersection);

				// Normalize
				const double clen = cdir.length();
				if(clen < epsilon)
				{
					// Point on edge, check neighbouring triangle
					saved_values = true;
					save_intersection_type = EDGE;
					save_intersection = candidate;
					save_iedge1 = edge2;
					save_iedge2 = edge3;
					save_neighbour = m_edge_triangle[m_current_triangle->e2->companion];
				}
				else
				{
					cdir /= clen;
					const double cos = cdir * m_direction;
					if(((index != 3) && (cos > 0.9)) || cos > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_iedge1 = edge2;
						m_iedge2 = edge3;
						return m_edge_triangle[m_current_triangle->e2->companion];
					}
				}
			}
		}
		if(index != 2)
		{
			if(u3 == u)
			{
				m_iedge1 = edge3;
				m_iedge2 = edge1;
				intersection_type = POINT;
				m_intersection = p3;
				return m_current_triangle;
			}

			if(u1 == u)
			{
				m_iedge1 = edge3;
				m_iedge2 = edge1;
				intersection_type = POINT;
				m_intersection = p3;
				return m_current_triangle;
			}

			if((u3 < u && u < u1) || (u1 < u && u < u3))
			{
				const double coef = (u - u3) / (u1 - u3);
				const k3d::point3 candidate = p3 + (p1 - p3) * coef;
				k3d::vector3 cdir = (candidate - m_intersection);

				// Normalize
				const double clen = cdir.length();
				if(clen < epsilon)
				{
					// Point on edge, check neighbouring triangle
					saved_values = true;
					save_intersection_type = EDGE;
					save_intersection = candidate;
					save_iedge1 = edge3;
					save_iedge2 = edge1;
					save_neighbour = m_edge_triangle[m_current_triangle->e3->companion];
				}
				else
				{
					cdir /= clen;
					const double cos = cdir * m_direction;
					if(((index != 3) && (cos > 0.9)) || cos > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_iedge1 = edge3;
						m_iedge2 = edge1;
						return m_edge_triangle[m_current_triangle->e3->companion];
					}
				}
			}
		}

		if(saved_values)
		{
			intersection_type = save_intersection_type;
			m_intersection = save_intersection;
			m_iedge1 = save_iedge1;
			m_iedge2 = save_iedge2;

			return save_neighbour;
		}

		// Check whether point is near an edge, then check neighbour
		const double edge1_dist = detail::distance_to_segment(m_intersection, p1, p2);
		const double edge2_dist = detail::distance_to_segment(m_intersection, p2, p3);
		const double edge3_dist = detail::distance_to_segment(m_intersection, p3, p1);

		unsigned long edge_index = 0;
		double edge_distance = edge1_dist;
		if(edge2_dist < edge_distance)
		{
			edge_index = 1;
			edge_distance = edge2_dist;
		}
		if(edge3_dist < edge_distance)
		{
			edge_index = 2;
			edge_distance = edge3_dist;
		}

		const double nearest_edge_length = edge_index == 0 ? k3d::distance(p2, p1) : edge_index == 1 ? k3d::distance(p3, p2) : k3d::distance(p3, p1);
		const double edge_ratio = edge_distance / nearest_edge_length;
		if(edge_ratio < 0.01)
		{
			intersection_type = EDGE;
			m_iedge1 = 0;
			m_iedge2 = 0;
			return m_edge_triangle[edge_index == 0 ? m_current_triangle->e1->companion : (edge_index == 1 ? m_current_triangle->e2->companion : m_current_triangle->e3->companion)];
		}

		// Point outside triangle
		assert_not_reached();
		return 0;
	}

	// Turn around v to find which triangle is crossed by the isoparametric flow
	void iso_vertex_case(const double u, k3d::legacy::split_edge* v, k3d::legacy::split_edge* v_start, const bool forward, intersection_t& intersection_type)
	{
		k3d::legacy::split_edge* v_start2 = v_start;
		k3d::legacy::split_edge* v_current = v_start;
		m_intersection = v->vertex->position;

		m_iedge1 = 0;
		m_iedge2 = 0;

		do
		{
			k3d::legacy::split_edge* edge1 = m_current_triangle->e1;
			k3d::legacy::split_edge* edge2 = m_current_triangle->e2;
			k3d::legacy::split_edge* edge3 = m_current_triangle->e3;
			const double u1 = m_scalar_field[edge1->vertex];
			const double u2 = m_scalar_field[edge2->vertex];
			const double u3 = m_scalar_field[edge3->vertex];

			m_direction = m_current_triangle->tangent_vector;
			if(!forward)
				m_direction = - m_direction;

			intersection_type = NONE;
			if(v->vertex == edge1->vertex)
			{
				if(((u2 < u && u < u3) || (u3 < u && u < u2)))
				{
					const k3d::point3 p2 = m_current_triangle->e2->vertex->position;
					const k3d::point3 p3 = m_current_triangle->e3->vertex->position;
					const k3d::point3 candidate = p2 + (p3 - p2) * (u - u2) / (u3 - u2);
					if((candidate - m_intersection) * m_direction > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_current_triangle = m_edge_triangle[m_current_triangle->e2->companion];
						m_through = m_current_triangle;
						return;
					}
				}
				else if(u2 == u || u3 == u)
				{
					m_through = m_current_triangle;
					return;
				}

				m_current_triangle = m_edge_triangle[m_current_triangle->e1->companion];
				if(v_current == edge2)
					v_start2 = edge3;

				v_current = edge2;
			}
			else if(v->vertex == edge2->vertex)
			{
				if(((u3 < u && u < u1) || (u1 < u && u < u3)))
				{
					const k3d::point3 p3 = m_current_triangle->e3->vertex->position;
					const k3d::point3 p1 = m_current_triangle->e1->vertex->position;
					const k3d::point3 candidate = p3 + (p1 - p3) * (u - u3) / (u1 - u3);
					if((candidate - m_intersection) * m_direction > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_current_triangle = m_edge_triangle[m_current_triangle->e3->companion];
						m_through = m_current_triangle;
						return;
					}
				}
				else if(u3 == u || u1 == u)
				{
					m_through = m_current_triangle;
					return;
				}

				m_current_triangle = m_edge_triangle[m_current_triangle->e2->companion];
				if(v_current == edge3)
					v_start2 = edge1;

				v_current = edge3;
			}
			else if(v->vertex == edge3->vertex)
			{
				if(((u1 < u && u < u2) || (u2 < u && u < u1)))
				{
					const k3d::point3 p1 = m_current_triangle->e1->vertex->position;
					const k3d::point3 p2 = m_current_triangle->e2->vertex->position;
					const k3d::point3 candidate = p1 + (p2 - p1) * (u - u1) / (u2 - u1);
					if((candidate - m_intersection) * m_direction > 0)
					{
						intersection_type = EDGE;
						m_intersection = candidate;
						m_current_triangle = m_edge_triangle[m_current_triangle->e1->companion];
						m_through = m_current_triangle;
						return;
					}
				}
				else if(u1 == u || u2 == u)
				{
					m_through = m_current_triangle;
					return;
				}

				m_current_triangle = m_edge_triangle[m_current_triangle->e3->companion];
				if(v_current == edge1)
					v_start2 = edge2;

				v_current = edge1;
			}
			else
				assert_not_reached();

		}
		while(v_current->vertex != v_start2->vertex);

		assert_not_reached();
	}

	bool isoparametric_flow_line(triangle_t start_triangle, const k3d::point3& start_point, const double u, points_t& point_list, triangles_t& triangle_list, segments_t& iso_flows)
	{
		// Up flow points ...
		m_current_triangle = start_triangle;
		m_intersection = start_point;

		m_direction = m_current_triangle->tangent_vector;
		m_missed = false;
		m_iedge1 = 0;
		m_iedge2 = 0;
		intersection_t intersection_type = NONE;

		// Skip triangle middle
		m_current_triangle = iso_find_triangle_intersection(u, intersection_type);
		return_val_if_fail(intersection_type != NONE, false);

		k3d::point3 first_intersection = m_intersection;

		k3d::point3 previous_intersection = m_intersection;

		point_list.push_back(m_intersection);
		triangle_list.push_back(m_current_triangle);

		m_direction = m_current_triangle->tangent_vector;
		if(intersection_type == EDGE)
			m_current_triangle = iso_find_triangle_intersection(u, intersection_type);
		else if(intersection_type == POINT)
			iso_vertex_case(u, m_iedge1, m_iedge2, true, intersection_type);
		else
			return_val_if_fail(false, false);

		segment_t* first_segment = 0;
		segment_t* previous_segment = 0;
		triangle_t segment_triangle = 0;

		segments_t segments;
		std::vector<unsigned long> throughs;
		do
		{
			// Avoid loops that may happen in convoluted mesh areas
			if(m_intersection == previous_intersection)
				break;

			segment_triangle = m_through;
			segment_t* segment = new segment_t(previous_intersection, m_intersection);
			segments.push_back(segment);
			throughs.push_back(m_through->number);

			if(previous_segment)
			{
				previous_segment->next = segment;
				segment->previous = previous_segment;
			}
			else
				first_segment = segment;
			previous_segment = segment;

			point_list.push_back(m_intersection);
			triangle_list.push_back(m_current_triangle);

			previous_intersection = m_intersection;

			m_direction = m_current_triangle->tangent_vector;
			if(intersection_type == EDGE)
				m_current_triangle = iso_find_triangle_intersection(u, intersection_type);
			else if(intersection_type == POINT)
				iso_vertex_case(u, m_iedge1, m_iedge2, true, intersection_type);
			else
				return_val_if_fail(false, false);

			if(!flow_distance_test(false, false))
				return false;
		}
		while(!detail::equal_vectors(m_intersection, first_intersection));

		segments_t::const_iterator s = segments.begin();
		std::vector<unsigned long>::const_iterator t = throughs.begin();
		for(; s != segments.end(); ++s, ++t)
			m_iso_triangle_segments[*t].push_back(*s);

		segment_t* segment = new segment_t(previous_intersection, first_intersection);
		m_iso_triangle_segments[m_through->number].push_back(segment);

		if(previous_segment)
		{
			previous_segment->next = segment;
			segment->previous = previous_segment;
		}
		else
			first_segment = segment;
		previous_segment = segment;

		// Sanity checks ...
		return_val_if_fail(first_segment && previous_segment, false);

		// Save starting segment
		iso_flows.push_back(first_segment);

		// Isoparametric flows always form a loop
		previous_segment->next = first_segment;
		first_segment->previous = previous_segment;

		return true;
	}

	struct seed_point_t
	{
		seed_point_t(const double K, triangle_t T, k3d::point3 P, const double U) :
			key(K), triangle(T), point(P), u(U)
		{
		}

		double key;

		triangle_t triangle;
		k3d::point3 point;
		double u;

		friend bool operator < (const seed_point_t& lhs, const seed_point_t& rhs)
		{
			// Sort in reverse order
			return lhs.key > rhs.key;
		}
	};

	bool draw_isoparametric_flows(segments_t& iso_flows)
	{
		// Get isoparametric spacing
		const double h2 = m_h2.pipeline_value();
		m_isoparametric_distance = h2;

		// Find first seed point
		k3d::point3 new_point;
		triangle_t new_triangle;
		double new_u;
		return_val_if_fail(gradient_seed_point(m_starting_triangle, m_starting_point, m_starting_u, h2, new_triangle, new_point, new_u, true) && new_u != m_starting_u, false);

		// Create first isoparametric flow
		points_t first_flow;
		triangles_t first_indices;
		if(!isoparametric_flow_line(new_triangle, new_point, new_u, first_flow, first_indices, iso_flows))
			return true;

		// Save it
		std::vector<points_t> points;
		std::vector<triangles_t> triangles;
		std::vector<double> values;

		points.push_back(first_flow);
		triangles.push_back(first_indices);
		values.push_back(new_u);

		// Go up gradient flow
		unsigned long current_flow = 0;
		do
		{
			// Get seed points
			typedef std::list<seed_point_t> seed_points_t;
			seed_points_t seed_points;
			for(unsigned long n = 0; n < points[current_flow].size(); ++n)
			{
				k3d::point3& intersection = points[current_flow][n];
				triangle_t triangle = triangles[current_flow][n];
				const double u = values[current_flow];

				triangle_t new_triangle;
				k3d::point3 new_point;
				double new_u;
				if(new_u != u && gradient_seed_point(triangle, intersection, u, h2, new_triangle, new_point, new_u, true))
				{
					if(!near_extremum(new_point, m_isoparametric_distance))
						seed_points.push_back(seed_point_t(std::fabs(new_u - u), new_triangle, new_point, new_u));
				}
			}

			// Biggest keys first
			seed_points.sort();

			// Draw new flow
			for(seed_points_t::const_iterator seed = seed_points.begin(); seed != seed_points.end(); ++seed)
			{
				triangle_t triangle = seed->triangle;
				k3d::point3 point = seed->point;
				const double u = seed->u;

				m_current_triangle = triangle;
				m_intersection = point;
				m_new_u = u;
				if(!flow_distance_test(true, false))
					continue;

				points_t new_flow;
				triangles_t new_triangles;
				if(isoparametric_flow_line(triangle, point, u, new_flow, new_triangles, iso_flows))
				{
					points.push_back(new_flow);
					triangles.push_back(new_triangles);
					values.push_back(u);
				}
			}

			++current_flow;
		}
		while(current_flow < points.size());

		return true;
	}

	/// Find forward or backward point at given distance from seed_point (following constant u)
	bool isoparametric_seed_point(triangle_t start_triangle, const k3d::point3& start_point, const double u, const double distance, k3d::point3& seed_point, triangle_t& seed_triangle, const bool forward)
	{
		m_current_triangle = start_triangle;
		m_intersection = start_point;

		m_missed = false;
		m_iedge1 = 0;
		m_iedge2 = 0;

		k3d::point3 previous_point = m_intersection;

		m_direction = m_current_triangle->tangent_vector;
		if(!forward)
			m_direction = - m_direction;
		intersection_t intersection_type;
		m_current_triangle = iso_find_triangle_intersection(u, intersection_type);

		return_val_if_fail(intersection_type != NONE, false);

		double length = 0;
		do
		{
			// Avoid loops that may happen in convoluted mesh areas
			if(m_intersection == previous_point)
				break;

			const double current_length = k3d::distance(m_intersection, previous_point);
			if(length + current_length >= distance)
			{
				seed_triangle = m_through;
				seed_point = previous_point + (m_intersection - previous_point) * (distance - length) / current_length;

				return true;
			}

			length += current_length;

			previous_point = m_intersection;

			m_direction = m_current_triangle->tangent_vector;
			if(!forward)
				m_direction = - m_direction;

			if(intersection_type == EDGE)
				m_current_triangle = iso_find_triangle_intersection(u, intersection_type);
			else if(intersection_type == POINT)
				iso_vertex_case(u, m_iedge1, m_iedge2, forward, intersection_type);
		}
		while(intersection_type != NONE);

		return false;
	}

	bool near_extremum(const k3d::point3& point, const double distance)
	{
		// TODO : compute distance along surface for better accuracy
		for(extrema_t::const_iterator extremum = m_extrema.begin(); extremum != m_extrema.end(); ++extremum)
		{
			if(k3d::distance(point, extremum->first->position) < distance)
				return true;
		}

			return false;
	}

	double flow_length(const points_t& point_list, const bool loop = false)
	{
		if(!point_list.size())
			return 0;

		points_t::const_iterator point = point_list.begin();
		points_t::const_iterator end = point_list.end();

		k3d::point3 previous_point = *point;
		double length = 0;
		for(++point; point != end; ++point)
		{
			length += k3d::distance(*point, previous_point);
			previous_point = *point;
		}

		if(loop)
			length += k3d::distance(*point_list.begin(), previous_point);

		return length;
	}

	void connectivity_graph(const segments_t& gradient_flows, const segments_t& iso_flows, point_segments_map_t& point_segments_map, crossings_t& gradient_crossings, crossings_t& iso_crossings)
	{
		// Find intersections within each triangle
		for(unsigned long t = 0; t < m_gradient_triangle_segments.size(); ++t)
		{
			const unsigned long ng = m_gradient_triangle_segments[t].size();
			const unsigned long ni = m_iso_triangle_segments[t].size();
			for(unsigned long g = 0; g < ng; ++g)
				for(unsigned long i = 0; i < ni; ++i)
				{
					segment_t* segment1 = m_gradient_triangle_segments[t][g];
					const k3d::point3 g1 = segment1->p1;
					const k3d::point3 g2 = segment1->p2;
					segment_t* segment2 = m_iso_triangle_segments[t][i];
					const k3d::point3 i1 = segment2->p1;
					const k3d::point3 i2 = segment2->p2;

					double r;
					double s;
					if(!detail::segment_intersection(g1, g2, i1, i2, r, s))
						continue;

					if(!(r >= -1e-6 && r <= 1 + 1e-6 && s >= -1e-6 && s <= 1 + 1e-6))
						continue;

					// Create point
					const k3d::point3 p = g1 + (g2 - g1) * r;
					k3d::legacy::point* const p_p = new k3d::legacy::point(p);

					// Save it
					segment1->points.push_back(p_p);
					segment2->points.push_back(p_p);
					point_segments_map.insert(std::make_pair(p_p, flow_intersection_t(segment1, segment2)));
				}
		}

		// Sort intersection points within each segment (along segment)
		for(segment_lists_t::iterator segment_list = m_gradient_triangle_segments.begin(); segment_list != m_gradient_triangle_segments.end(); ++segment_list)
			for(segments_t::iterator segment = segment_list->begin(); segment != segment_list->end(); ++segment)
				(*segment)->sort_points();

		for(segment_lists_t::iterator segment_list = m_iso_triangle_segments.begin(); segment_list != m_iso_triangle_segments.end(); ++segment_list)
			for(segments_t::iterator segment = segment_list->begin(); segment != segment_list->end(); ++segment)
				(*segment)->sort_points();

		// Build sets of crossings (remove segments without any crossing
		// and crossings that don't have any previous or next crossing)
		for(unsigned long t = 0; t < gradient_flows.size(); ++t)
		{
			crossing_t* new_gradient_crossing = 0;
			crossing_t* crossing = 0;

			segment_t* segment = gradient_flows[t];

			// Create crossing list
			k3d::point3 previous_position;
			while(segment)
			{
				for(std::vector<k3d::legacy::point*>::iterator point = segment->points.begin(); point != segment->points.end(); ++point)
				{
					point_segments_map_t::iterator intersection = point_segments_map.find(*point);
					if(intersection == point_segments_map.end())
						continue;

					segment_t* new_segment = intersection->second.isoparametric_segment;
					k3d::legacy::point* p = *point;
					if(next_flow_point(new_segment, p) || previous_flow_point(new_segment, p))
					{
						if(crossing && p->position == previous_position)
							continue;

						crossing_t* new_crossing = new crossing_t(p);
						intersection->second.gradient_c = new_crossing;

						if(crossing)
						{
							crossing->next = new_crossing;
							new_crossing->previous = crossing;
							crossing = new_crossing;
						}
						else
						{
							new_gradient_crossing = crossing = new_crossing;
						}

						previous_position = p->position;
					}
				}

				segment = segment->next;
			}

			if(new_gradient_crossing)
			{
				if(new_gradient_crossing->next || new_gradient_crossing->previous)
					gradient_crossings.push_back(new_gradient_crossing);
				else
				{
					// Single crossing in current flow, remove it
					k3d::legacy::point* single_point = new_gradient_crossing->vertex;
					point_segments_map_t::iterator intersection = point_segments_map.find(single_point);
					flow_intersection_t& flow_intersection = intersection->second;

					flow_intersection.gradient_segment->remove_point(single_point);
					flow_intersection.isoparametric_segment->remove_point(single_point);
					delete single_point;

					point_segments_map.erase(intersection);
				}
			}
		}

		for(unsigned long t = 0; t < iso_flows.size(); ++t)
		{
			crossing_t* crossing = 0;

			segment_t* segment = iso_flows[t];
			segment_t* first_segment = segment;

			k3d::point3 previous_position;
			crossing_t* first_crossing = 0;
			while(segment)
			{
				for(std::vector<k3d::legacy::point*>::iterator point = segment->points.begin(); point != segment->points.end(); ++point)
				{
					point_segments_map_t::iterator intersection = point_segments_map.find(*point);
					if(intersection == point_segments_map.end())
						continue;

					// Skip orphaned gradient segments
					if(!intersection->second.gradient_segment && !intersection->second.isoparametric_segment)
						continue;

					segment_t* new_segment = intersection->second.gradient_segment;
					k3d::legacy::point* p = *point;
					if(next_flow_point(new_segment, p) || previous_flow_point(new_segment, p))
					{
						if(crossing && p->position == previous_position)
							continue;

						crossing_t* new_crossing = new crossing_t(p);
						intersection->second.isoparametric_c = new_crossing;

						if(crossing)
						{
							crossing->next = new_crossing;
							new_crossing->previous = crossing;
							crossing = new_crossing;
						}
						else
						{
							first_crossing = crossing = new_crossing;
							iso_crossings.push_back(crossing);
						}

						previous_position = p->position;
					}
				}

				segment = segment->next;
				if(segment == first_segment)
				{
					// Loop crossings too
					if(crossing)
					{
						crossing->next = first_crossing;
						first_crossing->previous = crossing;
					}

					break;
				}
			}
		}
	}

	void connect_polygons(k3d::legacy::mesh& output, k3d::legacy::polyhedron& polyhedron, crossings_t& gradient_crossings, crossings_t& iso_crossings, point_segments_map_t& point_segments)
	{
		k3d::imaterial* const material = m_material.pipeline_value();

		for(crossings_t::iterator crossing = gradient_crossings.begin(); crossing != gradient_crossings.end(); ++crossing)
		{
			crossing_t* start_crossing = *crossing;

			while(start_crossing)
			{
				crossing_t* c = start_crossing;
				start_crossing = start_crossing->next;
				if(!start_crossing)
					break;

				k3d::legacy::point* first_point = c->vertex;
				c->forward = true;

				bool gradient_edge = true;
				bool gradient_forward = true;
				bool isoparametric_forward = true;

				bool is_face = true;
				crossings_t vertices;
				do
				{
					if(is_face)
					{
						is_face = false;
					}
					else if(c->vertex == first_point)
					{
						is_face = true;
						break;
					}

					vertices.push_back(c);

					// Next point
					crossing_t* c2;
					if((gradient_edge && gradient_forward) || (!gradient_edge && isoparametric_forward))
					{
						c2 = c->next;
						c2->forward = true;
					}
					else
					{
						c2 = c->previous;
						c2->forward = false;
					}

					if(!c2)
						break;

					// Test whether we can turn right
					point_segments_map_t::iterator inters = point_segments.find(c2->vertex);
					assert_warning(inters != point_segments.end());

					crossing_t* c3;
					if(gradient_edge)
						c3 = inters->second.isoparametric_c;
					else
						c3 = inters->second.gradient_c;

					if(!c3)
					{
						// Can't turn at c3, go forward
						c = c2;
						continue;
					}

					// Virtually turn right
					bool new_gradient_forward = gradient_forward;
					bool new_isoparametric_forward = isoparametric_forward;
					if(gradient_edge)
						new_gradient_forward = !new_gradient_forward;
					else
						new_isoparametric_forward = !new_isoparametric_forward;

					bool new_gradient_edge = !gradient_edge;

					crossing_t* c4;
					if((new_gradient_edge && new_gradient_forward) || (!new_gradient_edge && new_isoparametric_forward))
					{
						c4 = c3->next;
						c3->forward = true;
					}
					else
					{
						c4 = c3->previous;
						c3->forward = false;
					}

					if(!c4)
					{
						// Can't turn at c3, save c2 and go on
						c = c2;
						continue;
					}

					c = c3;

					gradient_edge = new_gradient_edge;
					gradient_forward = new_gradient_forward;
					isoparametric_forward = new_isoparametric_forward;
				}
				while(c);

				if(!is_face)
					continue;

				std::vector<k3d::legacy::split_edge*> new_edges;
				for(crossings_t::const_iterator v = vertices.begin(); v != vertices.end(); ++v)
				{
					crossing_t* crossing = *v;

					k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(crossing->vertex);
					new_edges.push_back(new_edge);

					// Save split_edge
					if(!crossing->forward)
						crossing = crossing->previous;
assert_warning(crossing);
if(crossing)
					if(!crossing->up_edge)
					{
						crossing->up_edge = new_edge;
					}
					else
					{
						assert_warning(!crossing->down_edge);
//if(crossing->down_edge)
//	k3d::log() << " c : " << crossing->point->position << std::endl;
						crossing->down_edge = new_edge;
					}
				}

				// TODO : triangulate faces with more than 4 edges
				k3d::legacy::face* face = new k3d::legacy::face(new_edges.front(), material);
				k3d::legacy::loop_edges(new_edges.begin(), new_edges.end());
				polyhedron.faces.push_back(face);
			}
		}

		// Build triangle fans around extrema ...
		for(extrema_t::const_iterator extremum = m_extrema.begin(); extremum != m_extrema.end(); ++extremum)
		{
			k3d::point3 e = extremum->first->position;

			// Find nearest isoparametric crossing ring
			crossing_t* closest_crossing = 0;
			double distance = 0;
			for(crossings_t::const_iterator iso_crossing = iso_crossings.begin(); iso_crossing != iso_crossings.end(); ++iso_crossing)
			{
				crossing_t* crossing = *iso_crossing;

				// Skip crossings that already share two faces
				if(crossing->up_edge && crossing->down_edge)
					continue;

				const double crossing_distance = k3d::distance(crossing->vertex->position, e);
				if(!closest_crossing || crossing_distance < distance)
				{
					closest_crossing = crossing;
					distance = crossing_distance;
				}
			}

			if(!closest_crossing)
			{
				assert_not_reached();
				continue;
			}

			k3d::legacy::point* const extremum_point = new k3d::legacy::point(e);
			output.points.push_back(extremum_point);
			triangle_fan(closest_crossing, extremum_point, polyhedron, material);
		}

		// Set companions, delete crossings, add intersection points
		for(crossings_t::iterator crossing = gradient_crossings.begin(); crossing != gradient_crossings.end(); ++crossing)
		{
			crossing_t* c = *crossing;

			while(c)
			{
				// Add intersection point to the output mesh
				output.points.push_back(c->vertex);

				crossing_t* previous_c = c;

				c = c->next;

				// Join edges
				if(previous_c->up_edge && previous_c->down_edge)
					k3d::legacy::join_edges(*previous_c->up_edge, *previous_c->down_edge);

				delete previous_c;
			}
		}
		for(crossings_t::iterator crossing = iso_crossings.begin(); crossing != iso_crossings.end(); ++crossing)
		{
			crossing_t* c = *crossing;
			crossing_t* first_crossing = c;

			while(c)
			{
				crossing_t* previous_c = c;

				c = c->next;

				// Join edges
				assert_warning(previous_c->up_edge && previous_c->down_edge);
				if(previous_c->up_edge && previous_c->down_edge)
					k3d::legacy::join_edges(*previous_c->up_edge, *previous_c->down_edge);

				delete previous_c;

				if(c == first_crossing)
					break;
			}
		}
	}

	// Next point on segment
	k3d::legacy::point* next_flow_point(segment_t*& current_segment, const k3d::legacy::point* point)
	{
		if(point)
		{
			k3d::legacy::point* new_point = current_segment->next_point(point);
			if(new_point)
				return new_point;
		}

		segment_t* first = current_segment;
		while(current_segment->next)
		{
			current_segment = current_segment->next;
			k3d::legacy::point* new_point = current_segment->first_point();
			if(new_point)
				return new_point;

			if(current_segment == first)
				return 0;
		}

		return 0;
	}

	k3d::legacy::point* previous_flow_point(segment_t*& current_segment, const k3d::legacy::point* point)
	{
		if(point)
		{
			k3d::legacy::point* new_point = current_segment->previous_point(point);
			if(new_point)
				return new_point;
		}

		segment_t* first = current_segment;
		while(current_segment->previous)
		{
			current_segment = current_segment->previous;
			k3d::legacy::point* new_point = current_segment->last_point();
			if(new_point)
				return new_point;

			if(current_segment == first)
				return 0;
		}

		return 0;
	}

	void triangle_fan(crossing_t* first_crossing, k3d::legacy::point* extremum_point, k3d::legacy::polyhedron& polyhedron, k3d::imaterial* const Material)
	{
		return_if_fail(first_crossing && extremum_point);

		typedef std::map<k3d::legacy::point*, k3d::legacy::split_edge*> edge_map_t;
		edge_map_t companions;

		crossing_t* crossing = first_crossing;
		do
		{
			k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(extremum_point);
			k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(crossing->vertex);
			k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(crossing->next->vertex);

			// Reverse triangle ?
			return_if_fail(crossing->next);
			return_if_fail(crossing->up_edge || crossing->down_edge);

			if(crossing->up_edge)
			{
				if(crossing->next->vertex == crossing->up_edge->face_clockwise->vertex)
					std::swap(edge2, edge3);

				crossing->down_edge = edge2;
			}
			else
			{
				if(crossing->next->vertex == crossing->down_edge->face_clockwise->vertex)
					std::swap(edge2, edge3);

				crossing->up_edge = edge2;
			}

			// Save face
			k3d::legacy::face* face = new k3d::legacy::face(edge1, Material);
			polyhedron.faces.push_back(face);

			edge1->face_clockwise = edge2;
			edge2->face_clockwise = edge3;
			edge3->face_clockwise = edge1;

			// Set companions
			edge_map_t::iterator companion = companions.find(edge2->vertex);
			if(companion != companions.end())
				k3d::legacy::join_edges(*edge1, *companion->second);
			else
				companions.insert(std::make_pair(edge2->vertex, edge1));

			companion = companions.find(edge3->vertex);
			if(companion != companions.end())
				k3d::legacy::join_edges(*edge3, *companion->second);
			else
				companions.insert(std::make_pair(edge3->vertex, edge3));

			// Next
			crossing = crossing->next;
		}
		while(crossing && crossing != first_crossing);
	}


	// Check for new input
	bool m_input_changed;

	// Flow ends
	extrema_t m_extrema;

	// Input geometry
	std::map<edge_t, triangle_t> m_edge_triangle;
	triangles_t m_triangles;

	// Harmonic scalar field values
	std::map<k3d::legacy::point*, double> m_scalar_field;

	// Flow segments
	segment_lists_t m_gradient_triangle_segments;
	segment_lists_t m_iso_triangle_segments;

	// Temp variables
	triangle_t m_starting_triangle;
	k3d::point3 m_starting_point;
	double m_starting_u;

	k3d::point3 m_intersection;
	edge_t m_iedge1;
	edge_t m_iedge2;
	k3d::vector3 m_direction;
	double m_new_u;
	triangle_t m_current_triangle;
	bool m_missed;
	edge_t m_missed_index;
	triangle_t m_through;

	double m_gradient_distance;
	double m_isoparametric_distance;

	// User defined values
	k3d_data(k3d::mesh_selection, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, k3d::mesh_selection_serialization) m_minima_selection;
	k3d_data(k3d::mesh_selection, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, k3d::mesh_selection_serialization) m_maxima_selection;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_min_index;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_max_index;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_h1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_h2;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_minima_value;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_maxima_value;
	//k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_curvature_sensibility;
	//k3d_data_property(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint) m_remove_t_junctions;
};

/////////////////////////////////////////////////////////////////////////////
// quadrilateral_remeshing_factory

k3d::iplugin_factory& quadrilateral_remeshing_factory()
{
	return quadrilateral_remeshing::get_factory();
}

} // namespace quadrilateral_remeshing

} // namespace module
