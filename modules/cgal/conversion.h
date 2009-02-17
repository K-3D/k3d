#ifndef MODULES_CGAL_CONVERSION_H
#define MODULES_CGAL_CONVERSION_H

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
		\brief Conversion between CGAL and K-3D formats
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "k3d_to_nef.h"

// For debug off output
#include <CGAL/IO/Nef_polyhedron_iostream_3.h>

#include <k3dsdk/imaterial.h>

#include <boost/scoped_ptr.hpp>

#include <map>

namespace module
{

namespace cgal
{

/// Visitor class to help building the k3d mesh
template<typename nef_t>
class nef_visitor
{
	typedef typename nef_t::Vertex_const_iterator Vertex_const_iterator;
	typedef typename nef_t::Halffacet_const_handle Halffacet_const_handle;
	typedef typename nef_t::SHalfedge_const_handle SHalfedge_const_handle;
	typedef typename nef_t::Halffacet_cycle_const_iterator Halffacet_cycle_const_iterator;
	typedef typename nef_t::SHalfedge_around_facet_const_circulator SHalfedge_around_facet_const_circulator;
public:
	nef_visitor(k3d::polyhedron::primitive& Polyhedron, const CGAL::Object_index<Vertex_const_iterator>& VertexIndices)
	: m_polyhedron(Polyhedron), m_vertex_indices(VertexIndices), m_edge(0)
	{
		m_polyhedron.first_faces.push_back(0);
		m_polyhedron.face_counts.push_back(0);
		m_polyhedron.polyhedron_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);
	}
	
	void visit(Halffacet_const_handle OppositeFacet)
	{
		return_if_fail(nef_t::Infi_box::is_standard(OppositeFacet->plane()));
		
		SHalfedge_const_handle se;
		Halffacet_cycle_const_iterator fc;
		     	
		Halffacet_const_handle f = OppositeFacet->incident_volume()->mark() == 1 ? OppositeFacet->twin() : OppositeFacet;
		
		fc = f->facet_cycles_begin();
		se = SHalfedge_const_handle(fc);
		return_if_fail(se!=0);
		SHalfedge_around_facet_const_circulator hc_start(se);
		SHalfedge_around_facet_const_circulator hc_end(hc_start);
		const k3d::uint_t face_first_edge = m_polyhedron.edge_points.size();
		CGAL_For_all(hc_start,hc_end)
		{
			m_polyhedron.edge_points.push_back(m_vertex_indices[hc_start->source()->center_vertex()]);
			m_polyhedron.clockwise_edges.push_back(m_polyhedron.edge_points.size());
			m_polyhedron.edge_selections.push_back(0.0);
		}
		m_polyhedron.clockwise_edges.back() = face_first_edge;
		m_polyhedron.face_first_loops.push_back(m_polyhedron.loop_first_edges.size());
		m_polyhedron.loop_first_edges.push_back(face_first_edge);
		m_polyhedron.face_loop_counts.push_back(1);
		m_polyhedron.face_materials.push_back(static_cast<k3d::imaterial*>(0));
		m_polyhedron.face_selections.push_back(0.0);
		++m_polyhedron.face_counts.back();
		++fc;
		CGAL_For_all(fc, f->facet_cycles_end())
		{
			se = SHalfedge_const_handle(fc);
			return_if_fail(se!=0);
			SHalfedge_around_facet_const_circulator hole_start(se);
			SHalfedge_around_facet_const_circulator hole_end(hole_start);
			const k3d::uint_t hole_first_edge = m_polyhedron.edge_points.size();
			CGAL_For_all(hole_start,hole_end)
			{
				m_polyhedron.edge_points.push_back(m_vertex_indices[hole_start->source()->center_vertex()]);
				m_polyhedron.clockwise_edges.push_back(m_polyhedron.edge_points.size());
				m_polyhedron.edge_selections.push_back(0.0);
			}
			m_polyhedron.clockwise_edges.back() = hole_first_edge;
			m_polyhedron.loop_first_edges.push_back(hole_first_edge);
			++m_polyhedron.face_loop_counts.back();
		}
	}
	
