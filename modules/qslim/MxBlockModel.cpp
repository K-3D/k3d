// MixKit -- Code library for multiresolution surface modeling
// Copyright (c) 1998, Michael Garland
//
// Contact: http://graphics.cs.uiuc.edu/~garland/software/qslim.html
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief MxBlockModel
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxBlockModel.h"

////////////////////////////////////////////////////////////////////////
//
// Basic allocation routines
//

MxFaceID MxBlockModel::alloc_face(MxVertexID v1, MxVertexID v2, MxVertexID v3)
{
	MxFaceID id = faces.size();
	faces.push_back(MxFace(v1,v2,v3));
	return id;
}

MxVertexID MxBlockModel::alloc_vertex(double x, double y, double z)
{
	MxVertexID id = vertices.size();
	vertices.push_back(MxVertex(x,y,z));
	return id;
}

MxVertexID MxBlockModel::add_vertex(double x, double y, double z)
{
	MxVertexID id = alloc_vertex(x,y,z);
	init_vertex(id);
	return id;
}

void MxBlockModel::remove_vertex(MxVertexID v)
{
	assert_warning(v < vertices.size());

	free_vertex(v);
	vertices.erase(vertices.begin() + v);
}

void MxBlockModel::remove_face(MxFaceID f)
{
	assert_warning(f < faces.size());

	free_face(f);
	faces.erase(faces.begin() + f);
}

MxFaceID MxBlockModel::add_face(unsigned long v1, unsigned long v2, unsigned long v3, bool will_link)
{
	MxFaceID id = alloc_face(v1, v2, v3);
	if(will_link)
		init_face(id);

	return id;
}

////////////////////////////////////////////////////////////////////////
//
// Utility methods for computing characteristics of faces.
//

Vec3 MxBlockModel::compute_face_normal(MxFaceID f, bool will_unitize)
{
	Vec3 v1 = vertex(face(f)[0]).pos;
	Vec3 v2 = vertex(face(f)[1]).pos;
	Vec3 v3 = vertex(face(f)[2]).pos;

	Vec3 a = v2 - v1;
	Vec3 b = v3 - v1;

	Vec3 n = a ^ b;

	if(will_unitize)
		n.Normalize();

	return n;
}

double MxBlockModel::compute_face_area(MxFaceID f)
{
	Vec3 n = compute_face_normal(f, false);
	return 0.5 * n.Length();
}

double MxBlockModel::compute_corner_angle(MxFaceID f, unsigned long i)
{
	unsigned long i_prev = (i==0)?2:i-1;
	unsigned long i_next = (i==2)?0:i+1;

	Vec3 e_prev = corner(f, i_prev) - corner(f, i);
	e_prev.Normalize();
	Vec3 e_next = corner(f, i_next) - corner(f, i);
	e_next.Normalize();

	return acos(e_prev * e_next);
}


