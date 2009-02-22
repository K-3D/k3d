// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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


#include "normal_cache.h"
#include "vbo.h"
#include "sds_cache.h"

#include <k3dsdk/hints.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/any.hpp>

#include <map>

namespace module
{

namespace opengl
{

namespace painters
{

////////
// class vbo
///////

vbo::vbo( )
{
	glGenBuffers(1, &m_name);
}

vbo::~ vbo( )
{
	glDeleteBuffers(1, &m_name);
}

vbo::operator GLuint( ) const
{
	return m_name;
}

///////////
// class point_vbo
///////////

void point_vbo::bind()
{
	if(!m_vbo)
	{
		throw vbo_exception("point_vbo is null");
	}
	if(!glIsBuffer(*m_vbo))
	{
		throw vbo_exception("point_vbo is not a buffer!");
	}
	glBindBuffer(GL_ARRAY_BUFFER, *m_vbo);	
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void point_vbo::on_schedule(k3d::inode* Painter)
{
	delete m_vbo;
	m_vbo = 0;
	m_indices.clear();
}

void point_vbo::on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
{
	if (m_indices.empty()) // Only set indices once (they are cleared upon execute()
	{
		m_indices = Hint->changed_points;
	}
}

void point_vbo::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	return_if_fail(k3d::validate_points(Mesh));
	const k3d::mesh::points_t& points = *(Mesh.points);
	bool new_vbo = false;
	
	if (!m_vbo) // OpenGL VBO functions may only be called in the drawing context, i.e. during paint_mesh or select_mesh
	{
		m_vbo = new vbo();
		new_vbo = true;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, *m_vbo);
	
	if (m_indices.empty() || new_vbo)
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(points[0]) * points.size(), &points[0], GL_STATIC_DRAW);
	}
	else
	{
		k3d::point3* vertices = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for (size_t index = 0; index != m_indices.size(); ++index)
		{
			vertices[m_indices[index]] = points[m_indices[index]];
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	m_indices.clear(); // Clear indices for next schedule call
}

////////
// Class edge_vbo
////////

void edge_vbo::bind()
{
	if(!m_vbo)
	{
		throw vbo_exception("edge_vbo is null");
	}
	if(!glIsBuffer(*m_vbo))
	{
		throw vbo_exception("edge_vbo is not a buffer!");
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_vbo);
}

void edge_vbo::on_schedule(k3d::inode* Painter)
{
	delete m_vbo;
	m_vbo = 0;
}

void edge_vbo::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	return_if_fail(k3d::validate_polyhedra(Mesh));
	if (m_vbo)
		return;
	
	m_vbo = new vbo();
	
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * edge_points.size() * 2, 0, GL_STATIC_DRAW); // left uninited
	// init index buffer with indices from edge_points and clockwise_edges
	GLuint* indices = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE)); // map buffer memory in indices
	for (size_t i = 0; i < edge_points.size(); ++i)
	{
		indices[2*i] = static_cast<GLuint>(edge_points[i]);
		indices[2*i + 1] = static_cast<GLuint>(edge_points[clockwise_edges[i]]);
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER); // release indices
}

///////////////
// triangle_vbo
///////////////

void triangle_vbo::on_schedule(k3d::inode* Painter)
{
	delete m_point_vbo;
	m_point_vbo = 0;
	delete m_index_vbo;
	m_index_vbo = 0;
	delete m_normal_vbo;
	m_normal_vbo = 0;
	m_indices.clear();
	m_corner_to_face.clear();
	schedule_data<cached_triangulation>(m_mesh, 0, Painter);
	schedule_data<normal_cache>(m_mesh, 0, Painter);
}

void triangle_vbo::on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
{
	if (m_indices.empty()) // Only set indices once (they are cleared upon execute()
	{
		m_indices = Hint->changed_points;
	}
	schedule_data<cached_triangulation>(m_mesh, Hint, Painter);
	schedule_data<normal_cache>(m_mesh, Hint, Painter);
}

