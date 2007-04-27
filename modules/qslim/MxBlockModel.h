#ifndef MXBLOCKMODEL_INCLUDED
#define MXBLOCKMODEL_INCLUDED

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

#include <k3dsdk/result.h>

#include "MxMesh.h"

#include <vector>

class MxBlockModel
{
private:
	std::vector<MxVertex> vertices;
	std::vector<MxFace> faces;

protected:
	virtual MxVertexID alloc_vertex(double, double, double);
	virtual void init_vertex(MxVertexID) {}
	virtual void free_vertex(MxVertexID) {}
	virtual MxFaceID alloc_face(MxVertexID, MxVertexID, MxVertexID);
	virtual void init_face(MxFaceID) {}
	virtual void free_face(MxFaceID) {}

public:
	MxBlockModel(int nvert, int nface)
	{
		vertices.reserve(nvert);
		faces.reserve(nface);
	}

	virtual ~MxBlockModel()
	{
	}

	unsigned long vert_count() const { return vertices.size(); }
	unsigned long face_count() const { return faces.size(); }

	MxVertexID add_vertex(double, double, double);
	MxFaceID add_face(unsigned long, unsigned long, unsigned long, bool will_link=true);

	MxVertexID add_vertex(double *v) { return add_vertex(v[0], v[1], v[2]); }
	MxFaceID add_face(unsigned int *f) { return add_face(f[0], f[1], f[2]); }

	void remove_vertex(MxVertexID v);
	void remove_face(MxFaceID f);

	MxVertex& vertex(unsigned int i) { return vertices[i]; }
	MxFace& face(unsigned int i) { return faces[i]; }
	MxVertex& corner(MxFaceID f, short i) { return vertex(face(f)[i]); }

	Vec3 compute_face_normal(MxFaceID, bool will_unitize=true);
	double compute_face_area(MxFaceID);

	double compute_corner_angle(MxFaceID, unsigned long);
};

#endif // MXBLOCKMODEL_INCLUDED

