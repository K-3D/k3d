// K-3D SDS preview
// Copyright (c) 2005, Bart Janssens
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

/** \file Functions for SDS.
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/


#include "subdivision_algorithms.h"

//#include <k3dsdk/high_res_timer.h>

#include <cmath>

namespace k3d
{

namespace sds
{

	
const double pi = 3.14159265358979323846;

/////////
// sds_point
//////////

void sds_point::update()
{
	if(m_valid)
		return;
	double n = static_cast<double>(corners.size());
	if(n != face_vertices.size()) // corner lies on edge. Position is updated from edge_vertex.
		return;
	position_t corners_pos(0,0,0);
	for(sdspoints_t::iterator it = corners.begin(); it != corners.end(); ++it)
		corners_pos += (*it)->original;
	position_t faces_pos(0,0,0);
	for(facevertices_t::iterator it = face_vertices.begin(); it != face_vertices.end(); ++it)
		faces_pos += (*it)->vertex;
	double nninv = 1/(n*n);
	vertex = (n-2.0)/n * (original) + nninv * corners_pos + nninv * faces_pos;
	m_valid = true;
}

////////
// edge_vertex
////////

void edge_vertex::update()
{
	if(m_valid && face_vertices[1]) // mesh border needs updating beyond the selection, so we just recalculate all of them
		return;
	return_if_fail(corners[1]);
	vertex = face_vertices[1] ? (corners[0]->original + corners[1]->original + face_vertices[0]->vertex + face_vertices[1]->vertex)/4.0 : (corners[0]->original + corners[1]->original)/2.0;
	// edge of object
	if(!face_vertices[1])
	{
		corners[0]->add_border_position((corners[0]->original + vertex)/4);
		corners[1]->add_border_position((corners[1]->original + vertex)/4);
	}
	m_valid = true;
}

///////
// sds_mipmap
///////

sds_mipmap::sds_mipmap(patch_border & Top, patch_border & Right, patch_border & Bottom, patch_border & Left, const point_array & ParentPoints, int Level, int Size) : m_top(Top), m_right(Right), m_bottom(Bottom), m_left(Left), m_parent_points(ParentPoints), m_level(Level), m_size(Size), m_valid(false), m_normals_valid(false), m_nurbs_valid(false), m_points(boost::extents[Size][Size]), m_normals(boost::extents[Size][Size]), m_nurbs(boost::extents[Size+4][Size+4]), m_next_level(0)
{
	// initialise interior points and normals
	for(int i = 1; i < m_size-1; ++i)
	{
		for(int j = 1; j < m_size-1; ++j)
		{
			m_points[i][j] = new position_t(0.0,0.0,0.0);
			m_normals[i][j] = new position_t(0.0,0.0,0.0);
		}
	}
	int psize = (m_size-1)/2;

	// check index integrity
	return_if_fail(m_left.at(0, m_parent_points[0][0]) == m_bottom.at(0, m_parent_points[0][0]));
	return_if_fail(m_left.at(m_size-1, m_parent_points[0][0]) == m_top.at(0, m_parent_points[0][psize]));
	return_if_fail(m_top.at(m_size-1, m_parent_points[0][psize]) == m_right.at(m_size-1, m_parent_points[psize][0]));
	return_if_fail(m_bottom.at(m_size-1, m_parent_points[0][0]) == m_right.at(0, m_parent_points[psize][0]));

	// notify the borders of their new neighbour
	m_left.register_mipmap(this);
	m_top.register_mipmap(this);
	m_right.register_mipmap(this);
	m_bottom.register_mipmap(this);

	// initialise border points and normals
	for(int i = 0; i < m_size; ++i)
	{
		m_points[0][i] = m_left.at(i, m_parent_points[0][0]);
		m_points[i][0] = m_bottom.at(i, m_parent_points[0][0]);
		m_points[m_size-1][i] = m_right.at(i, m_parent_points[psize][0]);
		m_points[i][m_size-1] = m_top.at(i, m_parent_points[0][psize]);

		m_normals[0][i] = m_left.at_normal(i, m_parent_points[0][0]);
		m_normals[i][0] = m_bottom.at_normal(i, m_parent_points[0][0]);
		m_normals[m_size-1][i] = m_right.at_normal(i, m_parent_points[psize][0]);
		m_normals[i][m_size-1] = m_top.at_normal(i, m_parent_points[0][psize]);
	}

	// notify corners of their new face vertices
	m_left.register_face_vertex(m_points[0][0], m_points[1][1]);
	m_bottom.register_face_vertex(m_points[m_size-1][0], m_points[m_size-2][1]);
	m_right.register_face_vertex(m_points[m_size-1][m_size-1], m_points[m_size-2][m_size-2]);
	m_top.register_face_vertex(m_points[0][m_size-1], m_points[1][m_size-2]);

	// initialise NURBS patch
	int n_size = m_size + 4;
	// borders:
	for(int i = 0; i < n_size; ++i)
	{
		m_nurbs[0][i] = m_left.at_nurbs(i, m_points[0][0]);
		m_nurbs[i][0] = m_bottom.at_nurbs(i, m_points[0][0]);
		m_nurbs[n_size-1][i] = m_right.at_nurbs(i, m_points[m_size-1][0]);
		m_nurbs[i][n_size-1] = m_top.at_nurbs(i, m_points[0][m_size-1]);
	}
	if(m_size > 3)
	{
		for(int i = 1; i < n_size-1; ++i)
		{
			m_nurbs[3][i] = new position_t(0,0,0);
			m_nurbs[n_size-4][i] = new position_t(0,0,0);
		}
		for(int i = 4; i < m_size; ++i)
		{
			m_nurbs[i][3] = new position_t(0,0,0);
			m_nurbs[i][n_size-4] = new position_t(0,0,0);
			for(int j = 4; j < m_size; ++j)
			{
				m_nurbs[i][j] = m_points[i-2][j-2];
			}
		}
	}
	else
	{
		m_nurbs[3][3] = new position_t(0,0,0);
		m_nurbs[3][1] = new position_t(0,0,0);
		m_nurbs[3][2] = new position_t(0,0,0);
		m_nurbs[3][n_size-2] = new position_t(0,0,0);
		m_nurbs[3][n_size-3] = new position_t(0,0,0);
	}

	for(int i = 1; i < n_size-1; ++i)
	{
		m_nurbs[1][i] = new position_t(0,0,0);
		m_nurbs[2][i] = new position_t(0,0,0);
		m_nurbs[n_size-2][i] = new position_t(0,0,0);
		m_nurbs[n_size-3][i] = new position_t(0,0,0);
	}
	for(int i = 4; i < m_size; ++i)
	{
		m_nurbs[i][1] = new position_t(0,0,0);
		m_nurbs[i][2] = new position_t(0,0,0);
		m_nurbs[i][n_size-2] = new position_t(0,0,0);
		m_nurbs[i][n_size-3] = new position_t(0,0,0);
	}
	// knot vector
	int k = static_cast<int>(pow(2.0,m_level));
	m_ksize = n_size+4;
	m_knots = new float[m_ksize];
	m_knots[0] = 0.0;
	m_knots[1] = 0.0;
	m_knots[2] = 0.0;
	m_knots[3] = 0.0;
	m_knots[4] = 1.0;
	m_knots[5] = 1.0;
	for(int i = 2; i < k-1; ++i)
		m_knots[i+4] = i;
	m_knots[m_ksize-6] = k-1;
	m_knots[m_ksize-5] = k-1;
	m_knots[m_ksize-4] = k;
	m_knots[m_ksize-3] = k;
	m_knots[m_ksize-2] = k;
	m_knots[m_ksize-1] = k;
	if(m_size == 3)
	{
		m_knots[5] = 2.0;
		m_knots[6] = 3.0;
		m_knots[7] = 4.0;
		m_knots[8] = 4.0;
		m_knots[9] = 4.0;
		m_knots[10] = 4.0;
	}

	// register with corners
	m_left.add11(m_nurbs[0][0], m_nurbs[1][1]);
	m_bottom.add11(m_nurbs[n_size-1][0], m_nurbs[n_size-2][1]);
	m_right.add11(m_nurbs[n_size-1][n_size-1], m_nurbs[n_size-2][n_size-2]);
	m_top.add11(m_nurbs[0][n_size-1], m_nurbs[1][n_size-2]);

	m_left.add12(m_nurbs[0][0], m_nurbs[1][2]);
	m_bottom.add12(m_nurbs[n_size-1][0], m_nurbs[n_size-2][2]);
	m_right.add12(m_nurbs[n_size-1][n_size-1], m_nurbs[n_size-2][n_size-3]);
	m_top.add12(m_nurbs[0][n_size-1], m_nurbs[1][n_size-3]);

	m_left.add21(m_nurbs[0][0], m_nurbs[2][1]);
	m_bottom.add21(m_nurbs[n_size-1][0], m_nurbs[n_size-3][1]);
	m_right.add21(m_nurbs[n_size-1][n_size-1], m_nurbs[n_size-3][n_size-2]);
	m_top.add21(m_nurbs[0][n_size-1], m_nurbs[2][n_size-2]);

	m_left.add14(m_nurbs[0][0], m_nurbs[1][3]);
	m_bottom.add14(m_nurbs[n_size-1][0], m_nurbs[n_size-2][3]);
	m_right.add14(m_nurbs[n_size-1][n_size-1], m_nurbs[n_size-2][n_size-4]);
	m_top.add14(m_nurbs[0][n_size-1], m_nurbs[1][n_size-4]);

	m_left.add41(m_nurbs[0][0], m_nurbs[3][1]);
	m_bottom.add41(m_nurbs[n_size-1][0], m_nurbs[n_size-4][1]);
	m_right.add41(m_nurbs[n_size-1][n_size-1], m_nurbs[n_size-4][n_size-2]);
	m_top.add41(m_nurbs[0][n_size-1], m_nurbs[3][n_size-2]);
}

sds_mipmap::~sds_mipmap()
{
	delete m_next_level;

	for(int i = 1; i < m_size-1; ++i)
	{
		for(int j = 1; j < m_size-1; ++j)
		{
			delete m_points[i][j];
			delete m_normals[i][j];
		}
	}

	delete[] m_knots;

	// delete NURBS patch
	int n_size = m_size + 4;
	if(m_size > 3)
	{
		for(int i = 1; i < n_size-1; ++i)
		{
			delete m_nurbs[3][i];
			delete m_nurbs[n_size-4][i];
		}
		for(int i = 4; i < m_size; ++i)
		{
			delete m_nurbs[i][3];
			delete m_nurbs[i][n_size-4];
		}
	}
	else
	{
		delete m_nurbs[3][3];
		delete m_nurbs[3][1];
		delete m_nurbs[3][2];
		delete m_nurbs[3][n_size-2];
		delete m_nurbs[3][n_size-3];
	}

	for(int i = 1; i < n_size-1; ++i)
	{
		delete m_nurbs[1][i];
		delete m_nurbs[2][i];
		delete m_nurbs[n_size-2][i];
		delete m_nurbs[n_size-3][i];
	}
	for(int i = 4; i < m_size; ++i)
	{
		delete m_nurbs[i][1];
		delete m_nurbs[i][2];
		delete m_nurbs[i][n_size-2];
		delete m_nurbs[i][n_size-3];
	}
}

void sds_mipmap::update(int Level)
{
	if(Level > m_level)
	{
		return_if_fail(m_next_level);
		m_next_level->update(Level);
		return;
	}
	if(m_valid)
		return;
	// update face vertices
	for(int i = 1; i < m_size-1; i += 2)
	{
		for(int j = 1; j < m_size-1; j += 2)
		{
			int i_p = (i-1)/2;
			int j_p = (j-1)/2;
			*(m_points[i][j]) = (*(m_parent_points[i_p][j_p]) + *(m_parent_points[i_p][j_p+1]) + *(m_parent_points[i_p+1][j_p+1]) + *(m_parent_points[i_p+1][j_p])) / 4.0;
		}
	}

	// update edge vertices
	for(int i = 2; i < m_size-1; i += 2)
	{
		for(int j = 1; j < m_size-1; j += 2)
		{
			int j_p = (j-1)/2;
			int i_p = i/2;
			*(m_points[i][j]) = (*(m_parent_points[i_p][j_p]) + *(m_parent_points[i_p][j_p+1]) + *(m_points[i-1][j]) + *(m_points[i+1][j]))/4;
			*(m_points[j][i]) = (*(m_parent_points[j_p][i_p]) + *(m_parent_points[j_p+1][i_p]) + *(m_points[j][i-1]) + *(m_points[j][i+1]))/4;
		}
	}

	// update original points
	double n2inv = 0.0625;
	for(int i = 2; i < m_size-1; i += 2)
	{
		for(int j = 2; j < m_size-1; j += 2)
		{
			int i_p = i/2;
			int j_p = j/2;
			*m_points[i][j] = 0.5*(*m_parent_points[i_p][j_p]) + n2inv*((*m_points[i-1][j-1]) + *m_points[i-1][j+1] + *m_points[i+1][j+1] + *m_points[i+1][j-1]) + n2inv*((*m_parent_points[i_p+1][j_p]) + *m_parent_points[i_p-1][j_p] + *m_parent_points[i_p][j_p-1] + *m_parent_points[i_p][j_p+1]);
		}
	}

	m_valid = true;
	m_normals_valid = false;
	m_nurbs_valid = false;
}

void sds_mipmap::update_border(patch_border* border)
{
	int parsize = (m_size-1)/2;
	if(border == &m_left)
	{
		for(int i = 1; i < m_size-1; i += 2)
			*m_points[0][i] += *m_points[1][i];
		for(int i = 2; i < m_size-1; i += 2)
			*m_points[0][i] += *m_parent_points[1][i/2] + *m_points[1][i-1] + *m_points[1][i+1];
		return;
	}
	if(border == &m_bottom)
	{
		for(int i = 1; i < m_size-1; i += 2)
			*m_points[i][0] += *m_points[i][1];
		for(int i = 2; i < m_size-1; i += 2)
			*m_points[i][0] += *m_parent_points[i/2][1] + *m_points[i-1][1] + *m_points[i+1][1];
		return;
	}
	if(border == &m_top)
	{
		for(int i = 1; i < m_size-1; i += 2)
			*m_points[i][m_size-1] += *m_points[i][m_size-2];
		for(int i = 2; i < m_size-1; i += 2)
			*m_points[i][m_size-1] += *m_parent_points[i/2][parsize-1] + *m_points[i-1][m_size-2] + *m_points[i+1][m_size-2];
		return;
	}
	if(border == &m_right)
	{
		for(int i = 1; i < m_size-1; i += 2)
			*m_points[m_size-1][i] += *m_points[m_size-2][i];
		for(int i = 2; i < m_size-1; i += 2)
			*m_points[m_size-1][i] += *m_parent_points[parsize-1][i/2] + *m_points[m_size-2][i-1] + *m_points[m_size-2][i+1];
		return;
	}
}

void sds_mipmap::update_nurbs(int Level)
{
	if(Level > m_level)
	{
		return_if_fail(m_next_level);
		m_next_level->update_nurbs(Level);
		return;
	}
	if(m_nurbs_valid)
		return;
	int n_size = m_size+4;
	if(m_size > 3)
	{
		for(int i = 3; i < n_size-3; ++i)
		{
			*m_nurbs[i][1] = (2*(*m_points[i-2][0]) + (*m_points[i-2][1]))/3.0;
			*m_nurbs[i][2] = ((*m_points[i-2][0]) + (*m_points[i-2][1])*2)/3.0;
			*m_nurbs[i][3] = (2*(*m_points[i-2][1]) + *m_points[i-2][2])/3.0;
			*m_nurbs[i][n_size-2] = (2*(*m_points[i-2][m_size-1]) + (*m_points[i-2][m_size-2]))/3.0;
			*m_nurbs[i][n_size-3] = ((*m_points[i-2][m_size-1]) + (*m_points[i-2][m_size-2])*2)/3.0;
			*m_nurbs[i][n_size-4] = (2*(*m_points[i-2][m_size-2]) + *m_points[i-2][m_size-3])/3.0;
		}
		for(int i = 4; i < n_size-4; ++i)
		{
			*m_nurbs[3][i] = *m_points[1][i-2];
			*m_nurbs[n_size-4][i] = *m_points[m_size-2][i-2];
		}
		for(int i = 1; i < n_size-1; ++i)
		{
			*m_nurbs[1][i] = (2*(*m_nurbs[0][i]) + (*m_nurbs[3][i]))/3.0;
			*m_nurbs[2][i] = ((*m_nurbs[0][i]) + (*m_nurbs[3][i])*2)/3.0;
			*m_nurbs[3][i] = ((*m_nurbs[4][i]) + (*m_nurbs[3][i])*2)/3.0;
			*m_nurbs[n_size-2][i] = (2*(*m_nurbs[n_size-1][i]) + (*m_nurbs[n_size-4][i]))/3.0;
			*m_nurbs[n_size-3][i] = ((*m_nurbs[n_size-1][i]) + (*m_nurbs[n_size-4][i])*2)/3.0;
			*m_nurbs[n_size-4][i] = ((*m_nurbs[n_size-5][i]) + (*m_nurbs[n_size-4][i])*2)/3.0;
		}
	}
	else
	{
		*m_nurbs[3][3] = *m_points[1][1];
		for(int i = 3; i < n_size-3; ++i)
		{
			*m_nurbs[i][1] = (2*(*m_points[i-2][0]) + (*m_points[i-2][1]))/3.0;
			*m_nurbs[i][2] = ((*m_points[i-2][0]) + (*m_points[i-2][1])*2)/3.0;
			*m_nurbs[i][n_size-2] = (2*(*m_points[i-2][m_size-1]) + (*m_points[i-2][m_size-2]))/3.0;
			*m_nurbs[i][n_size-3] = ((*m_points[i-2][m_size-1]) + (*m_points[i-2][m_size-2])*2)/3.0;
		}
		for(int i = 1; i < n_size-1; ++i)
		{
			*m_nurbs[1][i] = (2*(*m_nurbs[0][i]) + (*m_nurbs[3][i]))/3.0;
			*m_nurbs[2][i] = ((*m_nurbs[0][i]) + (*m_nurbs[3][i])*2)/3.0;
			*m_nurbs[n_size-2][i] = (2*(*m_nurbs[n_size-1][i]) + (*m_nurbs[n_size-4][i]))/3.0;
			*m_nurbs[n_size-3][i] = ((*m_nurbs[n_size-1][i]) + (*m_nurbs[n_size-4][i])*2)/3.0;
		}
		//*m_nurbs[3][3] = (*m_nurbs[2][3] + *m_nurbs[4][3] + *m_nurbs[3][2] + *m_nurbs[3][4])/4.0;
	}
	m_nurbs_valid = true;
}

const position_t& sds_mipmap::at_neighbour(const int i, patch_border * Border)
{
	if(Border == &m_left)
	{
		if(m_points[0][0] == Border->at(0))
			return *m_points[1][i];
		return *m_points[1][m_size-1-i];
	}
	if(Border == &m_bottom)
	{
		if(m_points[0][0] == Border->at(0))
			return *m_points[i][1];
		return *m_points[m_size-1-i][1];
	}
	if(Border == &m_top)
	{
		if(m_points[0][m_size-1] == Border->at(0))
			return *m_points[i][m_size-2];
		return *m_points[m_size-1-i][m_size-2];
	}
	if(Border == &m_right)
	{
		if(m_points[m_size-1][0] == Border->at(0))
			return *m_points[m_size-2][i];
		return *m_points[m_size-2][m_size-1-i];
	}
	return_val_if_fail(false, *(new position_t(0,0,0)));
}

void sds_mipmap::update_normals(int Level)
{
	if(m_level == Level)
	{
			// update normals
		if(!m_normals_valid)
		{
			for(int i = 1; i < m_size-1; ++i)
			{
				for(int j = 1; j < m_size-1; ++j)
				{
					*m_normals[i][j] = (normal(*m_points[i][j], *m_points[i-1][j], *m_points[i-1][j+1], *m_points[i][j+1]) +
							normal(*m_points[i][j], *m_points[i][j+1], *m_points[i+1][j+1], *m_points[i+1][j]) +
							normal(*m_points[i][j], *m_points[i+1][j], *m_points[i+1][j-1], *m_points[i][j-1]) +
							normal(*m_points[i][j], *m_points[i][j-1], *m_points[i-1][j-1], *m_points[i-1][j]))/4.0;
				}
			}
			m_normals_valid = true;
		}
	}
	else
	{
		return_if_fail(m_next_level);
		m_next_level->update_normals(Level);
	}
}

/////////
// face_vertex
/////////

void face_vertex::update(const_positions_t& Points)
{
	if (Points.size() != corners.size())
		throw cache_invalid_exception();
	unsigned long n = Points.size();
	position_t position(0,0,0);
	position_t zero(0,0,0);
	for(unsigned long i = 0; i < n; ++i)
	{
		corners[i]->original = *(Points[i]);
		corners[i]->invalidate();
		corners[i]->vertex = zero;
		edge_vertices[i]->invalidate();
		position += *(Points[i]);
	}
	vertex = position / static_cast<double>(n);
	for(unsigned long i = 0; i < mipmaps.size(); ++i)
	{
		mipmaps[i]->invalidate();
	}
}

//////////
// patch_corner
//////////

void patch_corner::update()
{
	double n = static_cast<double>(m_corners.size());
	if(n != m_face_vertices.size()) // corner lies on mesh boundary
	{
		if(m_boundary_left && m_boundary_right)
		{
			*m_point = 0.5*m_parent + 0.25*(*m_boundary_left + *m_boundary_right);
			m_normal_valid = false;
		}
		return;
	}
	if(m_valid)
		return;
	position_t corners_pos(0,0,0);
	for(positions_t::iterator it = m_corners.begin(); it != m_corners.end(); ++it)
	{
		corners_pos += **it;
	}
	position_t faces_pos(0,0,0);
	for(positions_t::iterator it = m_face_vertices.begin(); it != m_face_vertices.end(); ++it)
		faces_pos += **it;
	double nninv = 1/(n*n);
	*m_point = (n-2.0)/n * m_parent + nninv * corners_pos + nninv * faces_pos;
	m_valid = true;
	m_nurbs_valid = false;
	m_normal_valid = false;
}


void patch_corner::update_normal()
{
	if(m_normal_valid)
		return;
	position_t edges(0,0,0);
	unsigned long n = m_normals.size();
	for(unsigned long i = 0; i < n; ++i)
	{
		edges += *m_normals[i];
	}
	m_normal = edges / static_cast<double>(n);
	m_normal_valid = true;
}

void patch_corner::update_nurbs()
{
	double n = static_cast<double>(m_edge_vertices.size());
	if(n != m_corners.size() || n != m_face_vertices.size()) // corner on boundary
	{
		*m_nurbs_position = position();
		m_nurbs_valid = true;
		m_smooth = true;
		return;
	}
	if(m_nurbs_valid)
		return;
//	if(n == 4.0)
//	{
//		*m_nurbs_position = *m_point;
//		m_nurbs_valid = true;
//		m_smooth = true;
//		return;
//	}

	position_t p(0,0,0);
	for(unsigned long i = 0; i < n; ++i)
	{
		p += 4*(*m_edge_vertices[i]) + *m_face_vertices[i];
	}
	*m_nurbs_position = n * position() / (n+5) + p / ((n+5)*n);
	m_nurbs_valid = true;
	m_smooth = false;
}

void patch_corner::smooth()
{
	// FIXME: Results in points at "infinity"
	return;
	double n = static_cast<double>(m_edge_vertices.size());
	if(m_smooth)
	{
		if(m_next_level)
			m_next_level->smooth();
		return;
	}
	if(n != m_corners.size() || n != m_face_vertices.size())
		return;
	corner_smoothing& s = corner_smoothing::get_instance();
	if(n > 4 && (m_edge_vertices.size() % 2) == 0)
	{
		position_t r = s.r(m_Q40);
		if(r != k3d::point3(0, 0, 0))
			s.add_h(r, m_Q40, m_Q41, m_Q14);
	}
	if(n != 4.0)
	{
		positions_t Q20_orig;
		for(unsigned long i = 0; i < m_Q20.size(); ++i)
			Q20_orig.push_back(new position_t(*m_Q20[i]));
		s.q10(m_Q10, nurbs_position());
		s.q20(m_Q20, m_Q40, m_Q10, nurbs_position());
		s.q11(m_Q11, m_Q10, m_Q40, m_Q20);
		s.q12(m_Q21, m_Q12, m_Q20, Q20_orig);
		for(unsigned long i = 0; i < Q20_orig.size(); ++i)
			delete Q20_orig[i];
	}
	m_smooth = true;
}

////////
// patch_border
////////

patch_border::patch_border(position_t** Parent, int Psize, patch_corner* Top, patch_corner* Bottom, const int Level) :
	m_parent(Parent), m_psize(Psize), m_top(Top), m_bottom(Bottom), m_level(Level), m_valid(false), m_normals_valid(false), m_nurbs_valid(false), m_left(0), m_right(0), m_next_level(0)
{
	m_size = static_cast<int>(pow(2.0, static_cast<double>(m_level)) + 1);
	m_points = new position_t*[m_size];
	m_points[0] = &m_top->position();
	m_points[m_size-1] = &m_bottom->position();
	for(int i = 1; i < m_size-1; ++i)
		m_points[i] = new position_t(0,0,0);
	assert(m_size > 2);
	m_bottom->add_corner(m_parent[(m_size-1)/2-1]);
	m_top->add_corner(m_parent[1]);
	m_bottom->add_edge_vertex(m_points[m_size-2]);
	m_top->add_edge_vertex(m_points[1]);

	// normals initialisation
	m_normals = new position_t*[m_size];
	m_normals[0] = &m_top->normal();
	m_normals[m_size-1] = &m_bottom->normal();
	for(int i = 1; i < m_size-1; ++i)
		m_normals[i] = new position_t(0,0,0);
	m_bottom->add_normal(m_normals[m_size-2]);
	m_top->add_normal(m_normals[1]);
	// nurbs curve initialisation
	m_nsize = m_size + 4;
	m_nurbs = new position_t*[m_nsize];
	m_nurbs[0] = &m_top->nurbs_position();
	m_nurbs[m_nsize-1] = &m_bottom->nurbs_position();
	for(int i = 1; i < m_nsize-1; ++i)
		m_nurbs[i] = new position_t(0,0,0);
	m_bottom->add10(m_nurbs[m_nsize-2]);
	m_top->add10(m_nurbs[1]);
	m_bottom->add20(m_nurbs[m_nsize-3]);
	m_top->add20(m_nurbs[2]);
	m_bottom->add40(m_nurbs[m_nsize-4]);
	m_top->add40(m_nurbs[3]);
}

patch_border::~patch_border()
{
	delete m_next_level;
	for(int i = 1; i < m_size-1; ++i)
		delete m_points[i];
	for(int i = 1; i < m_size-1; ++i)
		delete m_normals[i];
	delete[] m_points;
	delete[] m_normals;
	for(int i = 1; i < m_nsize-1; ++i)
		delete m_nurbs[i];
	delete[] m_nurbs;
}

void patch_border::register_face_vertex(position_t * Corner, position_t * FaceVertex)
{
	if(Corner == &m_top->position())
	{
		m_top->add_face_vertex(FaceVertex);
	}
	else if(Corner == &m_bottom->position())
	{
		m_bottom->add_face_vertex(FaceVertex);
	}
	else
	{
		return_if_fail(false);
	}
}

void patch_border::update(int Level)
{
	if(Level == m_level)
	{
		if(!m_right) // border
		{
			for(int i = 1; i < m_size-1; i += 2)
			{
				int i_p = (i-1)/2;
				*m_points[i] = 0.5*(*m_parent[i_p] + *m_parent[i_p+1]);
			}
			for(int i = 2; i < m_size-1; i += 2)
			{
				*m_points[i] = *m_parent[i/2]*0.5 + 0.25 * (*m_points[i-1] + *m_points[i+1]);
			}
			m_top->update();
			m_bottom->update();
//			for(int i = 1; i < m_size-1; ++i)
//			{
//				*m_normals[i] = normal(*m_points[i-1], *m_points[i+1], m_left->at_neighbour(i, this));
//			}
			return;
		}
		if(m_valid)
			return;
		double n2inv = 0.0625;
		// edge points
		if(m_left && m_right)
		{
			m_left->update_border(this);
			m_right->update_border(this);
		}
		for(int i = 1; i < m_size-1; i += 2)
		{
			int i_p = (i-1)/2;
			*m_points[i] += *m_parent[i_p] + *m_parent[i_p+1];
			*m_points[i] /= 4.0;
		}
		// old points
		for(int i = 2; i < m_size-1; i += 2)
		{
			int i_p = i/2;
			*m_points[i] += *m_parent[i_p-1] + *m_parent[i_p+1];
			*m_points[i] *= n2inv;
			*m_points[i] += 0.5*(*m_parent[i_p]);
		}
		m_top->update();
		m_bottom->update();
		m_valid = true;
		m_normals_valid = false;
		m_nurbs_valid = false;
	}
	else
	{
		return_if_fail(m_next_level);
		m_next_level->update(Level);
	}
}

void patch_border::update_normals()
{
	if(m_right)
	{
		for(int i = 1; i < m_size-1; ++i)
		{
			*m_normals[i] = (normal(*m_points[i], m_left->at_neighbour(i, this), m_left->at_neighbour(i+1, this), *m_points[i+1]) +
					normal(*m_points[i], *m_points[i+1], m_right->at_neighbour(i+1, this), m_right->at_neighbour(i, this)) +
					normal(*m_points[i], m_right->at_neighbour(i, this), m_right->at_neighbour(i-1, this), *m_points[i-1]) +
					normal(*m_points[i], *m_points[i-1], m_left->at_neighbour(i-1, this), m_left->at_neighbour(i, this)))/4.0;
		}
	}
	else
	{
		for(int i = 1; i < m_size-1; ++i)
		{
			*m_normals[i] = (normal(*m_points[i], m_left->at_neighbour(i, this), m_left->at_neighbour(i+1, this), *m_points[i+1]) +
					normal(*m_points[i], *m_points[i-1], m_left->at_neighbour(i-1, this), m_left->at_neighbour(i, this)))/2.0;
		}
	}
	m_normals_valid = true;
}

void patch_border::update_nurbs(int Level)
{
	if(Level == m_level)
	{
		if(!m_right) // mesh border
		{
			m_top->update_nurbs();
			m_bottom->update_nurbs();
			*m_nurbs[1] = (*m_points[0]*2.0 + *m_points[1])/3.0;
			*m_nurbs[2] = (*m_points[0] + *m_points[1]*2.0)/3.0;
			*m_nurbs[3] = (*m_points[2] + *m_points[1]*2.0)/3.0;
			*m_nurbs[m_nsize-2] = (*m_points[m_size-1]*2.0 + *m_points[m_size-2])/3.0;
			*m_nurbs[m_nsize-3] = (*m_points[m_size-1] + *m_points[m_size-2]*2.0)/3.0;
			*m_nurbs[m_nsize-4] = (*m_points[m_size-3] + *m_points[m_size-2]*2.0)/3.0;
			if(m_size > 3)
			{
			for(int i = 4; i < m_size; ++i)
				*m_nurbs[i] = *m_points[i-2];
			}
			else
			{
				*m_nurbs[3] = (*m_nurbs[2] + *m_nurbs[m_nsize-3])*0.5;
			}
			m_nurbs_valid = true;
			return;
		}
		if(m_nurbs_valid)
			return;
		m_top->update_nurbs();
		m_bottom->update_nurbs();
		// knot insertion
		position_t left = (2.0*m_left->at_neighbour(0, this) + m_left->at_neighbour(1, this))/3.0;
		position_t right = (2.0*m_right->at_neighbour(0, this) + m_right->at_neighbour(1, this))/3.0;
		position_t p = (*m_points[0]*2.0 + *m_points[1])/3.0;
		*m_nurbs[1] = (left + 4.0*p + right)/6.0;
		left = (m_left->at_neighbour(0, this) + 2.0*m_left->at_neighbour(1, this))/3.0;
		right = (m_right->at_neighbour(0, this) + 2.0*m_right->at_neighbour(1, this))/3.0;
		p = (*m_points[0] + *m_points[1]*2.0)/3.0;
		*m_nurbs[2] = (left + 4.0*p + right)/6.0;
		left = (2.0*m_left->at_neighbour(m_size-1, this) + m_left->at_neighbour(m_size-2, this))/3.0;
		right = (2.0*m_right->at_neighbour(m_size-1, this) + m_right->at_neighbour(m_size-2, this))/3.0;
		p = (*m_points[m_size-1]*2.0 + *m_points[m_size-2])/3.0;
		*m_nurbs[m_nsize-2] = (left + 4.0*p + right)/6.0;
		left = (m_left->at_neighbour(m_size-1, this) + 2.0*m_left->at_neighbour(m_size-2, this))/3.0;
		right = (m_right->at_neighbour(m_size-1, this) + 2.0*m_right->at_neighbour(m_size-2, this))/3.0;
		p = (*m_points[m_size-1] + *m_points[m_size-2]*2.0)/3.0;
		*m_nurbs[m_nsize-3] = (left + 4.0*p + right)/6.0;
		if(m_size > 3) // insert Q4
		{
			left = (m_left->at_neighbour(2, this) + 2.0*m_left->at_neighbour(1, this))/3.0;
			right = (m_right->at_neighbour(2, this) + 2.0*m_right->at_neighbour(1, this))/3.0;
			p = (*m_points[2] + *m_points[1]*2.0)/3.0;
			*m_nurbs[3] = (left + 4.0*p + right)/6.0;
			left = (m_left->at_neighbour(m_size-3, this) + 2.0*m_left->at_neighbour(m_size-2, this))/3.0;
			right = (m_right->at_neighbour(m_size-3, this) + 2.0*m_right->at_neighbour(m_size-2, this))/3.0;
			p = (*m_points[m_size-3] + *m_points[m_size-2]*2.0)/3.0;
			*m_nurbs[m_nsize-4] = (left + 4.0*p + right)/6.0;
		}
		else
		{
			*m_nurbs[3] = (m_left->at_neighbour(1, this) + *m_points[1]*4 + m_right->at_neighbour(1, this))/6.0;
		}
		for(int i = 4; i < m_size; ++i)
			*m_nurbs[i] = (m_left->at_neighbour(i-2, this) + *m_points[i-2]*4 + m_right->at_neighbour(i-2, this))/6.0;
		m_nurbs_valid = true;
	}
	else
	{
		return_if_fail(m_next_level);
		m_next_level->update_nurbs(Level);
	}
}

void patch_border::register_mipmap(sds_mipmap* map)
{
	if(m_left)
	{
		m_right = map;
		return;
	}
	m_left = map;
}


/////////
// corner_smoothing
////////

corner_smoothing* corner_smoothing::m_Instance = 0;

corner_smoothing& corner_smoothing::get_instance()
{
	if(m_Instance == 0)
		m_Instance = new corner_smoothing();
	return *m_Instance;
}

position_t corner_smoothing::r(const positions_t& Q40)
{
	int n = Q40.size();
	position_t r(0,0,0);
	for(int i = 0; i < n; ++i)
		r += powi(i+1) * (*Q40[i]);
	r /= static_cast<double>(n);
	return r;
}

void corner_smoothing::add_h(position_t r, positions_t & Q40, positions_t & Q41, positions_t & Q14)
{
	int n = Q40.size();
	for(int i = 0; i < n; ++i)
	{
		position_t h = -powi(i+1)*r;
		*Q40[i] += h;
		*Q41[i] += h;
		*Q14[i] -= h;
	}
}

void corner_smoothing::q10(positions_t & Q10, const position_t & Q00)
{
	int n = Q10.size();
	A(n, Q10);
	for(int i = 0; i < n; ++i)
	{
		k3d::point3& p1 = *Q10[i];
//		k3d::log() << debug << "modified " << p1[0] << ", " << p1[1] << ", " << p1[2];
		*Q10[i] += Q00;
//		k3d::log() << debug << " to " << p1[0] << ", " << p1[1] << ", " << p1[2] << std::endl;
	}
}

void corner_smoothing::q20(positions_t & Q20, const positions_t & Q40, const positions_t & Q10, const position_t & Q00)
{
	int n = Q20.size();
	for(int i = 0; i < n; ++i)
		*Q20[i] = (*Q40[i] + 6*(*Q10[i]) + 2*Q00)/5;
}

void corner_smoothing::q11(positions_t & Q11, const positions_t & Q10, const positions_t & Q40, const positions_t & Q20)
{
	int n = Q11.size();
	double c = std::cos(2*pi/static_cast<double>(n))/6.0;
	for(int i = 0; i < n; ++i)
		*Q11[i] = *Q10[i] + c*(*Q40[i] - *Q20[i]);
	B(n, Q11);
}

void corner_smoothing::A(unsigned long n, positions_t & Q10)
{
	while (m_A.size() < n)
		m_A.push_back(0);
	matrix* A;
	if(m_A[n-1] == 0)
	{
		double a = static_cast<double>(2/n);
		double b = 2.0*pi/static_cast<double>(n);
		A = new matrix(boost::extents[n][n]);
		m_A[n-1] = A;
		for(unsigned long i = 0; i < n; ++i)
		{
			for(unsigned long j = 0; j < n; ++j)
			{
				(*A)[i][j] = a*cos(b*(i-j));
			}
		}
	}
	else
	{
		A = m_A[n-1];
	}
	for(unsigned long i = 0; i < n; ++i)
	{
		for(unsigned long j = 0; j < n; ++j)
		{
//			k3d::log() << debug << (*A)[j][i] << ", ";
		}
//		k3d::log() << debug << std::endl;
	}
	mat_mult(*A, Q10);
}

void corner_smoothing::B(unsigned long n, positions_t & Q)
{
	while (m_B.size() < n)
		m_B.push_back(0);
	matrix* B;
	if(m_B[n-1] == 0)
	{
		B = new matrix(boost::extents[n][n]);
		m_B[n-1] = B;
		if((n % 2) != 0)
		{
			for(unsigned long i = 0; i < n; ++i)
			{
				for(unsigned long j = 0; j < n; ++j)
				{
					(*B)[i][j] = powi((n+i-j)%n);
				}
			}
		}
		else
		{
			for(unsigned long i = 0; i < n; ++i)
			{
				for(unsigned long j = 0; j < n; ++j)
				{
					(*B)[i][j] = powi(j) - 2.0*((n+i-j)%n)*powi(j-i)/static_cast<double>(n);
				}
			}
		}
	}
	else
	{
		B = m_B[n-1];
	}
	mat_mult(*B, Q);
}

void corner_smoothing::mat_mult(matrix & A, positions_t & p)
{
	positions_t result;
	int n = p.size();
	for(int i = 0; i < n; ++i)
	{
		position_t* element = new position_t(0,0,0);
		for(int j = 0; j < n; ++j)
			*element += A[j][i]*(*p[j]);
		result.push_back(element);
	}
	for(int i = 0; i < n; ++i)
	{
		*p[i] = *result[i];
		delete result[i];
	}
}

void corner_smoothing::q12(positions_t& Q21, positions_t& Q12, const positions_t& Q20, const positions_t& Q20_orig)
{
	int n = Q21.size();
	for(int i = 0; i < n; ++i)
	{
		position_t diff = *Q20[i] - *Q20_orig[i];
		*Q21[i] += diff;
		*Q12[(n+i-1)%n] += diff;
	}
}

////////
// higher_level_cache
////////

higher_level_cache::higher_level_cache(const unsigned long Levels, const bool UseNURBS, facevertices_t& Faces) :
		m_levels(Levels), m_use_nurbs(UseNURBS)
{
	return_if_fail(m_levels > 1);
	generate(m_levels-1, Faces);
}

void higher_level_cache::update(facevertices_t& UpdatedMaps)
{
	//k3d::timer dftimer;
	for(unsigned long i = 1; i < m_levels; ++i)
	{
		// recalculate interior points
		for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
		{
			(*face)->update_mipmaps(i);
		}
		// recalculate borders and corners
		for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
		{
			face_vertex::mipmaps_t& maps = (*face)->mipmaps;
			for(face_vertex::mipmaps_t::iterator map = maps.begin(); map != maps.end(); ++map)
				(*map)->update_borders(i);
		}

		// NURBS part
		if(m_use_nurbs)
		{
			for(bordermap_t::iterator it = m_borders.begin(); it != m_borders.end(); ++it)
			{
				inner_bordermap_t* map = it->second;
				for(inner_bordermap_t::iterator in_it = map->begin(); in_it != map->end(); ++in_it)
					in_it->second->update_nurbs(i);
			}
			for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
			{
				(*face)->update_nurbs(i);
			}
			for(cornermap_t::iterator corner = m_patch_corners.begin(); corner != m_patch_corners.end(); ++corner)
			{
				corner->second->smooth();
			}
		}
	}

	// normal calculation is only needed for the level that is displayed
	// recalculate interior normals
	for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
	{
		face_vertex::mipmaps_t& maps = (*face)->mipmaps;
		for(face_vertex::mipmaps_t::iterator map = maps.begin(); map != maps.end(); ++map)
			(*map)->update_normals(m_levels-1);
	}
	// recalculate border normals
	for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
	{
		face_vertex::mipmaps_t& maps = (*face)->mipmaps;
		for(face_vertex::mipmaps_t::iterator map = maps.begin(); map != maps.end(); ++map)
			(*map)->update_border_normals(m_levels-1);
	}
	// recalculate corner normals
	for(facevertices_t::iterator face = UpdatedMaps.begin(); face != UpdatedMaps.end(); ++face)
	{
		face_vertex::mipmaps_t& maps = (*face)->mipmaps;
		for(face_vertex::mipmaps_t::iterator map = maps.begin(); map != maps.end(); ++map)
			(*map)->update_corner_normals(m_levels-1);
	}
	//k3d::log() << debug << "Spent " << dftimer.elapsed() << "s updating SDS cache." << std::endl;
}

void higher_level_cache::generate(const int level, facevertices_t& Faces)
{
	int n_mipmaps = 0;
	for(facevertices_t::iterator face = Faces.begin(); face != Faces.end(); ++face)
	{
		int n = (*face)->corners.size();
		for(int i = 0; i < n; ++i)
		{
			point_array& parent = *(new point_array(boost::extents[2][2]));
			parent[0][0] = &((*face)->edge_vertices[(n+i-1)%n]->vertex);
			parent[0][1] = &((*face)->corners[i]->vertex);
			parent[1][1] = &((*face)->edge_vertices[i]->vertex);
			parent[1][0] = &((*face)->vertex);

			return_if_fail(parent[0][0] != parent[0][1]);
			return_if_fail(parent[0][0] != parent[1][1]);
			return_if_fail(parent[0][0] != parent[1][0]);
			return_if_fail(parent[1][1] != parent[1][0]);
			return_if_fail(parent[1][1] != parent[0][1]);
			return_if_fail(parent[0][1] != parent[1][0]);

			(*face)->mipmaps.push_back(new sds_mipmap(*get_border((*face)->corners[i], (*face)->edge_vertices[i]),
			*get_border((*face)->edge_vertices[i], *face),
			*get_border(*face,(*face)->edge_vertices[(n+i-1)%n]),
			*get_border((*face)->edge_vertices[(n+i-1)%n], (*face)->corners[i]), parent, 1, 3));
			m_patch_parents.push_back(&parent);
			++n_mipmaps;
		}
	}
	for(int i = 2; i <= level; ++i)
	{
		for(facevertices_t::iterator face = Faces.begin(); face != Faces.end(); ++face)
		{
			for(unsigned long j = 0; j < (*face)->mipmaps.size(); ++j)
				(*face)->mipmaps[j]->generate(i);
		}
	}
	for(bordermap_t::iterator it = m_borders.begin(); it != m_borders.end(); ++it)
	{
		inner_bordermap_t* map = it->second;
		for(inner_bordermap_t::iterator in_it = map->begin(); in_it != map->end(); ++in_it)
		{
			in_it->second->register_boundary_points();
		}
	}
//	k3d::log() << debug << "generated " << m_border_parents.size() << " borders, " << n_mipmaps << " mipmaps and " << m_patch_corners.size() << " corners" << std::endl;
}

higher_level_cache::~ higher_level_cache()
{
	for(bordermap_t::iterator it = m_borders.begin(); it != m_borders.end(); ++it)
	{
		inner_bordermap_t* map = it->second;
		for(inner_bordermap_t::iterator in_it = map->begin(); in_it != map->end(); ++in_it)
			delete in_it->second;
		it->second->clear();
		delete it->second;
	}
	for(cornermap_t::iterator it = m_patch_corners.begin(); it != m_patch_corners.end(); ++it)
		delete it->second;
	for(unsigned long i = 0; i < m_patch_parents.size(); ++i)
		delete m_patch_parents[i];
	for(unsigned long i = 0; i < m_border_parents.size(); ++i)
		delete[] m_border_parents[i];
}

patch_border* higher_level_cache::get_border(valid_point* Start, valid_point* End)
{
	return_val_if_fail(Start != End, 0);
	patch_border* border = retrieve_border(Start, End);
	if(border != 0)
		return border;
	bordermap_t::iterator it = m_borders.find(End);
	inner_bordermap_t* inner_map;
	if(it != m_borders.end())
	{
		inner_map = it->second;
	}
	else
	{
		inner_map = new inner_bordermap_t();
		m_borders[End] = inner_map;
	}
	return_val_if_fail(inner_map->find(Start) == inner_map->end(), 0);
	position_t** parent = new position_t*[2];
	parent[0] = &End->vertex;
	parent[1] = &Start->vertex;
	m_border_parents.push_back(parent);
	border = new patch_border(parent, 2, get_patch_corner(End), get_patch_corner(Start), 1);
	(*inner_map)[Start] = border;
	return border;
}

patch_border* higher_level_cache::retrieve_border(valid_point* Start, valid_point* End, bool recurse)
{
	bordermap_t::iterator it = m_borders.find(Start);
	inner_bordermap_t* inner_map = 0;
	if(it != m_borders.end())
	{
		inner_map = it->second;
		inner_bordermap_t::iterator border_it = inner_map->find(End);
		if(border_it != inner_map->end())
			return border_it->second;
	}
	if(recurse)
		return retrieve_border(End, Start, false);
	return 0;
}

patch_corner* higher_level_cache::get_patch_corner(valid_point* Point)
{
	cornermap_t::iterator it = m_patch_corners.find(Point);
	if(it != m_patch_corners.end())
	{
		return it->second;
	}
	patch_corner* corner = new patch_corner(Point->vertex);
	m_patch_corners[Point] = corner;
	return corner;
}

position_t normal(const position_t& p1, const position_t& p2, const position_t& p3, const position_t& p4)
{
	// normal calculation taken from k3d::normal()
	position_t n(0, 0, 0);

	n[0] += (p1[1] + p2[1]) * (p2[2] - p1[2]);
	n[1] += (p1[2] + p2[2]) * (p2[0] - p1[0]);
	n[2] += (p1[0] + p2[0]) * (p2[1] - p1[1]);

	n[0] += (p2[1] + p3[1]) * (p3[2] - p2[2]);
	n[1] += (p2[2] + p3[2]) * (p3[0] - p2[0]);
	n[2] += (p2[0] + p3[0]) * (p3[1] - p2[1]);

	n[0] += (p3[1] + p4[1]) * (p4[2] - p3[2]);
	n[1] += (p3[2] + p4[2]) * (p4[0] - p3[0]);
	n[2] += (p3[0] + p4[0]) * (p4[1] - p3[1]);

	n[0] += (p4[1] + p1[1]) * (p1[2] - p4[2]);
	n[1] += (p4[2] + p1[2]) * (p1[0] - p4[0]);
	n[2] += (p4[0] + p1[0]) * (p1[1] - p4[1]);

	return 0.5*n;
}

position_t normal(const position_t& p1, const position_t& p2, const position_t& p3)
{
	// normal calculation taken from k3d::normal()
	position_t n(0, 0, 0);

	n[0] += (p1[1] + p2[1]) * (p2[2] - p1[2]);
	n[1] += (p1[2] + p2[2]) * (p2[0] - p1[0]);
	n[2] += (p1[0] + p2[0]) * (p2[1] - p1[1]);

	n[0] += (p2[1] + p3[1]) * (p3[2] - p2[2]);
	n[1] += (p2[2] + p3[2]) * (p3[0] - p2[0]);
	n[2] += (p2[0] + p3[0]) * (p3[1] - p2[1]);

	n[0] += (p3[1] + p1[1]) * (p1[2] - p3[2]);
	n[1] += (p3[2] + p1[2]) * (p1[0] - p3[0]);
	n[2] += (p3[0] + p1[0]) * (p1[1] - p3[1]);

	return 0.5*n;
}

} // namespace sds

} // namespace k3d


