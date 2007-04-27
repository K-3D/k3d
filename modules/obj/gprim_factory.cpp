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

#include <k3dsdk/shared_pointer.h>

namespace libk3dobj
{

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory::implementation

class gprim_factory::implementation
{
public:
	implementation(k3d::dev::mesh& Mesh) :
		target_mesh(Mesh),

		points(0),
		point_selection(0),

		first_faces(0),
		face_counts(0),
		types(0),
		face_first_loops(0),
		face_loop_counts(0),
		face_selection(0),
		face_materials(0),
		loop_first_edges(0),
		edge_points(0),
		clockwise_edges(0)
	{
	}

	~implementation()
	{
		if(first_faces)
		{
			face_counts->push_back(face_first_loops->size() - first_faces->front());
		}
	}

	k3d::dev::mesh& target_mesh;
	k3d::dev::mesh::points_t* points;
	k3d::dev::mesh::selection_t* point_selection;

	k3d::dev::mesh::indices_t* first_faces;
	k3d::dev::mesh::counts_t* face_counts;
	k3d::dev::mesh::polyhedra_t::types_t* types;
	k3d::dev::mesh::indices_t* face_first_loops;
	k3d::dev::mesh::counts_t* face_loop_counts;
	k3d::dev::mesh::selection_t* face_selection;
	k3d::dev::mesh::materials_t* face_materials;
	k3d::dev::mesh::indices_t* loop_first_edges;
	k3d::dev::mesh::indices_t* edge_points;
	k3d::dev::mesh::indices_t* clockwise_edges;
	k3d::dev::mesh::selection_t* edge_selection;
};

//////////////////////////////////////////////////////////////////////////////////////////
// gprim_factory

gprim_factory::gprim_factory(k3d::dev::mesh& Mesh) :
	m_implementation(new implementation(Mesh))
{
}

gprim_factory::~gprim_factory()
{
	delete m_implementation;
}

void gprim_factory::add_point(const k3d::point3& Point)
{
	if(!m_implementation->points)
	{
		m_implementation->points = k3d::make_unique(m_implementation->target_mesh.points);
		m_implementation->point_selection = k3d::make_unique(m_implementation->target_mesh.point_selection);
	}

	m_implementation->points->push_back(Point);
	m_implementation->point_selection->push_back(0.0);
}

void gprim_factory::add_polygon(const k3d::dev::mesh::indices_t& Points)
{
	return_if_fail(Points.size());

	if(!m_implementation->first_faces)
	{
		k3d::dev::mesh::polyhedra_t* const polyhedra = k3d::make_unique(m_implementation->target_mesh.polyhedra);
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

		m_implementation->first_faces->push_back(0);
		m_implementation->types->push_back(k3d::dev::mesh::polyhedra_t::POLYGONS);
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

} // namespace libk3dobj

