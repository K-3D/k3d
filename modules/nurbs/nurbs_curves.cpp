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

#include <k3dsdk/metadata.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/table_copier.h>

#include "nurbs_curves.h"

#include <boost/scoped_ptr.hpp>
#include <sstream>

namespace k3d { namespace nurbs_curve { class primitive; } }

namespace module
{

namespace nurbs
{

namespace detail
{

using namespace k3d; // for easy moving to the SDK

struct inf_norm
{
	inf_norm(const k3d::double_t Threshold = 0.000001) : threshold(Threshold) {}

	const bool_t operator()(const point3& p1, const point3& p2)
	{
		if (fabs(p1[0] - p2[0]) < threshold
				&& fabs(p1[1] - p2[1]) < threshold
				&& fabs(p1[2] - p2[2]) < threshold)
			return true;
		return false;
	}

	const k3d::double_t threshold;
};

template<typename ComparatorT>
void lookup_duplicate_points(const mesh::points_t& Points, mesh::indices_t& PointMap, ComparatorT Equal)
{
	const uint_t point_count = Points.size();
	mesh::bools_t duplicate_points(point_count, false);
	PointMap.resize(point_count);
	for(uint_t i = 0; i != point_count; ++i)
	{
		if(duplicate_points[i])
			continue;
		for(uint_t j = i+1; j != point_count; ++j)
		{
			if(Equal(Points[i], Points[j]))
			{
				duplicate_points[j] = true;
				PointMap[j] = i;
			}
		}
		PointMap[i] = i;
	}
}

void remap_points(mesh::indices_t& PrimitivePoints, const mesh::indices_t& PointMap)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		PrimitivePoints[i] = PointMap[PrimitivePoints[i]];
}

// Taken from mesh.cpp, will be consolidated when the duplicate_points methods are moved there
struct remap_primitive_points
{
	remap_primitive_points(mesh::indices_t& PointMap) :
		point_map(PointMap)
	{
	}

	void operator()(const string_t&, const table&, const string_t&, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(metadata::key::domain()) != metadata::value::point_indices_domain())
			return;

		if(mesh::indices_t* const array = dynamic_cast<mesh::indices_t*>(&Array.writable()))
			remap_points(*array, point_map);
	}

	mesh::indices_t& point_map;
};

/// Replace duplicate points (up to a threshold) with the first point of each series of duplicates in the index arrays
// TODO: Move this to mesh
template<typename ComparatorT>
void replace_duplicate_points(mesh& Mesh, ComparatorT Equal)
{
	// Create a mapping between duplicate points and their replacement
	mesh::indices_t point_map;
	lookup_duplicate_points(*Mesh.points, point_map, Equal);

	// Update generic mesh primitives so they use the correct indices ...
	mesh::visit_arrays(Mesh, remap_primitive_points(point_map));
}

} // namespace detail

void replace_duplicate_points(k3d::mesh& Mesh)
{
	detail::replace_duplicate_points(Mesh, detail::inf_norm());
}

void add_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve)
{
	k3d::table_copier point_attribute_copier(InputMesh.point_attributes, OutputMesh.point_attributes);
	k3d::table_copier curve_attribute_copier(InputCurves.curve_attributes, OutputCurves.curve_attributes);
	k3d::table_copier varying_attribute_copier(InputCurves.varying_attributes, OutputCurves.varying_attributes);
	k3d::table_copier vertex_attribute_copier(InputCurves.vertex_attributes, OutputCurves.vertex_attributes);
	const k3d::uint_t curve_point_start = InputCurves.curve_first_points[Curve];
	const k3d::uint_t curve_point_end = curve_point_start + InputCurves.curve_point_counts[Curve];
	OutputCurves.curve_first_points.push_back(OutputCurves.curve_points.size());
	OutputCurves.curve_point_counts.push_back(InputCurves.curve_point_counts[Curve]);
	OutputCurves.curve_orders.push_back(InputCurves.curve_orders[Curve]);
	curve_attribute_copier.push_back(Curve);
	k3d::mesh::points_t& output_points = OutputMesh.points.writable();
	k3d::mesh::selection_t& output_point_selection = OutputMesh.point_selection.writable();
	for(k3d::uint_t curve_point = curve_point_start; curve_point != curve_point_end; ++curve_point)
	{
		varying_attribute_copier.push_back(OutputCurves.curve_points.size());
		vertex_attribute_copier.push_back(OutputCurves.curve_points.size());
		OutputCurves.curve_points.push_back(output_points.size());
		OutputCurves.curve_point_weights.push_back(InputCurves.curve_point_weights[curve_point]);
		const k3d::uint_t old_point = InputCurves.curve_points[curve_point];
		output_points.push_back(InputMesh.points->at(old_point));
		output_point_selection.push_back(InputMesh.point_selection->at(old_point));
		point_attribute_copier.push_back(old_point);
	}
	OutputCurves.curve_first_knots.push_back(OutputCurves.curve_knots.size());
	OutputCurves.curve_selections.push_back(InputCurves.curve_selections[Curve]);
	const k3d::uint_t curve_knot_start = InputCurves.curve_first_knots[Curve];
	const k3d::uint_t curve_knot_end = curve_knot_start + InputCurves.curve_point_counts[Curve] + InputCurves.curve_orders[Curve];
	for(k3d::uint_t knot = curve_knot_start; knot != curve_knot_end; ++knot)
	{
		OutputCurves.curve_knots.push_back(InputCurves.curve_knots[knot]);
	}
	OutputCurves.constant_attributes = InputCurves.constant_attributes;
	OutputCurves.material = InputCurves.material;
}

