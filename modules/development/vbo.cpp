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



#include "vbo.h"

#include <k3dsdk/hints.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/any.hpp>

#include <map>

namespace module
{

namespace development
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

////////
// class component_selection
///////

void component_selection::on_execute(const k3d::mesh& Mesh)
{
	if (!m_selection_array)
		on_schedule(Mesh, 0);
	const k3d::mesh::selection_t& selection_array = *m_selection_array;
	for (size_t i = 0; i < selection_array.size();)
	{
		size_t start = i;
		k3d::mesh_selection::record record(start, i+1, selection_array[i]);
		while (i < selection_array.size() && record.weight == selection_array[i])
		{
			record.end = i+1;
			++i;
		}
		m_selection_records.push_back(record);
	}
}

///////////
// class point_vbo
///////////

void point_vbo::bind()
{
	return_if_fail(glIsBuffer(*m_vbo));
	glBindBuffer(GL_ARRAY_BUFFER, *m_vbo);	
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void point_vbo::on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
{
	k3d::hint::mesh_geometry_changed_t* geometry_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
	if (!geometry_hint)
	{
		delete m_vbo;
		m_vbo = 0;
		m_indices.clear();
	}
	else if (m_indices.empty()) // Only set indices once (they are cleared upon execute()
	{
		m_indices = geometry_hint->changed_points;
	}
}

void point_vbo::on_execute(const k3d::mesh& Mesh)
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
		k3d::log() << debug << "VBO: Creating new point VBO. Empty buffer: " << m_indices.empty() << ", new_vbo: " << new_vbo << std::endl;
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
	return_if_fail(glIsBuffer(*m_vbo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_vbo);
}

void edge_vbo::on_execute(const k3d::mesh& Mesh)
{
	return_if_fail(k3d::validate_polyhedra(Mesh));
	delete m_vbo;
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

//////////
// class face
/////////

face::face( ) :
	m_smooth_polys(0),
	m_smooth_quads(0),
	m_smooth_tris(0),
	m_flat_polys(0),
	m_flat_quads(0),
	m_flat_tris(0),
	m_flat_points(0),
	m_smooth_normals(0),
	m_flat_normals(0)
{
}

face::~ face( )
{
	glDeleteBuffers(1, &m_flat_tris);
	glDeleteBuffers(1, &m_flat_quads);
	glDeleteBuffers(1, &m_flat_polys);
	glDeleteBuffers(1, &m_smooth_tris);
	glDeleteBuffers(1, &m_smooth_quads);
	glDeleteBuffers(1, &m_smooth_polys);
	glDeleteBuffers(1, &m_flat_points);
	glDeleteBuffers(1, &m_smooth_normals);
	glDeleteBuffers(1, &m_flat_normals);
}

typedef std::vector< std::pair<size_t, bool> > corners_t;

// helper function for face::convert
void insert_face(const size_t Face, const bool DuplicatePoints,
								 const corners_t& FaceCorners,
								 const k3d::mesh::points_t& Points,
								 k3d::mesh::points_t& FlatPoints,
								 k3d::typed_array<GLuint>& SmoothIndices,
								 k3d::typed_array<GLuint>& FlatIndices,
								 indexmap_t& SmoothMap, indexmap_t& FlatMap,
								 indexmultimap_t& FlatToSmooth,
								 indexmultimap_t& SmoothToFlat,
								 indexmultimap_t& PositionLink,
								 std::vector<size_t>* SmoothPolyCounts = 0,
								 std::vector<size_t>* FlatPolyCounts = 0)
{
	size_t corner_count = FaceCorners.size();
	if (!DuplicatePoints) // face had no sharp edges
	{
		SmoothMap[Face] = SmoothPolyCounts ? SmoothPolyCounts->size() : SmoothIndices.size()/FaceCorners.size();
		for (size_t i = 0; i != corner_count; ++i)
			SmoothIndices.push_back(FaceCorners[i].first);
		if (SmoothPolyCounts)
			SmoothPolyCounts->push_back(corner_count);
	}
	else // face had sharp edges: duplicate points and link up points that are smooth with the smooth VBO
	{
		FlatMap[Face] = FlatPolyCounts ? FlatPolyCounts->size() : FlatIndices.size()/FaceCorners.size();
		for (size_t i = 0; i != corner_count; ++i)
		{
			FlatPoints.push_back(Points[FaceCorners[i].first]); // Add a copy of the point position to the flat point list
			PositionLink.insert(std::make_pair(FaceCorners[i].first, FlatPoints.size() - 1)); // Add link to original point
			if (FaceCorners[i].second) // smooth point of a flat face -> keep normals linked with original mesh
			{
				FlatToSmooth.insert(std::make_pair(FlatPoints.size()-1, FaceCorners[i].first));
				SmoothToFlat.insert(std::make_pair(FaceCorners[i].first, FlatPoints.size()-1));
			}
			FlatIndices.push_back(FlatPoints.size() - 1); // update index buffer for flat polygons
		}
		if (FlatPolyCounts)
			FlatPolyCounts->push_back(corner_count);
	}
}

// helper function to calculate normals (placed in Mormals and LinkedNormals) in face::convert
void calculate_normals(std::vector<k3d::normal3>& Normals, std::vector<k3d::normal3> LinkedNormals, const k3d::mesh::points_t& Points, const k3d::typed_array<GLuint>& FaceIndices, std::vector<size_t>* FaceCornerCounts, size_t FaceCornerCount, indexmultimap_t& PointLinks)
{
	size_t face = 0;
	for (size_t face_corner = 0; face_corner != FaceIndices.size(); )
	{
		size_t corner_count = FaceCornerCount;
		if (FaceCornerCounts) // polygons, so number of corners can vary per face
		{
			return_if_fail(face < FaceCornerCounts->size());
			corner_count = FaceCornerCounts->at(face);
		}
		size_t last_corner = face_corner + corner_count;
		k3d::normal3 face_normal(0.0,0.0,0.0);
		for (size_t corner = face_corner; corner != last_corner; ++corner)
		{ // normal calculation routine shamelessly ripped from k3d::normal
			const k3d::point3& i = Points[FaceIndices[corner]];
			size_t next_corner = corner + 1;
			if (next_corner == last_corner) // wrap around
				next_corner = face_corner;
			const k3d::point3& j = Points[FaceIndices[next_corner]];

			face_normal[0] += (i[1] + j[1]) * (j[2] - i[2]);
			face_normal[1] += (i[2] + j[2]) * (j[0] - i[0]);
			face_normal[2] += (i[0] + j[0]) * (j[1] - i[1]);
		}
		face_normal /= face_normal.length();
		++face;
		for (; face_corner != last_corner; ++face_corner)
		{
			Normals[FaceIndices[face_corner]] += face_normal;
			indexmultimap_t::iterator linked_points = PointLinks.find(FaceIndices[face_corner]);
			if (linked_points != PointLinks.end()) // Update corresponding normals in the other set if a link is defined
			{
				indexmultimap_t::iterator endpoint = PointLinks.upper_bound(FaceIndices[face_corner]);
				for (; linked_points != endpoint; ++linked_points)
				{
					LinkedNormals[linked_points->second] += face_normal;
				}
			}
		}
	}
}

void face::convert(const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t & Polyhedra )
{
	const k3d::mesh::indices_t& face_first_loops = *Polyhedra.face_first_loops;
	const k3d::mesh::indices_t& loop_first_edges = *Polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& clockwise_edges = *Polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *Polyhedra.edge_points;
	
	m_smooth_trimap.clear();
	m_smooth_quadmap.clear();
	m_smooth_polymap.clear();
	m_flat_trimap.clear();
	m_flat_quadmap.clear();
	m_flat_polymap.clear();
	
	k3d::typed_array<GLuint> smooth_polys, smooth_quads, smooth_tris, flat_polys, flat_quads, flat_tris; // face corner indices into Points and flat_points
	std::set<size_t> duplicated_points; // Keep track of what points have been added to the flat point list
	k3d::mesh::points_t flat_points; // Growing buffer of points belonging to flat faces

	const size_t face_count = face_first_loops.size();
	for(size_t face = 0; face != face_count; ++face)
	{
		corners_t face_corners; // all face corners
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		bool duplicate_points = false;
		for(size_t edge = first_edge; ; )
		{
			bool sharp = is_sharp(edge, Points, Polyhedra);
			size_t point1 = edge_points[edge]; // first point of the edge
			size_t point2 = edge_points[clockwise_edges[edge]]; // second point of the edge
			std::pair<size_t, bool> point = std::make_pair(point1, false);
			if (sharp)
			{
				bool duplicated = (duplicated_points.find(point1) != duplicated_points.end() && duplicated_points.find(point2) != duplicated_points.end());
				if (!duplicated)
				{
					duplicated_points.insert(point1);
					duplicated_points.insert(point2);
				}
				if (!duplicate_points)
					duplicate_points = duplicated;
			}
			point.second = (!sharp && !is_sharp(clockwise_edges[edge], Points, Polyhedra));
			face_corners.push_back(point);
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		if (face_corners.size() == 3)
		{
			insert_face(face, duplicate_points, face_corners, Points, flat_points, smooth_tris, flat_tris, m_smooth_trimap, m_flat_trimap, m_flat_to_smooth, m_smooth_to_flat, m_position_link);
		}
		else if (face_corners.size() == 4)
		{
			insert_face(face, duplicate_points, face_corners, Points, flat_points, smooth_quads, flat_quads, m_smooth_quadmap, m_flat_quadmap, m_flat_to_smooth, m_smooth_to_flat, m_position_link);
		}
		else
		{
			insert_face(face, duplicate_points, face_corners, Points, flat_points, smooth_polys, flat_polys, m_smooth_polymap, m_flat_polymap, m_flat_to_smooth, m_smooth_to_flat, m_position_link, &m_smooth_polycounts, &m_flat_polycounts);
		}
	}
	
	// Clear old VBO's
	glDeleteBuffers(1, &m_flat_tris);
	glDeleteBuffers(1, &m_flat_quads);
	glDeleteBuffers(1, &m_flat_polys);
	glDeleteBuffers(1, &m_smooth_tris);
	glDeleteBuffers(1, &m_smooth_quads);
	glDeleteBuffers(1, &m_smooth_polys);
	glDeleteBuffers(1, &m_flat_points);
	glDeleteBuffers(1, &m_smooth_normals);
	glDeleteBuffers(1, &m_flat_normals);
	
	// Point VBO for flat faces
	if (!flat_points.empty())
	{
		glGenBuffers(1, &m_flat_points);
		glBindBuffer(GL_ARRAY_BUFFER, m_flat_points);
		glBufferData(GL_ARRAY_BUFFER, sizeof(flat_points[0]) * flat_points.size(), &flat_points[0], GL_STATIC_DRAW);
	}
	
	// VBO's for face indices and normals
	std::vector<k3d::normal3> smooth_normals(Points.size(), k3d::normal3(0.0,0.0,0.0));
	std::vector<k3d::normal3> flat_normals(flat_points.size(), k3d::normal3(0.0,0.0,0.0));
	if (!smooth_tris.empty())
	{
		// indices
		glGenBuffers(1, &m_smooth_tris);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_tris);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(smooth_tris[0]) * smooth_tris.size(), &smooth_tris[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(smooth_normals, flat_normals, Points, smooth_tris, 0, 3, m_smooth_to_flat);
	}
	if (!smooth_quads.empty())
	{
		// indices
		glGenBuffers(1, &m_smooth_quads);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_quads);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(smooth_quads[0]) * smooth_quads.size(), &smooth_quads[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(smooth_normals, flat_normals, Points, smooth_quads, 0, 4, m_smooth_to_flat);
	}
	if (!smooth_polys.empty())
	{
		// indices
		glGenBuffers(1, &m_smooth_polys);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_polys);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(smooth_polys[0]) * smooth_polys.size(), &smooth_polys[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(smooth_normals, flat_normals, Points, smooth_polys, &m_smooth_polycounts, 0, m_smooth_to_flat);
	}
	if (!flat_tris.empty())
	{
		// indices
		glGenBuffers(1, &m_flat_tris);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_tris);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_tris[0]) * flat_tris.size(), &flat_tris[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(flat_normals, smooth_normals, flat_points, flat_tris, 0, 3, m_flat_to_smooth);
	}
	if (!flat_quads.empty())
	{
		glGenBuffers(1, &m_flat_quads);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_quads);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_quads[0]) * flat_quads.size(), &flat_quads[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(flat_normals, smooth_normals, flat_points, flat_quads, 0, 4, m_flat_to_smooth);
	}
	if (!flat_polys.empty())
	{
		glGenBuffers(1, &m_flat_polys);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_polys);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flat_polys[0]) * flat_polys.size(), &flat_polys[0], GL_STATIC_DRAW);
		// normals
		calculate_normals(flat_normals, smooth_normals, flat_points, flat_polys, &m_flat_polycounts, 0, m_flat_to_smooth);


	}
	// Generate VBOs for the normals
	if (!smooth_normals.empty())
	{
		glGenBuffers(1, &m_smooth_normals);
		glBindBuffer(GL_ARRAY_BUFFER, m_smooth_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(smooth_normals[0]) * smooth_normals.size(), &smooth_normals[0], GL_STATIC_DRAW);
	}
	if (!flat_normals.empty())
	{
		glGenBuffers(1, &m_flat_normals);
		glBindBuffer(GL_ARRAY_BUFFER, m_flat_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(flat_normals[0]) * flat_normals.size(), &flat_normals[0], GL_STATIC_DRAW);
	}
}

