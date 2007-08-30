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

#include "conversion.h"
#include "k3d_to_nef.h"

#include <map>

namespace libk3dbooleans
{

typedef std::map<const Polyhedron::Vertex*, size_t> point_map_t;

// helper function to add points
size_t get_point(boost::shared_ptr<k3d::mesh::points_t>& Points, point_map_t& PointMap, Polyhedron::Halfedge::Vertex_const_handle Vertex)
{
	std::pair<point_map_t::iterator, bool> insert_result = PointMap.insert(std::make_pair(&(*Vertex), Points->size()));
	if (insert_result.second)
	{
		Point_3 mesh_point = Vertex->point();
		Points->push_back(k3d::point3(CGAL::to_double(mesh_point.x()), CGAL::to_double(mesh_point.y()), CGAL::to_double(mesh_point.z()))); 
	}
	return insert_result.first->second;
}

void to_mesh(const Polyhedron& Polyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material)
{
  boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t());
  boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
	boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t());
	boost::shared_ptr<k3d::mesh::indices_t> face_first_loops(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::selection_t> face_selection(new k3d::mesh::selection_t());
	boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> edge_points(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::selection_t> edge_selection(new k3d::mesh::selection_t());
  point_map_t point_map;
  
  types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
  
  for (Polyhedron::Facet_const_iterator f = Polyhedron.facets_begin(); f != Polyhedron.facets_end(); ++f)
  {
  	size_t face_first_loop = loop_first_edges->size();
  	const size_t first_edge = edge_points->size();
  	loop_first_edges->push_back(first_edge);
  	
  	Polyhedron::Facet::Halfedge_around_facet_const_circulator facet_start = f->facet_begin();
  	Polyhedron::Facet::Halfedge_around_facet_const_circulator edge_circulator = facet_start;
  	do
  	{
  		const Polyhedron::Halfedge& halfedge = *edge_circulator;
			edge_points->push_back(get_point(points, point_map, halfedge.prev()->vertex()));
			clockwise_edges->push_back(edge_points->size());
			edge_selection->push_back(0.0);
  		++edge_circulator;
  	} while (facet_start != edge_circulator); 
  	clockwise_edges->back() = first_edge;
  	
  	face_first_loops->push_back(face_first_loop);
		face_loop_counts->push_back(1);
		face_selection->push_back(0.0);
  }
  
  boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t(points->size()));
  
  first_faces->push_back(0);
	face_counts->push_back(face_first_loops->size());
	boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t(face_first_loops->size(), Material));
	
  polyhedra->edge_selection = edge_selection;
	polyhedra->clockwise_edges = clockwise_edges;
	polyhedra->edge_points = edge_points;
	polyhedra->loop_first_edges = loop_first_edges;
	polyhedra->face_materials = face_materials;
	polyhedra->face_selection = face_selection;
	polyhedra->face_loop_counts = face_loop_counts;
	polyhedra->face_first_loops = face_first_loops;
	polyhedra->types = types;
	polyhedra->face_counts = face_counts;
	polyhedra->first_faces = first_faces;
  Mesh.points = points;
  Mesh.point_selection = point_selection;
  Mesh.polyhedra = polyhedra;
}

// helper types and functions for to_mesh
typedef std::map<const Nef_polyhedron::Vertex*, size_t> vertices_t;
typedef std::pair<size_t, size_t> edge_t;
typedef std::vector<edge_t> edges_t;
typedef std::vector<edges_t> edges_per_facet_t;
typedef std::map<const Nef_polyhedron::Halffacet*, size_t> facet_map_t;

size_t get_vertex_number(const Nef_polyhedron::Vertex* Vertex, vertices_t& Vertices, boost::shared_ptr<k3d::mesh::points_t>& Points)
{
	std::pair<vertices_t::iterator, bool> insert_result = Vertices.insert(std::make_pair(Vertex, Points->size()));
	if (insert_result.second)
	{
		Point_3 mesh_point = Vertex->point();
		Points->push_back(k3d::point3(CGAL::to_double(mesh_point.x()), CGAL::to_double(mesh_point.y()), CGAL::to_double(mesh_point.z()))); 
	}
	return insert_result.first->second;
}

