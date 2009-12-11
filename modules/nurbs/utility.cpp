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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "utility.h"

namespace module
{

namespace nurbs
{

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

knot_vector_merger::knot_vector_merger(const k3d::mesh::knots_t& UnifiedKnots, const k3d::uint_t Order) : unified_knots(UnifiedKnots), order(Order)
{
	std::transform(unified_knots.begin(), unified_knots.end(), unified_knots.begin(), knot_normalizer(unified_knots.front(), unified_knots.back()));
}

void knot_vector_merger::operator()(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t& Curve)
{
	k3d::mesh::points_t points;
	k3d::mesh::knots_t knots;
	k3d::mesh::weights_t weights;
	extract_curve_arrays(points, knots, weights, InputMesh, InputCurves, Curve, true);
	if(unified_knots.size() < knots.size())
		throw std::runtime_error("Unified knot vector is too short");
	// First, we check if the old knot vector has knots that are not in the common knot vector (i.e. knots that were added manually using knot vector editing)
	k3d::uint_t i = 0;
	for(k3d::uint_t knot_idx = 0; knot_idx != knots.size(); ++knot_idx)
	{
		const k3d::double_t old_knot = knots[knot_idx];
		const k3d::uint_t new_mul = multiplicity(unified_knots, old_knot, 0, unified_knots.size());
		const k3d::uint_t old_mul = multiplicity(knots, old_knot, 0, knots.size());
		if(new_mul < old_mul && std::abs(unified_knots[i] - knots[knot_idx]) > 0.000001)
		{
			const k3d::double_t previous_knot = knot_idx == 0 ? knots.front() : knots[knot_idx - 1];
			const k3d::double_t next_knot = knot_idx == knots.size() - 1 ? knots.back() : knots[knot_idx - 1];
			k3d::uint_t i = knot_idx;
			while(unified_knots[i] < previous_knot || multiplicity(knots, unified_knots[i], 0, knots.size()) >= multiplicity(unified_knots, unified_knots[i], 0, unified_knots.size()))
				++i;
			knots[knot_idx] = unified_knots[i];
		}
		++i;
	}
	// Next, add the missing knots
	for(k3d::uint_t knot_idx = 0; knot_idx != unified_knots.size(); ++knot_idx)
	{
		const k3d::double_t u = unified_knots[knot_idx];
		const k3d::uint_t old_mul = multiplicity(knots, u, 0, knots.size());
		const k3d::uint_t new_mul = multiplicity(unified_knots, u, 0, unified_knots.size());
		if(old_mul > new_mul)
			throw std::runtime_error("Error inserting knots when creating a common knot vector");
		if(old_mul < new_mul)
			insert_knot(points, knots, weights, u, 1, order);
	}
	k3d::nurbs_curve::add_curve(OutputMesh, OutputCurves, order, points, weights, knots);
}

} //namespace nurbs

} //namespace module
