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

#include <k3dsdk/nurbs_curve.h>

#include "nurbs_curves.h"

namespace k3d { namespace nurbs_curve { class primitive; } }

namespace module
{

namespace nurbs
{

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

void close_curve(k3d::mesh& Mesh, k3d::nurbs_curve::primitive& NurbsCurve, k3d::uint_t curve, bool keep_ends)
{
	/*
			- get both end points
			if(keep_ends)
					. add the middle of the points as new one and use the old ends as CV
			else . replace them by their arithmetical middle
	*/
	const k3d::uint_t curve_points_begin = NurbsCurve.curve_first_points[curve];
	const k3d::uint_t curve_points_end = curve_points_begin + NurbsCurve.curve_point_counts[curve];

	const k3d::uint_t curve_knots_begin = NurbsCurve.curve_first_knots[curve];
	const k3d::uint_t curve_knots_end = curve_knots_begin + (curve_points_end - curve_points_begin) + NurbsCurve.curve_orders[curve];

	k3d::point3 new_point = (Mesh.points->at(NurbsCurve.curve_points[curve_points_begin]) + Mesh.points->at(NurbsCurve.curve_points[curve_points_end - 1])) * 0.5;
	Mesh.points.writable().push_back(new_point);
	Mesh.point_selection.writable().push_back(0.0);

	k3d::uint_t new_index = Mesh.points->size() - 1;

	if (!keep_ends)
	{
		replace_point(NurbsCurve, new_index, curve, NurbsCurve.curve_points[curve_points_begin], true);
		replace_point(NurbsCurve, new_index, curve, NurbsCurve.curve_points[curve_points_end - 1], true);
	}
	else
	{
		normalize_knot_vector(NurbsCurve, curve);
		k3d::uint_t order = NurbsCurve.curve_orders[curve];
		//take the first knot different from 0 and the last one different from 1.0, double the
		k3d::uint_t first = curve_knots_begin;

		while (NurbsCurve.curve_knots[first] == 0.0 && first < curve_knots_end)
		{
			first++;
		}

		k3d::uint_t last = curve_knots_end - 1;

		while (NurbsCurve.curve_knots[last] == 1.0 && last >= curve_knots_begin)
		{
			last--;
		}
		//insert knot at the end
		double diff = 1.0 - NurbsCurve.curve_knots[last];

		for (k3d::uint_t i = last + 1; i < curve_knots_end; i++)
			NurbsCurve.curve_knots[i] += diff;

		//insert knot at the beginning
		diff = NurbsCurve.curve_knots[first];

		for (int i = first - 1; i >= static_cast<int>(curve_knots_begin); i--)
		{
			k3d::log() << i << std::endl;
			NurbsCurve.curve_knots[i] -= diff;
		}

		k3d::mesh::knots_t::iterator last_iter = NurbsCurve.curve_knots.begin() + last + 1;
		NurbsCurve.curve_knots.insert(last_iter, 1.0);
		k3d::mesh::knots_t::iterator first_iter = NurbsCurve.curve_knots.begin() + first;
		NurbsCurve.curve_knots.insert(first_iter, 0.0);

		//insert point indices to the new point
		k3d::mesh::indices_t::iterator point_iter = NurbsCurve.curve_points.begin() + curve_points_begin;
		NurbsCurve.curve_points.insert(point_iter, new_index);

		point_iter = NurbsCurve.curve_points.begin() + curve_points_end + 1; //because we've already inserted one point
		NurbsCurve.curve_points.insert(point_iter, new_index);

		k3d::mesh::weights_t::iterator weights_iter = NurbsCurve.curve_point_weights.begin() + curve_points_begin;
		NurbsCurve.curve_point_weights.insert(weights_iter, 1.0);
		weights_iter = NurbsCurve.curve_point_weights.begin() + curve_points_end;
		NurbsCurve.curve_point_weights.insert(weights_iter, 1.0);

		NurbsCurve.curve_point_counts[curve] += 2;

		const k3d::uint_t curve_count = NurbsCurve.curve_first_points.size();
		for (k3d::uint_t curr_curve = 0; curr_curve < curve_count; curr_curve++)
		{
			if (NurbsCurve.curve_first_points[curve] < NurbsCurve.curve_first_points[curr_curve])
				NurbsCurve.curve_first_points[curr_curve] += 2;
			if (NurbsCurve.curve_first_knots[curve] < NurbsCurve.curve_first_knots[curr_curve])
				NurbsCurve.curve_first_knots[curr_curve] += 2;
		}
		normalize_knot_vector(NurbsCurve, curve);
	}
}

} //namespace nurbs

} //namespace module
