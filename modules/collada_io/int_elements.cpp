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
	\author Barbiero Mattia
*/
#include <dae.h>
#include <dae/daeDom.h>
#include "int_elements.h"
#include <iostream>
#include <dom/domPolylist.h>
#include <dom/domTriangles.h>

#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

using namespace std;
namespace module
{

namespace collada
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader_implementation

	intGeometry::intGeometry(domGeometry& geomElement, const k3d::matrix4& ccst)
	{
		//k3d::log() << debug << "una geom" << std::endl;
		domMesh *meshElement = geomElement.getMesh();

		k3d::mesh::points_t& points = Mesh.points.create();
		k3d::mesh::texture_coordinates_t texture_coordinates;
		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Mesh));
		polyhedron->first_faces.push_back(0);
		polyhedron->face_counts.push_back(0);
		polyhedron->polyhedron_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);

		domInputLocalOffset* vertex_input;
		domInputLocalOffset* normal_input;
		domInputLocalOffset* texcoord_input;

		for(int i=0; i<meshElement->getPolylist_array().getCount()>0; i++)
		{
			texcoord_input = NULL;
			domPolylist *polygons = meshElement->getPolylist_array()[i];

			// polyCount stores how many polygons there are
			int polyCount = 0;
			// inputArraysCount stores how many polygon arrays are
			int inputArraysCount = 0;
	
			// offset for the mesh declaration input string, 
			// vertex, normal and texture values are stored in a single array
			// if one of them is not present it is not taken into account
			int v_offset = -1;
			int n_offset = -1;
			int t_offset = -1;

			int max_offset = 0;

			polyCount = polygons->getCount();
			inputArraysCount = polygons->getInput_array().getCount();

			// Routine to declare the offsets if these attributes are present
			// only VERTEX attributes are obligatory
			for (int i=0; i<inputArraysCount; i++)
			{
				if(strcmp(polygons->getInput_array()[i]->getSemantic(),"VERTEX")==0){
					v_offset = polygons->getInput_array()[i]->getOffset();
					vertex_input = polygons->getInput_array()[i];
				}else
				if(strcmp(polygons->getInput_array()[i]->getSemantic(),"NORMAL")==0){
					n_offset = polygons->getInput_array()[i]->getOffset();
					normal_input = polygons->getInput_array()[i];
				}else
				if(strcmp(polygons->getInput_array()[i]->getSemantic(),"TEXCOORD")==0){
					t_offset = polygons->getInput_array()[i]->getOffset();
					texcoord_input = polygons->getInput_array()[i];
				}
				if(max_offset< polygons->getInput_array()[i]->getOffset())
					max_offset = polygons->getInput_array()[i]->getOffset();
			}

			domSource *source_vertex;
			domSource *source_normal;
			domSource *source_texcoord=NULL;
			daeURI *elementURI;

			domVertices* domV;
			domV = daeSafeCast<domVertices>(vertex_input->getSource().getElement());
	
			source_vertex = daeSafeCast<domSource>(domV->getInput_array()[0]->getSource().getElement());
	
			if(texcoord_input)
			{
				elementURI = &texcoord_input->getSource();
				source_texcoord = daeSafeCast<domSource>(elementURI->getElement());
			}

			domFloat_array *floatArray_vertex;
			domFloat_array *floatArray_normal;
			domFloat_array *floatArray_texcoord;

			if(source_vertex)
				floatArray_vertex = &source_vertex->getFloat_array()[0];

			if(source_texcoord)
				floatArray_texcoord = &source_texcoord->getFloat_array()[0];

			int stride = source_vertex->getTechnique_common()->getAccessor()->getStride();

			// Assume there are 3 values per vertex with a stride of 3.
			// Copy the vertices into my structure one-by-one
			// (converts from COLLADA's doubles to floats).

			for ( unsigned int i = 0; i < floatArray_vertex->getCount(); i+=stride )
			{
				k3d::point3 vertex(floatArray_vertex->getValue()[i],
						floatArray_vertex->getValue()[i+1],
						floatArray_vertex->getValue()[i+2]);
				points.push_back(ccst*vertex);
			}

			if(source_texcoord)
			{
				stride = source_texcoord->getTechnique_common()->getAccessor()->getStride();
				for ( unsigned int i = 0; i < floatArray_texcoord->getCount(); i+=stride)
				{
					k3d::texture3 texture(floatArray_texcoord->getValue()[i],
									1-floatArray_texcoord->getValue()[i+1],
									1);
					texture_coordinates.push_back(texture);
				}
			}

			// Stores the list of polygons
			domP *poly = polygons->getP();

			max_offset++;
	
			int tot = 0;
	
			// Add polygons into gprim_factory taking into account offsets to parse correctly
			for (int i=0;i<polyCount;i++)
			{
				k3d::mesh::indices_t vertex_coordinates;
				k3d::mesh::indices_t texture_coordinates;
				k3d::mesh::indices_t normal_coordinates;
		
				// Copy all the indices from the domP into my structure.
				int vcount = polygons->getVcount()->getValue()[i];
				const k3d::uint_t face_first_edge = polyhedron->edge_points.size();
				for(int v=0;v<vcount*max_offset;v+=max_offset)
				{
					if(v_offset!=-1)
					{
						polyhedron->edge_points.push_back(poly->getValue()[tot+v+v_offset]);
						polyhedron->clockwise_edges.push_back(polyhedron->edge_points.size());
						polyhedron->edge_selections.push_back(0.0);
					}
					if(t_offset!=-1)
						texture_coordinates.push_back(poly->getValue()[tot+v+t_offset]);
				}
				tot+=vcount*max_offset;
				polyhedron->clockwise_edges.back() = face_first_edge;
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->loop_first_edges.push_back(face_first_edge);
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_materials.push_back(static_cast<k3d::imaterial*>(0));
				polyhedron->face_selections.push_back(0.0);
				++polyhedron->face_counts.back();
			}
		}

		for(int i=0; i<meshElement->getTriangles_array().getCount()>0; i++)
		{
			texcoord_input = NULL;
			domTriangles *triangles = meshElement->getTriangles_array()[i];

			// polyCount stores how many polygons there are
			int polyCount = 0;
			// inputArraysCount stores how many polygon arrays are
			int inputArraysCount = 0;
	
			// offset for the mesh declaration input string, 
			// vertex, normal and texture values are stored in a single array
			// if one of them is not present it is not taken into account
			int v_offset = -1;
			int n_offset = -1;
			int t_offset = -1;

			int max_offset = 0;
	
			domP *poly;

			polyCount = triangles->getCount();
			inputArraysCount = triangles->getInput_array().getCount();

			// Routine to declare the offsets if these attributes are present
			// only VERTEX attributes are obligatory
			for (int i=0; i<inputArraysCount; i++)
			{
				if(strcmp(triangles->getInput_array()[i]->getSemantic(),"VERTEX")==0){
					v_offset = triangles->getInput_array()[i]->getOffset();
					vertex_input = triangles->getInput_array()[i];
				}else
				if(strcmp(triangles->getInput_array()[i]->getSemantic(),"NORMAL")==0){
					n_offset = triangles->getInput_array()[i]->getOffset();
					normal_input = triangles->getInput_array()[i];
				}else
				if(strcmp(triangles->getInput_array()[i]->getSemantic(),"TEXCOORD")==0){
					t_offset = triangles->getInput_array()[i]->getOffset();
					texcoord_input = triangles->getInput_array()[i];
				}
				if(max_offset< triangles->getInput_array()[i]->getOffset())
					max_offset = triangles->getInput_array()[i]->getOffset();
			}

			domSource *source_vertex;
			domSource *source_normal;
			domSource *source_texcoord=NULL;
			daeURI *elementURI;
		
			
			domVertices* domV;
			domV = daeSafeCast<domVertices>(vertex_input->getSource().getElement());
	
			source_vertex = daeSafeCast<domSource>(domV->getInput_array()[0]->getSource().getElement());
	
			if(texcoord_input)
			{
				elementURI = &texcoord_input->getSource();
				source_texcoord = daeSafeCast<domSource>(elementURI->getElement());
			}
	
			domFloat_array *floatArray_vertex;
			domFloat_array *floatArray_normal;
			domFloat_array *floatArray_texcoord;
		
			if(source_vertex)
				floatArray_vertex = &source_vertex->getFloat_array()[0];

			if(source_texcoord)
				floatArray_texcoord = &source_texcoord->getFloat_array()[0];
	
			int stride = source_vertex->getTechnique_common()->getAccessor()->getStride();
		
			// Assume there are 3 values per vertex with a stride of 3.
			// Copy the vertices into my structure one-by-one
			// (converts from COLLADA's doubles to floats).
	
			for ( unsigned int i = 0; i < floatArray_vertex->getCount(); i+=stride )
			{
				k3d::point3 vertex(floatArray_vertex->getValue()[i],
						floatArray_vertex->getValue()[i+1],
						floatArray_vertex->getValue()[i+2]);
				points.push_back(ccst*vertex);
			}

			if(source_texcoord)
			{
				stride = source_texcoord->getTechnique_common()->getAccessor()->getStride();
				for ( unsigned int i = 0; i < floatArray_texcoord->getCount(); i+=stride)
				{
					k3d::texture3 texture(floatArray_texcoord->getValue()[i],
									1-floatArray_texcoord->getValue()[i+1],
									1);
					texture_coordinates.push_back(texture);
				}
			}

			// Stores the list of polygons
			poly = triangles->getP();

			max_offset++;
	
			int tot = 0;
	
			// Add polygons into gprim_factory taking into account offsets to parse correctly
			for (int i=0;i<polyCount;i++)
			{
				k3d::mesh::indices_t vertex_coordinates;
				k3d::mesh::indices_t texture_coordinates;
				k3d::mesh::indices_t normal_coordinates;
		
				// Copy all the indices from the domP into my structure.
				int vcount = 3;
				const k3d::uint_t face_first_edge = polyhedron->edge_points.size();
				for(int v=0;v<vcount*max_offset;v+=max_offset)
				{
					if(v_offset!=-1)
					{
						polyhedron->edge_points.push_back(poly->getValue()[tot+v+v_offset]);
						polyhedron->clockwise_edges.push_back(polyhedron->edge_points.size());
						polyhedron->edge_selections.push_back(0.0);
					}
					if(t_offset!=-1)
						texture_coordinates.push_back(poly->getValue()[tot+v+t_offset]);
				}
				tot+=vcount*max_offset;
				polyhedron->clockwise_edges.back() = face_first_edge;
				polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
				polyhedron->loop_first_edges.push_back(face_first_edge);
				polyhedron->face_loop_counts.push_back(1);
				polyhedron->face_materials.push_back(static_cast<k3d::imaterial*>(0));
				polyhedron->face_selections.push_back(0.0);
				++polyhedron->face_counts.back();
			}
		}
	}

	intLight::intLight(domLight& Light, const k3d::matrix4& ccst)
	{
		//TODO Extract light attributes
	}

	intCamera::intCamera(domCamera& Camera, const k3d::matrix4& ccst)
	{
		//TODO Extract camera attributes
	}
}}}



