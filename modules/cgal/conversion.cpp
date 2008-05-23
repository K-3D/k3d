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

#include <k3dsdk/gprim_factory.h>

#include <map>

namespace module
{

namespace cgal
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

// Note: Nef_polyhedron to mesh conversion based on the to_Polyhedron_3 conversion from Nef_polyhedron_3.h in the CGAL distribution,
// so part is (c) the authors of that file.

/// Visitor class to help building the k3d mesh
class nef_visitor
{
public:
	nef_visitor(k3d::gprim_factory& Factory, const CGAL::Object_index<Nef_polyhedron::Vertex_const_iterator>& VertexIndices)
	: m_factory(Factory), m_vertex_indices(VertexIndices) {}
	
	void visit(Nef_polyhedron::Halffacet_const_handle OppositeFacet)
	{
		return_if_fail(Nef_polyhedron::Infi_box::is_standard(OppositeFacet->plane()));
		
		Nef_polyhedron::SHalfedge_const_handle se;
		Nef_polyhedron::Halffacet_cycle_const_iterator fc;
		     	
		Nef_polyhedron::Halffacet_const_handle f = OppositeFacet->twin();
		
		k3d::mesh::indices_t corners;
		fc = f->facet_cycles_begin();
		se = Nef_polyhedron::SHalfedge_const_handle(fc);
		return_if_fail(se!=0);
		Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_start(se);
		Nef_polyhedron::SHalfedge_around_facet_const_circulator hc_end(hc_start);
		CGAL_For_all(hc_start,hc_end)
		{
			corners.push_back(m_vertex_indices[hc_start->source()->center_vertex()]);
		}
		m_factory.add_polygon(corners);
		++fc;
		CGAL_For_all(fc, f->facet_cycles_end())
		{
			k3d::mesh::indices_t hole_corners;
			se = Nef_polyhedron::SHalfedge_const_handle(fc);
			return_if_fail(se!=0);
			Nef_polyhedron::SHalfedge_around_facet_const_circulator hole_start(se);
			Nef_polyhedron::SHalfedge_around_facet_const_circulator hole_end(hole_start);
			CGAL_For_all(hole_start,hole_end)
			{
				hole_corners.push_back(m_vertex_indices[hole_start->source()->center_vertex()]);
			}
			m_factory.add_hole(hole_corners);
		}
	}
	
	void visit(Nef_polyhedron::SFace_const_handle) {}
  void visit(Nef_polyhedron::Halfedge_const_handle) {}
  void visit(Nef_polyhedron::Vertex_const_handle) {}
  void visit(Nef_polyhedron::SHalfedge_const_handle) {}
  void visit(Nef_polyhedron::SHalfloop_const_handle) {}
	
private:
	k3d::gprim_factory& m_factory;
	const CGAL::Object_index<Nef_polyhedron::Vertex_const_iterator>& m_vertex_indices;
};

void to_mesh(Nef_polyhedron& NefPolyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material)
{ 
	k3d::gprim_factory factory(Mesh);
	k3d::int32_t skip_volumes = Nef_polyhedron::Infi_box::extended_kernel() ? 2 : 1;
	
	Nef_polyhedron::Vertex_const_iterator v;
	CGAL::Object_index<Nef_polyhedron::Vertex_const_iterator> vertex_indices; 
	CGAL_forall_vertices(v, NefPolyhedron)
	{
		if(Nef_polyhedron::Infi_box::is_standard(v->point()))
		{
			Point_3 mesh_point = v->point();
			factory.add_point(k3d::point3(CGAL::to_double(mesh_point.x()), CGAL::to_double(mesh_point.y()), CGAL::to_double(mesh_point.z())));
			vertex_indices[v] = Mesh.points->size() - 1;
		}
	}
	
	nef_visitor V(factory, vertex_indices);
	Nef_polyhedron::Volume_const_handle c;
	CGAL_forall_volumes(c, NefPolyhedron)
	{
		if(skip_volumes-- <= 0)
		  NefPolyhedron.visit_shell_objects(Nef_polyhedron::SFace_const_handle(c->shells_begin()),V);
	}
}

boost::shared_ptr<Nef_polyhedron> to_nef(const k3d::mesh& Mesh)
{
	SNC_structure snc;
	try
	{
		k3d_to_nef(Mesh, snc);
	}
	catch(...)
	{
		k3d::log() << error << "Failed to convert to Nef" << std::endl;
		return boost::shared_ptr<Nef_polyhedron>();
	}
	boost::shared_ptr<Nef_polyhedron> nef(new Nef_polyhedron(snc));
	nef->build_external_structure();
	nef->simplify();
	return nef;
}

void k3d_failure_handler(const char *type, const char *expr, const char* file, int line, const char* msg)
{
  k3d::log() << error << "CGAL " << type << ": " << expr << " at " << file << ":" << line << ". " << msg << std::endl;
}

} // namespace cgal

} // namespace module
