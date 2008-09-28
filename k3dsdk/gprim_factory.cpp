// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "gprim_factory.h"
#include "imaterial.h"

namespace k3d
{

namespace detail
{

// Knot multiplicity validation identical to knotvector.cc from the GLU toolkit.
bool validate_knot_vector(const uint_t Order, const mesh::knots_t& Knots)
{
	double tol = 1e-5;
	uint_t multi = 1;
	bool valid = true;
	for(uint_t kindex = Knots.size() - 1; kindex >= 1; kindex-- )
	{
		if(Knots[kindex] - Knots[kindex-1] < tol)
		{
	    multi++;
	    continue;
		}
		if (multi > Order)
		{
	    valid = false;
	    break;
		}
		multi = 1;
	}

	if (!valid)
	{
		log() << debug << "Invalid knot vector:" << std::endl;
		for (uint_t knot = 0; knot != Knots.size(); ++knot)
			log() << debug << "    " << Knots[knot] << std::endl;
	}

	return valid;
}

// Normalize the knot vector so it falls in the interval [0, 1]
void normalize_knot_vector(mesh::knots_t& Knots, double& Factor, double& Offset)
{
	const double min = Knots.front();
	const double max = Knots.back();
	const double factor = 1 / (max - min);
	Factor = factor;
	Offset = min;
	for (uint_t knot = 0; knot != Knots.size(); ++knot)
	{
		Knots[knot] = factor * (Knots[knot] - min);
	}
}

}

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory::implementation

class gprim_factory::implementation
{
public:
	implementation(mesh& Mesh) :
		target_mesh(Mesh),

		points(0),
		point_selection(0),
		point_weights(0),

		first_faces(0),
		face_counts(0),
		types(0),
		face_first_loops(0),
		face_loop_counts(0),
		face_selection(0),
		face_materials(0),
		loop_first_edges(0),
		edge_points(0),
		clockwise_edges(0),

		nurbs_patch_first_points(0),
		nurbs_patch_u_point_counts(0),
		nurbs_patch_v_point_counts(0),
		nurbs_patch_u_orders(0),
		nurbs_patch_v_orders(0),
		nurbs_patch_u_first_knots(0),
		nurbs_patch_v_first_knots(0),
		nurbs_patch_selection(0),
		nurbs_patch_materials(0),
		nurbs_patch_points(0),
		nurbs_patch_point_weights(0),
		nurbs_patch_u_knots(0),
		nurbs_patch_v_knots(0),

		patch_trim_curve_loop_counts(0),
		patch_first_trim_curve_loops(0),
		trim_points(0),
		trim_point_selection(0),
		first_trim_curves(0),
		trim_curve_counts(0),
		trim_curve_loop_selection(0),
		trim_curve_first_points(0),
		trim_curve_point_counts(0),
		trim_curve_orders(0),
		trim_curve_first_knots(0),
		trim_curve_selection(0),
		trim_curve_points(0),
		trim_curve_point_weights(0),
		trim_curve_knots(0),

		first_curves(0),
		curve_counts(0),
		materials(0),
		curve_first_points(0),
		curve_point_counts(0),
		curve_orders(0),
		curve_first_knots(0),
		curve_selection(0),
		curve_points(0),
		curve_point_weights(0),
		curve_knots(0),

		trim_curve_closed(true)
	{
	}

	~implementation()
	{
		if(first_faces)
		{
			face_counts->push_back(face_first_loops->size() - first_faces->front());
		}

		delete point_weights;
	}

	mesh& target_mesh;
	mesh::points_t* points;
	mesh::selection_t* point_selection;
	mesh::weights_t* point_weights; // Note: *not* part of the mesh!

	mesh::indices_t* first_faces;
	mesh::counts_t* face_counts;
	mesh::polyhedra_t::types_t* types;
	mesh::indices_t* face_first_loops;
	mesh::counts_t* face_loop_counts;
	mesh::selection_t* face_selection;
	mesh::materials_t* face_materials;
	mesh::indices_t* loop_first_edges;
	mesh::indices_t* edge_points;
	mesh::indices_t* clockwise_edges;
	mesh::selection_t* edge_selection;

