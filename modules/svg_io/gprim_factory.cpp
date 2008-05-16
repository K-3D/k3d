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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
	\author Tim Shead (tshead@k-3d.com)
*/

#include "gprim_factory.h"

#include <k3dsdk/imaterial.h>
#include <k3dsdk/shared_pointer.h>

namespace module
{

namespace svg
{

namespace io
{

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory::implementation

class gprim_factory::implementation
{
public:
	implementation(k3d::mesh& Mesh) :
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

		curve_group(0),

		first_curves(0),
		curve_counts(0),
		curve_materials(0),
		curve_first_points(0),
		curve_point_counts(0),
		curve_orders(0),
		curve_first_knots(0),
		curve_selection(0),
		curve_points(0),
		curve_point_weights(0),
		curve_knots(0)
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

	k3d::mesh& target_mesh;
	k3d::mesh::points_t* points;
	k3d::mesh::selection_t* point_selection;
	k3d::mesh::weights_t* point_weights; // Note: *not* part of the mesh!

	k3d::mesh::indices_t* first_faces;
	k3d::mesh::counts_t* face_counts;
	k3d::mesh::polyhedra_t::types_t* types;
	k3d::mesh::indices_t* face_first_loops;
	k3d::mesh::counts_t* face_loop_counts;
	k3d::mesh::selection_t* face_selection;
	k3d::mesh::materials_t* face_materials;
	k3d::mesh::indices_t* loop_first_edges;
	k3d::mesh::indices_t* edge_points;
	k3d::mesh::indices_t* clockwise_edges;
	k3d::mesh::selection_t* edge_selection;

	k3d::mesh::indices_t* nurbs_patch_first_points;
	k3d::mesh::counts_t* nurbs_patch_u_point_counts;
	k3d::mesh::counts_t* nurbs_patch_v_point_counts;
	k3d::mesh::orders_t* nurbs_patch_u_orders;
	k3d::mesh::orders_t* nurbs_patch_v_orders;
	k3d::mesh::indices_t* nurbs_patch_u_first_knots;
	k3d::mesh::indices_t* nurbs_patch_v_first_knots;
	k3d::mesh::selection_t* nurbs_patch_selection;
	k3d::mesh::materials_t* nurbs_patch_materials;
	k3d::mesh::indices_t* nurbs_patch_points;
	k3d::mesh::weights_t* nurbs_patch_point_weights;
	k3d::mesh::knots_t* nurbs_patch_u_knots;
	k3d::mesh::knots_t* nurbs_patch_v_knots;

	k3d::mesh::nurbs_curve_groups_t* curve_group;
	k3d::mesh::indices_t* curve_points;
	k3d::mesh::selection_t* curve_selection;
	k3d::mesh::weights_t* curve_point_weights;
	k3d::mesh::knots_t* curve_knots;

