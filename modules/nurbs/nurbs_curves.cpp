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

void normalize_knot_vector(k3d::nurbs_curve::primitive& NurbsCurve, const k3d::uint_t Curve)
{
	const k3d::uint_t curve_point_begin = NurbsCurve.curve_first_points[Curve];
	const k3d::uint_t curve_point_end = curve_point_begin + NurbsCurve.curve_point_counts[Curve];

	const k3d::uint_t curve_knots_begin = NurbsCurve.curve_first_knots[Curve];
	const k3d::uint_t curve_knots_end = curve_knots_begin + (curve_point_end - curve_point_begin) + NurbsCurve.curve_orders[Curve];

	k3d::double_t min = NurbsCurve.curve_knots[curve_knots_begin];
	k3d::double_t max = NurbsCurve.curve_knots[curve_knots_end - 1] - min;

	for (k3d::uint_t knot = curve_knots_begin; knot < curve_knots_end; knot++)
	{
		NurbsCurve.curve_knots[knot] = (NurbsCurve.curve_knots[knot] - min) / max;
	}
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

void elevate_curve_degree(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurve, const k3d::mesh::indices_t Curves)
{
	const k3d::uint_t curve_count = InputCurve.curve_first_points.size();
	const k3d::uint_t point_count = InputMesh.points->size();
	k3d::mesh::selection_t curve_selections(curve_count, 0.0);
	for(k3d::uint_t i = 0; i != Curves.size(); ++i)
	{
		curve_selections[Curves[i]] = 1.0;
	}
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		if(curve_selections[curve])
		{
			const k3d::uint_t t = 1;
			const k3d::uint_t order = InputCurve.curve_orders[curve];
			k3d::uint_t power = order - 1;
			const k3d::uint_t nr_points = InputCurve.curve_point_counts[curve];
			const k3d::mesh::points_t& input_points = *InputMesh.points;

			const k3d::uint_t curve_points_begin = InputCurve.curve_first_points[curve];
			const k3d::uint_t curve_points_end = curve_points_begin + InputCurve.curve_point_counts[curve];

			const k3d::uint_t curve_knots_begin = InputCurve.curve_first_knots[curve];
			const k3d::uint_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + InputCurve.curve_orders[curve];

			std::vector<std::vector<double> > bezalfs(power + t + 1, std::vector<double>(power + 1, 1.0));
			std::vector<k3d::point4> bpts(power + 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
			std::vector<k3d::point4> ebpts(power + t + 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
			std::vector<k3d::point4> next_bpts(power - 1, k3d::point4(0.0, 0.0, 0.0, 0.0));
			std::vector<double> alphas(power - 1, 0.0);

			k3d::mesh::knots_t new_knots(2*(nr_points + order + t), 0.0);
			std::vector<k3d::point4> new_points(2*(nr_points + t), k3d::point4(0.0, 0.0, 0.0, 0.0));

			fill_bezalfs(bezalfs, power, t);

			k3d::uint_t m = nr_points + power;
			k3d::uint_t mh = power + t;
			k3d::uint_t kind = mh + 1;
			int r = -1;
			k3d::uint_t a = power;
			k3d::uint_t b = order;
			k3d::uint_t cind = 1;
			k3d::double_t ua = InputCurve.curve_knots[curve_knots_begin];

			new_points[0] = homogeneous(input_points[InputCurve.curve_points[curve_points_begin]], InputCurve.curve_point_weights[curve_points_begin]);

			for (int i = 0; i <= power + t; i++)
				new_knots.push_back(ua);

			//initialize first bezier segment
			for (int i = 0; i <= power; i++)
			{
				bpts[i] = homogeneous(input_points[InputCurve.curve_points[curve_points_begin + i]], InputCurve.curve_point_weights[curve_points_begin + i]);
			}

			while (b < m)//big loop through knot vector
			{
				int i = b;
				while (b < m && fabs(InputCurve.curve_knots[curve_knots_begin + b] - InputCurve.curve_knots[curve_knots_begin + b + 1]) < 0.00001)
					b++;
				int mul = b - i + 1;
				mh = mh + mul + t;
				double ub = InputCurve.curve_knots[curve_knots_begin + b];
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
						alphas[k - mul - 1] = numer / (InputCurve.curve_knots[curve_knots_begin + a + k] - ua);
					}
					for (int j = 1; j <= r; j++)
					{
						int save = r - j;
						int s = mul + j;
						for (int k = power; k >= s; k--)
						{
							bpts[k] = alphas[k - s] * bpts[k] + (1.0 - alphas[k - s]) * bpts[k - 1];
						}
						next_bpts[save] = bpts[power];
					}
				}//end of "insert knot"
				else
					k3d::log() << debug << "Didnt have to insert knot " << ub << " because r = " << r << std::endl;
				//degree elevate bezier
				for (k3d::uint_t i = lbz; i <= power + t; i++)
				{
					//only points lbz, ... , power+t are used here
					ebpts[i] = k3d::point4(0.0, 0.0, 0.0, 0.0);
					k3d::uint_t mpi = std::min(power, i);
					for (k3d::uint_t j = std::max(k3d::uint_t(0), i - t); j <= mpi; j++)
					{
						ebpts[i] += k3d::to_vector(bezalfs[i][j] * bpts[j]);
					}
				}

				if (oldr > 1)
				{
					//must remove knot ua oldr times
					int first = kind - 2;
					int last = kind;
					double den = ub - ua;
					double bet = (ub - new_knots[kind - 1]) / den;
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
								double alf = (ub - new_knots[i]) / (ua - new_knots[i]);
								new_points[i] = alf * new_points[i] + (1.0 - alf) * new_points[i - 1];
							}
							if (j >= lbz)
							{
								if (j - tr <= kind - power - t + oldr)
								{
									double gam = (ub - new_knots[j - tr]) / den;
									ebpts[kj] = gam * ebpts[kj] + (1.0 - gam) * ebpts[kj + 1];
								}
								else
									ebpts[kj] = bet * ebpts[kj] + (1.0 - bet) * ebpts[kj + 1];
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
						new_knots[kind] = ua;
						kind++;
					}
				}
				for (int j = lbz; j <= rbz; j++)
				{
					new_points[cind] = ebpts[j];
					cind++;

				}

				if (b < m)
				{
					for (int j = 0; j < r; j++)
						bpts[j] = next_bpts[j];
					for (int j = r; j <= power; j++)
					{
						const k3d::uint_t bpt_idx = curve_points_begin + b - power + j;
						bpts[j] = homogeneous(input_points[InputCurve.curve_points[bpt_idx]], InputCurve.curve_point_weights[bpt_idx]);
					}
					a = b;
					b++;
					ua = ub;
				}
				else//end knot
				{
					for (int i = 0; i <= power + t; i++)
					{
						new_knots[kind + i] = ub;
					}
				}
			}//end while loop (b < m)

			const k3d::uint_t new_n = mh - power - t;
			new_points[new_n - 1] = homogeneous(input_points[InputCurve.curve_points[curve_points_end - 1]], InputCurve.curve_point_weights[curve_points_end - 1]);
			new_points.resize(new_n);
			new_knots.resize(new_n + order + t);
			k3d::mesh::weights_t elevated_curve_weigts;
			k3d::mesh::points_t elevated_points;
			for(k3d::uint_t i = 0; i != new_n; ++i)
			{
				const k3d::point4& p = new_points[i];
				elevated_curve_weigts.push_back(p[3]);
				elevated_points.push_back(k3d::point3(p[0], p[1], p[2]));
			}
			k3d::nurbs_curve::add_curve(OutputMesh, OutputCurve, order+1, elevated_points, elevated_curve_weigts, new_knots);
		}
		else // !curve_selections[curve]
		{
			add_curve(OutputMesh, OutputCurve, InputMesh, InputCurve, curve);
		}
		OutputCurve.curve_selections.back() = curve_selections[curve];
	}
	OutputCurve.material.push_back(InputCurve.material.back());
	replace_duplicate_points(OutputMesh);
	k3d::mesh::delete_unused_points(OutputMesh);
}

