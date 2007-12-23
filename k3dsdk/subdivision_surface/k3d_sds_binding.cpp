// K-3D SDS preview
// Copyright (c) 2005-2006, Bart Janssens
//
// Contact: bart.janssens@lid.kviv.be
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

/**	\file Implements K-3D interface for Catmull-Clark algorithm
		\author Bart Janssens <bart.janssens@lid.kviv.be>
 */

#include "k3d_sds_binding.h"

#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/selection.h>

namespace k3d
{

// Comparison operator to sort k3d::point3 in a map
inline bool operator < (const k3d::point3& a, const k3d::point3& b)
{
	if(a[0] < b[0])
		return true;
	if(a[0] > b[0])
		return false;
	if(a[1] < b[1])
		return true;
	if(a[1] > b[1])
		return false;
	if(a[2] < b[2])
		return true;

	return false;
}

namespace sds
{
	
	
typedef k3d::basic_rgba<float> color4d;

///////////
// k3d_cache_input
///////////

bool k3d_cache_input::set_input(const k3d::mesh* Input)
{
	set_new_addresses(*Input);
	return false; // Hint processing makes sure this gets called only if the input topology changed
}

void k3d_cache_input::set_new_addresses(const k3d::mesh& Mesh)
{
	m_input_points = Mesh.points;
	m_input_polyhedra = Mesh.polyhedra;
	m_point_selection = Mesh.point_selection;
}

k3d_cache_input::k3d_cache_input(boost::shared_ptr<const k3d::mesh::points_t> Points, boost::shared_ptr<const k3d::mesh::polyhedra_t> Polyhedra, boost::shared_ptr<const k3d::mesh::selection_t> PointSelection) :
		m_input_points(Points),
		m_input_polyhedra(Polyhedra),
		m_point_selection(PointSelection),
		m_edge_to_face(m_input_polyhedra->edge_points ? m_input_polyhedra->edge_points->size() : 0),
		m_companions(m_input_polyhedra->edge_points ? m_input_polyhedra->edge_points->size() : 0)
{
	if (m_input_points && m_input_polyhedra && m_input_polyhedra->face_first_loops)
	{
		const k3d::mesh::points_t& points = *m_input_points;
		const k3d::mesh::indices_t& face_first_loops = *m_input_polyhedra->face_first_loops;
		const k3d::mesh::indices_t& loop_first_edges = *m_input_polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& clockwise_edges = *m_input_polyhedra->clockwise_edges;
		const k3d::mesh::indices_t& edge_points = *m_input_polyhedra->edge_points;
		init_counters();
		const size_t face_count = face_first_loops.size();
		for(size_t face = 0; face != face_count; ++face)
		{
			face_vertex* fv = new face_vertex();
			m_face_vertices.push_back(fv);
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				m_edge_to_face[edge] = face;
				fv->edge_vertices.push_back(get_ev(edge_points[edge], edge_points[clockwise_edges[edge]], fv));
				
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
		
		// Calculate the edge companion map
		std::map<std::pair<size_t, size_t>, size_t > edge_end_points; // maps edge start and endpoint to edge number
		size_t edge_count = edge_points.size();
		for (size_t edge = 0; edge != edge_count; ++edge)
			edge_end_points[std::make_pair(edge_points[edge], edge_points[clockwise_edges[edge]])] = edge;
		for (size_t edge = 0; edge != edge_count; ++edge)
		{
			size_t start_point = edge_points[edge];
			size_t end_point = edge_points[clockwise_edges[edge]];
			size_t companion = edge;
			std::map<std::pair<size_t, size_t>, size_t >::iterator it = edge_end_points.find(std::make_pair(end_point, start_point));
			if (it != edge_end_points.end())
				companion = it->second;
			m_companions[edge] = companion;
		}
	}
}

void k3d_cache_input::update(const indices_t& Indices, facevertices_t& updated_maps)
{
	return_if_fail(m_input_points && m_input_polyhedra && m_input_polyhedra->face_first_loops);
	const k3d::mesh::points_t& points = *m_input_points;
	const k3d::mesh::indices_t& face_first_loops = *m_input_polyhedra->face_first_loops;
	const k3d::mesh::indices_t& loop_first_edges = *m_input_polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& clockwise_edges = *m_input_polyhedra->clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *m_input_polyhedra->edge_points;
	init_counters();
	const size_t face_count = face_first_loops.size();
	k3d::timer timer;
	indexset_t indexset;
	for (size_t i = 0; i != Indices.size(); ++i)
		indexset.insert(Indices[i]);
	if (indexset.empty() || (m_modified_faces.size() == face_first_loops.size()))
	{
		m_modified_faces.clear();
	}
	if (m_modified_faces.empty()) // search faces to modify using selection data
	{
		k3d::timer timer;
		for(size_t face = 0; face != face_count; ++face)
		{
			if(indexset.empty() || selected(face, indexset)) // selected(face) is expensive!
			{
				const_positions_t updated_points;
				const size_t first_edge = loop_first_edges[face_first_loops[face]];
				for(size_t edge = first_edge; ; )
				{
					updated_points.push_back(&(points[edge_points[edge]]));
	
					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
	
				m_face_vertices[face]->update(updated_points);
				
				updated_maps.push_back(m_face_vertices[face]);
				m_modified_faces.insert(std::make_pair(face, m_face_vertices[face]));
			}
		}
	}
	else // selection unchanged, use cached face list
	{
		for (facevertices_map::iterator fv = m_modified_faces.begin(); fv != m_modified_faces.end(); ++fv)
		{
			size_t face = fv->first;
			const_positions_t updated_points;
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				updated_points.push_back(&(points[edge_points[edge]]));

				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}

			m_face_vertices[face]->update(updated_points);
			
			updated_maps.push_back(m_face_vertices[face]);
		}
	}
	for(edgemap_t::iterator point = m_edge_vertices.begin(); point != m_edge_vertices.end(); ++point)
	{
		point->second->update();
	}
	for(pointmap_t::iterator point = m_sds_vertices.begin(); point != m_sds_vertices.end(); ++point)
	{
		point->second->update();
	}
	
	if (m_modified_faces.size() == face_first_loops.size())
	{
		m_all_faces = m_modified_faces;
	}
}

k3d_cache_input::~k3d_cache_input()
{
	for(edgemap_t::iterator it = m_edge_vertices.begin(); it != m_edge_vertices.end(); ++it)
	{
		delete it->second;
	}
	for(pointmap_t::iterator it = m_sds_vertices.begin(); it != m_sds_vertices.end(); ++it)
	{
		delete it->second;
	}
	for(facevertices_t::iterator it = m_face_vertices.begin(); it != m_face_vertices.end(); ++it)
	{
		delete *it;
	}
}

k3d_cache_input::edgemap_t::iterator k3d_cache_input::find_edge(size_t Start, size_t End)
{
	edgemap_t::iterator it = m_edge_vertices.find(std::make_pair(Start, End));
	if (it != m_edge_vertices.end())
		return it;
	return m_edge_vertices.find(std::make_pair(End, Start));
}

edge_vertex* k3d_cache_input::get_ev(size_t StartPoint, size_t EndPoint, face_vertex* Fv)
{
	edgemap_t::iterator evit = find_edge(StartPoint, EndPoint);
	if(evit != m_edge_vertices.end())
	{
		edge_vertex* ev = evit->second;
		ev->face_vertices[1] = Fv;
		sds_point* corner = get_corner(StartPoint);
		corner->face_vertices.push_back(Fv); // store face vertex connection
		ev->corners[1] = corner; // store edge vertex connection
		ev->corners[0]->corners.push_back(corner); // store corner-corner connections
		corner->corners.push_back(ev->corners[0]);
		Fv->corners.push_back(corner); // link from face vertex to the new SDS vertex
		return ev;
	}

	edge_vertex* ev = new edge_vertex();
	ev->corners[1] = get_corner(EndPoint); // default if no companion exists (above block to set the second corner is never used, in that case)
	m_edge_vertices[std::make_pair(StartPoint, EndPoint)] = ev;
	sds_point* corner = get_corner(StartPoint);
	ev->corners[0] = corner;
	corner->face_vertices.push_back(Fv);
	ev->face_vertices[0] = Fv;
	Fv->corners.push_back(corner);
	return ev;
}

sds_point* k3d_cache_input::get_corner(size_t Point)
{
	pointmap_t::iterator corner_it = m_sds_vertices.find(Point);
	if(corner_it != m_sds_vertices.end())
		return corner_it->second;
	sds_point* corner = new sds_point();
	m_sds_vertices[Point] = corner;
	return corner;
}

bool k3d_cache_input::selected(size_t Face, const indexset_t& Indices, int Recurse)
{
	const k3d::mesh::indices_t& clockwise_edges = *m_input_polyhedra->clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *m_input_polyhedra->edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *m_input_polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *m_input_polyhedra->face_first_loops;
	const k3d::mesh::selection_t& point_selection = *m_point_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_input_polyhedra;
	const k3d::mesh::selection_t& edge_selection = *polyhedra.edge_selection;
	const k3d::mesh::selection_t& face_selection = *polyhedra.face_selection;
	
	const size_t first_edge = loop_first_edges[face_first_loops[Face]];
	for(size_t edge = first_edge; ; )
	{
		if(Indices.find(edge_points[edge]) != Indices.end() ||
			(Recurse > 0 && selected(m_edge_to_face[m_companions[edge]], Indices, Recurse-1)))
		{
			return true;
		}
			
		edge = clockwise_edges[edge];
		if(edge == first_edge)
			break;
	}
	return false;
}

bool k3d_cache_input::no_selection()
{
	const k3d::mesh::selection_t& point_selection = *m_point_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_input_polyhedra;
	const k3d::mesh::selection_t& edge_selection = *polyhedra.edge_selection;
	const k3d::mesh::selection_t& face_selection = *polyhedra.face_selection;
	for (size_t i = 0; i != point_selection.size(); ++i)
	{
		if (point_selection[i])
			return false;
	}
	for (size_t i = 0; i != edge_selection.size(); ++i)
	{
		if (edge_selection[i])
			return false;
	}
	for (size_t i = 0; i != face_selection.size(); ++i)
	{
		if (face_selection[i])
			return false;
	}
	return true;
}

//////////////////
// k3d_sds_cache
//////////////////

void k3d_sds_cache::visit_faces(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly)
{
	k3d::uint_t size = static_cast<k3d::uint_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	facevertices_map& modified_faces = ModifiedOnly ? dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_modified_faces() : dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	k3d::uint_t mipmap_start = 0;
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		Visitor.on_face(face_vertex_iterator->first);
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			const point_array& normals = (*mipmap)->normals(Level-1);
			for(k3d::uint_t i = 0; i < size; ++i)
			{
				for(k3d::uint_t j = 0; j < size; ++j)
				{
					Visitor.on_point(*(points[i][j]), *(normals[i][j]));
				}
			}
			for(k3d::uint_t i = 0; i < size-1; ++i)
			{
				for(k3d::uint_t j = 0; j < size-1; ++j)
				{
					Visitor.on_subfacet(
							mipmap_start + i * size + j,
							mipmap_start + i * size + (j + 1),
							mipmap_start + (i + 1) * size + (j + 1),
							mipmap_start + (i + 1) * size + j);
				}
			}
			mipmap_start += size*size;
		}
	}
}

void k3d_sds_cache::visit_borders(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly)
{
	k3d::uint_t size = static_cast<k3d::uint_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	facevertices_map& modified_faces = ModifiedOnly ? dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_modified_faces() : dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		Visitor.on_face(face_vertex_iterator->first);
		face_vertex& f = *(face_vertex_iterator->second);
		face_vertex::mipmaps_t::iterator next_mipmap = f.mipmaps.begin();
		k3d::uint_t face = face_vertex_iterator->first;
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			Visitor.on_edge();
			const point_array& points = (*mipmap)->points(Level-1);
			++next_mipmap;
			const point_array& next_points = next_mipmap == f.mipmaps.end() ? (*f.mipmaps.begin())->points(Level-1) : (*next_mipmap)->points(Level-1);
			for(k3d::uint_t i = 0; i < size-1; ++i)
			{
				Visitor.on_point(*(points[i][size-1]));
				Visitor.on_point(*(points[i+1][size-1]));
				Visitor.on_point(*(next_points[0][i]));
				Visitor.on_point(*(next_points[0][i+1]));
			}
		}
	}
}

