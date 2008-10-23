#ifndef MODULES_CGAL_K3D_TO_NEF_H
#define MODULES_CGAL_K3D_TO_NEF_H

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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Converts a K-3D mesh to a Nef polyhedron. Based on polyhedron_3_to_nef3 from CGAL
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "conversion.h"

#include <CGAL/Nef_3/SNC_structure.h>
#include <CGAL/normal_vector_newell_3.h>

namespace module
{

namespace cgal
{

typedef CGAL::Plane_3<Kernel> Plane;
typedef CGAL::Vector_3<Kernel> Vector;
typedef std::vector<Point_3> points_t;

typedef Nef_polyhedron::SNC_structure SNC_structure;

/// Create a Point_3 from the given k3d::point3
Point_3 to_cgal_point3(const k3d::point3& Point)
{
//	NT common_denominator(1);
//	Rational x = CGAL::to_rational<Rational>(Point[0]);
//	common_denominator *= x.denominator();
//	Rational y = CGAL::to_rational<Rational>(Point[1]);
//	common_denominator *= y.denominator();
//	Rational z = CGAL::to_rational<Rational>(Point[2]);
//	common_denominator *= z.denominator();
//	
//	return Point_3(x.numerator()*common_denominator/x.denominator(), y.numerator()*common_denominator/y.denominator(), z.numerator()*common_denominator/z.denominator(), common_denominator);
	return Point_3(Point[0], Point[1], Point[2]);
}

/// Returns the plane containing the face given to operator().
struct face_plane {
	face_plane(const k3d::mesh& Mesh, points_t& Points) : m_mesh(Mesh), m_points(Points) {}
	