void to_mesh(Nef_polyhedron& NefPolyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material)
{
	boost::shared_ptr<k3d::mesh::points_t> points(new k3d::mesh::points_t());
  boost::shared_ptr<k3d::mesh::polyhedra_t> polyhedra(new k3d::mesh::polyhedra_t());
	boost::shared_ptr<k3d::mesh::indices_t> first_faces(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::polyhedra_t::types_t> types(new k3d::mesh::polyhedra_t::types_t());
	boost::shared_ptr<k3d::mesh::indices_t> face_first_loops(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::counts_t> face_loop_counts(new k3d::mesh::counts_t());
	boost::shared_ptr<k3d::mesh::selection_t> face_selection(new k3d::mesh::selection_t());
	boost::shared_ptr<k3d::mesh::indices_t> loop_first_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> edge_points(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::indices_t> clockwise_edges(new k3d::mesh::indices_t());
	boost::shared_ptr<k3d::mesh::selection_t> edge_selection(new k3d::mesh::selection_t());
  
  types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
  
  k3d::log() << debug << "Looping over " << NefPolyhedron.number_of_halfedges() << " halfedges" << std::endl;
  
  vertices_t vertices;
  facet_map_t facet_map;
  
  // Assign a number to each facet, so the facet order does not depend on the facet's memory address
  size_t num_facets = 0;
  for (Nef_polyhedron::Halffacet_const_iterator facet = NefPolyhedron.halffacets_begin(); facet != NefPolyhedron.halffacets_end(); ++facet)
  {
  	if (&(*facet->incident_volume()) && facet->incident_volume()->mark()) // Only consider facets that enclose the marked volume
  	{
  		facet_map.insert(std::make_pair(&(*facet), num_facets)); 
  		num_facets++;
  	}
  }
  
  k3d::log() << debug << "Detected " << num_facets << " facets" << std::endl;
  
  edges_per_facet_t edges_per_facet(num_facets);
  
  for (Nef_polyhedron::Halfedge_const_iterator e = NefPolyhedron.halfedges_begin(); e != NefPolyhedron.halfedges_end(); ++e)
  {
		const Nef_polyhedron::Vertex* start = &(*e->source());
		const Nef_polyhedron::Vertex* end = &(*e->target());
		
		edge_t edge = std::make_pair(get_vertex_number(start, vertices, points), get_vertex_number(end, vertices, points));
		
		// Every halfedge (=svertex) has two outgoing sedges, one of which corresponds to a facet of the enclosed volume (mark==true)
		if (e->out_sedge()->facet()->incident_volume()->mark()) // First outgoing sedge
		{
			edges_per_facet[facet_map[&(*e->out_sedge()->facet())]].push_back(edge);
		}
		else // second outgoing sedge
		{
			return_if_fail(e->out_sedge()->sprev()->twin()->facet()->incident_volume()->mark());
			edges_per_facet[facet_map[&(*e->out_sedge()->sprev()->twin()->facet())]].push_back(edge);
		}
  }
  
  for (size_t facet = 0; facet != edges_per_facet.size(); ++facet)
  {
  	edges_t edges = edges_per_facet[facet];
//  	k3d::log() << "cycles for facet " << facet << std::endl;
//  	for (edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
//  		k3d::log() << debug << "    " << edge->first << "-" << edge->second << std::endl;

		// First, we need to sort the un-ordered edge list that is stored for each facet so we get the actual edge loops
  	std::vector<edges_t> cycles;
  	while (!edges.empty())
  	{
  		edges_t cycle;
  		cycle.push_back(edges.front());
  		edges.erase(edges.begin());
  		while (cycle.back().second != cycle.front().first && !edges.empty())
  		{
	  		for (edges_t::iterator edge = edges.begin(); edge != edges.end(); )
	  		{
	  			if (edge->first == cycle.back().second && edge->second != cycle.back().first)
	  			{
	  				cycle.push_back(*edge);
	  				edge = edges.erase(edge);
	  			}
	  			else
	  			{
	  				++edge;
	  			}
	  		}
  		}
  		if (cycle.size() > 2 && cycle.back().second == cycle.front().first)
  			cycles.push_back(cycle);
  	}
  	
  	size_t face_first_loop = loop_first_edges->size();
  	
  	for (size_t i = 0; i != cycles.size(); ++i)
  	{
	  	const size_t first_edge = edge_points->size();
	  	loop_first_edges->push_back(first_edge);
  		edges_t cycle = cycles[i];
  		for (edges_t::iterator edge = cycle.begin(); edge != cycle.end(); ++edge)
  		{
  			edge_points->push_back(edge->first);
  			clockwise_edges->push_back(edge_points->size());
  			edge_selection->push_back(0.0);
  		}
  		clockwise_edges->back() = first_edge; 
  	}
  	
   	face_first_loops->push_back(face_first_loop);
		face_loop_counts->push_back(cycles.size());
		face_selection->push_back(0.0);   
  }
  
  boost::shared_ptr<k3d::mesh::selection_t> point_selection(new k3d::mesh::selection_t(points->size()));
  
  first_faces->push_back(0);
	face_counts->push_back(face_first_loops->size());
	boost::shared_ptr<k3d::mesh::materials_t> face_materials(new k3d::mesh::materials_t(face_first_loops->size(), Material));
	
  polyhedra->edge_selection = edge_selection;
	polyhedra->clockwise_edges = clockwise_edges;
	polyhedra->edge_points = edge_points;
	polyhedra->loop_first_edges = loop_first_edges;
	polyhedra->face_materials = face_materials;
	polyhedra->face_selection = face_selection;
	polyhedra->face_loop_counts = face_loop_counts;
	polyhedra->face_first_loops = face_first_loops;
	polyhedra->types = types;
	polyhedra->face_counts = face_counts;
	polyhedra->first_faces = first_faces;
  Mesh.points = points;
  Mesh.point_selection = point_selection;
  Mesh.polyhedra = polyhedra;
}

boost::shared_ptr<Nef_polyhedron> to_nef(const k3d::mesh& Mesh, const k3d::matrix4& Matrix)
{
	SNC_structure snc;
	k3d_to_nef(Mesh, Matrix, snc);
	boost::shared_ptr<Nef_polyhedron> nef(new Nef_polyhedron(snc));
	nef->build_external_structure();
	nef->simplify();
	return nef;
}

void k3d_failure_handler(const char *type, const char *expr, const char* file, int line, const char* msg)
{
  k3d::log() << error << "CGAL " << type << ": " << expr << " at " << file << ":" << line << ". " << msg << std::endl;
}

}
