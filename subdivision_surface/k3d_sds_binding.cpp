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

void k3d_cache_input::update(bool all, facevertices_t& updated_maps)
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
	if (all || (m_modified_faces.size() == face_first_loops.size()))
	{
		m_modified_faces.clear();
	}
	if (m_modified_faces.empty()) // search faces to modify using selection data
	{
		k3d::timer timer;
		for(size_t face = 0; face != face_count; ++face)
		{
			if(all || selected(face)) // selected(face) is expensive!
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

bool k3d_cache_input::selected(size_t Face, int Recurse)
{
	const k3d::mesh::indices_t& clockwise_edges = *m_input_polyhedra->clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *m_input_polyhedra->edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *m_input_polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *m_input_polyhedra->face_first_loops;
	const k3d::mesh::selection_t& point_selection = *m_point_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_input_polyhedra;
	const k3d::mesh::selection_t& edge_selection = *polyhedra.edge_selection;
	const k3d::mesh::selection_t& face_selection = *polyhedra.face_selection;
	
	if (face_selection[Face])
		return true;
	
	const size_t first_edge = loop_first_edges[face_first_loops[Face]];
	for(size_t edge = first_edge; ; )
	{
		if(edge_selection[edge] ||
			point_selection[edge_points[edge]] ||
			(Recurse > 0 && selected(m_edge_to_face[m_companions[edge]], Recurse-1)))
		{
			return true;
		}
			
		edge = clockwise_edges[edge];
		if(edge == first_edge)
			break;
	}
	return false;
}

////////
// k3d_basic_opengl_sds_cache
////////

void k3d_basic_opengl_sds_cache::draw_faces(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	if (Level > m_levels)
	{
		k3d::log() << debug << "Setting level to " << Level << std::endl;
		set_levels(Level);
		update();
	}
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	const k3d::mesh::selection_t& face_selection = *m_first_level_cache_mesh->polyhedra->face_selection;
	
	std::vector<position_t> points_array;
	std::vector<position_t> normals_array;
	std::vector<k3d::color> colors_array;
	std::vector<GLuint> indices;
	
	size_t mipmap_start = 0;
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		k3d::color color = face_selection[face_vertex_iterator->first] ? m_selected_color : m_unselected_color;
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			const point_array& normals = (*mipmap)->normals(Level-1);
			for(int i = 0; i < size; ++i)
			{
				for(int j = 0; j < size; ++j)
				{
					points_array.push_back(*(points[i][j]));
					normals_array.push_back(*(normals[i][j]));
					colors_array.push_back(color);
				}
			}
			for(int i = 0; i < size-1; ++i)
			{
				for(int j = 0; j < size-1; ++j)
				{
					indices.push_back(static_cast<GLuint>(mipmap_start + i * size + j));
					indices.push_back(static_cast<GLuint>(mipmap_start + i * size + (j + 1)));
					indices.push_back(static_cast<GLuint>(mipmap_start + (i + 1) * size + (j + 1)));
					indices.push_back(static_cast<GLuint>(mipmap_start + (i + 1) * size + j));
				}
			}
			mipmap_start += size*size;
		}
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (Selected)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_DOUBLE, 0, &colors_array[0]);
	}
	glVertexPointer(3, GL_DOUBLE, 0, &points_array[0]);
	glNormalPointer(GL_DOUBLE, 0, &normals_array[0]);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void k3d_basic_opengl_sds_cache::draw_borders(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	if (Level > m_levels)
	{
		k3d::log() << debug << "Setting level to " << Level << std::endl;
		set_levels(Level);
		update();
	}
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	const companions_t& companions = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->companions();
	
	const k3d::mesh::selection_t& edge_selection = *m_first_level_cache_mesh->polyhedra->edge_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	std::vector<position_t> points_array;
	std::vector<k3d::color> colors_array;

	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		face_vertex::mipmaps_t::iterator next_mipmap = f.mipmaps.begin();
		size_t face = face_vertex_iterator->first;
		size_t edge = loop_first_edges[face_first_loops[face]];
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			++next_mipmap;
			const point_array& next_points = next_mipmap == f.mipmaps.end() ? (*f.mipmaps.begin())->points(Level-1) : (*next_mipmap)->points(Level-1);
			for(int i = 0; i < size-1; ++i)
			{
				points_array.push_back(*(points[i][size-1]));
				points_array.push_back(*(points[i+1][size-1]));
				points_array.push_back(*(next_points[0][i]));
				points_array.push_back(*(next_points[0][i+1]));
				k3d::color color = edge_selection[edge] || edge_selection[companions[edge]] ? m_selected_color : m_unselected_color;
				colors_array.push_back(color);
				colors_array.push_back(color);
				colors_array.push_back(color);
				colors_array.push_back(color);
			}
			edge = clockwise_edges[edge];
		}
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, &points_array[0]);
	if (Selected)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_DOUBLE, 0, &colors_array[0]);
	}
	glDrawArrays(GL_LINES, 0, points_array.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void k3d_basic_opengl_sds_cache::draw_corners(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	if (Level > m_levels)
	{
		k3d::log() << debug << "Setting level to " << Level << std::endl;
		set_levels(Level);
		update();
	}
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	
	const k3d::mesh::selection_t& point_selection = *m_first_level_cache_mesh->point_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	std::vector<position_t> points_array;
	std::vector<k3d::color> colors_array;
	
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		size_t face = face_vertex_iterator->first;
		size_t edge = loop_first_edges[face_first_loops[face]];
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			points_array.push_back(*(points[0][size-1]));
			k3d::color color = point_selection[edge_points[edge]] ? m_selected_color : m_unselected_color;
			colors_array.push_back(color);
			edge = clockwise_edges[edge];
		}
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, &points_array[0]);
	if (Selected)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_DOUBLE, 0, &colors_array[0]);
	}
	glDrawArrays(GL_POINTS, 0, points_array.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void k3d_basic_opengl_sds_cache::select_faces(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
		
	return_if_fail(Level <= m_levels);
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	const k3d::mesh::selection_t& face_selection = *m_first_level_cache_mesh->polyhedra->face_selection;
	
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);	
		std::vector<position_t> points_array;
		std::vector<GLuint> indices;
		size_t mipmap_start = 0;
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			for(int i = 0; i < size; ++i)
			{
				for(int j = 0; j < size; ++j)
				{
					points_array.push_back(*(points[i][j]));
				}
			}
			for(int i = 0; i < size-1; ++i)
			{
				for(int j = 0; j < size-1; ++j)
				{
					indices.push_back(static_cast<GLuint>(mipmap_start + i * size + j));
					indices.push_back(static_cast<GLuint>(mipmap_start + i * size + (j + 1)));
					indices.push_back(static_cast<GLuint>(mipmap_start + (i + 1) * size + (j + 1)));
					indices.push_back(static_cast<GLuint>(mipmap_start + (i + 1) * size + j));
				}
			}
			mipmap_start += size*size;
		}
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face_vertex_iterator->first);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &points_array[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);
		glDisableClientState(GL_VERTEX_ARRAY);
		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
}

