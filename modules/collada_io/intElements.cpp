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
#include "intElements.h"
#include "gprim_factory.h"
#include <iostream>
#include <dom/domPolylist.h>
#include <dom/domTriangles.h>
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
		k3d::log() << debug << "una geom" << std::endl;
		domMesh *meshElement = geomElement.getMesh();

		k3d::gprim_factory factory(Mesh);

		domInputLocalOffset* vertex_input;
		domInputLocalOffset* normal_input;
		domInputLocalOffset* texcoord_input;

		for(int i=0; i<meshElement->getPolylist_array().getCount()>0; i++)
		{
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


////////////
/////////////
//////////////
			domSource *source_vertex;
			domSource *source_normal;
			domSource *source_texcoord;
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
		
			//if(source_normal)
			//	floatArray_normal = &source_normal->getFloat_array()[0];
		
			if(source_texcoord)
				floatArray_texcoord = &source_texcoord->getFloat_array()[0];
	
			int stride = source_vertex->getTechnique_common()->getAccessor()->getStride();
		
			// Assume there are 3 values per vertex with a stride of 3.
			// Copy the vertices into my structure one-by-one
			// (converts from COLLADA's doubles to floats).
	
			for ( unsigned int i = 0; i < floatArray_vertex->getCount(); i+=stride )
			{
				k3d::point4 vertex(floatArray_vertex->getValue()[i],
						floatArray_vertex->getValue()[i+1],
						floatArray_vertex->getValue()[i+2],
						1);
				//k3d::texture3 texture(floatArray_texcoord->getValue()[i],
				//		floatArray_texcoord->getValue()[i+1],
				//		1);
				factory.add_point(ccst*vertex);
				//factory.add_texcoord(texture);
			}

			if(floatArray_texcoord->getCount()>0)
			{
				k3d::log() << debug << "a;sldkfjas;dlfkjads" << std::endl;
				stride = source_texcoord->getTechnique_common()->getAccessor()->getStride();
				//k3d::log() << debug << stride << " " << floatArray_texcoord->getCount() << std::endl;
				for ( unsigned int i = 0; i < floatArray_texcoord->getCount(); i+=stride)
				{
					k3d::texture3 texture(floatArray_texcoord->getValue()[i],
									1-floatArray_texcoord->getValue()[i+1],
									1);
					factory.add_texcoord(texture);
				}
			}
////////////
///////////
//////////

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
				for(int v=0;v<vcount*max_offset;v+=max_offset)
				{
					if(v_offset!=-1)
						vertex_coordinates.push_back(poly->getValue()[tot+v+v_offset]);		
					if(n_offset!=-1)
						normal_coordinates.push_back(poly->getValue()[tot+v+n_offset]);
					if(t_offset!=-1)
						texture_coordinates.push_back(poly->getValue()[tot+v+t_offset]);
				}
				tot+=vcount*max_offset;
				// Push this polygon into the list of polygons in my structure.
				factory.add_polygon(vertex_coordinates, texture_coordinates); //and soon texture_coordinates, normal_coordinates
			}

			//Add points
		}

		for(int i=0; i<meshElement->getTriangles_array().getCount()>0; i++)
		{
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


////////////
/////////////
//////////////
			domSource *source_vertex;
			domSource *source_normal;
			domSource *source_texcoord;
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
		
			//if(source_normal)
			//	floatArray_normal = &source_normal->getFloat_array()[0];
		
			if(source_texcoord)
				floatArray_texcoord = &source_texcoord->getFloat_array()[0];
	
			int stride = source_vertex->getTechnique_common()->getAccessor()->getStride();
		
			// Assume there are 3 values per vertex with a stride of 3.
			// Copy the vertices into my structure one-by-one
			// (converts from COLLADA's doubles to floats).
	
			for ( unsigned int i = 0; i < floatArray_vertex->getCount(); i+=stride )
			{
				k3d::point4 vertex(floatArray_vertex->getValue()[i],
						floatArray_vertex->getValue()[i+1],
						floatArray_vertex->getValue()[i+2],
						1);
				//if(source_texcoord)
				{
				//	k3d::texture3 texture(floatArray_texcoord->getValue()[i],
								//floatArray_texcoord->getValue()[i+1],
								
				//				1);
					//factory.add_texcoord(texture);
				}
				factory.add_point(ccst*vertex);
			}

			//if(source_texcoord)
			{
				//stride = source_texcoord->getTechnique_common()->getAccessor()->getStride();
				//k3d::log() << debug << stride << " " << floatArray_texcoord->getCount() << std::endl;
				//for ( unsigned int i = 0; i < floatArray_texcoord->getCount(); i+=stride)
				{
					//k3d::texture3 texture(floatArray_texcoord->getValue()[i],
					//				floatArray_texcoord->getValue()[i+1],
					//				1);
				}
			}
////////////
///////////
//////////


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
				for(int v=0;v<vcount*max_offset;v+=max_offset)
				{
					if(v_offset!=-1)
						vertex_coordinates.push_back(poly->getValue()[tot+v+v_offset]);		
					if(n_offset!=-1)
						normal_coordinates.push_back(poly->getValue()[tot+v+n_offset]);
					if(t_offset!=-1)
						texture_coordinates.push_back(poly->getValue()[tot+v+t_offset]);
				}
				tot+=vcount*max_offset;
				// Push this polygon into the list of polygons in my structure.
				factory.add_polygon(vertex_coordinates); //and soon texture_coordinates, normal_coordinates
			}
		}

		// Copy the vertices we are going to use. To keep things simple,
		// we will assume there is only one domSource and domFloatArray in the domMesh,
		// that it is the array of vertices, and that it is in X, Y, Z format. A real
		// app would find the vertices by starting with domPolygons and following
		// the links through the domInput, domVertices, domSource, domFloat_array,
		// and domTechnique.
		
		//For now each input MUST have only one source