void face::draw_smooth_range( const size_t Start, const size_t End ) const
{
	return_if_fail(End >= Start);
	
	if (!glIsBuffer(m_smooth_normals))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, m_smooth_normals); // activate normals VBO
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, 0);

	if (!m_smooth_trimap.empty() && glIsBuffer(m_smooth_tris))
	{
		range_t range = local_indices(Start, End, m_smooth_trimap);
		if (range.first < range.second)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_tris);
			glDrawElements(GL_TRIANGLES, 3*(range.second - range.first), GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 3*range.first);
		}
	}
	if (!m_smooth_quadmap.empty() && glIsBuffer(m_smooth_quads))
	{
		range_t range = local_indices(Start, End, m_smooth_quadmap);
		if (range.first < range.second)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_quads);
			glDrawElements(GL_QUADS, 4*(range.second - range.first), GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 4*range.first);
		}
	}
	if (!m_smooth_polymap.empty() && glIsBuffer(m_smooth_polys))
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_polys);
		range_t range = local_indices(Start, End, m_smooth_polymap);
		if (range.first < range.second)
		{
			int current_face = 0;
			if (range.first != 0) // we need to add polygon sizes to get the real start index for the VBO
			{
				for (size_t i = 0; i != range.first; ++i)
					current_face += m_smooth_polycounts[i];
			}
			for (size_t i = range.first; i < range.second; ++i)
			{
				glDrawElements(GL_POLYGON, m_smooth_polycounts[i], GL_UNSIGNED_INT, static_cast<GLuint*>(0) + current_face);
				current_face += m_smooth_polycounts[i];
			}
		}
	}
	glDisableClientState(GL_NORMAL_ARRAY);
}