void k3d_basic_opengl_sds_cache::select_borders(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	
	return_if_fail(Level <= m_levels);
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	
	const k3d::mesh::selection_t& edge_selection = *m_first_level_cache_mesh->polyhedra->edge_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		face_vertex::mipmaps_t::iterator next_mipmap = f.mipmaps.begin();
		size_t face = face_vertex_iterator->first;
		size_t edge = loop_first_edges[face_first_loops[face]];
		std::vector<position_t> points_array;
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			++next_mipmap;
			const point_array& next_points = next_mipmap == f.mipmaps.end() ? (*f.mipmaps.begin())->points(Level-1) : (*next_mipmap)->points(Level-1);
			for(int i = 0; i < size-1; ++i)
			{
				points_array.push_back(*(points[i][size-1]));
				points_array.push_back(*(points[i+1][size-1]));
				points_array.push_back(*(next_points[0][i]));
				points_array.push_back(*(next_points[0][i+1]));
			}
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_DOUBLE, 0, &points_array[0]);
			glDrawArrays(GL_LINES, 0, points_array.size());
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			k3d::gl::pop_selection_token(); // ABSOLUTE_SPLIT_EDGE
			edge = clockwise_edges[edge];
		}
	}
}

void k3d_basic_opengl_sds_cache::select_corners(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;

	return_if_fail(Level <= m_levels);
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;

	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	
	const k3d::mesh::selection_t& point_selection = *m_first_level_cache_mesh->point_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
	{
		face_vertex& f = *(face_vertex_iterator->second);
		size_t face = face_vertex_iterator->first;
		size_t edge = loop_first_edges[face_first_loops[face]];
		for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
		{
			const point_array& points = (*mipmap)->points(Level-1);
			const position_t& position = (*(points[0][size-1]));
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_POINT, edge_points[edge]);
			glBegin(GL_POINTS);
			glVertex3d(position[0], position[1], position[2]);
			glEnd();
			k3d::gl::pop_selection_token(); // ABSOLUTE_POINT
			edge = clockwise_edges[edge];
		}
	}
}