	void visit(typename nef_t::SFace_const_handle) {}
  void visit(typename nef_t::Halfedge_const_handle edge) {}
  void visit(typename nef_t::Vertex_const_handle) {}
  void visit(SHalfedge_const_handle) {}
  void visit(typename nef_t::SHalfloop_const_handle) {}
	
private:
	k3d::polyhedron::primitive& m_polyhedron;
	const CGAL::Object_index<Vertex_const_iterator>& m_vertex_indices;
	k3d::uint_t m_edge;
};

/// Converts a Nef_polyhedron to a k3d mesh
template<typename nef_t>
void to_mesh(nef_t& NefPolyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material)
{
	typedef typename nef_t::Vertex_const_iterator Vertex_const_iterator;
	typedef typename nef_t::Point_3 Point_3;
	typedef typename nef_t::Volume_const_handle Volume_const_handle;
	typedef typename nef_t::SFace_const_handle SFace_const_handle;
	
	boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Mesh));
	k3d::mesh::points_t& points = Mesh.points.create();
	k3d::mesh::selection_t& point_selection = Mesh.point_selection.create();
	
	k3d::int32_t skip_volumes = nef_t::Infi_box::extended_kernel() ? 2 : 1;
	
	//k3d::log() << debug << "--------------- BEGIN OUTPUT NEF ----------------" << std::endl;
	//k3d::log() << NefPolyhedron << std::endl;
	//k3d::log() << debug << "---------------- END OUTPUT NEF -----------------" << std::endl;
	
	Vertex_const_iterator v;
	CGAL::Object_index<Vertex_const_iterator> vertex_indices; 
	CGAL_forall_vertices(v, NefPolyhedron)
	{
		if(nef_t::Infi_box::is_standard(v->point()))
		{
			Point_3 mesh_point = v->point();
			points.push_back(k3d::point3(CGAL::to_double(mesh_point.x()), CGAL::to_double(mesh_point.y()), CGAL::to_double(mesh_point.z())));
			point_selection.push_back(0.0);
			vertex_indices[v] = points.size() - 1;
		}
	}
	
	nef_visitor<nef_t> V(*polyhedron, vertex_indices);
	Volume_const_handle c;
	CGAL_forall_volumes(c, NefPolyhedron)
	{
		if(skip_volumes-- <= 0)
		{
			NefPolyhedron.visit_shell_objects(SFace_const_handle(c->shells_begin()),V);
		}
	}
}

/// Converts a k3d mesh to a Nef polyhedron
/**
 * Note: nef_t to mesh conversion based on the to_Polyhedron_3 conversion from Nef_Polyhedron_3.h in the CGAL distribution,
 * so part is (c) the authors of that file.
 */
template<typename nef_t>
boost::shared_ptr<nef_t> to_nef(const k3d::mesh& Mesh)
{
	typedef typename nef_t::SNC_structure SNC_structure;
	SNC_structure snc;
	try
	{
		k3d_to_nef<nef_t>(Mesh, snc);
	}
	catch(std::exception& E)
	{
		k3d::log() << error << "CGAL: Failed to convert to Nef: " << E.what() << std::endl;
		return boost::shared_ptr<nef_t>();
	}
	boost::shared_ptr<nef_t> nef(new nef_t(snc));
	nef->build_external_structure();
	nef->simplify();
	//k3d::log() << debug << "--------------- BEGIN INPUT NEF ----------------" << std::endl;
	//k3d::log() << *nef << std::endl;
	//k3d::log() << debug << "---------------- END INPUT NEF -----------------" << std::endl;
	return nef;
}

} // namespace cgal

} // namespace module

#endif // !MODULES_CGAL_CONVERSION_H
