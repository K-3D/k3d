#ifndef MODULES_CGAL_K3D_TO_NEF_H
#define MODULES_CGAL_K3D_TO_NEF_H

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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Converts a K-3D mesh to a Nef polyhedron. Based on polyhedron_3_to_nef3 from CGAL
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "cgal_simplifying_constructors.h"

#include <CGAL/assertions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Homogeneous.h>
#include <CGAL/Nef_3/SNC_structure.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/normal_vector_newell_3.h>
#include <CGAL/to_rational.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/polyhedron.h>

namespace module
{

namespace cgal
{

/// Using standard integers, with the risk of overflow
/**
 * The purpose of this kernel is to avoid creating points with coordinates that are to big to
 * represent in a 64 bit integer. Unfortunately, even when simplifying the homogeneous coordinates as much
 * as possible, this happens even in the simplest of cases when doing booleans.
 * TODO: See if we can use this with a number type that switches to Gmpz only when int64 overflows
 */
typedef simplifying_kernel<k3d::int64_t> int_kernel;
/// Homogeneous kernel, using the GMP integer of arbitrary length. Robust but slow.
typedef CGAL::Homogeneous<CGAL::Gmpz> exact_kernel;
/// Corresponding Nef polyhedron types
typedef CGAL::Nef_polyhedron_3<int_kernel, CGAL::SNC_indexed_items> int_nef;
typedef CGAL::Nef_polyhedron_3<exact_kernel, CGAL::SNC_indexed_items> exact_nef;

/// Convert coordinates to the format of kernel_t
template<typename kernel_t>
class point_converter
{
public:
	
	typedef typename kernel_t::RT RT;
	typedef typename kernel_t::Point_3 Point_3; 
	
	point_converter() : m_factor(1e10)
	{
	}
	
	const Point_3 operator()(const k3d::point3& Point)
	{
		return Point_3(RT(k3d::round(Point[0]*m_factor)), RT(k3d::round(Point[1]*m_factor)), RT(k3d::round(Point[2]*m_factor)), RT(m_factor));
	}

private:
	/// Factor to multiply by before rounding off 
	const k3d::double_t m_factor;
};

/// Returns the plane containing the face given to operator()
template<typename kernel_t>
class face_plane
{
	typedef typename kernel_t::Point_3 Point_3;
	typedef typename kernel_t::Vector_3 Vector_3;
	typedef typename kernel_t::Plane_3 Plane_3;
	typedef std::vector<Point_3> points_t;
public:
	face_plane(const k3d::polyhedron::const_primitive& Polyhedron, const points_t& Points) : m_polyhedron(Polyhedron), m_points(Points) {}
	
  typename kernel_t::Plane_3 operator()(size_t FirstLoop) {
		
		std::vector<size_t> corners;
		points_t corner_points;
		size_t first_edge = m_polyhedron.loop_first_edges[FirstLoop];
		for(size_t edge = first_edge; ; )
		{
			size_t point = m_polyhedron.edge_points[edge];
			corner_points.push_back(m_points[point]);
			
			edge = m_polyhedron.clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		
		if (corner_points.size() < 3)
			throw std::runtime_error("face_plane: Error determining plane: less than 3 points");
		
		Vector_3 n;
		
		CGAL::normal_vector_newell_3(corner_points.begin(), corner_points.end(), n);
		
		Plane_3 plane(corner_points[0], n);
		if(plane.is_degenerate())
		{
			std::stringstream error_message;
			error_message << "face_plane: Degenerate plane with normal " << n;
			throw std::runtime_error(error_message.str());
		}
		
    return plane;
  }
  
  const k3d::polyhedron::const_primitive& m_polyhedron;
  const points_t& m_points;
};

template<typename nef_t>
void k3d_to_nef(const k3d::mesh::points_t& Points, const k3d::polyhedron::const_primitive& Polyhedron, typename nef_t::SNC_structure& S)
{
	typedef typename nef_t::Kernel Kernel;
	typedef typename nef_t::Plane_3 Plane_3;
	typedef typename nef_t::Vector_3 Vector_3;
	typedef typename nef_t::Point_3 Point_3;
	typedef std::vector<Point_3> points_t;
	
	typedef typename nef_t::SNC_structure							SNC_structure;
  typedef typename SNC_structure::SNC_decorator      SNC_decorator;
  typedef typename SNC_structure::SM_decorator       SM_decorator;
  typedef typename SNC_structure::Vertex_handle      Vertex_handle;
  typedef typename SNC_structure::SVertex_handle     SVertex_handle;
  typedef typename SNC_structure::SHalfedge_handle   SHalfedge_handle;
  typedef typename SNC_structure::SFace_handle       SFace_handle;
  typedef typename SNC_structure::Sphere_point       Sphere_point;
  typedef typename SNC_structure::Sphere_segment     Sphere_segment;
  typedef typename SNC_structure::Sphere_circle      Sphere_circle;
  typedef std::vector<SHalfedge_handle> 		indices_t;

	points_t points;
	
	point_converter<Kernel> converter;
	
	// Convert to Point_3 representation
	for (size_t point = 0; point != Points.size(); ++point)
	{ 
		points.push_back(converter(Points[point]));
	}
	
	std::vector<Plane_3> planes(Polyhedron.face_first_loops.size());
  
	std::transform(Polyhedron.face_first_loops.begin(), Polyhedron.face_first_loops.end(),
		planes.begin(), face_plane<Kernel>(Polyhedron, points));
	
	// Calculate companions
	k3d::mesh::indices_t companions;
	k3d::mesh::bools_t boundary_edges;
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.edge_points, Polyhedron.clockwise_edges, boundary_edges, companions);
	