////////
// k3d_vbo_sds_cache
////////

k3d_vbo_sds_cache::~k3d_vbo_sds_cache()
{
	for (size_t i = 0; i < m_point_vbos.size(); ++i)
	{
		glDeleteBuffers(1, &m_point_vbos[i]);
		glDeleteBuffers(1, &m_face_vbos[i]);
		glDeleteBuffers(1, &m_normals_vbos[i]);
		glDeleteBuffers(1, &m_edge_vbos[i]);
		glDeleteBuffers(1, &m_corner_vbos[i]);
		glDeleteBuffers(1, &m_corner_color_vbos[i]);
	}
}

void k3d_vbo_sds_cache::draw_faces(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	
	return_if_fail(m_levels > 1); // VBO support for levels > 1 is easier, looks better and should work comfortably on any modern PC
	
	if (Selected)
	{
		return_if_fail(!m_face_color_vbos.empty());
		return_if_fail(glIsBuffer(m_face_color_vbos[Level-2]));
		glBindBuffer(GL_ARRAY_BUFFER, m_face_color_vbos[Level-2]);
		glColorPointer(4, GL_FLOAT, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	return_if_fail(glIsBuffer(m_normals_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_normals_vbos[Level-2]);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, 0);
	
	return_if_fail(glIsBuffer(m_face_vbos[Level-2]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_face_vbos[Level-2]);
	
	return_if_fail(m_n_faces.size() > Level-2);
	return_if_fail(m_n_faces[Level-2] > 0);
	glDrawElements(GL_QUADS, 4*m_n_faces[Level-2], GL_UNSIGNED_INT, static_cast<GLuint*>(0));
}

void k3d_vbo_sds_cache::draw_borders(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
		
	return_if_fail(m_levels > 1);
	
	if (Selected)
	{
		return_if_fail(!m_edge_color_vbos.empty());
		return_if_fail(glIsBuffer(m_edge_color_vbos[Level-2]));
		glBindBuffer(GL_ARRAY_BUFFER, m_edge_color_vbos[Level-2]);
		glColorPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	
	// Draw VBO contents
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	return_if_fail(glIsBuffer(m_edge_vbos[Level-2]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edge_vbos[Level-2]);
	
	return_if_fail(m_n_edges.size() > Level-2);
	return_if_fail(m_n_edges[Level-2] > 0);
	glDrawElements(GL_LINES, 2*m_n_edges[Level-2], GL_UNSIGNED_INT, static_cast<GLuint*>(0));
}

void k3d_vbo_sds_cache::draw_corners(size_t Level, bool Selected)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	
	return_if_fail(m_levels > 1);
	
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	if (Selected)
	{
		return_if_fail(!m_corner_color_vbos.empty());
		return_if_fail(glIsBuffer(m_corner_color_vbos[Level-2]));
		glBindBuffer(GL_ARRAY_BUFFER, m_corner_color_vbos[Level-2]);
		glColorPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	
	return_if_fail(glIsBuffer(m_corner_vbos[Level-2]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_corner_vbos[Level-2]);
	
	return_if_fail(m_n_corners.size() > Level-2);
	return_if_fail(m_n_corners[Level-2] > 0);
	glDrawElements(GL_POINTS, m_n_corners[Level-2], GL_UNSIGNED_INT, static_cast<GLuint*>(0));
}

void k3d_vbo_sds_cache::select_faces(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	return_if_fail(glIsBuffer(m_face_vbos[Level-2]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_face_vbos[Level-2]);
			
	const size_t face_count = face_first_loops.size();
	size_t face_range_start = 0;
	for(size_t face = 0; face != face_count; ++face)
	{		
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face);
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		for(size_t edge = first_edge; ; )
		{ 
			glDrawElements(GL_QUADS, 4*(size-1)*(size-1), GL_UNSIGNED_INT, static_cast<GLuint*>(0) + face_range_start);
			face_range_start += 4*(size-1)*(size-1);
						
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
}

void k3d_vbo_sds_cache::select_borders(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	return_if_fail(glIsBuffer(m_edge_vbos[Level-2]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edge_vbos[Level-2]);
	
	const size_t face_count = face_first_loops.size();
	for(size_t face = 0; face != face_count; ++face)
	{
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		for(size_t edge = first_edge; ; )
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_SPLIT_EDGE, edge);

			glDrawElements(GL_LINES, (size-1)*4, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + (size-1)*4*edge);

			k3d::gl::pop_selection_token();
			
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}
}

void k3d_vbo_sds_cache::select_corners(size_t Level)
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;

	size_t i = Level - 2;
	
	size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(Level-1)))+1;
	
	return_if_fail(!m_point_vbos.empty());
	return_if_fail(glIsBuffer(m_point_vbos[Level-2]));
	glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[Level-2]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	const size_t face_count = face_first_loops.size();
	size_t point_index = 0;
	for (size_t face = 0; face != face_count; ++face)
	{
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		for (size_t edge = first_edge; ; )
		{
			
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_POINT, edge_points[edge]);

			glBegin(GL_POINTS);
			glArrayElement(point_index + size - 1);
			glEnd();

			k3d::gl::pop_selection_token();
			
			point_index += size*size;
			
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}
}

void k3d_vbo_sds_cache::update_selection()
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	
	const k3d::mesh::selection_t& point_selection = *m_first_level_cache_mesh->point_selection;
	const k3d::mesh::selection_t& face_selection = *m_first_level_cache_mesh->polyhedra->face_selection;
	const k3d::mesh::selection_t& edge_selection = *m_first_level_cache_mesh->polyhedra->edge_selection;
	const k3d::mesh::polyhedra_t& polyhedra = *m_first_level_cache_mesh->polyhedra;
	const k3d::mesh::indices_t& clockwise_edges = *polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *polyhedra.edge_points;
	const k3d::mesh::indices_t& loop_first_edges = *polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *polyhedra.face_first_loops;
	
	const k3d::color& selected_color = m_selected_color;
	
	if (m_selected_corners.size() != m_levels-1)
	{
		m_selected_corners = std::vector<std::vector<size_t> >(m_levels-1);
		m_selected_edges = std::vector<std::vector<size_t> >(m_levels-1);
		m_selected_faces = std::vector<std::vector<size_t> >(m_levels-1);
	}
	
	for (size_t level = 2; level <= m_levels; ++level)
	{
		size_t i = level - 2;
		
		size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(level-1)))+1;
		
		/////////
		// corners
		////////
		
		{
		return_if_fail(glIsBuffer(m_corner_color_vbos[i]));
		glBindBuffer(GL_ARRAY_BUFFER, m_corner_color_vbos[i]);
		k3d::color* point_colors = static_cast<k3d::color*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		
		// color last selection to unselected color
		for (size_t point = 0; point != m_selected_corners[i].size(); ++point)
		{
			point_colors[m_selected_corners[i][point]] = m_unselected_color;
		}
		m_selected_corners[i].clear();
		
		const size_t face_count = face_first_loops.size();
		size_t point_index = 0;
		for(size_t face = 0; face != face_count; ++face)
		{
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				if (point_selection[edge_points[edge]])
				{
					point_colors[point_index + size - 1] = selected_color;
					m_selected_corners[i].push_back(point_index + size - 1);
				}
				point_index += size*size;
				
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		} // corners
		
		///////////
		// edges
		///////////
		{
		return_if_fail(glIsBuffer(m_edge_color_vbos[i]));
		glBindBuffer(GL_ARRAY_BUFFER, m_edge_color_vbos[i]);
		k3d::color* point_colors = static_cast<k3d::color*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		
		const companions_t& companions = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->companions();
		
		// color last selection to unselected color
		for (size_t point = 0; point != m_selected_edges[i].size(); ++point)
		{
			point_colors[m_selected_edges[i][point]] = m_unselected_color;
		}
		m_selected_edges[i].clear();
		
		const size_t face_count = face_first_loops.size();
		for(size_t face = 0; face != face_count; ++face)
		{
			size_t mipmap_start = m_face_indices[i][face];
			const size_t first_edge = loop_first_edges[face_first_loops[face]];
			for(size_t edge = first_edge; ; )
			{
				size_t next_mipmap_start = clockwise_edges[edge] == first_edge ? m_face_indices[i][face] : mipmap_start + size*size;
				if (edge_selection[edge] || edge_selection[companions[edge]])
				{
					// points at the corner are shared between patch borders, so we only color interior points.
					// This results in a faded look at the selected edge endpoints 
					for(int j = 1; j < size; ++j)
					{
						point_colors[mipmap_start + (size-1) + size * j] = m_selected_color;
						m_selected_edges[i].push_back(mipmap_start + (size-1) + size * j);
					}
					for(int j = 0; j < size-1; ++j)
					{
						point_colors[next_mipmap_start + j] = m_selected_color;
						m_selected_edges[i].push_back(next_mipmap_start + j);
					}
				}
				mipmap_start += size*size;
				
				edge = clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		} // edges
		
		///////////
		// faces
		///////////
		{
		return_if_fail(glIsBuffer(m_face_color_vbos[i]));
		glBindBuffer(GL_ARRAY_BUFFER, m_face_color_vbos[i]);
		color4d* point_colors = static_cast<color4d*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		
		// color last selection to unselected color
		for (size_t point = 0; point != m_selected_faces[i].size(); ++point)
		{
			point_colors[m_selected_faces[i][point]] = color4d(m_unselected_color.red, m_unselected_color.green, m_unselected_color.blue);
		}
		m_selected_faces[i].clear();
		
		const size_t face_count = face_first_loops.size();
		for(size_t face = 0; face != face_count; ++face)
		{
			if (face_selection[face])
			{
				size_t mipmap_start = m_face_indices[i][face];
				const size_t first_edge = loop_first_edges[face_first_loops[face]];
				for(size_t edge = first_edge; ; )
				{
					for(int j = 0; j < size; ++j)
					{
						for(int k = 0; k < size; ++k)
						{
							point_colors[mipmap_start + j * size + k] = color4d(m_selected_color.red, m_selected_color.green, m_selected_color.blue);
							m_selected_faces[i].push_back(mipmap_start + j * size + k);
						}
					}
					mipmap_start += size*size;
					
					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		} // faces
	}
}

void k3d_vbo_sds_cache::client_output( k3d::mesh * Output )
{
	draw_faces(m_levels, false);
}

void k3d_vbo_sds_cache::client_output_nurbs( k3d::mesh * Output )
{
}

void k3d_vbo_sds_cache::regenerate_vbos()
{
	if (!m_first_level_cache_mesh || !k3d::validate_polyhedra(*m_first_level_cache_mesh))
		return;
	
	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_all_faces();
	
	return_if_fail((modified_faces.size() == m_first_level_cache_mesh->polyhedra->face_first_loops->size()));

	k3d::timer timer;

	for (size_t i = 0; i < m_point_vbos.size(); ++i)
	{
		glDeleteBuffers(1, &m_point_vbos[i]);
		glDeleteBuffers(1, &m_face_vbos[i]);
		glDeleteBuffers(1, &m_normals_vbos[i]);
		glDeleteBuffers(1, &m_edge_vbos[i]);
		glDeleteBuffers(1, &m_corner_vbos[i]);
		glDeleteBuffers(1, &m_corner_color_vbos[i]);
		glDeleteBuffers(1, &m_edge_color_vbos[i]);
		glDeleteBuffers(1, &m_face_color_vbos[i]);
	}
	m_face_indices.clear();

	m_point_vbos = std::vector<GLuint>(m_levels-1);
	m_face_vbos = std::vector<GLuint>(m_levels-1);
	m_normals_vbos = std::vector<GLuint>(m_levels-1);
	m_edge_vbos = std::vector<GLuint>(m_levels-1);
	m_corner_vbos = std::vector<GLuint>(m_levels-1);
	m_corner_color_vbos = std::vector<GLuint>(m_levels-1);
	m_edge_color_vbos = std::vector<GLuint>(m_levels-1);
	m_face_color_vbos = std::vector<GLuint>(m_levels-1);
	m_n_points = std::vector<size_t>(m_levels-1);
	m_n_faces = std::vector<size_t>(m_levels-1);
	m_n_edges = std::vector<size_t>(m_levels-1);
	m_n_corners = std::vector<size_t>(m_levels-1);
	m_face_indices = std::vector<std::vector<size_t> >(m_levels-1);
	m_selected_corners = std::vector<std::vector<size_t> >(m_levels-1);
	m_selected_edges = std::vector<std::vector<size_t> >(m_levels-1);
	m_selected_faces = std::vector<std::vector<size_t> >(m_levels-1);
	
	for (size_t level = 2; level <= m_levels; ++level)
	{
		size_t i = level - 2;
		
		size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(level-1)))+1;
		
		// Update counters
		if (modified_faces.empty())
			return;
	
		m_n_points[i] = 0;
		m_n_faces[i] = 0;
		m_n_edges[i] = 0;
		m_n_corners[i] = 0;
		m_face_indices[i].clear();
		for (facevertices_map::iterator face_vertex_iterator = modified_faces.begin(); face_vertex_iterator != modified_faces.end(); ++face_vertex_iterator)
		{
			m_face_indices[i].push_back(m_n_points[i]);
			face_vertex& fv = *(face_vertex_iterator->second);
			m_n_points[i] += size*size*fv.mipmaps.size();
			m_n_faces[i] += (size-1)*(size-1)*fv.mipmaps.size();
			m_n_edges[i] += 2*(size-1)*fv.mipmaps.size();
			m_n_corners[i] += fv.mipmaps.size();
		}
		
		// Generate buffers
		glGenBuffers(1, &m_point_vbos[i]);
		glGenBuffers(1, &m_face_vbos[i]);
		glGenBuffers(1, &m_normals_vbos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, m_n_points[i] * sizeof(position_t), 0, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, m_n_points[i] * sizeof(position_t), 0, GL_STATIC_DRAW);

		glGenBuffers(1, &m_corner_color_vbos[i]);
		glGenBuffers(1, &m_edge_color_vbos[i]);
		glGenBuffers(1, &m_face_color_vbos[i]);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_face_vbos[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * m_n_faces[i] * sizeof(GLuint), 0, GL_STATIC_DRAW);
		
		// Fill the face index VBO
		GLuint* index_vbo = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE));
		size_t index = 0;
		for(facevertices_map::iterator fv = modified_faces.begin(); fv != modified_faces.end(); ++fv)
		{
			face_vertex& f = *(fv->second);
			size_t mipmap_start = m_face_indices[i][fv->first];
			for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
			{
				for(int i = 0; i < size-1; ++i)
				{
					for(int j = 0; j < size-1; ++j)
					{
						index_vbo[index] = static_cast<GLuint>(mipmap_start + i * size + j);
						index_vbo[index + 1] = static_cast<GLuint>(mipmap_start + i * size + (j + 1));
						index_vbo[index + 2] = static_cast<GLuint>(mipmap_start + (i + 1) * size + (j + 1));
						index_vbo[index + 3] = static_cast<GLuint>(mipmap_start + (i + 1) * size + j);
						index += 4;
					}
				}
				mipmap_start += size*size;
			}
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER); 
		
		// Fill the edge index VBO
		glGenBuffers(1, &m_edge_vbos[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_edge_vbos[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*m_n_edges[i] * sizeof(GLuint), 0, GL_STATIC_DRAW);
		index_vbo = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE));
		index = 0;
		for(facevertices_map::iterator fv = modified_faces.begin(); fv != modified_faces.end(); ++fv)
		{
			face_vertex& f = *(fv->second);
			size_t mipmap_start = m_face_indices[i][fv->first];
			size_t mipmap_number = 0;
			for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
			{
				// a border between patches consists of the right edge of the mipmap associated with the edge number and the bottom edge of the next mipmap
				size_t next_mipmap_start = mipmap_number == f.mipmaps.size() - 1 ? m_face_indices[i][fv->first] : mipmap_start + size*size;
				for(int i = 0; i < size-1; ++i)
				{
					// right edge of mimmap "X"
					index_vbo[index] = static_cast<GLuint>(mipmap_start + (size-1) + size * i);
					index_vbo[index + 1] = static_cast<GLuint>(mipmap_start + (size-1) + size * (i + 1));
					// bottom edge of mipmap "X+1"
					index_vbo[index + 2] = static_cast<GLuint>(next_mipmap_start + i);
					index_vbo[index + 3] = static_cast<GLuint>(next_mipmap_start + (i + 1));
					index += 4;
				}
				mipmap_start += size*size;
				++mipmap_number;
			}
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		
		// Fill the corner index VBO
		glGenBuffers(1, &m_corner_vbos[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_corner_vbos[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_n_corners[i] * sizeof(GLuint), 0, GL_STATIC_DRAW);
		index_vbo = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE));
		index = 0;
		for(facevertices_map::iterator fv = modified_faces.begin(); fv != modified_faces.end(); ++fv)
		{
			face_vertex& f = *(fv->second);
			size_t mipmap_start = m_face_indices[i][fv->first];
			for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
			{
				index_vbo[index] = static_cast<GLuint>(mipmap_start + size - 1);
				mipmap_start += size*size;
				++index;
			}
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
	
	init_color_vbos();
	
	k3d::log() << debug << "SDS: regenerate_vbos for " << modified_faces.size() << " faces took " << timer.elapsed() << "s" << std::endl;
}

void k3d_vbo_sds_cache::init_color_vbos()
{
	for (size_t level = 2; level <= m_levels; ++level)
	{
		size_t i = level - 2;
		size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(level-1)))+1;
		
		std::vector<k3d::color> default_colors(m_n_points[i], m_unselected_color);
		glBindBuffer(GL_ARRAY_BUFFER, m_corner_color_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, m_n_points[i] * sizeof(k3d::color), &default_colors[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, m_edge_color_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, m_n_points[i] * sizeof(k3d::color), &default_colors[0], GL_STATIC_DRAW);
		
		std::vector<color4d> default_colors4d(m_n_points[i], color4d(m_unselected_color.red, m_unselected_color.green, m_unselected_color.blue));
		glBindBuffer(GL_ARRAY_BUFFER, m_face_color_vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, m_n_points[i] * sizeof(color4d), &default_colors4d[0], GL_STATIC_DRAW);
	}
}

void k3d_vbo_sds_cache::update_vbo_positions()
{
	facevertices_map& modified_faces = dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_modified_faces();
	
	for (size_t level = 2; level <= m_levels; ++level)
	{
		size_t i = level - 2;
		size_t size = static_cast<size_t>(pow(2.0, static_cast<double>(level-1)))+1;
		
		// fill or update point VBO
		return_if_fail(glIsBuffer(m_point_vbos[i]));
		glBindBuffer(GL_ARRAY_BUFFER, m_point_vbos[i]);
		position_t* point_vbo = static_cast<position_t*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for(facevertices_map::iterator fv = modified_faces.begin(); fv != modified_faces.end(); ++fv)
		{
			face_vertex& f = *(fv->second);
			size_t mipmap_start = m_face_indices[i][fv->first];
			for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
			{
				const point_array& points = (*mipmap)->points(level-1);
				for(int i = 0; i < size; ++i)
				{
					for(int j = 0; j < size; ++j)
					{
						point_vbo[mipmap_start + i * size + j] = *(points[i][j]);
					}
				}
				mipmap_start += size*size;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		
		// fill or update normals VBO
		return_if_fail(glIsBuffer(m_normals_vbos[i]));
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_vbos[i]);
		position_t* normals_vbo = static_cast<position_t*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for(facevertices_map::iterator fv = modified_faces.begin(); fv != modified_faces.end(); ++fv)
		{
			face_vertex& f = *(fv->second);
			size_t mipmap_start = m_face_indices[i][fv->first];
			for(face_vertex::mipmaps_t::iterator mipmap = f.mipmaps.begin(); mipmap != f.mipmaps.end(); ++mipmap)
			{
				const point_array& normals = (*mipmap)->normals(level-1);
				for(int i = 0; i < size; ++i)
				{
					for(int j = 0; j < size; ++j)
					{
						normals_vbo[mipmap_start + i * size + j] = *(normals[i][j]);
					}
				}
				mipmap_start += size*size;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void k3d_vbo_sds_cache::set_colors(const k3d::color Unselected, const k3d::color Selected)
{
	if (Unselected != m_unselected_color || Selected != m_selected_color)
	{
		k3d::log() << debug << "SDS: Colors changed" << std::endl;
		k3d_opengl_sds_cache::set_colors(Unselected, Selected);
		init_color_vbos();
		update_selection();
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

