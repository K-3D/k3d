#ifndef MXSTDMODEL_INCLUDED
#define MXSTDMODEL_INCLUDED

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
		\brief MxStdModel
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxBlockModel.h"

typedef std::vector<unsigned long> vertex_list_t;
typedef std::vector<unsigned long> face_list_t;

class MxPairContraction
{
public:
	MxPairContraction() {}
	MxPairContraction(const MxPairContraction& c) { *this = c; }

	MxPairContraction& operator=(const MxPairContraction& c);

	MxVertexID v1, v2;
	Vec3 dv1;
	Vec3 dv2; // dv2 is not really necessary

	unsigned long delta_pivot;
	face_list_t delta_faces;
	face_list_t dead_faces;
};

class MxFaceContraction
{
public:
	MxFaceID f;
	double dv1[3], dv2[3], dv3[3];

	face_list_t delta_faces;
	face_list_t dead_faces;
};

typedef MxPairContraction MxPairExpansion;

// Masks for internal tag bits
#define MX_VALID_FLAG 0x01
#define MX_PROXY_FLAG 0x02
#define MX_TOUCHED_FLAG 0x04

class MxStdModel : public MxBlockModel
{
private:
	struct vertex_data {
	    unsigned char mark, tag;             // Internal tag bits
	    unsigned char user_mark, user_tag;   // External tag bits
	};
	struct face_data {
	    unsigned char mark, tag;             // Internal tag bits
	    unsigned char user_mark, user_tag;   // External tag bits
	};

	std::vector<vertex_data> v_data;
	std::vector<face_data> f_data;
	std::vector<face_list_t> face_links;

protected:

	// Accessors for internal tag and mark bits
	unsigned long v_check_tag(MxVertexID i, unsigned long tag) const {return v_data[i].tag&tag;}
	void v_set_tag(MxVertexID i, unsigned long tag) { v_data[i].tag |= tag; }
	void v_unset_tag(MxVertexID i, unsigned long tag) { v_data[i].tag &= ~tag; }
	unsigned char vmark(MxVertexID i) const { return v_data[i].mark; }
	void vmark(MxVertexID i, unsigned char m) { v_data[i].mark = m; }

	unsigned long f_check_tag(MxFaceID i, unsigned long tag) const { return f_data[i].tag&tag; }
	void f_set_tag(MxFaceID i, unsigned long tag) { f_data[i].tag |= tag; }
	void f_unset_tag(MxFaceID i, unsigned long tag) { f_data[i].tag &= ~tag; }
	unsigned char fmark(MxFaceID i) const { return f_data[i].mark; }
	void fmark(MxFaceID i, unsigned char m) { f_data[i].mark = m; }

protected:
	MxVertexID alloc_vertex(double, double, double);
	void free_vertex(MxVertexID);
	void free_face(MxFaceID);
	MxFaceID alloc_face(MxVertexID, MxVertexID, MxVertexID);
	void init_face(MxFaceID);

public:
	MxStdModel(unsigned int nvert, unsigned int nface) :
		MxBlockModel(nvert,nface)
	{
		v_data.reserve(nvert);
		f_data.reserve(nface);
		face_links.reserve(nvert);
	}

	virtual ~MxStdModel() {}

	////////////////////////////////////////////////////////////////////////
	// Tagging and marking
	unsigned long vertex_is_valid(MxVertexID i) const
	{ return v_check_tag(i,MX_VALID_FLAG); }
	void vertex_mark_valid(MxVertexID i) { v_set_tag(i,MX_VALID_FLAG); }
	void vertex_mark_invalid(MxVertexID i) { v_unset_tag(i,MX_VALID_FLAG); }

	unsigned long face_is_valid(MxFaceID i) const {return f_check_tag(i,MX_VALID_FLAG);}
	void face_mark_valid(MxFaceID i) { f_set_tag(i,MX_VALID_FLAG); }
	void face_mark_invalid(MxFaceID i) { f_unset_tag(i,MX_VALID_FLAG); }

	// Accessors for external tag and mark bits
	unsigned long vertex_check_tag(MxVertexID i, unsigned long tag) const
	{ return v_data[i].user_tag&tag; }
	void vertex_set_tag(MxVertexID i, unsigned long tag) { v_data[i].user_tag|=tag; }
	void vertex_unset_tag(MxVertexID i, unsigned long tag) {v_data[i].user_tag&= ~tag;}
	unsigned char vertex_mark(MxVertexID i) { return v_data[i].user_mark; }
	void vertex_mark(MxVertexID i, unsigned char m) { v_data[i].user_mark=m; }

	unsigned long face_check_tag(MxFaceID i, unsigned long tag) const
	{ return f_data[i].user_tag&tag; }
	void face_set_tag(MxFaceID i, unsigned long tag) { f_data[i].user_tag|=tag; }
	void face_unset_tag(MxFaceID i, unsigned long tag) {f_data[i].user_tag&= ~tag;}
	unsigned char face_mark(MxFaceID i) { return f_data[i].user_mark; }
	void face_mark(MxFaceID i, unsigned char m) { f_data[i].user_mark = m; }

	////////////////////////////////////////////////////////////////////////
	// Neighborhood collection and management
	void mark_neighborhood(MxVertexID, unsigned short mark=0);
	void collect_unmarked_neighbors(MxVertexID, face_list_t& faces);
	void mark_neighborhood_delta(MxVertexID, short delta);
	void partition_marked_neighbors(MxVertexID, unsigned short pivot,
		face_list_t& below, face_list_t& above);

	void mark_corners(const face_list_t& faces, unsigned short mark=0);
	void collect_unmarked_corners(const face_list_t& faces, vertex_list_t& verts);

	void collect_edge_neighbors(MxVertexID, MxVertexID, face_list_t&);
	void collect_vertex_star(MxVertexID v, vertex_list_t& verts);

	face_list_t& neighbors(MxVertexID v) { return face_links[v]; }
	const face_list_t& neighbors(MxVertexID v) const { return face_links[v]; }

	////////////////////////////////////////////////////////////////////////
	// Primitive transformation operations
	void remap_vertex(MxVertexID from, MxVertexID to);
	MxVertexID split_edge(MxVertexID v1,MxVertexID v2,double x,double y,double z);
	MxVertexID split_edge(MxVertexID v1, MxVertexID v2);

	void flip_edge(MxVertexID v1, MxVertexID v2);

	// split_face3
	void split_face4(MxFaceID f, MxVertexID *newverts=0);

	void unlink_face(MxFaceID f);

	////////////////////////////////////////////////////////////////////////
	// Contraction and related operations
	void compact_vertices();
	void remove_degeneracy(face_list_t&);

	// Pair contraction interface
	void compute_contraction(MxVertexID, MxVertexID,
		MxPairContraction *, const double *vnew=0);
	void apply_contraction(const MxPairContraction&);
	void apply_expansion(const MxPairExpansion&);
	void contract(MxVertexID v1, MxVertexID v2,
		const double *, MxPairContraction *);

	// Triple contraction interface
	void compute_contraction(MxFaceID, MxFaceContraction *);
	void contract(MxVertexID v1, MxVertexID v2, MxVertexID v3,
		const double *vnew, face_list_t& changed);

	// Generalized contraction interface
	void contract(MxVertexID v1, const vertex_list_t& rest,
		const double *vnew, face_list_t& changed);
};

#endif // MXSTDMODEL_INCLUDED