void face::draw_flat_range( const size_t Start, const size_t End ) const
{
	if (!glIsBuffer(m_flat_normals))
		return;
	glBindBuffer(GL_ARRAY_BUFFER, m_flat_normals); // activate normals VBO
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, 0, 0);

	if (!m_flat_trimap.empty() && glIsBuffer(m_flat_tris))
	{
		range_t range = local_indices(Start, End, m_flat_trimap);
		if (range.first < range.second)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_tris);
			glDrawElements(GL_TRIANGLES, 3*(range.second - range.first), GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 3*range.first);
		}
	}
	if (!m_flat_quadmap.empty() && glIsBuffer(m_flat_quads))
	{
		range_t range = local_indices(Start, End, m_flat_quadmap);
		if (range.first < range.second)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_quads);
			glDrawElements(GL_QUADS, 4*(range.second - range.first), GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 4*range.first);
		}
	}
	if (!m_flat_polymap.empty() && glIsBuffer(m_flat_polys))
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_polys);
		range_t range = local_indices(Start, End, m_flat_polymap);
		if (range.first < range.second)
		{
			int current_face = 0;
			if (range.first != 0) // we need to add polygon sizes to get the real start index for the VBO
			{
				for (size_t i = 0; i != range.first; ++i)
					current_face += m_flat_polycounts[i];
			}
			for (size_t i = range.first; i < range.second; ++i)
			{
				glDrawElements(GL_POLYGON, m_flat_polycounts[i], GL_UNSIGNED_INT, static_cast<GLuint*>(0) + current_face);
				current_face += m_flat_polycounts[i];
			}
		}
	}
	glDisableClientState(GL_NORMAL_ARRAY);
}

