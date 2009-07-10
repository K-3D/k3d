// K-3D
// Copyright (c) 2005-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

#include <set>

namespace module
{

namespace mesh
{

namespace detail
{

typedef std::vector<k3d::legacy::split_edge*> polygon_t;

void get_edges(k3d::legacy::face* Face, polygon_t& Polygon)
{
	k3d::legacy::split_edge* edge = Face->first_edge;
	do
	{
		Polygon.push_back(edge);

		edge = edge->face_clockwise;
	}
	while(edge != Face->first_edge);
}

/// Finds shortest edge lengths to get most natural bridge
// returns best start index in Edges2 corresponding to Edges1[0]
const unsigned long best_bridge_index(polygon_t& Edges1, polygon_t& Edges2)
{
	const unsigned long size1 = Edges1.size();
	const unsigned long size2 = Edges2.size();

	unsigned long best_index = 0;
	double best_length = 0;
	for(unsigned long i = 0; i < size1; ++i)
	{
		double length = 0;
		for(unsigned long n = 0; n < size1; ++n)
		{
			k3d::legacy::split_edge* edge1 = Edges1[n];
			k3d::legacy::split_edge* edge2 = Edges2[(i + n) % size2];
			length += k3d::distance(edge1->vertex->position, edge2->face_clockwise->vertex->position);
		}

		if(i == 0)
		{
			best_index = 0;
			best_length = length;
		}
		else if(length < best_length)
		{
			best_index = i;
			best_length = length;
		}
	}

	return best_index;
}

void bridge(k3d::legacy::face* Face1, k3d::legacy::face* Face2, k3d::legacy::polyhedron* Polyhedron)
{
	// Get first face edges
	polygon_t edges1;
	get_edges(Face1, edges1);

	// Get second face edges and reverse them
	polygon_t edges2;
	get_edges(Face2, edges2);
	std::reverse(edges2.begin(), edges2.end());

	if(edges1.size() == edges2.size())
	{
		const unsigned long start2 = best_bridge_index(edges1, edges2);

		k3d::legacy::split_edge* first_edge = 0;
		k3d::legacy::split_edge* previous_edge = 0;

		const unsigned long size = edges1.size();
		for(unsigned long n = 0; n < size; ++n)
		{
			k3d::legacy::split_edge* edge1 = edges1[n];
			k3d::legacy::split_edge* edge2 = edges2[(start2 + n) % size];

			k3d::legacy::split_edge* new_edge1 = new k3d::legacy::split_edge(edge1->vertex);
			if(edge1->companion)
			{
				k3d::legacy::join_edges(*edge1->companion, *new_edge1);
				edge1->companion = 0;
			}

			k3d::legacy::split_edge* new_edge2 = new k3d::legacy::split_edge(edge1->face_clockwise->vertex);

			k3d::legacy::split_edge* new_edge3 = new k3d::legacy::split_edge(edge2->vertex);
			if(edge2->companion)
			{
				k3d::legacy::join_edges(*edge2->companion, *new_edge3);
				edge2->companion = 0;
			}

			k3d::legacy::split_edge* new_edge4 = new k3d::legacy::split_edge(edge2->face_clockwise->vertex);

			// Set companions
			if(!previous_edge)
			{
				first_edge = new_edge4;
			}
			else
			{
				k3d::legacy::join_edges(*previous_edge, *new_edge4);
			}

			previous_edge = new_edge2;

			// Loop edges
			new_edge1->face_clockwise = new_edge2;
			new_edge2->face_clockwise = new_edge3;
			new_edge3->face_clockwise = new_edge4;
			new_edge4->face_clockwise = new_edge1;

			k3d::legacy::face* new_face = new k3d::legacy::face(new_edge1, Face1->material);
			Polyhedron->faces.push_back(new_face);
		}

		k3d::legacy::join_edges(*previous_edge, *first_edge);
	}
}

void bridge_faces(k3d::legacy::mesh& Mesh)
{
	k3d::legacy::face* face1 = 0;
	k3d::legacy::polyhedron* polyhedron1 = 0;
	k3d::legacy::face* face2 = 0;
	k3d::legacy::polyhedron* polyhedron2 = 0;

	for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_i = Mesh.polyhedra.begin(); polyhedron_i != Mesh.polyhedra.end(); ++polyhedron_i)
	{
		k3d::legacy::polyhedron* polyhedron = *polyhedron_i;
		for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron->faces.begin(); face != polyhedron->faces.end(); ++face)
		{
			if((*face)->selection_weight)
			{
				if(!face1)
				{
					face1 = (*face);
					polyhedron1 = polyhedron;
					continue;
				}

				if(!face2)
				{
					face2 = (*face);
					polyhedron2 = polyhedron;
					goto Bridge;
				}
			}
		}
	}

	// Bridge first two faces found
	if(!face1 || !face2)
		return;

	Bridge:

	// Merge polyhedra if required
	if(polyhedron1 != polyhedron2)
	{
		polyhedron1->faces.insert(polyhedron1->faces.end(), polyhedron2->faces.begin(), polyhedron2->faces.end());
		Mesh.polyhedra.erase(std::find(Mesh.polyhedra.begin(), Mesh.polyhedra.end(), polyhedron2));
		polyhedron2->faces.clear();
		delete polyhedron2;
	}

	bridge(face1, face2, polyhedron1);

	// Delete faces replaced by bridge
	k3d::legacy::polyhedron::faces_t& faces = polyhedron1->faces;
	faces.erase(std::find(faces.begin(), faces.end(), face1));
	faces.erase(std::find(faces.begin(), faces.end(), face2));

	delete face1;
	delete face2;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// bridge_faces_implementation

class bridge_faces_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	bridge_faces_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Mesh);
		detail::bridge_faces(Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bridge_faces_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x8e744389, 0x5d284d38, 0x95bb89cc, 0x1b9e528e),
				"BridgeFaces",
				"Creates new polygons forming a bridge between two selected polygons",
				"Polygon",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bridge_faces_factory

k3d::iplugin_factory& bridge_faces_factory()
{
	return bridge_faces_implementation::get_factory();
}

} // namespace mesh

} // namespace module