	k3d::mesh::indices_t* first_curves;
	k3d::mesh::counts_t* curve_counts;
	k3d::mesh::materials_t* curve_materials;
	k3d::mesh::indices_t* curve_first_points;
	k3d::mesh::counts_t* curve_point_counts;
	k3d::mesh::orders_t* curve_orders;
	k3d::mesh::indices_t* curve_first_knots;
};

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory

gprim_factory::gprim_factory(k3d::mesh& Mesh) :
	m_implementation(new implementation(Mesh))
{
}

gprim_factory::~gprim_factory()
{
	delete m_implementation;
}

int gprim_factory::add_point(const k3d::point3& Point)
{
	if(!m_implementation->points)
	{
		m_implementation->points = k3d::make_unique(m_implementation->target_mesh.points);
		m_implementation->point_selection = k3d::make_unique(m_implementation->target_mesh.point_selection);
		m_implementation->point_weights = new k3d::mesh::weights_t(); // Note: *not* part of the mesh!
	}

	m_implementation->points->push_back(Point);
	m_implementation->point_selection->push_back(0.0);
	m_implementation->point_weights->push_back(1.0);
	return m_implementation->points->size();
}

int gprim_factory::add_point(const k3d::point4& Point)
{
	if(!m_implementation->points)
	{
		m_implementation->points = k3d::make_unique(m_implementation->target_mesh.points);
		m_implementation->point_selection = k3d::make_unique(m_implementation->target_mesh.point_selection);
		m_implementation->point_weights = new k3d::mesh::weights_t(); // Note: *not* part of the mesh!
	}

	m_implementation->points->push_back(k3d::point3(Point[0], Point[1], Point[2]));
	m_implementation->point_selection->push_back(0.0);
	m_implementation->point_weights->push_back(Point[3]);
	return m_implementation->points->size();
}

void gprim_factory::add_polygon(const k3d::mesh::indices_t& Points)
{
	return_if_fail(Points.size());

	if(!m_implementation->first_faces)
	{
		k3d::mesh::polyhedra_t* const polyhedra = k3d::make_unique(m_implementation->target_mesh.polyhedra);
		m_implementation->first_faces = k3d::make_unique(polyhedra->first_faces);
		m_implementation->face_counts = k3d::make_unique(polyhedra->face_counts);
		m_implementation->types = k3d::make_unique(polyhedra->types);
		m_implementation->face_first_loops = k3d::make_unique(polyhedra->face_first_loops);
		m_implementation->face_loop_counts = k3d::make_unique(polyhedra->face_loop_counts);
		m_implementation->face_selection = k3d::make_unique(polyhedra->face_selection);
		m_implementation->face_materials = k3d::make_unique(polyhedra->face_materials);
		m_implementation->loop_first_edges = k3d::make_unique(polyhedra->loop_first_edges);
		m_implementation->edge_points = k3d::make_unique(polyhedra->edge_points);
		m_implementation->clockwise_edges = k3d::make_unique(polyhedra->clockwise_edges);
		m_implementation->edge_selection = k3d::make_unique(polyhedra->edge_selection);

		m_implementation->face_counts->push_back(0);
		m_implementation->first_faces->push_back(0);
		m_implementation->types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
	}

	m_implementation->face_first_loops->push_back(m_implementation->loop_first_edges->size());
	m_implementation->face_loop_counts->push_back(1);
	m_implementation->face_selection->push_back(0.0);
	m_implementation->face_materials->push_back(static_cast<k3d::imaterial*>(0));
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

void gprim_factory::add_curve(const k3d::mesh::indices_t& Points, int order)
{
	return_if_fail(Points.size());
	if(!m_implementation->first_curves)
	{
		k3d::mesh::nurbs_curve_groups_t* const nurbs_curve_groups = k3d::make_unique(m_implementation->target_mesh.nurbs_curve_groups);
		m_implementation->first_curves = k3d::make_unique(nurbs_curve_groups->first_curves);
		m_implementation->curve_counts = k3d::make_unique(nurbs_curve_groups->curve_counts);
		m_implementation->curve_materials = k3d::make_unique(nurbs_curve_groups->materials);
		m_implementation->curve_first_points = k3d::make_unique(nurbs_curve_groups->curve_first_points);
		m_implementation->curve_point_counts = k3d::make_unique(nurbs_curve_groups->curve_point_counts);
		m_implementation->curve_orders = k3d::make_unique(nurbs_curve_groups->curve_orders);
		m_implementation->curve_first_knots = k3d::make_unique(nurbs_curve_groups->curve_first_knots);
		m_implementation->curve_selection = k3d::make_unique(nurbs_curve_groups->curve_selection);
		m_implementation->curve_points = k3d::make_unique(nurbs_curve_groups->curve_points);
		m_implementation->curve_point_weights = k3d::make_unique(nurbs_curve_groups->curve_point_weights);
		m_implementation->curve_knots = k3d::make_unique(nurbs_curve_groups->curve_knots);
		
		m_implementation->curve_counts->push_back(1);
		m_implementation->first_curves->push_back(0);		
	}
	else
		m_implementation->curve_counts->back() = m_implementation->curve_counts->back()+1;

	m_implementation->curve_materials->push_back(static_cast<k3d::imaterial*>(0));
	m_implementation->curve_first_points->push_back(m_implementation->curve_points->size());
	m_implementation->curve_point_counts->push_back(Points.size());
	//m_implementation->curve_orders->push_back(3);
	m_implementation->curve_orders->push_back(order);
	m_implementation->curve_first_knots->push_back(m_implementation->curve_knots->size());

	
	k3d::uint_t surr_knots = m_implementation->curve_orders->back(); 

	if(surr_knots % 2 != 0)
	{
		surr_knots--;
		m_implementation->curve_knots->push_back(0);
	}
	for(int prefix_knots = 0; prefix_knots <= (int)(surr_knots * 0.5); prefix_knots++)
	//for(int i=0; i<2; i++)
	{
		m_implementation->curve_knots->push_back(0);
	}

	for(k3d::uint_t point = 0; point < Points.size(); point++ )
	{
		m_implementation->curve_points->push_back(Points[point]);
		m_implementation->curve_point_weights->push_back(1.0);
		m_implementation->curve_selection->push_back(0.0);
		m_implementation->curve_knots->push_back(m_implementation->curve_knots->back()+1);
	}
	//for(int i=0; i<3; i++)
		//m_implementation->curve_knots->push_back(m_implementation->curve_knots->back()+1);

	
	for(k3d::uint_t suffix_knots = 0; suffix_knots < static_cast<k3d::uint_t>(surr_knots * 0.5); suffix_knots++)
	//for(int i=0; i<2; i++)
	{
		m_implementation->curve_knots->push_back(m_implementation->curve_knots->back());
		//m_implementation->curve_knots->push_back(3);
	}

	//for(int i=0; i<m_implementation->curve_knots->size(); i++)
		//k3d::log() << debug << "Curve_knot " << i << ": " << m_implementation->curve_knots->at(i) << std::endl;
	
}

} // namespace io

} // namespace svg

} // namespace module

