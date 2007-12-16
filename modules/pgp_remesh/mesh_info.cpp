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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>
#include <iostream>
#include <map>
#include "mesh_info.h"
 
namespace libk3dquadremesh
{

namespace detail {

	void calc_edge_face_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		for(size_t i = 0; i < Polyhedra.face_first_loops->size(); ++i) {
			size_t edge = Polyhedra.loop_first_edges->at(Polyhedra.face_first_loops->at(i));
			size_t first = edge;
			do {
				adj[edge] = i;				
				edge = Polyhedra.clockwise_edges->at(edge);
			} while( edge != first );
		}
		// error checking?
	}		

	/// TODO: Add desc
	void calc_edge_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		for(size_t i = 0; i < Polyhedra.loop_first_edges->size(); ++i) {
			size_t edge = Polyhedra.loop_first_edges->at(i);
			size_t first = edge;
			do {
				adj[Polyhedra.clockwise_edges->at(edge)] = edge;				
				edge = Polyhedra.clockwise_edges->at(edge);
			} while( edge != first );
		}
		// error checking?
	}		

	/// TODO: Add desc
	void calc_edge_companion_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		std::map<std::pair<size_t, size_t>, size_t> segments;

		for(size_t i = 0; i < Polyhedra.edge_points->size(); ++i) {
			size_t v0 = Polyhedra.edge_points->at(i);
			size_t v1 = Polyhedra.edge_points->at(Polyhedra.clockwise_edges->at(i));

			segments[std::pair<size_t, size_t>(v0,v1)] = i;
		}

		for(size_t i = 0; i < Polyhedra.edge_points->size(); ++i) {
			size_t v0 = Polyhedra.edge_points->at(i);
			size_t v1 = Polyhedra.edge_points->at(Polyhedra.clockwise_edges->at(i));

			size_t comp = segments[std::pair<size_t, size_t>(v1,v0)];

			adj[i] = comp;
		}
		// assert correctness
	}

	/// TODO: Add desc	
	void calc_vert_edge_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		for(size_t i = 0; i < Polyhedra.edge_points->size(); ++i) {
			size_t vert = Polyhedra.edge_points->at(i);
			adj.at(vert) = i;
		}
	}

	/// TODO: Add desc	
	void calc_vert_edge_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		indices_t comp;
		comp.resize(Polyhedra.edge_points->size());

		calc_edge_companion_adj(Polyhedra, comp);
		for(size_t i = 0; i < Polyhedra.edge_points->size(); ++i) {
			size_t vert = Polyhedra.edge_points->at(comp.at(i));
			adj.at(vert) = i;
		}
	}

	/// TODO: Add desc	
	void calc_edge_vert_ccw_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		for(size_t i = 0; i < Polyhedra.edge_points->size(); ++i) {
			adj[i] = Polyhedra.edge_points->at(Polyhedra.clockwise_edges->at(i));
		}
	}

	/// TODO: Add desc
	void calc_face_poly_adj(const k3d::mesh::polyhedra_t& Polyhedra, indices_t& adj) 
	{
		indices_t comp;
		indices_t edge_face;
		
		comp.resize(Polyhedra.edge_points->size());
		edge_face.resize(Polyhedra.edge_points->size());

		calc_edge_companion_adj(Polyhedra, comp);
		calc_edge_face_adj(Polyhedra, edge_face);

		size_t empty = 0xFFFFFFFF;
		
		std::fill(adj.begin(), adj.end(), empty);
		std::vector<size_t> stack;

		// Depth first traversal of faces in polyhedra to label all the faces
		for(size_t i = 0; i < Polyhedra.first_faces->size(); ++i) {
			stack.push_back(Polyhedra.first_faces->at(i));
			while(!stack.empty()) {
				size_t f = stack.back();
				stack.pop_back();
				if(adj[f] != empty) {
					adj[f] = i;
					size_t edge = Polyhedra.loop_first_edges->at(Polyhedra.face_first_loops->at(f));
					size_t first = edge;

					do {
						stack.push_back(edge_face[comp[edge]]);
						edge = Polyhedra.clockwise_edges->at(edge);
					} while( edge != first );
				}
			}
		}
	}
};
}; // namespace pgp_module