bool face::bind_flat_points( ) const
{
	bool result = glIsBuffer(m_flat_points);
	if (result)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_flat_points);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
	}
	return result;
}

void face::draw_selection( ) const
{
	// smooth faces:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_tris);
	for(indexmap_t::const_iterator face = m_smooth_trimap.begin(); face != m_smooth_trimap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 3*face->second);

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_quads);
	for(indexmap_t::const_iterator face = m_smooth_quadmap.begin(); face != m_smooth_quadmap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 4*face->second);

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_smooth_polys);
	size_t current_face = 0;
	for(indexmap_t::const_iterator face = m_smooth_polymap.begin(); face != m_smooth_polymap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_POLYGON, m_smooth_polycounts[face->second], GL_UNSIGNED_INT, static_cast<GLuint*>(0) + current_face);
		current_face += m_smooth_polycounts[face->second];

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
	
	// flat faces:
	if (!bind_flat_points())
		return;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_tris);
	for(indexmap_t::const_iterator face = m_flat_trimap.begin(); face != m_flat_trimap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 3*face->second);

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_quads);
	for(indexmap_t::const_iterator face = m_flat_quadmap.begin(); face != m_flat_quadmap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(0) + 4*face->second);

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flat_polys);
	current_face = 0;
	for(indexmap_t::const_iterator face = m_flat_polymap.begin(); face != m_flat_polymap.end(); ++face)
	{
		k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_FACE, face->first);

		glDrawElements(GL_POLYGON, m_flat_polycounts[face->second], GL_UNSIGNED_INT, static_cast<GLuint*>(0) + current_face);
		current_face += m_flat_polycounts[face->second];

		k3d::gl::pop_selection_token(); // ABSOLUTE_FACE
	}
}

