// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief 
		\author Ian South-Dickinson (ian.southd@gmail.com)
*/
#ifndef PGP_MESH_INFO_H
#define PGP_MESH_INFO_H

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>
#include <iostream>
#include <map>

namespace libk3dquadremesh
{

namespace detail {
	#include <modules/qslim/MxMath.h>
	typedef k3d::mesh::indices_t indices_t;
	typedef size_t vert_t;
	typedef size_t edge_t;
	typedef size_t face_t;
	typedef size_t poly_t;

	/// TODO
	void calc_edge_face_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);
	/// TODO: Add desc
	void calc_edge_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);

	/// TODO: Add desc
	void calc_edge_companion_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);

	/// TODO: Add desc	
	void calc_vert_edge_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);

	/// TODO: Add desc	
	void calc_vert_edge_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);

	/// TODO: Add desc	
	void calc_edge_vert_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj);

	/// TODO: Add desc
	void calc_face_poly_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) ;


	/// TODO: Add desc
	class mesh_info 
	{
	public:
		mesh_info() 
		{
			num_edges = 0;
			num_faces = 0;
			num_verts = 0;
		}

		mesh_info(const k3d::mesh* Mesh) 
			: mesh(Mesh)
		{
			num_edges = Mesh->polyhedra->edge_points->size();
			num_faces = Mesh->polyhedra->face_first_loops->size();
			num_verts = Mesh->points->size();
			
			edge_comp.resize(num_edges);
			edge_ccw.resize(num_edges);
			edge_face.resize(num_edges);
			
			face_edge.resize(num_faces);
			face_poly.resize(num_faces);
			vert_edge.resize(num_verts);
			edge_vert.resize(num_edges);
			vert_edge_cw.resize(num_verts);
			// face->edge is missing
			for(size_t f = 0; f < num_faces; ++f) {
				face_edge[f] = mesh->polyhedra->loop_first_edges->at(mesh->polyhedra->face_first_loops->at(f));
			}

			calc_edge_face_adj(*mesh->polyhedra, edge_face);
			calc_edge_ccw_adj(*mesh->polyhedra, edge_ccw);
			calc_edge_companion_adj(*mesh->polyhedra, edge_comp);
			calc_vert_edge_adj(*mesh->polyhedra, vert_edge_cw);
			calc_vert_edge_ccw_adj(*mesh->polyhedra, vert_edge);
			calc_edge_vert_ccw_adj(*mesh->polyhedra, edge_vert);
			
			calc_face_poly_adj(*mesh->polyhedra, face_poly);
		}
 
		class Face;
		class Edge;
		class Vert;

		class Vert 
		{
		public:
			Vert(const mesh_info& _info, vert_t _vert = -1) :
				info(_info), index(_vert) {}
			
			Vert& operator=(vert_t vert) 
			{ 
				index = vert; 
				return *this; 
			}

			vert_t operator() () const
			{
				return index;
			}

			Vec3 pos() const
			{
				return Vec3(info.mesh->points->at(index).n);
			}

			Edge edge() const {
				return Edge(info, info.vert_edge[index]);
			}

			const mesh_info& info;
			vert_t index;
		};

		class Face 
		{
		public:
			Face(const mesh_info& _info, face_t _face = -1) :
				info(_info), index(_face) {}
			
			Face& operator=(face_t face) 
			{ 
				index = face; 
				return *this; 
			}

			Edge operator[] (size_t e) const
			{
				edge_t edge = info.mesh->polyhedra->loop_first_edges->at(
					info.mesh->polyhedra->face_first_loops->at(index));
				
				for(int i = 0; i < e; i++) {
					edge = info.edge_ccw[edge]; 
				}

				return Edge(info, edge);
			}
			
			face_t operator() () const
			{
				return index;
			}

			const mesh_info& info;
			face_t index;
		};

		class Edge 
		{
		public:
			Edge(const mesh_info& _info, edge_t _edge = -1) :
				info(_info), index(_edge) {}
			
			Edge(const Edge& e) :
				info(e.info), index(e.index) {}

			//Edge& operator=(edge_t edge) 
			//{ 
			//	index = edge; 
			//	return *this; 
			//}
			
			Edge& operator=(const Edge& edge) 
			{ 
				index = edge.index; 
				return *this; 
			}
	
			Edge next() const 
			{
				return Edge(info, info.edge_ccw[index]);
			}

			Edge prev() const 
			{
				return Edge(info, info.mesh->polyhedra->clockwise_edges->at(index));
			}

			Face face() const 
			{
				return Face(info, info.edge_face[index]);
			}

			Vert vert() const 
			{
				return Vert(info, info.edge_vert[index]);
			}

			Edge comp() const
			{
				return Edge(info, info.edge_comp[index]);
			}

			edge_t operator() () const
			{
				return index;
			}

			Vec3 start() const
			{
				return  Vec3(info.mesh->points->at(info.edge_vert[index]).n);
			}

			Vec3 end() const
			{
				return  Vec3(info.mesh->points->at(info.edge_vert[info.edge_ccw[index]]).n);
			}	

			Vec3 dir() const
			{
				return end()-start();
			}	

		private:
			const mesh_info& info;
			edge_t index;
		};



		size_t num_edges;
		size_t num_faces;
		size_t num_verts;

		Face getFace(face_t face) const
		{
			// error checking?
			return Face(*this, face);
		}		
		Edge getEdge(edge_t edge) const
		{
			// error checking?
			return Edge(*this, edge);
		}

		Vert getVert(vert_t vert) const
		{
			// error checking?
			return Vert(*this, vert);
		}

		const k3d::mesh* mesh;

		indices_t edge_comp;
		indices_t edge_face;
		indices_t edge_vert;
		indices_t edge_ccw;		

		indices_t face_edge;
		indices_t face_poly;
		indices_t vert_edge;
		indices_t poly_face;
		indices_t vert_edge_cw;
	};
};
}; // namespace pgp_module
#endif