void triangle_vbo::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	cached_triangulation& triangle_cache = get_data<cached_triangulation>(&Mesh, Painter);
	const k3d::mesh::points_t& points = triangle_cache.points();
	
	if (!m_point_vbo) // OpenGL VBO functions may only be called in the drawing context, i.e. during paint_mesh or select_mesh
	{
		m_point_vbo = new vbo();
		m_indices.clear();
	}
	
	cached_triangulation::index_vectors_t& point_links = triangle_cache.point_links();
	
	glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
	
	if (m_indices.empty())
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(points[0]) * points.size(), &points[0], GL_STATIC_DRAW);
	}
	else
	{
		k3d::point3* vertices = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for (k3d::uint_t index = 0; index != m_indices.size(); ++index)
		{
			k3d::mesh::indices_t triangle_points = point_links[m_indices[index]];
			for (k3d::uint_t i = 0; i != triangle_points.size(); ++i)
				vertices[triangle_points[i]] = points[triangle_points[i]];
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	cached_triangulation::indices_t& triangles = triangle_cache.indices();
	cached_triangulation::index_vectors_t& face_points = triangle_cache.face_points();
	
	if (!m_index_vbo)
	{
		m_index_vbo = new vbo();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * triangles.size(), &triangles[0], GL_STATIC_DRAW);
		m_corner_to_face.resize(points.size());
		for (k3d::uint_t face = 0; face != face_points.size(); ++face)
		{
			k3d::mesh::indices_t& corners = face_points[face];
			for (k3d::uint_t i = 0; i != corners.size(); ++i)
			{ 
				m_corner_to_face[corners[i]] = face;
			}
		}
	}
		
	if (!m_normal_vbo)
	{
		m_normal_vbo = new vbo();
		m_indices.clear();
	}
	
	normal_cache& n_cache = get_data<normal_cache>(&Mesh, Painter);
	
	glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(k3d::normal3) * points.size(), 0, GL_STATIC_DRAW);
	k3d::normal3* normalbuffer = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
	
	if (m_indices.empty())
	{
		for (k3d::uint_t face = 0; face != face_points.size(); ++face)
		{
			k3d::mesh::indices_t& corners = face_points[face];
			for (k3d::uint_t i = 0; i != corners.size(); ++i)
			{ 
				normalbuffer[corners[i]] = n_cache.face_normals(Painter).at(face);
			}
		}
	}
	else
	{
		for (k3d::uint_t index = 0; index != m_indices.size(); ++index) // for all transformed points
		{
			k3d::mesh::indices_t triangle_points = point_links[m_indices[index]]; // get the associated VBO point indices
			for (k3d::uint_t i = 0; i != triangle_points.size(); ++i) // For all those associated points (one per face)
			{
				k3d::uint_t face = m_corner_to_face[triangle_points[i]]; // Get the face this point belongs to
				const k3d::normal3& n = n_cache.face_normals(Painter).at(face);
				k3d::mesh::indices_t& corners = face_points[face];
				for (k3d::uint_t j = 0; j != corners.size(); ++j)
				{ 
					normalbuffer[corners[j]] = n;
				}
			}
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	m_indices.clear();
}

void triangle_vbo::bind()
{
	if (!glIsBuffer(*m_point_vbo) || !glIsBuffer(*m_index_vbo))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
	
	if (glIsBuffer(*m_normal_vbo))
	{
		glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo); // activate normals VBO
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, 0);
	}
}

void triangle_vbo::draw_range(k3d::uint_t Start, k3d::uint_t End, k3d::inode* Painter)
{
	cached_triangulation& triangle_cache = get_data<cached_triangulation>(m_mesh, Painter);
	k3d::mesh::indices_t& face_starts = triangle_cache.face_starts();
	if (face_starts.empty())
		return;
	cached_triangulation::indices_t& indices = triangle_cache.indices();
	k3d::uint_t startindex = face_starts[Start];
	k3d::uint_t endindex = End == (face_starts.size()) ? indices.size() : face_starts[End];
	glDrawElements(GL_TRIANGLES, endindex - startindex, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + startindex);
}

////////////
// sds_face_vbo
/////////////