void face::on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
{
	k3d::hint::mesh_geometry_changed_t* geometry_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
	if (!geometry_hint)
		return;
	if (m_indices.empty()) // Only set indices once (they are cleared upon execute()
	{
		m_indices = geometry_hint->changed_points;
	}
}

void face::on_execute(const k3d::mesh& Mesh)
{
	if (!glIsBuffer(m_smooth_normals) || !glIsBuffer(m_flat_normals))
		convert(*Mesh.points, *Mesh.polyhedra);
	if (m_indices.empty())
	{
		size_t n = Mesh.points->size();
		for (size_t i = 0; i != n; ++i)
			m_indices.push_back(i);
	}
	update(*Mesh.points, *Mesh.polyhedra, m_indices);
	m_indices.clear();
}

void face::update( const k3d::mesh::points_t & Points, const k3d::mesh::polyhedra_t & Polyhedra, const k3d::mesh::indices_t & TransformedPoints )
{
	typedef std::map<size_t, k3d::normal3> normal_map_t;
	
	const k3d::mesh::indices_t& face_first_loops = *Polyhedra.face_first_loops;
	const k3d::mesh::indices_t& loop_first_edges = *Polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& clockwise_edges = *Polyhedra.clockwise_edges;
	const k3d::mesh::indices_t& edge_points = *Polyhedra.edge_points;
	
	if (m_last_transformed_points.size() != TransformedPoints.size() || m_last_transformed_points != TransformedPoints) // check if points changed
 	{
		m_affected_faces.clear();
		m_affected_points.clear();
		get_deformed_faces(Polyhedra, TransformedPoints, m_affected_faces, m_affected_points);
		m_affected_smooth_corners.clear();
		m_affected_flat_corners.clear();
		if (glIsBuffer(m_smooth_normals))
			get_face_points(m_affected_smooth_corners, false);
		if (glIsBuffer(m_flat_normals))
			get_face_points(m_affected_flat_corners, true);
		m_last_transformed_points = TransformedPoints;
 	}

	// Calculate the per-face normals for all the affected faces and put them in a map
	normal_map_t face_normals;
	for (indexlist_t::iterator face = m_affected_faces.begin(); face != m_affected_faces.end(); ++face)
	{
		face_normals.insert(std::make_pair(*face, k3d::normal(edge_points, clockwise_edges, Points, loop_first_edges[face_first_loops[*face]])));
	}

	// update positions of the "flat" points
	if (glIsBuffer(m_flat_points))
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_flat_points);
		k3d::point3* flat_positions = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for (size_t i = 0; i != TransformedPoints.size(); ++i)
		{
			indexmultimap_t::iterator linked_points = m_position_link.find(TransformedPoints[i]);
			if (linked_points != m_position_link.end()) // Update corresponding point in the other set if a link is defined
			{
				indexmultimap_t::iterator endpoint = m_position_link.upper_bound(TransformedPoints[i]);
				for (; linked_points != endpoint; ++linked_points)
				{
					flat_positions[linked_points->second] = Points[TransformedPoints[i]];
				}
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	// Update smooth normals
	normal_map_t linked_flat_normals; // must be stored separately, we can't map both the smooth and the flat normals at the same time
	if (glIsBuffer(m_smooth_normals))
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_smooth_normals);
		k3d::normal3* smooth_normals = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		// Reset to 0 first
		for (indexlist_t::iterator face = m_affected_faces.begin(); face != m_affected_faces.end(); ++face)
		{
			std::vector<size_t>& corners = m_affected_smooth_corners[*face];
			for (std::vector<size_t>::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				smooth_normals[*corner] = k3d::normal3(0.0,0.0,0.0);
			}
		}
		// Add new value
		for (indexlist_t::iterator face = m_affected_faces.begin(); face != m_affected_faces.end(); ++face)
		{
			std::vector<size_t>& corners = m_affected_smooth_corners[*face];
			for (std::vector<size_t>::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				smooth_normals[*corner] += face_normals[*face]; // smooth normal update
				indexmultimap_t::iterator links = m_smooth_to_flat.find(*corner);
				if (links != m_smooth_to_flat.end()) // update link, if any
				{
					indexmultimap_t::iterator links_end = m_smooth_to_flat.upper_bound(*corner);
					for (indexmultimap_t::iterator link = links; link != links_end; ++link)
						linked_flat_normals[link->second] += face_normals[*face];
				}
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	// Update flat normals
	normal_map_t linked_smooth_normals; // must be stored separately, we can't map both the flat and the smooth normals at the same time
	if (glIsBuffer(m_flat_normals))
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_flat_normals);
		k3d::normal3* flat_normals = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		// Reset to 0 first
		for (indexlist_t::iterator face = m_affected_faces.begin(); face != m_affected_faces.end(); ++face)
		{
			std::vector<size_t>& corners = m_affected_flat_corners[*face];
			for (std::vector<size_t>::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				flat_normals[*corner] = k3d::normal3(0.0,0.0,0.0);
			}
		}
		// Add new value
		for (indexlist_t::iterator face = m_affected_faces.begin(); face != m_affected_faces.end(); ++face)
		{
			std::vector<size_t>& corners = m_affected_flat_corners[*face];
			for (std::vector<size_t>::iterator corner = corners.begin(); corner != corners.end(); ++corner)
			{
				flat_normals[*corner] += face_normals[*face]; // flat normal update
				indexmultimap_t::iterator links = m_flat_to_smooth.find(*corner);
				if (links != m_flat_to_smooth.end()) // update link, if any
				{
					indexmultimap_t::iterator links_end = m_flat_to_smooth.upper_bound(*corner);
					for (indexmultimap_t::iterator link = links; link != links_end; ++link)
						linked_smooth_normals[link->second] += face_normals[*face];
				}
			}
		}
		// Update linked normals
		for (normal_map_t::const_iterator link = linked_flat_normals.begin(); link != linked_flat_normals.end(); ++link)
			flat_normals[link->first] += link->second;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	// Update linked smooth normals
	if (glIsBuffer(m_smooth_normals) && !linked_smooth_normals.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_smooth_normals);
		k3d::normal3* smooth_normals = static_cast<k3d::normal3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for (normal_map_t::const_iterator link = linked_smooth_normals.begin(); link != linked_smooth_normals.end(); ++link)
			smooth_normals[link->first] += link->second;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

face::range_t face::local_indices( size_t Start, size_t End, const indexmap_t & Map ) const
{
	indexmap_t::const_iterator start_it = Map.lower_bound(Start);
	if (start_it->first <= End)
	{
		indexmap_t::const_iterator end_it = Map.lower_bound(End);
		if (end_it == Map.end() || end_it->first > End)
		{
			--end_it;
		}
		return std::make_pair(start_it->second, end_it->second + 1);
	}
	return std::make_pair(0, 0);
}

// YAHF (Yet Another Helper Function), to avoid too much copy/paste. Puts the point indices of all Faces into Corners, using IndexMap, VBO and CornerCount
void get_face_corners(const indexlist_t& Faces, std::map<size_t, std::vector<size_t> >& Corners, indexmap_t IndexMap, GLuint VBO, size_t CornerCount, std::vector<size_t>* PolyCounts = 0)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
	const GLuint* face_indices = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY));
	for (indexlist_t::const_iterator face = Faces.begin(); face != Faces.end(); ++face)
	{
		indexmap_t::iterator real_face = IndexMap.find(*face);
		if (real_face != IndexMap.end())
		{
			std::vector<size_t> face_corners;
			size_t corner_count = PolyCounts ? PolyCounts->at(real_face->second) : CornerCount;
			size_t first_corner = CornerCount * real_face->second;
			if (PolyCounts)
			{
				first_corner = 0;
				for (size_t i = 0; i != real_face->second; ++i)
					first_corner += PolyCounts->at(i);
			}
			size_t next_corner = first_corner + corner_count;
			for (size_t corner = first_corner; corner != next_corner; ++corner)
			{
				face_corners.push_back(face_indices[corner]);
			}
			Corners.insert(std::make_pair(*face, face_corners));
		}
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void face::get_face_points(global_corners_t& Corners, bool Flat)
{
	if (!Flat)
	{
		if (glIsBuffer(m_smooth_tris))
		{
			get_face_corners(m_affected_faces, Corners, m_smooth_trimap, m_smooth_tris, 3);
		}
		if (glIsBuffer(m_smooth_quads))
		{
			get_face_corners(m_affected_faces, Corners, m_smooth_quadmap, m_smooth_quads, 4);
		}
		if (glIsBuffer(m_smooth_polys))
		{
			get_face_corners(m_affected_faces, Corners, m_smooth_polymap, m_smooth_polys, 0, &m_smooth_polycounts);
		}
	}
	else
	{
		if (glIsBuffer(m_flat_tris))
		{
			get_face_corners(m_affected_faces, Corners, m_flat_trimap, m_flat_tris, 3);
		}
		if (glIsBuffer(m_flat_quads))
		{
			get_face_corners(m_affected_faces, Corners, m_flat_quadmap, m_flat_quads, 4);
		}
		if (glIsBuffer(m_flat_polys))
		{
			get_face_corners(m_affected_faces, Corners, m_flat_polymap, m_flat_polys, 0, &m_flat_polycounts);
		}
	}
}

//////////
// flat_face
///////////

bool flat_face::is_sharp( const size_t Edge, const k3d::mesh::points_t & Points, const k3d::mesh::polyhedra_t & Polyhedra )
{
	return true;
}

///////////
// smooth face
///////////

bool smooth_face::is_sharp( const size_t Edge, const k3d::mesh::points_t & Points, const k3d::mesh::polyhedra_t & Polyhedra )
{
	return false;
}

//////////
// edge_face
/////////

typedef k3d::typed_array<bool> sharpness_array_t;

bool edge_face::is_sharp( const size_t Edge, const k3d::mesh::points_t & Points, const k3d::mesh::polyhedra_t & Polyhedra )
{
	k3d::mesh::named_arrays::const_iterator array_it = Polyhedra.face_varying_data.find("N");
	if (array_it ==  Polyhedra.face_varying_data.end())
		return false;
	if (!dynamic_cast<sharpness_array_t*>(array_it->second.get()))
		return false;
	
	sharpness_array_t& sharpness_array = *dynamic_cast<sharpness_array_t*>(array_it->second.get());
	return sharpness_array[Edge];
}

////////////
// sds_cache
/////////////

sds_cache::~sds_cache()
{
	// disconnect these, so they no longer point into freed memory
	for (size_t i = 0; i != m_connections.size(); ++i)
		m_connections[i].disconnect();
}

void sds_cache::level_changed()
{
	// search the highest level requested by the clients
	levels = 0;
	for (sds_cache::levels_t::iterator level_it = m_levels.begin(); level_it != m_levels.end(); ++level_it)
	{
		const long new_level = boost::any_cast<const long>((*level_it)->property_value());
		levels = new_level > levels ? new_level : levels;
	}
	k3d::log() << debug << "New maximum level: " << levels << std::endl;
	m_scheduled = true;
}

void sds_cache::register_property(k3d::iproperty* LevelProperty)
{
	if (m_levels.insert(LevelProperty).second)
	{
		m_connections.push_back(LevelProperty->property_deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &sds_cache::remove_property), LevelProperty)));
		level_changed();
	}
}