	mesh::indices_t* nurbs_patch_first_points;
	mesh::counts_t* nurbs_patch_u_point_counts;
	mesh::counts_t* nurbs_patch_v_point_counts;
	mesh::orders_t* nurbs_patch_u_orders;
	mesh::orders_t* nurbs_patch_v_orders;
	mesh::indices_t* nurbs_patch_u_first_knots;
	mesh::indices_t* nurbs_patch_v_first_knots;
	mesh::selection_t* nurbs_patch_selection;
	mesh::materials_t* nurbs_patch_materials;
	mesh::indices_t* nurbs_patch_points;
	mesh::weights_t* nurbs_patch_point_weights;
	mesh::knots_t* nurbs_patch_u_knots;
	mesh::knots_t* nurbs_patch_v_knots;

	mesh::counts_t* patch_trim_curve_loop_counts;
	mesh::indices_t* patch_first_trim_curve_loops;
	mesh::points_2d_t* trim_points;
	mesh::selection_t* trim_point_selection;
	mesh::indices_t* first_trim_curves;
	mesh::counts_t* trim_curve_counts;
	mesh::selection_t* trim_curve_loop_selection;
	mesh::indices_t* trim_curve_first_points;
	mesh::counts_t* trim_curve_point_counts;
	mesh::orders_t* trim_curve_orders;
	mesh::indices_t* trim_curve_first_knots;
	mesh::selection_t* trim_curve_selection;
	mesh::indices_t* trim_curve_points;
	mesh::weights_t* trim_curve_point_weights;
	mesh::knots_t* trim_curve_knots;

	mesh::indices_t* first_curves;
	mesh::counts_t* curve_counts;
	mesh::materials_t* materials;
	mesh::indices_t* curve_first_points;
	mesh::counts_t* curve_point_counts;
	mesh::orders_t* curve_orders;
	mesh::indices_t* curve_first_knots;
	mesh::selection_t* curve_selection;
	mesh::indices_t* curve_points;
	mesh::weights_t* curve_point_weights;
	mesh::knots_t* curve_knots;

	// Applied normalization factors (needed for trim curve adjustment)
	mesh::knots_t u_offsets;
	mesh::knots_t u_factors;
	mesh::knots_t v_offsets;
	mesh::knots_t v_factors;