/// Helper class to update the point VBO
class update_face_vbo_visitor : public k3d::sds::ipatch_surface_visitor
{
public:
	update_face_vbo_visitor(const k3d::mesh::indices_t& FaceStarts, const GLuint* Indices, k3d::point3* Points) :
		m_face_starts(FaceStarts),
		m_indices(Indices),
		m_points(Points),
		m_index(0) 
	{}
	void on_vertex(const k3d::point3& Point, const k3d::normal3& Normal)
	{
		modified_indices.push_back(m_index);
		m_points[m_index++] = Point;
		normals.push_back(Normal); // Unfortunately, we need temp storage for the normals, since we can't map two buffers at once
	}
	void on_edge(const k3d::uint_t PointIndex) {}
	void on_patch(k3d::uint_t Face)
	{
		m_index = m_indices[m_face_starts[Face]];
	}
	k3d::mesh::indices_t modified_indices;
	k3d::mesh::normals_t normals;
private:
	const k3d::mesh::indices_t& m_face_starts;
	const GLuint* m_indices;
	k3d::point3* m_points;
	k3d::uint_t m_index;
};

// Create face arrays in case interpolateboundary is not set
class face_visitor_no_boundary : public k3d::sds::ipatch_surface_visitor
{
public:
	face_visitor_no_boundary(const k3d::mesh::bools_t& BoundaryFaces) : m_boundary_faces(BoundaryFaces), m_ignore_face(false)
	{
	}
	void on_vertex(const k3d::point3& Point, const k3d::normal3& Normal)
	{
		points_array.push_back(Point);
		normals_array.push_back(Normal);
	}
	void on_edge(const k3d::uint_t PointIndex)
	{
		if (m_ignore_face)
			return;
		indices.push_back(PointIndex);
	}
	virtual void on_patch(k3d::uint_t Face)
	{
		m_ignore_face = m_boundary_faces[Face];
		face_starts.push_back(indices.size());
	}
	k3d::mesh::points_t points_array;
	k3d::mesh::normals_t normals_array;
	std::vector<GLuint> indices;
	k3d::mesh::indices_t face_starts;
	const k3d::mesh::bools_t& m_boundary_faces;
	bool m_ignore_face;
};

/// Helper class to update the point VBO when interpolateboundary is not set
class update_face_vbo_visitor_no_boundary : public k3d::sds::ipatch_surface_visitor
{
public:
	update_face_vbo_visitor_no_boundary(const k3d::mesh::indices_t& FaceStarts, const GLuint* Indices, k3d::point3* Points, const k3d::mesh::bools_t& BoundaryFaces) :
		m_face_starts(FaceStarts),
		m_indices(Indices),
		m_points(Points),
		m_index(0),
		m_boundary_faces(BoundaryFaces),
		m_ignore_face(false)
	{}
	void on_vertex(const k3d::point3& Point, const k3d::normal3& Normal)
	{
		if (m_ignore_face)
			return;
		modified_indices.push_back(m_index);
		m_points[m_index++] = Point;
		normals.push_back(Normal); // Unfortunately, we need temp storage for the normals, since we can't map two buffers at once
	}
	void on_edge(const k3d::uint_t PointIndex) {}
	void on_patch(k3d::uint_t Face)
	{
		m_ignore_face = m_boundary_faces[Face];
		if (m_ignore_face)
			return;
		m_index = m_indices[m_face_starts[Face]];
	}
	k3d::mesh::indices_t modified_indices;
	k3d::mesh::normals_t normals;
private:
	const k3d::mesh::indices_t& m_face_starts;
	const GLuint* m_indices;
	k3d::point3* m_points;
	k3d::uint_t m_index;
	const k3d::mesh::bools_t& m_boundary_faces;
	bool m_ignore_face;
};

typedef k3d::typed_array<std::string> tags_t;