void sds_cache::remove_property(k3d::iproperty* LevelProperty)
{
	m_levels.erase(LevelProperty);
}

////////////
// sds_vbo_cache
/////////////

void sds_vbo_cache::on_execute(const k3d::mesh& Mesh)
{
	if (levels > 0 || regenerate)
	{
		k3d::log() << debug << "SDS: Setting new level to " << levels << std::endl;
		cache.set_input(&Mesh);
		cache.set_levels(levels);
		update = true;
		update_selection = true;
		regenerate = true;
	}
	cache.set_new_addresses(Mesh);
	if (update)
	{
		cache.update();
	}
	if (regenerate)
	{
		cache.regenerate_vbos();
	}
	if (update)
	{
		cache.update_vbo_positions();
	}
	if (update_selection)
	{
		cache.update_selection();
		if (!update)
			cache.clear_modified_faces();
	}
	
	regenerate = false;
	update = false;
	update_selection = false;
	levels = 0;
}

////////////
// sds_gl_cache
/////////////

void sds_gl_cache::on_execute(const k3d::mesh& Mesh)
{
	cache.set_new_addresses(Mesh);
	if (levels > 0)
	{
		k3d::log() << debug << "SDS: Setting new level to " << levels << std::endl;
		cache.set_input(&Mesh);
		cache.set_levels(levels);
		update = true;
	}
	if (update)
	{
		cache.update();
		cache.clear_modified_faces();
	}
	update = false;
	levels = 0;
}