void k3d_sds_cache::visit_corners(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly)
{
	k3d::uint_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	facevertices_map& modified_faces = ModifiedOnly ? dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_modified_faces() : dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		Visitor.on_face(face_vertex_iterator->first);
		face_vertex& f = *(face_vertex_iterator->second);
		size_t face = face_vertex_iterator->first;
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			Visitor.on_point(*(points[0][size-1]));
			Visitor.on_edge();
		}
	}
}

//////////
// k3d_mesh_sds_cache
//////////

typedef std::map<const k3d::point3* const, k3d::legacy::point*> vertex_map_t;

k3d::legacy::point* create_unique_point(vertex_map_t& VertexMap, const k3d::point3& Position, k3d::legacy::mesh& Mesh)
{
	vertex_map_t::iterator i = VertexMap.find(&Position);
	if(i != VertexMap.end())
		return i->second;

	// Create a new point
	k3d::legacy::point* new_point = new k3d::legacy::point(Position);
	return_val_if_fail(new_point, 0);
	Mesh.points.push_back(new_point);

	VertexMap.insert(std::make_pair(&Position, new_point));

	return new_point;
}

typedef std::vector<k3d::legacy::split_edge*> edges_t;
typedef std::vector<edges_t> subdivided_edges_t; // each element of this type is a vector of edges that resulted from one edge in the original mesh