void connect_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::uint_t Primitive1Index, const k3d::uint_t Curve1Index, const k3d::bool_t Curve1FirstPointSelection, const k3d::uint_t Primitive2Index, const k3d::uint_t Curve2Index, const k3d::bool_t Curve2FirstPointSelection)
{
//	//	if point is the first or last point of both curves, flip one of them
//	//	.choose them in a way that they can be joined
//	//	.normalize knot vectors
//	//	.add 1 to all knots in the 2nd curve
//	//	.put all points, weights and knots from 2nd into 1st curve
//	//	.delete 2nd curve
//
//	if (m_nurbs_curve.curve_orders.at(curve1) < m_nurbs_curve.curve_orders.at(curve2))
//	{
//		int dif = m_nurbs_curve.curve_orders.at(curve2) - m_nurbs_curve.curve_orders.at(curve1);
//		for (int i = 0; i < dif; i++)
//			curve_degree_elevate(curve1);
//	}
//	else if (m_nurbs_curve.curve_orders.at(curve2) < m_nurbs_curve.curve_orders.at(curve1))
//	{
//		int dif = m_nurbs_curve.curve_orders.at(curve1) - m_nurbs_curve.curve_orders.at(curve2);
//		for (int i = 0; i < dif; i++)
//			curve_degree_elevate(curve2);
//	}
//
//
//	const k3d::uint_t curve_points_begin[2] = {m_nurbs_curve.curve_first_points.at(curve1), m_nurbs_curve.curve_first_points.at(curve2)};
//	const k3d::uint_t curve_points_end[2] = { curve_points_begin[0] + m_nurbs_curve.curve_point_counts.at(curve1), curve_points_begin[1] + m_nurbs_curve.curve_point_counts.at(curve2) };
//
//	const k3d::uint_t curve_knots_begin[2] = { m_nurbs_curve.curve_first_knots.at(curve1), m_nurbs_curve.curve_first_knots.at(curve2)};
//	const k3d::uint_t curve_knots_end[2] = { curve_knots_begin[0] + m_nurbs_curve.curve_orders.at(curve1) + m_nurbs_curve.curve_point_counts.at(curve1), curve_knots_begin[1] + m_nurbs_curve.curve_orders.at(curve2) + m_nurbs_curve.curve_point_counts.at(curve2)};
//
//	//we might need to use curve 2 as curve 1 so we dont have to flip
//	k3d::uint_t use_curve1 = curve1;
//	k3d::uint_t use_curve2 = curve2;
//
//	k3d::uint_t use1 = 0;
//	k3d::uint_t use2 = 1;
//
//	if (point1 == curve_points_begin[0] && point2 == curve_points_begin[1])
//		flip_curve(curve1);
//	else if (point1 == curve_points_end[0] - 1 && point2 == curve_points_end[1] - 1)
//		flip_curve(curve2);
//	else if (point1 == curve_points_begin[0] && point2 == curve_points_end[1] - 1)
//	{
//		use_curve1 = curve2;
//		use_curve2 = curve1;
//		use1 = 1;
//		use2 = 0;
//	}
//	else if (point1 == curve_points_end[0] - 1 && point2 == curve_points_begin[1])
//	{
//		//no changes
//	}
//	else
//	{
//		k3d::log() << error << "You need to select and end-point of each curve!" << std::endl;
//		return;
//	}
//
//	//if this is true, then we can remove one of the points
//	bool isOnePoint = point3_float_equal(mesh_points->at(m_nurbs_curve.curve_points.at(curve_points_end[use2] - 1)), mesh_points->at(m_nurbs_curve.curve_points.at(curve_points_begin[use1])), 0.000001);
//
//	normalize_knot_vector(curve1);
//	normalize_knot_vector(curve2);
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Points" << std::endl;
//	//copy points
//	k3d::mesh::indices_t::iterator points_begin = m_nurbs_curve.curve_points.begin() + curve_points_begin[use2];
//	if (isOnePoint) points_begin++;
//	k3d::mesh::indices_t::iterator points_end = m_nurbs_curve.curve_points.begin() + curve_points_end[use2];
//
//	k3d::mesh::indices_t::iterator points_insert_at = m_nurbs_curve.curve_points.begin() + curve_points_end[use1];
//	m_nurbs_curve.curve_points.insert(points_insert_at, points_begin, points_end);
//
//	k3d::uint_t point_offset = curve_points_end[use2] - curve_points_begin[use2];
//	if (isOnePoint) point_offset--;
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "PointOffset " << point_offset << std::endl;
//
//	m_nurbs_curve.curve_point_counts.at(use_curve1) += point_offset;
//
//	for (k3d::uint_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
//	{
//		if (m_nurbs_curve.curve_first_points.at(curr_curve) > m_nurbs_curve.curve_first_points.at(use_curve1))
//			m_nurbs_curve.curve_first_points.at(curr_curve) += point_offset;
//	}
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Weights" << std::endl;
//	//copy weights
//
//	k3d::mesh::weights_t::iterator weights_begin = m_nurbs_curve.curve_point_weights.begin() + curve_points_begin[use2];
//	k3d::mesh::weights_t::iterator weights_end = m_nurbs_curve.curve_point_weights.begin() + curve_points_end[use2];
//
//	k3d::mesh::weights_t::iterator weights_insert_at = m_nurbs_curve.curve_point_weights.begin() + curve_points_end[use1];
//	if (isOnePoint)
//	{
//		weights_insert_at--;
//		*weights_insert_at = (*weights_insert_at + *weights_begin) / 2;
//		weights_insert_at++;
//		weights_begin++;
//	}
//
//	m_nurbs_curve.curve_point_weights.insert(weights_insert_at, weights_begin, weights_end);
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Copy Knots" << std::endl;
//	//copy knots
//	k3d::mesh::knots_t::iterator knots1_begin = m_nurbs_curve.curve_knots.begin() + curve_knots_begin[use1];
//	k3d::mesh::knots_t::iterator knots1_end = m_nurbs_curve.curve_knots.begin() + curve_knots_end[use1];
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found where to insert new knots" << std::endl;
//
//	//only delete within the 1st curve
//	k3d::uint_t offset1 = std::count(knots1_begin, knots1_end, 1.0); //Get the number that need deleting
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found " << offset1 << " knots with value 1.0 in curve 1" << std::endl;
//
//	m_nurbs_curve.curve_knots.erase(std::remove(knots1_begin, knots1_end, 1.0), knots1_end);
//
//	//if curve_knots_begin[use_curve1] < curve_knots_begin[use_curve2] do nothing, else subtract offset1
//	knots1_end = m_nurbs_curve.curve_knots.begin() + curve_knots_end[use1] - offset1;
//	k3d::mesh::knots_t::iterator knots2_begin = m_nurbs_curve.curve_knots.begin();
//	k3d::mesh::knots_t::iterator knots2_end = m_nurbs_curve.curve_knots.begin();
//
//	if (curve_knots_begin[use2] > curve_knots_begin[use1])
//	{
//		knots2_begin +=  curve_knots_begin[use2] - offset1;
//		knots2_end +=  curve_knots_end[use2] - offset1;
//		if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "With offset " << std::distance(knots2_begin, knots2_end) << std::endl;
//	}
//	else
//	{
//		knots2_begin +=  curve_knots_begin[use2];
//		knots2_end +=  curve_knots_end[use2];
//		if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Without offset " << std::distance(knots2_begin, knots2_end) << std::endl;
//	}
//
//	knots1_end--;
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Adding " << 1 << " to each knot of 2nd curve" << std::endl;
//
//	std::stringstream str;
//	str << "Knot vector:";
//
//	for (k3d::mesh::knots_t::iterator i = knots2_begin; i < knots2_end; ++i)
//	{
//		*i += 1;
//		str << " " << *i;
//	}
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << str.str() << std::endl;
//
//	double find_first = *knots2_begin;
//	k3d::uint_t offset2 = std::count(knots2_begin, knots2_end, find_first);
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Found " << offset2 << " knots with value " << find_first << " in curve 2" << std::endl;
//
//	knots1_end = m_nurbs_curve.curve_knots.begin() + curve_knots_end[use1] - offset1 - 1;
//	knots2_begin += offset2;
//
//	k3d::mesh::knots_t new_knots;
//	double a = *knots1_end;
//	if (isOnePoint)
//	{
//		for (int i = 0; i < m_nurbs_curve.curve_orders.at(use_curve1); i++)
//		{
//			new_knots.push_back(1.0);//the first one is just a dummy but we need the rest
//		}
//		new_knots.push_back(*knots2_begin);
//	}
//	else
//	{
//		double step = (*knots2_begin - a) / (m_nurbs_curve.curve_orders.at(use_curve1) + 1);
//
//		for (int i = 0; i < m_nurbs_curve.curve_orders.at(use_curve1) + 2; i++)
//		{
//			new_knots.push_back(a + step*i);
//			MY_DEBUG << new_knots.back() << std::endl;
//		}
//	}
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Going to insert at pos " << std::distance(m_nurbs_curve.curve_knots.begin(), knots1_end) << " with size " << m_nurbs_curve.curve_knots.size() << std::endl;
//	knots1_end++;
//	new_knots.erase(new_knots.begin());
//	m_nurbs_curve.curve_knots.insert(knots1_end, new_knots.begin(), new_knots.end());
//
//	knots1_end = m_nurbs_curve.curve_knots.begin() + curve_knots_end[use1] - offset1 + new_knots.size();
//	knots2_begin = m_nurbs_curve.curve_knots.begin() + curve_knots_begin[use2] + offset2 + 1;
//	knots2_end = m_nurbs_curve.curve_knots.begin() + curve_knots_end[use2];
//
//	if (curve_knots_begin[use2] > curve_knots_begin[use1])
//	{
//		knots2_begin += new_knots.size() - offset1;
//		knots2_end += new_knots.size() - offset1;
//	}
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserting : " << std::distance(knots2_begin, knots2_end) << " knots from " << std::distance(m_nurbs_curve.curve_knots.begin(), knots2_begin) << " ( is " << *knots2_begin << ") to " << std::distance(m_nurbs_curve.curve_knots.begin(), knots2_end) << " ( is " << *knots2_end << ")" << std::endl;
//
//	new_knots.clear();
//	new_knots.insert(new_knots.end(), knots2_begin, knots2_end);
//
//	for (k3d::mesh::knots_t::iterator i = new_knots.begin(); i != new_knots.end(); ++i)
//	{
//		MY_DEBUG << *i << std::endl;
//	}
//
//	m_nurbs_curve.curve_knots.insert(knots1_end, new_knots.begin(), new_knots.end());
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Inserted Knots" << std::endl;
//
//	int knot_offset = curve_knots_end[use2] - curve_knots_begin[use2] - offset1 - offset2 + m_nurbs_curve.curve_orders.at(use_curve1);
//	if (isOnePoint)
//		knot_offset--;
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Offset for first_knots is " << knot_offset << std::endl;
//
//	for (k3d::uint_t curr_curve = 0; curr_curve < count_all_curves_in_groups(); curr_curve++)
//	{
//		if (m_nurbs_curve.curve_first_knots.at(curr_curve) > m_nurbs_curve.curve_first_knots.at(use_curve1))
//			m_nurbs_curve.curve_first_knots.at(curr_curve) += knot_offset;
//	}
//
//	if (MODULE_NURBS_DEBUG) k3d::log() << debug << nurbs_debug << "Deleting 2nd curve.." << std::endl;
//	//delete the 2nd curve
//	delete_curve(use_curve2);
}

} //namespace nurbs

} //namespace module