	bool trim_curve_closed; // Indicates if the last trim curve was closed
};

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory

gprim_factory::gprim_factory(mesh& Mesh) :
	m_implementation(new implementation(Mesh))
{
}

gprim_factory::~gprim_factory()
{
	delete m_implementation;
}

void gprim_factory::add_point(const point3& Point)
{
	if(!m_implementation->points)
	{
		m_implementation->points = &m_implementation->target_mesh.points.create();
		m_implementation->point_selection = &m_implementation->target_mesh.point_selection.create();
		m_implementation->point_weights = new mesh::weights_t(); // Note: *not* part of the mesh!
	}

	m_implementation->points->push_back(Point);
	m_implementation->point_selection->push_back(0.0);
	m_implementation->point_weights->push_back(1.0);
}

void gprim_factory::add_point(const point4& Point)
{
	if(!m_implementation->points)
	{
		m_implementation->points = &m_implementation->target_mesh.points.create();
		m_implementation->point_selection = &m_implementation->target_mesh.point_selection.create();
		m_implementation->point_weights = new mesh::weights_t(); // Note: *not* part of the mesh!
	}

	m_implementation->points->push_back(point3(Point[0], Point[1], Point[2]));
	m_implementation->point_selection->push_back(0.0);
	m_implementation->point_weights->push_back(Point[3]);
}

void gprim_factory::add_polygon(const mesh::indices_t& Points)
{
	return_if_fail(Points.size());

	if(!m_implementation->first_faces)
	{
		mesh::polyhedra_t* const polyhedra = &m_implementation->target_mesh.polyhedra.create();
		m_implementation->first_faces = &polyhedra->first_faces.create();
		m_implementation->face_counts = &polyhedra->face_counts.create();
		m_implementation->types = &polyhedra->types.create();
		m_implementation->face_first_loops = &polyhedra->face_first_loops.create();
		m_implementation->face_loop_counts = &polyhedra->face_loop_counts.create();
		m_implementation->face_selection = &polyhedra->face_selection.create();
		m_implementation->face_materials = &polyhedra->face_materials.create();
		m_implementation->loop_first_edges = &polyhedra->loop_first_edges.create();
		m_implementation->edge_points = &polyhedra->edge_points.create();
		m_implementation->clockwise_edges = &polyhedra->clockwise_edges.create();
		m_implementation->edge_selection = &polyhedra->edge_selection.create();
		m_implementation->first_faces->push_back(0);
		m_implementation->types->push_back(mesh::polyhedra_t::POLYGONS);
	}

	m_implementation->face_first_loops->push_back(m_implementation->loop_first_edges->size());
	m_implementation->face_loop_counts->push_back(1);
	m_implementation->face_selection->push_back(0.0);
	m_implementation->face_materials->push_back(static_cast<imaterial*>(0));
	m_implementation->loop_first_edges->push_back(m_implementation->edge_points->size());

	const size_t point_begin = 0;
	const size_t point_end = point_begin + Points.size();
	const size_t first_edge = m_implementation->edge_points->size();
	for(size_t point = point_begin; point != point_end; ++point)
	{
		m_implementation->edge_points->push_back(Points[point]);
		m_implementation->clockwise_edges->push_back(m_implementation->edge_points->size());
		m_implementation->edge_selection->push_back(0.0);
	}
	m_implementation->clockwise_edges->back() = first_edge;
}

void gprim_factory::add_hole(const mesh::indices_t& Points)
{
	return_if_fail(Points.size());
	return_if_fail(m_implementation->first_faces);

	m_implementation->face_loop_counts->back()++;
	m_implementation->loop_first_edges->push_back(m_implementation->edge_points->size());

	const size_t point_begin = 0;
	const size_t point_end = point_begin + Points.size();
	const size_t first_edge = m_implementation->edge_points->size();
	for(size_t point = point_begin; point != point_end; ++point)
	{
		m_implementation->edge_points->push_back(Points[point]);
		m_implementation->clockwise_edges->push_back(m_implementation->edge_points->size());
		m_implementation->edge_selection->push_back(0.0);
	}
	m_implementation->clockwise_edges->back() = first_edge;
}

bool gprim_factory::add_nurbs_patch(const size_t UOrder, const size_t VOrder, const mesh::indices_t& Points, const mesh::knots_t& UKnots, const mesh::knots_t VKnots, const mesh::weights_t& Weights)
{
	if(!m_implementation->nurbs_patch_first_points)
	{
		mesh::nurbs_patches_t* const nurbs_patches = &m_implementation->target_mesh.nurbs_patches.create();
		m_implementation->nurbs_patch_first_points = &nurbs_patches->patch_first_points.create();
		m_implementation->nurbs_patch_u_point_counts = &nurbs_patches->patch_u_point_counts.create();
		m_implementation->nurbs_patch_v_point_counts = &nurbs_patches->patch_v_point_counts.create();
		m_implementation->nurbs_patch_u_orders = &nurbs_patches->patch_u_orders.create();
		m_implementation->nurbs_patch_v_orders = &nurbs_patches->patch_v_orders.create();
		m_implementation->nurbs_patch_u_first_knots = &nurbs_patches->patch_u_first_knots.create();
		m_implementation->nurbs_patch_v_first_knots = &nurbs_patches->patch_v_first_knots.create();
		m_implementation->nurbs_patch_selection = &nurbs_patches->patch_selection.create();
		m_implementation->nurbs_patch_materials = &nurbs_patches->patch_materials.create();
		m_implementation->nurbs_patch_points = &nurbs_patches->patch_points.create();
		m_implementation->nurbs_patch_point_weights = &nurbs_patches->patch_point_weights.create();
		m_implementation->nurbs_patch_u_knots = &nurbs_patches->patch_u_knots.create();
		m_implementation->nurbs_patch_v_knots = &nurbs_patches->patch_v_knots.create();
	}

	m_implementation->u_offsets.push_back(0.0);
	m_implementation->u_factors.push_back(0.0);
	m_implementation->v_offsets.push_back(0.0);
	m_implementation->v_factors.push_back(0.0);

	uint_t patch_number = m_implementation->nurbs_patch_first_points->size();

	mesh::knots_t uknots = UKnots;
	mesh::knots_t vknots = VKnots;
	detail::normalize_knot_vector(uknots, m_implementation->u_factors[patch_number], m_implementation->u_offsets[patch_number]);
	detail::normalize_knot_vector(vknots, m_implementation->v_factors[patch_number], m_implementation->v_offsets[patch_number]);

	//return_val_if_fail(detail::validate_knot_vector(UOrder, uknots), false);
	//return_val_if_fail(detail::validate_knot_vector(VOrder, vknots), false);

	return_val_if_fail((uknots.size() - UOrder) * (vknots.size() - VOrder) == Points.size(), false);
	return_val_if_fail(Points.size() == Weights.size(), false);

	m_implementation->nurbs_patch_first_points->push_back(m_implementation->nurbs_patch_points->size());
	m_implementation->nurbs_patch_u_point_counts->push_back(uknots.size() - UOrder);
	m_implementation->nurbs_patch_v_point_counts->push_back(vknots.size() - VOrder);
	m_implementation->nurbs_patch_u_orders->push_back(UOrder);
	m_implementation->nurbs_patch_v_orders->push_back(VOrder);
	m_implementation->nurbs_patch_u_first_knots->push_back(m_implementation->nurbs_patch_u_knots->size());
	m_implementation->nurbs_patch_v_first_knots->push_back(m_implementation->nurbs_patch_v_knots->size());
	m_implementation->nurbs_patch_selection->push_back(0);
	m_implementation->nurbs_patch_materials->push_back(static_cast<imaterial*>(0));
	m_implementation->nurbs_patch_points->insert(m_implementation->nurbs_patch_points->end(), Points.begin(), Points.end());

	for(size_t i = 0; i != Weights.size(); ++i)
		m_implementation->nurbs_patch_point_weights->push_back(Weights[i]);

	m_implementation->nurbs_patch_u_knots->insert(m_implementation->nurbs_patch_u_knots->end(), uknots.begin(), uknots.end());
	m_implementation->nurbs_patch_v_knots->insert(m_implementation->nurbs_patch_v_knots->end(), vknots.begin(), vknots.end());

	if (m_implementation->patch_trim_curve_loop_counts)
	{
		m_implementation->patch_trim_curve_loop_counts->push_back(0);
		m_implementation->patch_first_trim_curve_loops->push_back(0);
	}

	return true;
}

bool gprim_factory::add_trim_curve(const uint_t Order, const mesh::points_2d_t& Points, const mesh::knots_t& Knots, const mesh::weights_t& Weights)
{
	mesh::nurbs_patches_t* const nurbs_patches = &m_implementation->target_mesh.nurbs_patches.writable();

	if (!m_implementation->patch_trim_curve_loop_counts)
	{
		m_implementation->patch_trim_curve_loop_counts = &nurbs_patches->patch_trim_curve_loop_counts.create();
		m_implementation->patch_first_trim_curve_loops = &nurbs_patches->patch_first_trim_curve_loops.create();
		m_implementation->patch_trim_curve_loop_counts->resize(m_implementation->nurbs_patch_first_points->size(), 0);
		m_implementation->patch_first_trim_curve_loops->resize(m_implementation->nurbs_patch_first_points->size(), 0);
		m_implementation->trim_points = &nurbs_patches->trim_points.create();
		m_implementation->trim_point_selection = &nurbs_patches->trim_point_selection.create();
		m_implementation->first_trim_curves = &nurbs_patches->first_trim_curves.create();
		m_implementation->trim_curve_counts = &nurbs_patches->trim_curve_counts.create();
		m_implementation->trim_curve_loop_selection = &nurbs_patches->trim_curve_loop_selection.create();
		m_implementation->trim_curve_first_points = &nurbs_patches->trim_curve_first_points.create();
		m_implementation->trim_curve_point_counts = &nurbs_patches->trim_curve_point_counts.create();
		m_implementation->trim_curve_orders = &nurbs_patches->trim_curve_orders.create();
		m_implementation->trim_curve_first_knots = &nurbs_patches->trim_curve_first_knots.create();
		m_implementation->trim_curve_selection = &nurbs_patches->trim_curve_selection.create();
		m_implementation->trim_curve_points = &nurbs_patches->trim_curve_points.create();
		m_implementation->trim_curve_point_weights = &nurbs_patches->trim_curve_point_weights.create();
		m_implementation->trim_curve_knots = &nurbs_patches->trim_curve_knots.create();
	}

	return_val_if_fail(!m_implementation->nurbs_patch_first_points->empty(), false);

	const uint_t patch = m_implementation->nurbs_patch_first_points->size() - 1;

	return_val_if_fail(Points.size() == Knots.size() - Order, false);
	return_val_if_fail(Points.size() == Weights.size(), false);

	double factor, offset;
	mesh::knots_t knots = Knots;
	detail::normalize_knot_vector(knots, factor, offset);

	//return_val_if_fail(detail::validate_knot_vector(Order, knots), false);

	mesh::counts_t& patch_trim_curve_loop_counts = *m_implementation->patch_trim_curve_loop_counts;
	mesh::indices_t& patch_first_trim_curve_loops = *m_implementation->patch_first_trim_curve_loops;
	mesh::counts_t& trim_curve_counts = *m_implementation->trim_curve_counts;
	if (patch_trim_curve_loop_counts[patch] == 0) // First curve loop for this patch
	{
		patch_first_trim_curve_loops[patch] = m_implementation->first_trim_curves->size();
	}
	bool newloop = m_implementation->trim_curve_closed;
	if (newloop) // Start of a new curve loop
	{
		++patch_trim_curve_loop_counts[patch];
		m_implementation->trim_curve_loop_selection->push_back(0.0);
		m_implementation->first_trim_curves->push_back(m_implementation->trim_curve_first_points->size());
		trim_curve_counts.push_back(0);
		m_implementation->trim_curve_closed = false;
	}
	++trim_curve_counts[patch_first_trim_curve_loops[patch] + patch_trim_curve_loop_counts[patch] - 1];
	//log() << debug << "   Adding trim curve " << m_implementation->trim_curve_first_points->size() << std::endl;
	m_implementation->trim_curve_first_points->push_back(m_implementation->trim_curve_points->size());
	m_implementation->trim_curve_orders->push_back(Order);
	m_implementation->trim_curve_selection->push_back(0.0);
	if (!newloop) // Replace the last old point with the first new one, so the curve is completely closed
	{
		m_implementation->trim_points->pop_back();
		m_implementation->trim_point_selection->pop_back();
	}
	for (mesh::points_2d_t::const_iterator point = Points.begin(); point != Points.end(); ++point)
	{
		m_implementation->trim_curve_points->push_back(m_implementation->trim_points->size());
		point2 point_2 = *point;
		// Correct the point so it fits into the corrected patch parameter space
		point_2[0] = m_implementation->u_factors[patch] * (point_2[0] - m_implementation->u_offsets[patch]);
		point_2[1] = m_implementation->v_factors[patch] * (point_2[1] - m_implementation->v_offsets[patch]);
		point_2[0] = point_2[0] > 1 ? 1 : point_2[0];
		point_2[0] = point_2[0] < 0 ? 0 : point_2[0];
		point_2[1] = point_2[1] > 1 ? 1 : point_2[1];
		point_2[1] = point_2[1] < 0 ? 0 : point_2[1];
		m_implementation->trim_points->push_back(point_2);
		//log() << debug << "      Adding trim curve point " << *point << std::endl;
		m_implementation->trim_point_selection->push_back(0.0);
	}
	m_implementation->trim_curve_point_counts->push_back(Points.size());
	m_implementation->trim_curve_first_knots->push_back(m_implementation->trim_curve_knots->size());
	m_implementation->trim_curve_knots->insert(m_implementation->trim_curve_knots->end(), knots.begin(), knots.end());
	m_implementation->trim_curve_point_weights->insert(m_implementation->trim_curve_point_weights->end(), Weights.begin(), Weights.end());
}

void gprim_factory::close_trim_loop()
{
	return_if_fail(m_implementation->trim_points);
	m_implementation->trim_points->pop_back();
	m_implementation->trim_point_selection->pop_back();
	uint_t first_curve = m_implementation->first_trim_curves->back();
	uint_t first_point = m_implementation->trim_curve_points->at(m_implementation->trim_curve_first_points->at(first_curve));
	m_implementation->trim_curve_points->pop_back();
	m_implementation->trim_curve_points->push_back(first_point);

	m_implementation->trim_curve_closed = true;
}

void gprim_factory::add_nurbs_curve(const uint_t Order, const mesh::indices_t& Points, const mesh::knots_t& Knots, const mesh::weights_t& Weights)
{
	mesh::nurbs_curve_groups_t* const nurbs_curves = m_implementation->target_mesh.nurbs_curve_groups
		? &m_implementation->target_mesh.nurbs_curve_groups.writable()
		: &m_implementation->target_mesh.nurbs_curve_groups.create();

	if (!m_implementation->first_curves)
	{
		m_implementation->first_curves = &nurbs_curves->first_curves.create();
		m_implementation->curve_counts = &nurbs_curves->curve_counts.create();
		m_implementation->materials = &nurbs_curves->materials.create();
		m_implementation->curve_first_points = &nurbs_curves->curve_first_points.create();
		m_implementation->curve_point_counts = &nurbs_curves->curve_point_counts.create();
		m_implementation->curve_orders = &nurbs_curves->curve_orders.create();
		m_implementation->curve_first_knots = &nurbs_curves->curve_first_knots.create();
		m_implementation->curve_selection = &nurbs_curves->curve_selection.create();
		m_implementation->curve_points = &nurbs_curves->curve_points.create();
		m_implementation->curve_point_weights = &nurbs_curves->curve_point_weights.create();
		m_implementation->curve_knots = &nurbs_curves->curve_knots.create();
		m_implementation->first_curves->push_back(0);
		m_implementation->curve_counts->push_back(0);
		m_implementation->materials->push_back(static_cast<imaterial*>(0));
	}
	mesh::counts_t& curve_counts = *m_implementation->curve_counts;
	++curve_counts[0];
	m_implementation->curve_first_points->push_back(m_implementation->curve_points->size());
	m_implementation->curve_point_counts->push_back(Points.size());
	m_implementation->curve_orders->push_back(Order);
	m_implementation->curve_first_knots->push_back(m_implementation->curve_knots->size());
	m_implementation->curve_selection->push_back(0.0);
	m_implementation->curve_points->insert(m_implementation->curve_points->end(), Points.begin(), Points.end());
	m_implementation->curve_point_weights->insert(m_implementation->curve_point_weights->end(), Weights.begin(), Weights.end());
	m_implementation->curve_knots->insert(m_implementation->curve_knots->end(), Knots.begin(), Knots.end());
}

const mesh& gprim_factory::target_mesh() const
{
	return m_implementation->target_mesh;
}

} // k3d