/// Copy attributes (selection, material, ...) of the original face to the destination faces
void copy_face_attributes(const k3d::legacy::face& SourceFace, k3d::legacy::polyhedron::faces_t& Faces, subdivided_edges_t& Edges, k3d::legacy::mesh::points_t& Points) {
	for (unsigned long face = 0; face < Faces.size(); ++face)
	{
		// face attributes to be copied go here:
		Faces[face]->material = SourceFace.material;
		Faces[face]->selection_weight = SourceFace.selection_weight;
	}
	
	int edge_number = 0;
	for (k3d::legacy::split_edge* edge = SourceFace.first_edge; edge; edge = edge->face_clockwise)
	{
		// vertex attributes to be copied go here:
		Points[(edge_number-1+Points.size())%Points.size()]->selection_weight = edge->vertex->selection_weight;
		edges_t& edges = Edges[edge_number];
		for (edges_t::const_iterator new_edge = edges.begin(); new_edge != edges.end(); ++new_edge)
			(*new_edge)->selection_weight = edge->selection_weight; // edge attributes to be copied go in this loop

		++edge_number;
		
		if(edge->face_clockwise == SourceFace.first_edge)
			break;
	}
}

void k3d_mesh_sds_cache::client_output(k3d::legacy::mesh* Output)
{
/*	k3d::legacy::mesh& Mesh = *Output;
	const unsigned long Level = m_levels;
	k3d_cache_input* input = dynamic_cast<k3d_cache_input*>(m_first_level_cache);
	return_if_fail(input);
	return_if_fail(!input->mesh().polyhedra.empty());
	return_if_fail(!input->mesh().polyhedra.front()->faces.empty());
	
	const k3d_cache_input::faces_per_polyhedron_t& faces_per_poly = input->faces_per_polyhedron();

	vertex_map_t vertex_map;

	for(unsigned long polynumber = 0; polynumber < faces_per_poly.size(); ++polynumber)
	{
		k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
		Mesh.polyhedra.push_back(polyhedron);
		const facevertices_t& faces = *faces_per_poly[polynumber];
		if(Level-1 == 0)
		{
			for(unsigned long face = 0; face != faces.size(); ++face)
			{
				k3d::legacy::face& original_face = *(input->mesh().polyhedra[polynumber]->faces[face]);
				face_vertex& f = *faces[face];
				unsigned long size = f.edge_vertices.size();
				k3d::legacy::polyhedron::faces_t new_faces;
				subdivided_edges_t new_edges(size);
				k3d::legacy::mesh::points_t new_corners;
				
				for(unsigned long i = 0; i < size; ++i)
				{
					const k3d::point3& vertex1 = f.vertex;
					const k3d::point3& vertex2 = f.edge_vertices[i]->vertex;
					const k3d::point3& vertex3 = f.corners[(i+1)%size]->vertex;
					const k3d::point3& vertex4 = f.edge_vertices[(i+1)%size]->vertex;
					k3d::legacy::point* point1 = create_unique_point(vertex_map, vertex1, Mesh);
					k3d::legacy::point* point2 = create_unique_point(vertex_map, vertex2, Mesh);
					k3d::legacy::point* point3 = create_unique_point(vertex_map, vertex3, Mesh);
					k3d::legacy::point* point4 = create_unique_point(vertex_map, vertex4, Mesh);
						//const k3d::point3 normal = normal(f.vertex, f.edge_vertices[i]->vertex, f.corners[(i+1)%size]->vertex, f.edge_vertices[(i+1)%size]->vertex);

					k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(point1);
					k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(point2);
					k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(point3);
					k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(point4);

					edge1->face_clockwise = edge2;
					edge2->face_clockwise = edge3;
					edge3->face_clockwise = edge4;
					edge4->face_clockwise = edge1;

					k3d::legacy::face* new_face = new k3d::legacy::face(edge1, 0);
					assert_warning(new_face);
					
					polyhedron->faces.push_back(new_face);
					new_faces.push_back(new_face);
					new_edges[i].push_back(edge2);
					new_edges[(i+1)%size].push_back(edge3);
					new_corners.push_back(point3);
				}
				copy_face_attributes(original_face, new_faces, new_edges, new_corners);
			}
		}
		else
		{
			int size = static_cast<int>(pow(2.0, static_cast<double>(Level-1)))+1;
			for(unsigned long face = 0; face != faces.size(); ++face)
			{
				k3d::legacy::face& original_face = *(input->mesh().polyhedra[polynumber]->faces[face]);
				face_vertex& f = *faces[face];
				k3d::legacy::polyhedron::faces_t new_faces;
				subdivided_edges_t new_edges(f.edge_vertices.size());
				k3d::legacy::mesh::points_t new_corners(f.edge_vertices.size());
				int corner_number = 0;
				for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
				{
					const point_array& points = (*mipmap)->points(Level-1);
							//const point_array& normals = (*mipmap)->normals(Level-1);
					for(int i = 0; i < size-1; ++i)
					{
						for(int j = 0; j < size-1; ++j)
						{
							const k3d::point3& vertex1 = *points[i][j];
							const k3d::point3& vertex2 = *points[i][j+1];
							const k3d::point3& vertex3 = *points[i+1][j+1];
							const k3d::point3& vertex4 = *points[i+1][j];
							k3d::legacy::point* point1 = create_unique_point(vertex_map, vertex1, Mesh);
							k3d::legacy::point* point2 = create_unique_point(vertex_map, vertex2, Mesh);
							k3d::legacy::point* point3 = create_unique_point(vertex_map, vertex3, Mesh);
							k3d::legacy::point* point4 = create_unique_point(vertex_map, vertex4, Mesh);
									//const k3d::point3 normal1 = *normals[i][j];
									//const k3d::point3 vector2 = *normals[i][j+1];
									//const k3d::point3 vector3 = *normals[i+1][j+1];
									//const k3d::point3 normal4 = *normals[i+1][j];
	
							k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(point1);
							k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(point2);
							k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(point3);
							k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(point4);
	
							edge1->face_clockwise = edge2;
							edge2->face_clockwise = edge3;
							edge3->face_clockwise = edge4;
							edge4->face_clockwise = edge1;
	
							k3d::legacy::face* new_face = new k3d::legacy::face(edge1, 0);
							assert_warning(new_face);
	
							polyhedron->faces.push_back(new_face);
							new_faces.push_back(new_face);
							if (i == 0)
								new_edges[(corner_number-1+new_edges.size())%new_edges.size()].push_back(edge1);
							if (j == size-2)
								new_edges[(corner_number)%new_edges.size()].push_back(edge2);
							if (i == 0 && j == size-2)
								new_corners[(corner_number-1 + new_corners.size())%new_corners.size()] = point2;
						}
					}
					++corner_number;
				}
				copy_face_attributes(original_face, new_faces, new_edges, new_corners);
			}
		}
		k3d::legacy::set_companions(*polyhedron);
	}*/
}

} // namespace sds

} // namespace k3d