void sds_face_vbo::update(const k3d::mesh& Mesh, const k3d::uint_t Level, sds_cache& Cache)
{
	if (!m_point_vbo) // new cache -> completely regenerate the VBOs
	{
		if(Mesh.polyhedra->constant_data.lookup<tags_t>("interpolateboundary"))
		{
			face_visitor visitor;
			Cache.visit_surface(Level, visitor);
			
			m_point_vbo = new vbo();
			glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.points_array[0]) * visitor.points_array.size(), &visitor.points_array[0], GL_STATIC_DRAW);
			
			m_normal_vbo = new vbo();
			glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.normals_array[0]) * visitor.normals_array.size(), &visitor.normals_array[0], GL_STATIC_DRAW);
			
			m_index_vbo = new vbo();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(visitor.indices[0]) * visitor.indices.size(), &visitor.indices[0], GL_STATIC_DRAW);
			
			face_starts = visitor.face_starts;
			index_size = visitor.indices.size();
		}
		else // interpolateboundary not set
		{
			k3d::polyhedron::create_edge_adjacency_lookup(*Mesh.polyhedra->edge_points, *Mesh.polyhedra->clockwise_edges, m_boundary_edges, m_companions);
			k3d::polyhedron::create_boundary_face_lookup(*Mesh.polyhedra->face_first_loops, *Mesh.polyhedra->face_loop_counts, *Mesh.polyhedra->loop_first_edges, *Mesh.polyhedra->clockwise_edges, m_boundary_edges, m_companions, boundary_faces);
			
			face_visitor_no_boundary visitor(boundary_faces);
			Cache.visit_surface(Level, visitor);
			
			m_point_vbo = new vbo();
			glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.points_array[0]) * visitor.points_array.size(), &visitor.points_array[0], GL_STATIC_DRAW);
			
			m_normal_vbo = new vbo();
			glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.normals_array[0]) * visitor.normals_array.size(), &visitor.normals_array[0], GL_STATIC_DRAW);
			
			m_index_vbo = new vbo();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(visitor.indices[0]) * visitor.indices.size(), &visitor.indices[0], GL_STATIC_DRAW);
			
			face_starts = visitor.face_starts;
			index_size = visitor.indices.size();
		}
	}
	else // Only update the point VBO with new positions
	{
		glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
		k3d::point3* points = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
		GLuint* indices = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY));
		
		if(Mesh.polyhedra->constant_data.lookup<tags_t>("interpolateboundary"))
		{
			update_face_vbo_visitor visitor(face_starts, indices, points);
			Cache.visit_surface(Level, visitor);
			
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			
			glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo);
			k3d::normal3* normals = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
			for(k3d::uint_t i = 0; i != visitor.modified_indices.size(); ++i)
				normals[visitor.modified_indices[i]] = visitor.normals[i];
		}
		else // interpolateboundary not set
		{
			update_face_vbo_visitor_no_boundary visitor(face_starts, indices, points, boundary_faces);
			Cache.visit_surface(Level, visitor);
			
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			
			glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo);
			k3d::normal3* normals = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
			for (k3d::uint_t i = 0; i != visitor.modified_indices.size(); ++i)
				normals[visitor.modified_indices[i]] = visitor.normals[i];
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void sds_face_vbo::bind()
{
	if (!glIsBuffer(*m_point_vbo) || !glIsBuffer(*m_index_vbo))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_index_vbo);
	
	if (glIsBuffer(*m_normal_vbo))
	{
		glBindBuffer(GL_ARRAY_BUFFER, *m_normal_vbo); // activate normals VBO
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, 0);
	}
}

/////////////
// sds_edge_vbo
/////////////

/// Helper class to update the point VBO
class update_edge_vbo_visitor : public k3d::sds::ipatch_boundary_visitor
{
public:
	update_edge_vbo_visitor(const k3d::mesh::indices_t& EdgeStarts,
			k3d::point3* Points,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& FaceFirstLoops) :
		m_edge_starts(EdgeStarts),
		m_points(Points),
		m_index(0),
		m_clockwise_edges(ClockwiseEdges),
		m_loop_first_edges(LoopFirstEdges),
		m_face_first_loops(FaceFirstLoops)
	{}
	void on_vertex(const k3d::point3& Point)
	{
		m_points[m_index++] = Point;
	}
	void on_boundary(const k3d::uint_t Edge)
	{
		m_index = m_edge_starts[Edge];
	}
private:
	const k3d::mesh::indices_t& m_edge_starts;
	k3d::point3* m_points;
	k3d::uint_t m_index;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_face_first_loops;
	k3d::uint_t m_edge;
};