void update_indices(k3d::mesh::indices_t& Indices, const k3d::uint_t Start, const k3d::int32_t Offset)
{
	for(k3d::uint_t i = 0; i != Indices.size(); ++i)
	{
		if(Indices[i] >= Start)
			Indices[i] += Offset;
	}
}

// Remove the item at Index
template<typename ArrayT>
void erase(ArrayT& Array, const k3d::uint_t Index)
{
	return_if_fail(Index < Array.size());
	Array.erase(Array.begin() + Index);
}

void delete_curve(k3d::nurbs_curve::primitive& Curves, const k3d::uint_t Curve)
{
	const k3d::uint_t curve_count = Curves.curve_first_points.size();

	k3d::table curve_attributes = Curves.curve_attributes.clone_types();
	k3d::table_copier uniform_copier(Curves.curve_attributes, curve_attributes);
	for(k3d::uint_t i = 0; i != Curve; ++i)
		uniform_copier.push_back(i);
	for(k3d::uint_t i = Curve + 1; i != curve_count; ++i)
		uniform_copier.push_back(i);
	k3d::table varying_attributes = Curves.varying_attributes.clone_types();
	k3d::table_copier varying_copier(Curves.varying_attributes, varying_attributes);
	k3d::uint_t curve_start_index = Curves.curve_first_points[Curve];
	k3d::uint_t curve_end_index = curve_start_index + Curves.curve_point_counts[Curve];
	for(k3d::uint_t i = 0; i != curve_start_index; ++i)
		varying_copier.push_back(i);
	for(k3d::uint_t i = curve_end_index; i != Curves.curve_points.size(); ++i)
		varying_copier.push_back(i);

	return_if_fail(Curve < curve_count);
	// Erase point indices and weights
	k3d::mesh::indices_t::iterator points_begin = Curves.curve_points.begin() + Curves.curve_first_points[Curve];
	k3d::mesh::indices_t::iterator points_end = points_begin + Curves.curve_point_counts[Curve];
	Curves.curve_points.erase(points_begin, points_end);
	update_indices(Curves.curve_first_points, Curves.curve_first_points[Curve] + Curves.curve_point_counts[Curve], -Curves.curve_point_counts[Curve]);
	k3d::mesh::weights_t::iterator weights_begin = Curves.curve_point_weights.begin() + Curves.curve_first_points[Curve];
	k3d::mesh::weights_t::iterator weights_end = weights_begin + Curves.curve_point_counts[Curve];
	Curves.curve_point_weights.erase(weights_begin, weights_end);
	// Erase knots
	k3d::mesh::knots_t::iterator knots_begin = Curves.curve_knots.begin() + Curves.curve_first_knots[Curve];
	k3d::mesh::knots_t::iterator knots_end = knots_begin + Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve];
	Curves.curve_knots.erase(knots_begin, knots_end);
	update_indices(Curves.curve_first_knots, Curves.curve_first_knots[Curve] + Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve], -(Curves.curve_point_counts[Curve] + Curves.curve_orders[Curve]));

	erase(Curves.curve_first_knots, Curve);
	erase(Curves.curve_first_points, Curve);
	erase(Curves.curve_orders, Curve);
	erase(Curves.curve_point_counts, Curve);
	erase(Curves.curve_selections, Curve);
}

struct knot_normalizer
{
	knot_normalizer(const k3d::double_t Min, const k3d::double_t Max) : min(Min), max(Max) {}
	k3d::double_t min, max;
	const k3d::double_t operator()(const k3d::double_t Knot)
	{
		return (Knot - min) / (max - min);
	}
};

void normalize_knot_vector(k3d::nurbs_curve::primitive& NurbsCurve, const k3d::uint_t Curve)
{
	k3d::mesh::knots_t::iterator curve_knots_begin = NurbsCurve.curve_knots.begin() + NurbsCurve.curve_first_knots[Curve];
	k3d::mesh::knots_t::iterator curve_knots_end = curve_knots_begin + NurbsCurve.curve_point_counts[Curve] + NurbsCurve.curve_orders[Curve];

	k3d::double_t min = *curve_knots_begin;
	k3d::double_t max = *(curve_knots_end - 1);

	std::transform(curve_knots_begin, curve_knots_end, curve_knots_begin, knot_normalizer(min, max));
}