/*
		domSource *source_vertex;
		domSource *source_normal;
		domSource *source_texcoord;
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
	
		if(source_normal)
			floatArray_normal = &source_normal->getFloat_array()[0];
	
		if(source_texcoord)
			floatArray_texcoord = &source_texcoord->getFloat_array()[0];

		int stride = source_vertex->getTechnique_common()->getAccessor()->getStride();
	
		// Assume there are 3 values per vertex with a stride of 3.
		// Copy the vertices into my structure one-by-one
		// (converts from COLLADA's doubles to floats).

		for ( unsigned int i = 0; i < floatArray_vertex->getCount(); i+=stride )
		{
			k3d::point4 vertex(floatArray_vertex->getValue()[i],
					floatArray_vertex->getValue()[i+1],
					floatArray_vertex->getValue()[i+2],
					1);
			factory.add_point(ccst*vertex);
		}
*/
		factory.attach_texcoords();


		//const k3d::mesh::indices_t& edge_points = Mesh.polyhedra->edge_points.get();

		//k3d::mesh::named_arrays_t::const_iterator array_it = Output.polyhedra->face_varying_data.find(m_texcoord_set_name.pipeline_value());
		//return_if_fail(array_it !=  Output.polyhedra->face_varying_data.end());
		//return_if_fail(dynamic_cast< texcoord_array_t* >(array_it->second.get()));
		//texcoord_array_t& texcoords = *dynamic_cast< texcoord_array_t* >(array_it->second.get());
		
		//k3d::matrix4 inv_matrix = inverse(m_input_matrix.pipeline_value());
		//for(size_t edge = 0; edge<Output.polyhedra->edge_points->size(); ++edge)
		{
		//	k3d::point3 point = inv_matrix * (*Output.points.get())[edge_points[edge]];
		//	texcoords[edge].n[0] = point.n[0];
		//	texcoords[edge].n[1] = point.n[1];
		//	texcoords[edge].n[2] = point.n[2];
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