  Plane operator()(size_t FirstLoop) {     
    const k3d::mesh::indices_t& face_first_loops = *m_mesh.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *m_mesh.polyhedra->face_loop_counts;
		const k3d::mesh::indices_t& loop_first_edges = *m_mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *m_mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *m_mesh.polyhedra->clockwise_edges;
		
		std::vector<size_t> corners;
		std::vector<Point_3> corner_points;
		size_t first_edge = loop_first_edges[FirstLoop];
		for(size_t edge = first_edge; ; )
		{
			size_t point = edge_points[edge];
			corner_points.push_back(m_points[point]);
			
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		
		if (corner_points.size() < 3)
			throw 1; // Using throw here, since there really is no usable default if this fails
		
		Vector n;
		
		CGAL::normal_vector_newell_3(corner_points.begin(), corner_points.end(), n);
		
		Plane plane(corner_points[0], n);
		if(plane.is_degenerate())
			throw 2;
		
    return plane;
  }
  
  const k3d::mesh& m_mesh;
  points_t& m_points;
};

void k3d_to_nef(const k3d::mesh& Mesh, SNC_structure& S)
{
  typedef SNC_structure::SNC_decorator      SNC_decorator;
  typedef SNC_structure::SM_decorator       SM_decorator;
  typedef SNC_structure::Vertex_handle      Vertex_handle;
  typedef SNC_structure::SVertex_handle     SVertex_handle;
  typedef SNC_structure::SHalfedge_handle   SHalfedge_handle;
  typedef SNC_structure::SFace_handle       SFace_handle;
  typedef SNC_structure::Sphere_point       Sphere_point;
  typedef SNC_structure::Sphere_segment     Sphere_segment;
  typedef SNC_structure::Sphere_circle      Sphere_circle;
  typedef std::vector<SHalfedge_handle> indices_t;

  typedef Polyhedron::Halfedge_around_vertex_const_circulator
                               Halfedge_around_vertex_const_circulator;
		
	const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const k3d::mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
	const k3d::mesh::selection_t& face_selection = *Mesh.polyhedra->face_selection;
	const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const k3d::mesh::points_t& k3d_points = *Mesh.points;
	points_t points;
	
	// Convert to Point_3 representation
	for (size_t point = 0; point != k3d_points.size(); ++point)
		points.push_back(to_cgal_point3(k3d_points[point]));
	
	std::vector<Plane> planes(face_first_loops.size());
  
	std::transform(face_first_loops.begin(), face_first_loops.end(),
		planes.begin(), face_plane(Mesh, points));

		  	
	// check coplanarity
  for (size_t face = 0; face != face_first_loops.size(); ++face)
	{
		for (size_t loop = 0; loop != face_loop_counts[face]; ++loop)
		{
			size_t first_edge = loop_first_edges[face_first_loops[face] + loop];
			for(size_t edge = first_edge; ; )
			{
				size_t point = edge_points[edge];
				if (!planes[face].has_on(points[point]))
				{
					const Point_3& p = points[point];
					const Plane& pl = planes[face];
					Point_3 proj = pl.projection(p);
					Vector diff(proj, p);
					k3d::log() << debug << "point " << p << " is not on face " << face << "! Projection: " << CGAL::to_double(proj.x()) << ", " << CGAL::to_double(proj.y()) << ", " << CGAL::to_double(proj.z()) << ", " << std::endl;
					k3d::log() << "face: " << CGAL::to_double(pl.a()) << ", " << CGAL::to_double(pl.b()) << ", " << CGAL::to_double(pl.c()) << ", " << (pl.d()) << std::endl;
					k3d::log() << error << "k3d_to_nef: Non planar faces in input" << std::endl;
					throw 3;
				}
				
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	// Calculate companions
	std::vector<size_t> companions(edge_points.size());
	std::map<std::pair<size_t, size_t>, size_t > edge_end_points; // maps edge start and endpoint to edge number
	std::vector<size_t> first_in_edges(points.size()); // Store an incoming edge for each point
	size_t edge_count = edge_points.size();
	for (size_t edge = 0; edge != edge_count; ++edge)
		edge_end_points[std::make_pair(edge_points[edge], edge_points[clockwise_edges[edge]])] = edge;
	for (size_t edge = 0; edge != edge_count; ++edge)
	{
		size_t start_point = edge_points[edge];
		size_t end_point = edge_points[clockwise_edges[edge]];
		first_in_edges[end_point] = edge;
		size_t companion = edge;
		std::map<std::pair<size_t, size_t>, size_t >::iterator it = edge_end_points.find(std::make_pair(end_point, start_point));
		if (it != edge_end_points.end())
		{
			companion = it->second;
		}
		else
		{
			// volume is not closed, user should be notified.
			assert_not_reached();
		}
		companions[edge] = companion;
	}
	
	// Provide an edge-to-face link
	std::vector<size_t> face_for_edge(edge_points.size());
	for (size_t face = 0; face != face_first_loops.size(); ++face)
	{
		for (size_t loop = 0; loop != face_loop_counts[face]; ++loop)
		{
			size_t first_edge = loop_first_edges[face_first_loops[face] + loop];
			for(size_t edge = first_edge; ; )
			{
				face_for_edge[edge] = face;
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
	
	indices_t indices(edge_points.size()); // for indexed items

  for (size_t point = 0; point != points.size(); ++point)
  {
	  Vertex_handle nv = S.new_vertex();
	  Point_3 pvpoint = points[point];
		nv->point() = pvpoint;
		nv->mark() = true;

		SM_decorator SM(&*nv);
		size_t prev_edge = first_in_edges[point];
		size_t edge = prev_edge;
		size_t first_edge = prev_edge;
		Point_3 pe_target_0 = points[edge_points[first_edge]];
		Point_3 sp_point_0(CGAL::ORIGIN+(pe_target_0-pvpoint));
		Sphere_point sp_0(sp_point_0);
		SVertex_handle sv_0 = SM.new_svertex(sp_0);
		sv_0->mark() = true; 
		edge = companions[clockwise_edges[edge]];

		SVertex_handle sv_prev = sv_0;

    do {
      Point_3 pe_target = points[edge_points[edge]]; 
      Point_3 sp_point = CGAL::ORIGIN+(pe_target-pvpoint);
      Sphere_point sp(sp_point);
      SVertex_handle sv = SM.new_svertex(sp);
      sv->mark() = true;
      Plane ss_plane(CGAL::ORIGIN, planes[face_for_edge[prev_edge]].opposite().orthogonal_vector());
      Sphere_circle ss_circle(ss_plane);

      SHalfedge_handle e = SM.new_shalfedge_pair(sv_prev, sv);
      e->circle() = ss_circle;
      e->twin()->circle() = ss_circle.opposite();
      e->mark() = e->twin()->mark() = true;
  
  		indices[prev_edge] = e;
  
      sv_prev = sv;
      prev_edge = edge;
      edge = companions[clockwise_edges[edge]];
    }
    while(edge != first_edge);

    Plane ss_plane(CGAL::ORIGIN, planes[face_for_edge[prev_edge]].opposite().orthogonal_vector());
    Sphere_circle ss_circle(ss_plane);
 
    SHalfedge_handle e = SM.new_shalfedge_pair(sv_prev, sv_0);
    e->circle() = ss_circle;
    e->twin()->circle() = ss_circle.opposite();
    e->mark() = e->twin()->mark() = true;
    
    indices[prev_edge] = e;

    // create faces
    SFace_handle fint = SM.new_sface();
    SFace_handle fext = SM.new_sface();
    SM.link_as_face_cycle(e, fint);
    SM.link_as_face_cycle(e->twin(), fext);
    
    // mark faces properly...
    fint->mark() = true;
    fext->mark() = false;
    SM.check_integrity_and_topological_planarity();   
  }
  
  // resolve indices
//  k3d::mesh::indices_t previous_edges(clockwise_edges.size());
//  for (size_t i = 0; i != clockwise_edges.size(); ++i)
//  {
//  	previous_edges[clockwise_edges[i]] = i;
//  }
  
  for (size_t face = 0; face != face_first_loops.size(); ++face)
	{
  	int se, set;
		for (size_t loop = 0; loop != face_loop_counts[face]; ++loop)
		{
			size_t first_edge = loop_first_edges[face_first_loops[face] + loop];
			if (loop == 0)
			{
				indices[first_edge]->set_index();
		    indices[first_edge]->twin()->set_index();
		    se  = indices[first_edge]->get_index();
		    set = indices[first_edge]->twin()->get_index();
			}
			else
			{
				indices[first_edge]->set_index(se);
		    indices[first_edge]->twin()->set_index(set);
			}
	    indices[first_edge]->twin()->source()->set_index();
	    int sv  = indices[first_edge]->twin()->source()->get_index();
			for(size_t edge = clockwise_edges[first_edge]; ; )
			{
				indices[edge]->set_index(se);
				indices[edge]->twin()->set_index(set);
				indices[edge]->source()->set_index(sv);
				indices[edge]->twin()->source()->set_index();
				sv = indices[edge]->twin()->source()->get_index();				

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
			indices[first_edge]->source()->set_index(sv);
		}
	}
  
}

} // namespace cgal

} // namespace module

#endif // !MODULES_CGAL_K3D_TO_NEF_H