void replace_point(k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t newIndex, k3d::uint_t curve, k3d::uint_t point, bool continuous)
{
	const k3d::uint_t curve_point_begin = NurbsCurve.curve_first_points[curve];
	const k3d::uint_t curve_point_end = curve_point_begin + NurbsCurve.curve_point_counts[curve];

	for (k3d::uint_t points = curve_point_begin; points < curve_point_end; ++points)
	{
		if (NurbsCurve.curve_points[points] == point)
		{
			//we found the index pointing to point1
			NurbsCurve.curve_points[points] = newIndex;
			if (continuous)
			{

				const k3d::uint_t curve_knots_begin = NurbsCurve.curve_first_knots[curve];
				const k3d::uint_t curve_knots_end = curve_knots_begin + curve_point_end - curve_point_begin + NurbsCurve.curve_orders[curve];

				const k3d::uint_t order = NurbsCurve.curve_orders[curve];
				const k3d::uint_t half_order = static_cast<k3d::uint_t>(floor(0.5 * order));
				const k3d::uint_t pos = half_order + (points - curve_point_begin) + curve_knots_begin;
				float knot_at_pos = NurbsCurve.curve_knots[points - curve_point_begin + curve_knots_begin + half_order];

				if (pos - curve_knots_begin < order)
				{
					for (k3d::uint_t x = curve_knots_begin; x < order + curve_knots_begin; ++x)
						NurbsCurve.curve_knots[x] = knot_at_pos;
					k3d::uint_t knot_pos = order + curve_knots_begin;
					while ((NurbsCurve.curve_knots[knot_pos + 1] - NurbsCurve.curve_knots[knot_pos] > 2) && (knot_pos < curve_knots_end - 1))
					{
						NurbsCurve.curve_knots[knot_pos + 1] = NurbsCurve.curve_knots[knot_pos] + 2;
						knot_pos++;
					}
				}
				else if (pos - curve_knots_begin + order < curve_knots_end)
				{
					for (k3d::uint_t x = curve_knots_end - 1; x > curve_knots_end - order; --x)
						NurbsCurve.curve_knots[x] = knot_at_pos;
					k3d::uint_t knot_pos = curve_knots_end - order;
					while ((NurbsCurve.curve_knots[knot_pos] - NurbsCurve.curve_knots[knot_pos - 1] > 2) && (knot_pos < curve_knots_begin))
					{
						NurbsCurve.curve_knots[knot_pos - 1] = NurbsCurve.curve_knots[knot_pos] - 2;
						knot_pos--;
					}
				}
				else
					k3d::log() << debug << "nurbs_curves: replace_point: Should split up the curve here" << std::endl;
			}
		}
	}
}

void close_curve(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::bool_t keep_ends)
{
	/*
			- get both end points
			if(keep_ends)
					. add the middle of the points as new one and use the old ends as CV
			else . replace them by their arithmetical middle
	*/

	add_curve(OutputMesh, OutputCurves, InputMesh, InputCurves, Curve);

	const k3d::uint_t curve = OutputCurves.curve_first_points.size() - 1;
	OutputCurves.curve_selections[curve] = 1.0;

	const k3d::uint_t curve_points_begin = OutputCurves.curve_first_points[curve];
	const k3d::uint_t curve_points_end = curve_points_begin + OutputCurves.curve_point_counts[curve];

	const k3d::uint_t curve_knots_begin = OutputCurves.curve_first_knots[curve];
	const k3d::uint_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + OutputCurves.curve_orders[curve];

	k3d::point3 new_point = (OutputMesh.points->at(OutputCurves.curve_points[curve_points_begin]) + OutputMesh.points->at(OutputCurves.curve_points[curve_points_end - 1])) * 0.5;
	OutputMesh.points.writable().push_back(new_point);
	OutputMesh.point_selection.writable().push_back(0.0);

	k3d::uint_t new_index = OutputMesh.points->size() - 1;

	if (!keep_ends)
	{
		replace_point(OutputCurves, new_index, curve, OutputCurves.curve_points[curve_points_begin], true);
		replace_point(OutputCurves, new_index, curve, OutputCurves.curve_points[curve_points_end - 1], true);
	}
	else
	{
		normalize_knot_vector(OutputCurves, curve);
		k3d::uint_t order = OutputCurves.curve_orders[curve];
		//take the first knot different from 0 and the last one different from 1.0, double the
		k3d::uint_t first = curve_knots_begin;

		while (OutputCurves.curve_knots[first] == 0.0 && first < curve_knots_end)
		{
			first++;
		}

		k3d::uint_t last = curve_knots_end - 1;

		while (OutputCurves.curve_knots[last] == 1.0 && last >= curve_knots_begin)
		{
			last--;
		}
		//insert knot at the end
		double diff = 1.0 - OutputCurves.curve_knots[last];

		for (k3d::uint_t i = last + 1; i < curve_knots_end; i++)
			OutputCurves.curve_knots[i] += diff;

		//insert knot at the beginning
		diff = OutputCurves.curve_knots[first];

		for (int i = first - 1; i >= static_cast<int>(curve_knots_begin); i--)
		{
			k3d::log() << i << std::endl;
			OutputCurves.curve_knots[i] -= diff;
		}

		k3d::mesh::knots_t::iterator last_iter = OutputCurves.curve_knots.begin() + last + 1;
		OutputCurves.curve_knots.insert(last_iter, 1.0);
		k3d::mesh::knots_t::iterator first_iter = OutputCurves.curve_knots.begin() + first;
		OutputCurves.curve_knots.insert(first_iter, 0.0);

		//insert point indices to the new point
		k3d::mesh::indices_t::iterator point_iter = OutputCurves.curve_points.begin() + curve_points_begin;
		OutputCurves.curve_points.insert(point_iter, new_index);

		point_iter = OutputCurves.curve_points.begin() + curve_points_end + 1; //because we've already inserted one point
		OutputCurves.curve_points.insert(point_iter, new_index);

		k3d::mesh::weights_t::iterator weights_iter = OutputCurves.curve_point_weights.begin() + curve_points_begin;
		OutputCurves.curve_point_weights.insert(weights_iter, 1.0);
		weights_iter = OutputCurves.curve_point_weights.begin() + curve_points_end;
		OutputCurves.curve_point_weights.insert(weights_iter, 1.0);

		OutputCurves.curve_point_counts[curve] += 2;

		normalize_knot_vector(OutputCurves, curve);
	}
}