///////////////
// Convenience functions
//////////////

void array_to_selection( const k3d::mesh::selection_t & SelectionArray, selection_records_t& Selection )
{
	Selection.clear();
	for (size_t i = 0; i < SelectionArray.size();)
	{
		size_t start = i;
		k3d::mesh_selection::record record(start, i+1, SelectionArray[i]);
		while (record.weight == SelectionArray[i] && i < SelectionArray.size())
		{
			record.end = i+1;
			++i;
		}
		Selection.push_back(record);
	}
}

void get_deformed_faces(const k3d::mesh::polyhedra_t& Polyhedra, const k3d::mesh::indices_t& Points, indexlist_t& AffectedFaces, std::set<size_t>& AffectedPoints)
{
	const k3d::mesh::indices_t& face_first_loops = *Polyhedra.face_first_loops;
	const k3d::mesh::indices_t& loop_first_edges = *Polyhedra.loop_first_edges;
	const k3d::mesh::indices_t& edge_points = *Polyhedra.edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Polyhedra.clockwise_edges;

	std::set<size_t> moving_points; // faster lookup of concerned points
	moving_points.insert(Points.begin(), Points.end());

	const size_t face_count = face_first_loops.size();
	// get points belonging to deformed or transformed faces
	for(size_t face = 0; face != face_count; ++face)
	{
		bool selected = false;
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		indexlist_t face_points;
		for(size_t edge = first_edge; ; )
		{
			if (!selected && moving_points.find(edge_points[edge]) != moving_points.end())
			{
				selected = true;
			}
			face_points.push_back(edge_points[edge]);
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
		if (selected)
		{
			AffectedPoints.insert(face_points.begin(), face_points.end());
		}
	}
	
	// get faces having AffectedPoints
	for(size_t face = 0; face != face_count; ++face)
	{
		const size_t first_edge = loop_first_edges[face_first_loops[face]];
		for(size_t edge = first_edge; ; )
		{
			if (AffectedPoints.find(edge_points[edge]) != AffectedPoints.end())
			{
				AffectedFaces.push_back(face);
				break;
			}
			edge = clockwise_edges[edge];
			if(edge == first_edge)
				break;
		}
	}	
}

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

} // namespace development

} // namespace module