void sds_edge_vbo::update(const k3d::mesh& Mesh, const k3d::uint_t Level, sds_cache& Cache)
{
	assert_not_implemented();
//	if (!m_point_vbo) // new cache -> completely regenerate the VBOs
//	{
//		edge_visitor visitor(*Mesh.polyhedra->clockwise_edges, *Mesh.polyhedra->loop_first_edges, *Mesh.polyhedra->face_first_loops);
//		Cache.visit_boundary(Level, visitor);
//		
//		m_point_vbo = new vbo();
//		glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.points_array[0]) * visitor.points_array.size(), &visitor.points_array[0], GL_STATIC_DRAW);
//		
//		edge_starts = visitor.edge_starts;
//		index_size = visitor.points_array.size();
//	}
//	else // Only update the point VBO with new positions
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
//		k3d::point3* points = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
//		
//		update_edge_vbo_visitor visitor(edge_starts, points, *Mesh.polyhedra->clockwise_edges, *Mesh.polyhedra->loop_first_edges, *Mesh.polyhedra->face_first_loops);
//		Cache.visit_boundary(Level, visitor);
//		
//		glUnmapBuffer(GL_ARRAY_BUFFER);
//	}
}

void sds_edge_vbo::bind()
{
	if (!glIsBuffer(*m_point_vbo))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

/////////////
// sds_point_vbo
/////////////

/// Helper class to update the point VBO
class update_point_vbo_visitor : public k3d::sds::ipatch_corner_visitor
{
public:
	update_point_vbo_visitor(k3d::point3* Points,
			const k3d::mesh::indices_t& ClockwiseEdges,
			const k3d::mesh::indices_t& LoopFirstEdges,
			const k3d::mesh::indices_t& FaceFirstLoops,
			const k3d::mesh::indices_t& EdgePoints) :
		m_points(Points),
		m_clockwise_edges(ClockwiseEdges),
		m_loop_first_edges(LoopFirstEdges),
		m_face_first_loops(FaceFirstLoops),
		m_edge_points(EdgePoints)
	{}
	void on_corner(const k3d::point3& Point)
	{
		//m_points[m_edge_points[m_edge]] = Point;
	}
private:
	k3d::point3* m_points;
	const k3d::mesh::indices_t& m_clockwise_edges;
	const k3d::mesh::indices_t& m_loop_first_edges;
	const k3d::mesh::indices_t& m_face_first_loops;
	const k3d::mesh::indices_t& m_edge_points;
	k3d::uint_t m_edge;
};

void sds_point_vbo::update(const k3d::mesh& Mesh, const k3d::uint_t Level, sds_cache& Cache)
{
	assert_not_implemented();
//	if (!m_point_vbo) // new cache -> completely regenerate the VBOs
//	{
//		point_visitor visitor(*Mesh.polyhedra->clockwise_edges, *Mesh.polyhedra->edge_points, *Mesh.polyhedra->loop_first_edges, *Mesh.polyhedra->face_first_loops, Mesh.points->size());
//		Cache.visit_corners(visitor, Level, false);
//		
//		m_point_vbo = new vbo();
//		glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(visitor.points_array[0]) * visitor.points_array.size(), &visitor.points_array[0], GL_STATIC_DRAW);
//		
//		index_size = visitor.points_array.size();
//	}
//	else // Only update the point VBO with new positions
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
//		k3d::point3* points = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
//		
//		update_point_vbo_visitor visitor(points, *Mesh.polyhedra->clockwise_edges, *Mesh.polyhedra->loop_first_edges, *Mesh.polyhedra->face_first_loops, *Mesh.polyhedra->edge_points);
//		Cache.visit_corners(visitor, Level, true);
//		
//		glUnmapBuffer(GL_ARRAY_BUFFER);
//	}
}

void sds_point_vbo::bind()
{
	if (!glIsBuffer(*m_point_vbo))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, *m_point_vbo);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

///////////////
// Convenience functions
//////////////

void clean_vbo_state()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_FOG_COORDINATE_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace painters

} // namespace opengl

} // namespace module