const k3d::uint_t factorial(const k3d::uint_t n)
{
	if (n == 0) return 1;
	if (n < 3) return n;
	return factorial(n -1)*n;
}

const k3d::double_t binomial_coefficient(const k3d::uint_t n, const k3d::uint_t k)
{
	return_val_if_fail(n >= k, 0.0);
	k3d::double_t result = factorial(n);
	result /= factorial(k) * factorial(n - k);
	return result;
}

void fill_bezalfs(std::vector<std::vector<double> >& bezalfs, int power, int t)
{
	int ph = power + t;
	int ph2 = ph / 2;

	bezalfs[0][0] = 1.0;
	bezalfs[ph][power] = 1.0;

	for (int i = 1; i <= ph2; i++)
	{
		double inv = 1.0 / binomial_coefficient(ph, i);
		int mpi = std::min(power, i);
		for (int j = std::max(0, i - t); j <= mpi; j++)
		{
			bezalfs[i][j] = inv * binomial_coefficient(power, j) * binomial_coefficient(t, i - j);
		}
	}

	for (int i = ph2 + 1; i <= ph - 1; i++)
	{
		int mpi = std::min(power, i);
		for (int j = std::max(0, i - t); j <= mpi; j++)
		{
			bezalfs[i][j] = bezalfs[ph - i][power - j];
		}
	}
}

/// Helper function to make a homogeneous point out of a point3
const k3d::point4 homogeneous(const k3d::point3& p, const k3d::double_t w)
{
	return k3d::point4(k3d::point4(p[0]*w, p[1]*w, p[2]*w, w));
}