	// Provide an edge-to-face link
	k3d::mesh::indices_t face_for_edge;
	k3d::polyhedron::create_edge_face_lookup(Polyhedron.face_first_loops, Polyhedron.face_loop_counts, Polyhedron.loop_first_edges, Polyhedron.clockwise_edges, face_for_edge);
	
	// Store an incoming edge for each point
	k3d::mesh::indices_t first_in_edges(points.size());
	size_t edge_count = Polyhedron.edge_points.size();
	for (size_t edge = 0; edge != edge_count; ++edge)
		first_in_edges[Polyhedron.edge_points[Polyhedron.clockwise_edges[edge]]] = edge;
	
	indices_t indices(Polyhedron.edge_points.size()); // for indexed items
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
		Point_3 pe_target_0 = points[Polyhedron.edge_points[first_edge]];
		Point_3 sp_point_0(CGAL::ORIGIN+(pe_target_0-pvpoint));
		Sphere_point sp_0(sp_point_0);
		SVertex_handle sv_0 = SM.new_svertex(sp_0);
		sv_0->mark() = true; 
		edge = companions[Polyhedron.clockwise_edges[edge]];

		SVertex_handle sv_prev = sv_0;

    do {
      Point_3 pe_target = points[Polyhedron.edge_points[edge]]; 
      Point_3 sp_point = CGAL::ORIGIN+(pe_target-pvpoint);
      Sphere_point sp(sp_point);
      SVertex_handle sv = SM.new_svertex(sp);
      sv->mark() = true;
      Plane_3 ss_plane(CGAL::ORIGIN, planes[face_for_edge[prev_edge]].opposite().orthogonal_vector());
      Sphere_circle ss_circle(ss_plane);

      SHalfedge_handle e = SM.new_shalfedge_pair(sv_prev, sv);
      e->circle() = ss_circle;
      e->twin()->circle() = ss_circle.opposite();
      e->mark() = e->twin()->mark() = true;
  
  		indices[prev_edge] = e;
  
      sv_prev = sv;
      prev_edge = edge;
      edge = companions[Polyhedron.clockwise_edges[edge]];
    }
    while(edge != first_edge);

    Plane_3 ss_plane(CGAL::ORIGIN, planes[face_for_edge[prev_edge]].opposite().orthogonal_vector());
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
  for (size_t face = 0; face != Polyhedron.face_first_loops.size(); ++face)
	{
  	int se, set;
		for (size_t loop = 0; loop != Polyhedron.face_loop_counts[face]; ++loop)
		{
			size_t first_edge = Polyhedron.loop_first_edges[Polyhedron.face_first_loops[face] + loop];
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
			for(size_t edge = Polyhedron.clockwise_edges[first_edge]; ; )
			{
				indices[edge]->set_index(se);
				indices[edge]->twin()->set_index(set);
				indices[edge]->source()->set_index(sv);
				indices[edge]->twin()->source()->set_index();
				sv = indices[edge]->twin()->source()->get_index();				

				edge = Polyhedron.clockwise_edges[edge];
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
