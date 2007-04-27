#ifndef MXGEOPRIMS_INCLUDED
#define MXGEOPRIMS_INCLUDED

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
		\brief MxMesh
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "MxMath.h"

typedef unsigned int ID;
typedef ID MxVertexID;
typedef ID MxFaceID;

class MxVertex
{
public:
	Vec3 pos;

	MxVertex() {}
	MxVertex(double x, double y, double z)
	{
		pos[0]=x;
		pos[1]=y;
		pos[2]=z;
	}

	MxVertex(const MxVertex& v) { *this = v; }

	MxVertex& operator=(const MxVertex& v)
	{
		pos[0]=v.pos[0];
		pos[1]=v.pos[1];
		pos[2]=v.pos[2];
		return *this;
	}

	operator const double*() const { return pos; }
	operator double*() { return pos; }

	double& operator[](int i)       { return pos[i]; }
	double  operator[](int i) const { return pos[i]; }
};

class MxEdge
{
public:
	MxVertexID v1, v2;

	MxEdge() { }
	MxEdge(MxVertexID a, MxVertexID b) { v1=a; v2=b; }
	MxEdge(const MxEdge& e) { v1=e.v1;  v2=e.v2; }

	MxVertexID opposite_vertex(MxVertexID v)
		{
			if(v == v1)
				return v2;

			assert_warning(v==v2);
			return v1;
		}
};

class MxFace
{
public:
	MxVertexID v[3];

	MxFace() {}

	MxFace(MxVertexID v0, MxVertexID v1, MxVertexID v2)
	{ v[0]=v0;  v[1]=v1;  v[2]=v2; }
	MxFace(const MxFace& f) { v[0]=f.v[0]; v[1]=f.v[1]; v[2]=f.v[2]; }

	MxVertexID& operator[](int i)       { return v[i]; }
	MxVertexID  operator[](int i) const { return v[i]; }

	int remap_vertex(MxVertexID from, MxVertexID to)
	{
		int nmapped = 0;
		for(int i = 0; i < 3; i++)
			if(v[i] == from)
				{
					v[i]=to;
					nmapped++;
				}

		return nmapped;
	}

	unsigned long find_vertex(MxVertexID i)
	{
		if(v[0]==i) return 0;
		else if(v[1]==i) return 1;
		else { assert_warning(v[2]==i); return 2; }
	}

	MxVertexID opposite_vertex(MxVertexID v0, MxVertexID v1)
	{
		if(v[0]!=v0 && v[0]!=v1) return v[0];
		else if(v[1]!=v0 && v[1]!=v1) return v[1];
		else { assert_warning(v[2]!=v0 && v[2]!=v1); return v[2]; }
	}

	bool is_inorder(MxVertexID v0, MxVertexID v1)
	{
		if(v[0]==v0) return v[1]==v1;
		else if(v[1]==v0) return v[2]==v1;
		else { assert_warning(v[2]==v0); return v[0]==v1; }
	}
};

inline std::ostream& operator<<(std::ostream& out, const MxVertex& v)
{
	return out << "v " << v[0] << " " << v[1] << " " << v[2];
}

inline std::ostream& operator<<(std::ostream& out, const MxFace& f)
{
	return out << "f " << f[0]+1 << " " <<  f[1]+1 << " " <<  f[2]+1;
}

#endif // MXGEOPRIMS_INCLUDED