void elevate_curve_degree(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::uint_t Elevations)
{
	k3d::mesh temp_mesh;
	temp_mesh.points.create();
	temp_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> temp_curves(k3d::nurbs_curve::create(temp_mesh));
	add_curve(temp_mesh, *temp_curves, InputMesh, InputCurves, Curve);
	for(k3d::uint_t level = 0; level != Elevations; ++level)
	{
		const k3d::uint_t curve = temp_curves->curve_first_points.size() - 1;
		const int t = 1;
		const k3d::uint_t order = temp_curves->curve_orders[curve];
		int power = order - 1;
		const k3d::uint_t nr_points = temp_curves->curve_point_counts[curve];

		const k3d::uint_t curve_points_begin = temp_curves->curve_first_points[curve];
		const k3d::uint_t curve_points_end = curve_points_begin + temp_curves->curve_point_counts[curve];

		const k3d::uint_t curve_knots_begin = temp_curves->curve_first_knots[curve];
		const k3d::uint_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + temp_curves->curve_orders[curve];

		std::vector<std::vector<double> > bezalfs(power + t + 1, std::vector<double>(power + 1, 1.0));
		std::vector<k3d::point4> bpts(power + 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
		std::vector<k3d::point4> ebpts(power + t + 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
		std::vector<k3d::point4> next_bpts(power - 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
		std::vector<double> alphas(power - 1, 0.0);

		k3d::mesh::knots_t new_knots(2*(nr_points + order + t), 0.0);
		std::vector<k3d::point4> new_points(2*(nr_points + t), k3d::point4(0.0, 0.0, 0.0, 0.0));

		fill_bezalfs(bezalfs, power, t);

		int m = nr_points + power;
		int mh = power + t;
		int kind = mh + 1;
		int r = -1;
		int a = power;
		int b = order;
		int cind = 1;
		double ua = temp_curves->curve_knots[curve_knots_begin];

		const k3d::mesh::points_t& input_points = *temp_mesh.points;

		new_points.at(0) = homogeneous(input_points[temp_curves->curve_points[curve_points_begin]], temp_curves->curve_point_weights[curve_points_begin]);

		for (int i = 0; i <= power + t; i++)
			new_knots.push_back(ua);

		//initialize first bezier segment
		for (int i = 0; i <= power; i++)
		{
			bpts.at(i) = homogeneous(input_points[temp_curves->curve_points[curve_points_begin + i]], temp_curves->curve_point_weights[curve_points_begin + i]);
		}

		while (b < m)//big loop through knot vector
		{
			int i = b;
			while (b < m && fabs(temp_curves->curve_knots[curve_knots_begin + b] - temp_curves->curve_knots[curve_knots_begin + b + 1]) < 0.00001)
				b++;
			int mul = b - i + 1;
			mh = mh + mul + t;
			double ub = temp_curves->curve_knots[curve_knots_begin + b];
			int oldr = r;
			r = power - mul;
			//insert knot ub r times
			int lbz, rbz;
			if (oldr > 0)
				lbz = (oldr + 2) / 2;
			else
				lbz = 1;

			if (r > 0)
				rbz = power + t - ((r + 1) / 2);
			else
				rbz = power + t;

			if (r > 0)
			{
				//insert knot to get bezier segment
				double numer = ub - ua;
				for (int k = power; k > mul; k--)
				{
					alphas.at(k - mul - 1) = numer / (temp_curves->curve_knots[curve_knots_begin + a + k] - ua);
				}
				for (int j = 1; j <= r; j++)
				{
					int save = r - j;
					int s = mul + j;
					for (int k = power; k >= s; k--)
					{
						bpts.at(k) = alphas.at(k - s) * bpts.at(k) + (1.0 - alphas.at(k - s)) * bpts.at(k - 1);
					}
					next_bpts.at(save) = bpts.at(power);
				}
			}//end of "insert knot"
			//degree elevate bezier
			for (int i = lbz; i <= power + t; i++)
			{
				//only points lbz, ... , power+t are used here
				ebpts.at(i) = k3d::point4(0.0, 0.0, 0.0, 0.0);
				int mpi = std::min(power, i);
				for (int j = std::max(0, i - t); j <= mpi; j++)
				{
					ebpts.at(i) += k3d::to_vector(bezalfs.at(i).at(j) * bpts.at(j));
				}
			}

			if (oldr > 1)
			{
				//must remove knot ua oldr times
				int first = kind - 2;
				int last = kind;
				double den = ub - ua;
				double bet = (ub - new_knots.at(kind - 1)) / den;
				//knot removal loop
				for (int tr = 1; tr < oldr; tr++)
				{
					int i = first;
					int j = last;
					int kj = j - kind + 1;
					while (j - i > tr)//loop and compute the new control points for one removal step
					{
						if (i < cind)
						{
							double alf = (ub - new_knots.at(i)) / (ua - new_knots.at(i));
							new_points.at(i) = alf * new_points.at(i) + (1.0 - alf) * new_points.at(i - 1);
						}
						if (j >= lbz)
						{
							if (j - tr <= kind - power - t + oldr)
							{
								double gam = (ub - new_knots.at(j - tr)) / den;
								ebpts.at(kj) = gam * ebpts.at(kj) + (1.0 - gam) * ebpts.at(kj + 1);
							}
							else
								ebpts.at(kj) = bet * ebpts.at(kj) + (1.0 - bet) * ebpts.at(kj + 1);
						}
						i++;
						j--;
						kj--;
					}
					first--;
					last++;
				}
			}//end of removing knot ua
			if (a != power)
			{
				for (int i = 0; i < power + t - oldr; i++)
				{
					new_knots.at(kind) = ua;
					kind++;
				}
			}
			for (int j = lbz; j <= rbz; j++)
			{
				new_points.at(cind) = ebpts.at(j);
				cind++;

			}

			if (b < m)
			{
				for (int j = 0; j < r; j++)
					bpts.at(j) = next_bpts.at(j);
				for (int j = r; j <= power; j++)
				{

					bpts[j] = homogeneous(input_points[temp_curves->curve_points[curve_points_begin + b - power + j]], temp_curves->curve_point_weights[curve_points_begin + b - power + j]);
				}
				a = b;
				b++;
				ua = ub;
			}
			else//end knot
			{
				for (int i = 0; i <= power + t; i++)
				{
					new_knots.at(kind + i) = ub;
				}
			}
		}//end while loop (b < m)

		const k3d::uint_t new_n = mh - power - t;
		new_points[new_n - 1] = homogeneous(input_points[temp_curves->curve_points[curve_points_end - 1]], temp_curves->curve_point_weights[curve_points_end - 1]);
		new_points.resize(new_n);
		new_knots.resize(new_n + order + t);
		k3d::mesh::weights_t elevated_curve_weigts;
		k3d::mesh::points_t elevated_points;
		for(k3d::uint_t i = 0; i != new_n; ++i)
		{
			const k3d::point4& p = new_points[i];
			elevated_curve_weigts.push_back(p[3]);
			elevated_points.push_back(k3d::point3(p[0]/p[3], p[1]/p[3], p[2]/p[3]));
		}
		k3d::nurbs_curve::add_curve(temp_mesh, *temp_curves, order+1, elevated_points, elevated_curve_weigts, new_knots);
	}
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_temp_curves(k3d::nurbs_curve::validate(temp_mesh, *temp_mesh.primitives.back()));
	add_curve(OutputMesh, OutputCurves, temp_mesh, *const_temp_curves, Elevations);
	OutputCurves.curve_selections.back() = 1.0;
}



void flip_curve(k3d::nurbs_curve::primitive& NurbsCurves, const k3d::uint_t curve)
{
	const k3d::uint_t curve_point_begin = NurbsCurves.curve_first_points[curve];
	const k3d::uint_t curve_point_end = curve_point_begin + NurbsCurves.curve_point_counts[curve];

	k3d::uint_t curve_knots_begin = NurbsCurves.curve_first_knots[curve];
	k3d::uint_t curve_knots_end = curve_knots_begin + NurbsCurves.curve_point_counts[curve] + NurbsCurves.curve_orders[curve];
	k3d::mesh::knots_t::iterator curve_knots_begin_it = NurbsCurves.curve_knots.begin() + curve_knots_begin;
	k3d::mesh::knots_t::iterator curve_knots_end_it = NurbsCurves.curve_knots.begin() + curve_knots_end;
	const k3d::double_t max_knot = NurbsCurves.curve_knots[curve_knots_end-1];
	for(k3d::uint_t i = curve_knots_begin; i != curve_knots_end; ++i) NurbsCurves.curve_knots[i] = max_knot - NurbsCurves.curve_knots[i];
	std::reverse(curve_knots_begin_it, curve_knots_end_it);

	//flip point indices
	k3d::mesh::indices_t::iterator curve_points_begin_iter = NurbsCurves.curve_points.begin() + curve_point_begin;
	k3d::mesh::indices_t::iterator curve_points_end_iter = NurbsCurves.curve_points.begin() + curve_point_end;
	std::reverse(curve_points_begin_iter, curve_points_end_iter);

	//flip weights
	k3d::mesh::weights_t::iterator point_weights_begin_iter = NurbsCurves.curve_point_weights.begin() + curve_point_begin;
	k3d::mesh::weights_t::iterator point_weights_end_iter = NurbsCurves.curve_point_weights.begin() + curve_point_end;
	std::reverse(point_weights_begin_iter, point_weights_end_iter);
}

void connect_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::uint_t Primitive1Index, const k3d::uint_t Curve1Index, const k3d::bool_t Curve1FirstPointSelection, const k3d::uint_t Primitive2Index, const k3d::uint_t Curve2Index, const k3d::bool_t Curve2FirstPointSelection)
{
	k3d::mesh temp_mesh;
	temp_mesh.points.create();
	temp_mesh.point_selection.create();

	boost::scoped_ptr<k3d::nurbs_curve::primitive> temp_curves(k3d::nurbs_curve::create(temp_mesh));
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves_prim1(k3d::nurbs_curve::validate(InputMesh, *InputMesh.primitives[Primitive1Index]));
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves_prim2(k3d::nurbs_curve::validate(InputMesh, *InputMesh.primitives[Primitive2Index]));

	const k3d::uint_t order1 = curves_prim1->curve_orders[Curve1Index];
	const k3d::uint_t order2 = curves_prim2->curve_orders[Curve2Index];
	if(order1 < order2)
	{
		elevate_curve_degree(temp_mesh, *temp_curves, InputMesh, *curves_prim1, Curve1Index, order2 - order1);
		add_curve(temp_mesh, *temp_curves, InputMesh, *curves_prim2, Curve2Index);
	}
	else
	{
		add_curve(temp_mesh, *temp_curves, InputMesh, *curves_prim1, Curve1Index);
		elevate_curve_degree(temp_mesh, *temp_curves, InputMesh, *curves_prim2, Curve2Index, order1 - order2);
	}

	const k3d::uint_t order = temp_curves->curve_orders[0]; // both orders are equal now

	if(Curve1FirstPointSelection)
		flip_curve(*temp_curves, 0);
	if(!Curve2FirstPointSelection)
		flip_curve(*temp_curves, 1);

	normalize_knot_vector(*temp_curves, 0);
	normalize_knot_vector(*temp_curves, 1);

	k3d::mesh::points_t points;
	k3d::mesh::weights_t weights;
	for(k3d::uint_t curve_point = 0; curve_point != temp_curves->curve_points.size(); ++curve_point)
	{
		points.push_back(temp_mesh.points->at(temp_curves->curve_points[curve_point]));
		weights.push_back(temp_curves->curve_point_weights[curve_point]);
	}

	k3d::mesh::knots_t knots;
	for(k3d::uint_t i = 0; i != temp_curves->curve_point_counts[0]; ++i)
	{
		knots.push_back(temp_curves->curve_knots[i]);
	}
	const k3d::uint_t knots2_begin = temp_curves->curve_first_knots[1] + order;
	const k3d::double_t step = (1 + temp_curves->curve_knots[knots2_begin] - knots.back()) / (order + 1);
	for(k3d::uint_t i = 0; i != order; ++i)
		knots.push_back(knots.back() + step);
	for(k3d::uint_t i = 0; i != temp_curves->curve_point_counts[1]; ++i)
		knots.push_back(1 + temp_curves->curve_knots[knots2_begin + i]);

	k3d::nurbs_curve::add_curve(OutputMesh, OutputCurves, order, points, weights, knots);
}

/// Predicate to find the first knot greater than the value given in the constructor
struct find_first_knot_after
{
	find_first_knot_after(const k3d::double_t KnotValue) : knot_value(KnotValue) {}
	k3d::bool_t operator()(const k3d::double_t TestKnot)
	{
		return TestKnot > knot_value;
	}
	const k3d::double_t knot_value;
};

/// Predicate to find knot multiplicity
struct find_knot_multiplicity
{
	find_knot_multiplicity(const k3d::double_t KnotValue) : knot_value(KnotValue) {}
	k3d::bool_t operator()(const k3d::double_t TestKnot)
	{
		return std::abs(TestKnot - knot_value) < 0.000001;
	}
	const k3d::double_t knot_value;
};

const k3d::uint_t multiplicity(const k3d::mesh::knots_t& Knots, const k3d::double_t u, const k3d::uint_t Begin, const k3d::uint_t Count)
{
	k3d::mesh::knots_t::const_iterator begin = Knots.begin() + Begin;
	k3d::mesh::knots_t::const_iterator end = begin + Count;
	return std::count_if(begin, end, find_knot_multiplicity(u));
}

/// Helper function to enable consecutive knot insertions
/**
    Implementation of Algorithm "CurveKnotInsertion" from
    "The NURBS book", page 151, by Piegl and Tiller
**/
void insert_knot(k3d::mesh::points_t& Points, k3d::mesh::knots_t& Knots, k3d::mesh::weights_t& Weights, const k3d::double_t u, const k3d::uint_t r, const k3d::uint_t Order)
{
	const k3d::uint_t nr_points = Points.size();
	k3d::mesh::knots_t normalized_knots(Knots.size());
	std::transform(Knots.begin(), Knots.end(), normalized_knots.begin(), knot_normalizer(Knots.front(), Knots.back()));

	k3d::mesh::knots_t::iterator knot_u = std::find_if(normalized_knots.begin(), normalized_knots.end(), find_first_knot_after(u));
	const k3d::uint_t s = std::count_if(normalized_knots.begin(), normalized_knots.end(), find_knot_multiplicity(u));

	//we found the first knot greater than u or we're at the end so thats our k now
	int k = knot_u - normalized_knots.begin() - 1;

	if (s + r > Order - 1)
	{
		k3d::log() << debug << "insert_knot: No knot inserted: target multiplicity " << s + r << " for knot " << u << " exceeds curve degree " << Order-1 << std::endl;
		return;
	}

	//*******************************************************************
	//Algorithm from "The NURBS book" page 151
	//*******************************************************************

	k3d::uint_t mp = normalized_knots.size();
	k3d::uint_t nq = nr_points + r;

	Knots.clear();
	Knots.insert(Knots.end(), normalized_knots.begin(), knot_u);
	for (k3d::uint_t i = 1; i <= r; i++)
	{
		Knots.push_back(u);
	}
	Knots.insert(Knots.end(), knot_u, normalized_knots.end());

	std::vector<k3d::point4> points(nq);
	for (k3d::uint_t i = 0; i <= k + 1 - Order; i++)
	{
		const k3d::point3& p = Points[i];
		points[i] = k3d::point4(p[0], p[1], p[2], Weights[i]);
	}

	for (k3d::uint_t i = k - s; i < nr_points; i++)
	{
		const k3d::point3& p = Points[i];
		points[i + r] = k3d::point4(p[0], p[1], p[2], Weights[i]);
	}

	std::vector<k3d::point4> tmp;
	for (k3d::uint_t i = 0; i <= Order - 1 - s; i++)
	{
		const k3d::point3& p = Points[i + k - (Order - 1)];
		const k3d::double_t w = Weights[i + k - (Order - 1)];
		tmp.push_back(k3d::point4(w*p[0], w*p[1], w*p[2], w));
	}
	k3d::uint_t L = 0;

	for (k3d::uint_t j = 1; j <= r; j++)
	{
		L = k - (Order - 1) + j;
		double alpha = 0.0;
		for (k3d::uint_t i = 0; i <= Order - 1 - j - s; i++)
		{
			alpha = (u - normalized_knots[L + i]) / (normalized_knots[i + k + 1] - normalized_knots[L + i]);
			tmp[i] = alpha * tmp.at(i + 1) + (1.0 - alpha) * tmp.at(i);
		}
		const k3d::point4& t1 = tmp[0];
		double w = t1[3];
		points[L] = k3d::point4(t1[0] / w, t1[1] / w, t1[2] / w, w);
		const k3d::point4& t2 = tmp[Order - 1 - j - s];
		w = t2[3];
		points[k + r - j - s] = k3d::point4(t2[0] / w, t2[1] / w, t2[2] / w, w);
	}

	for (k3d::uint_t i = L + 1; i < k - s; i++)
	{
		const k3d::point4& t = tmp[i - L];
		double w = t[3];
		points[i] = k3d::point4(t[0] / w, t[1] / w, t[2] / w, w);
	}

	const k3d::uint_t new_point_count = points.size();
	Points.resize(new_point_count);
	Weights.resize(new_point_count);
	for(k3d::uint_t i = 0; i != new_point_count; ++i)
	{
		const k3d::point4& p = points[i];
		Weights[i] = p[3];
		Points[i] = k3d::point3(p[0], p[1], p[2]);
	}
}

void extract_curve_arrays(k3d::mesh::points_t& Points, k3d::mesh::knots_t& Knots, k3d::mesh::weights_t& Weights, const k3d::mesh& Mesh, const k3d::nurbs_curve::const_primitive& Curves, const k3d::uint_t Curve, const k3d::bool_t NormalizeKnots)
{
	const k3d::uint_t order = Curves.curve_orders[Curve];
	const k3d::uint_t point_count = Curves.curve_point_counts[Curve];
	const k3d::uint_t points_begin = Curves.curve_first_points[Curve];
	const k3d::uint_t knots_begin = Curves.curve_first_knots[Curve];
	const k3d::uint_t knots_end = knots_begin + point_count + order;

	Points.resize(point_count);
	Knots.resize(knots_end - knots_begin);
	Weights.resize(point_count);

	for(k3d::uint_t point = 0; point != point_count; ++point)
	{
		const k3d::uint_t curve_point_idx = points_begin + point;
		Points[point] = Mesh.points->at(Curves.curve_points[curve_point_idx]);
		Weights[point] = Curves.curve_point_weights[curve_point_idx];
	}

	for(k3d::uint_t knot_idx = knots_begin; knot_idx != knots_end; ++knot_idx)
	{
		const k3d::uint_t i = knot_idx - knots_begin;
		Knots[i] = Curves.curve_knots[knot_idx];
	}
	if(NormalizeKnots)
	{
		const k3d::double_t min = Knots.front();
		const k3d::double_t max = Knots.back();
		std::transform(Knots.begin(), Knots.end(), Knots.begin(), knot_normalizer(min, max));
	}
}

void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, k3d::uint_t Curve, const k3d::double_t u, const k3d::uint_t r)
{
	k3d::mesh::points_t points;
	k3d::mesh::knots_t knots;
	k3d::mesh::weights_t weights;
	const k3d::uint_t order = InputCurves.curve_orders[Curve];
	extract_curve_arrays(points, knots, weights, InputMesh, InputCurves, Curve);
	insert_knot(points, knots, weights, u, r, order);
	k3d::nurbs_curve::add_curve(OutputMesh, OutputCurves, order, points, weights, knots);
}

void append_common_knot_vector(k3d::mesh::knots_t& CommonKnotVector, const k3d::nurbs_curve::const_primitive& NurbsCurves, const k3d::uint_t Curve)
{
	k3d::mesh::knots_t::const_iterator knots_begin = NurbsCurves.curve_knots.begin() + NurbsCurves.curve_first_knots[Curve];
	k3d::mesh::knots_t::const_iterator knots_end = knots_begin + NurbsCurves.curve_point_counts[Curve] + NurbsCurves.curve_orders[Curve];
	k3d::mesh::knots_t normalized_knots(knots_end - knots_begin);
	std::transform(knots_begin, knots_end, normalized_knots.begin(), knot_normalizer(*knots_begin, *(knots_end - 1)));

	for(k3d::mesh::knots_t::iterator knot = normalized_knots.begin(); knot != normalized_knots.end();)
	{
		const k3d::uint_t old_mul = std::count_if(CommonKnotVector.begin(), CommonKnotVector.end(), find_knot_multiplicity(*knot));
		const k3d::uint_t new_mul = std::count_if(knot, normalized_knots.end(), find_knot_multiplicity(*knot));
		if(new_mul > old_mul)
		{
			k3d::mesh::knots_t::iterator insert_loc = std::find_if(CommonKnotVector.begin(), CommonKnotVector.end(), find_first_knot_after(*knot));
			CommonKnotVector.insert(insert_loc, new_mul - old_mul, *knot);
		}
		knot += new_mul;
	}
}

void delete_empty_primitives(k3d::mesh& Mesh)
{
	for(k3d::mesh::primitives_t::iterator prim = Mesh.primitives.begin(); prim != Mesh.primitives.end();)
	{
		bool erase = false;
		for(k3d::mesh::named_tables_t::const_iterator structure = (*prim)->structure.begin(); structure != (*prim)->structure.end(); ++structure)
		{
			for(k3d::mesh::named_arrays_t::const_iterator array = structure->second.begin(); array != structure->second.end(); ++array)
			{
				if(array->second->get_metadata_value(k3d::metadata::key::domain()) == k3d::metadata::value::point_indices_domain())
					if(array->second->empty())
						erase = true;
			}
		}
		prim = erase ? Mesh.primitives.erase(prim) : prim + 1;
	}
}

} //namespace nurbs

} //namespace module
