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
		\brief MStdModel
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "MxStdModel.h"

#include <algorithm>

MxPairContraction& MxPairContraction::operator=(const MxPairContraction& c)
{
	v1 = c.v1;
	v2 = c.v2;
	dv1 = c.dv1;
	dv2 = c.dv2;

	delta_faces.clear();
	dead_faces.clear();

	for(unsigned long i=0; i<c.delta_faces.size(); i++)
		delta_faces.push_back(c.delta_faces[i]);
	for(unsigned long j=0; j<c.dead_faces.size(); j++)
		dead_faces.push_back(c.dead_faces[j]);

	delta_pivot = c.delta_pivot;

	return *this;
}

MxVertexID MxStdModel::alloc_vertex(double x, double y, double z)
{
	MxVertexID id = MxBlockModel::alloc_vertex(x,y,z);
	// !!
	vertex_data new_v;
	new_v.tag = 0x0;
	new_v.user_tag = 0x0;
	v_data.push_back(new_v);

	vertex_mark_valid(id);

	unsigned long l = face_links.size();
	face_links.push_back(face_list_t());
	assert_warning( l == id );
	assert_warning( neighbors(id).size() == 0 );

	return id;
}

void MxStdModel::free_vertex(MxVertexID v)
{
	face_links.erase(face_links.begin() + v);
	v_data.erase(v_data.begin() + v);
}

MxFaceID MxStdModel::alloc_face(MxVertexID v1, MxVertexID v2, MxVertexID v3)
{
	MxFaceID id = MxBlockModel::alloc_face(v1,v2,v3);
	face_data new_data;
	new_data.tag = 0x0;
	new_data.user_tag = 0x0;
	f_data.push_back(new_data);
	face_mark_valid(id);

	return id;
}

void MxStdModel::free_face(MxFaceID f)
{
	f_data.erase(f_data.begin() + f);
}

void MxStdModel::init_face(MxFaceID id)
{
	neighbors(face(id).v[0]).push_back(id);
	neighbors(face(id).v[1]).push_back(id);
	neighbors(face(id).v[2]).push_back(id);
}

void MxStdModel::mark_neighborhood(MxVertexID vid, unsigned short mark)
{
	assert_warning(vid < vert_count());

	for(unsigned long i = 0; i < neighbors(vid).size(); ++i)
	{
		unsigned int f = neighbors(vid)[i];
		fmark(f, mark);
	}
}

void MxStdModel::collect_unmarked_neighbors(MxVertexID vid, face_list_t& faces)
{
	assert_warning(vid < vert_count());

	for(unsigned long i = 0; i < neighbors(vid).size(); ++i)
	{
	unsigned int fid = neighbors(vid)[i];
	if( !fmark(fid) )
	{
	    faces.push_back(fid);
	    fmark(fid, 1);
	}
	}
}

void MxStdModel::mark_neighborhood_delta(MxVertexID vid, short delta)
{
	assert_warning( vid < vert_count() );
	for(unsigned long i=0; i<neighbors(vid).size(); i++)
	{
	unsigned long f = neighbors(vid)[i];
	fmark(f, fmark(f)+delta);
	}
}

void MxStdModel::partition_marked_neighbors(MxVertexID v, unsigned short pivot,
					    face_list_t& lo, face_list_t& hi)
{
	assert_warning(v < vert_count());
	for(unsigned long i=0; i<neighbors(v).size(); i++)
	{
	unsigned long f = neighbors(v)[i];
	if( fmark(f) )
	{
	    if( fmark(f) < pivot )  lo.push_back(f);
	    else  hi.push_back(f);
	    fmark(f, 0);
	}
	}
}

void MxStdModel::mark_corners(const face_list_t& faces, unsigned short mark)
{
	for(unsigned long i = 0; i < faces.size(); ++i)
		for(unsigned long j = 0; j < 3; ++j)
			vmark(face(faces[i])[j], mark);
}

void MxStdModel::collect_unmarked_corners(const face_list_t& faces, vertex_list_t& verts)
{
	for(unsigned long i = 0; i < faces.size(); ++i)
	for(unsigned long j = 0; j < 3; ++j)
	{
	    MxVertexID v = face(faces[i])[j];
	    if( !vmark(v) )
	    {
		verts.push_back(v);
		vmark(v, 1);
	    }
	}
}

void MxStdModel::collect_edge_neighbors(unsigned int v1, unsigned int v2, face_list_t& faces)
{
	mark_neighborhood(v1, 1);
	mark_neighborhood(v2, 0);
	collect_unmarked_neighbors(v1, faces);
}

void MxStdModel::collect_vertex_star(unsigned int v, vertex_list_t& verts)
{
	const face_list_t& N = neighbors(v);

	mark_corners(N, 0);
	vmark(v, 1); // Don't want to include v in the star
	collect_unmarked_corners(N, verts);
}


void MxStdModel::remap_vertex(unsigned int from, unsigned int to)
{
	assert_warning( from < vert_count() );
	assert_warning( to < vert_count() );
	assert_warning( vertex_is_valid(from) );
	assert_warning( vertex_is_valid(to) );

	for(unsigned int i=0; i<neighbors(from).size(); i++)
	face(neighbors(from)[i]).remap_vertex(from, to);

	mark_neighborhood(from, 0);
	mark_neighborhood(to, 1);
	collect_unmarked_neighbors(from, neighbors(to));

	vertex_mark_invalid(from);
	// Remove links in old vertex
	neighbors(from).clear();
}

unsigned int MxStdModel::split_edge(unsigned int a, unsigned int b)
{
	double *v1 = vertex(a), *v2 = vertex(b);

	return split_edge(a, b,
	                  (v1[X] + v2[X])/2.0f,
	                  (v1[Y] + v2[Y])/2.0f,
	                  (v1[Z] + v2[Z])/2.0f);
}

static
void remove_neighbor(face_list_t& faces, unsigned long f)
{
	face_list_t::iterator face = std::find(faces.begin(), faces.end(), f);
	if(face != faces.end())
		faces.erase(face);
}

unsigned int MxStdModel::split_edge(unsigned int v1, unsigned int v2,
			    double x, double y, double z)
{
	assert_warning( v1 < vert_count() );   assert_warning( v2 < vert_count() );
	assert_warning( vertex_is_valid(v1) ); assert_warning( vertex_is_valid(v2) );
	assert_warning( v1 != v2 );

	face_list_t faces;
	collect_edge_neighbors(v1, v2, faces);
	assert_warning(faces.size() > 0);

	unsigned int vnew = add_vertex(x,y,z);

	for(unsigned long i = 0; i < faces.size(); ++i)
	{
	unsigned int f = faces[i];
	unsigned int v3 = face(f).opposite_vertex(v1, v2);
	assert_warning( v3!=v1 && v3!=v2 );
	assert_warning( vertex_is_valid(v3) );

	// in f, remap v2-->vnew
	face(f).remap_vertex(v2, vnew);
	neighbors(vnew).push_back(f);

	// remove f from neighbors(v2)
	remove_neighbor(neighbors(v2), f);

	// assure orientation is consistent
	if( face(f).is_inorder(vnew, v3) )
	    add_face(vnew, v2, v3);
	else
	    add_face(vnew, v3, v2);
	}

	return vnew;
}

void MxStdModel::flip_edge(unsigned int v1, unsigned int v2)
{
	face_list_t faces;
	collect_edge_neighbors(v1, v2, faces);
	if(faces.size() != 2 )
		return;

	unsigned int f1 = faces[0];
	unsigned int f2 = faces[1];
	unsigned int v3 = face(f1).opposite_vertex(v1, v2);
	unsigned int v4 = face(f2).opposite_vertex(v1, v2);

	// ?? Should we check for convexity or assume thats been taken care of?

	remove_neighbor(neighbors(v1), f2);
	remove_neighbor(neighbors(v2), f1);
	neighbors(v3).push_back(f2);
	neighbors(v4).push_back(f1);

	face(f1).remap_vertex(v2, v4);
	face(f2).remap_vertex(v1, v3);
}

void MxStdModel::split_face4(unsigned int f, unsigned int *newverts)
{
	unsigned int v0 = face(f).v[0];
	unsigned int v1 = face(f).v[1];
	unsigned int v2 = face(f).v[2];

	unsigned int pivot = split_edge(v0, v1);
	unsigned int new1 = split_edge(v1, v2);
	unsigned int new2 = split_edge(v0, v2);

	if( newverts )
	{
	newverts[0] = pivot;
	newverts[1] = new1;
	newverts[2] = new2;
	}

	flip_edge(pivot, v2);
}

void MxStdModel::compact_vertices()
{
	MxVertexID oldID;
	MxVertexID newID = 0;

	for(oldID=0; oldID<vert_count(); oldID++)
	{
	if( vertex_is_valid(oldID) )
	{
	    if( newID != oldID )
	    {
		vertex(newID) = vertex(oldID);

		// Because we'll be freeing the link lists for the
		// old vertices, we actually have to swap values instead
		// of the simple copying in the block above.
		//
		face_list_t t = face_links[newID];
		face_links[newID] = face_links[oldID];
		face_links[oldID] = t;

		vertex_mark_valid(newID);

		for(unsigned int i=0; i<neighbors(newID).size(); i++)
		    face(neighbors(newID)[i]).remap_vertex(oldID, newID);
	    }
	    newID++;
	}
	}

	for(oldID = newID; newID < vert_count(); )
	remove_vertex(oldID);
}

void MxStdModel::unlink_face(MxFaceID fid)
{
	MxFace& f = face(fid);
	face_mark_invalid(fid);

	int found = 0;
	face_list_t::iterator face;

	for(unsigned long n = 0; n < 3; n++)
		{
			face = std::find(neighbors(f[n]).begin(), neighbors(f[n]).end(), fid);
			if(face != neighbors(f[n]).end())
				{
					found++;
					neighbors(f[n]).erase(face);
				}
		}

	assert_warning( found > 0 );

	for(unsigned long n = 0; n < 3; n++)
		assert_warning(std::find(neighbors(f[n]).begin(), neighbors(f[n]).end(), fid) == neighbors(f[n]).end());
}

void MxStdModel::remove_degeneracy(face_list_t& faces)
{
	for(unsigned long i = 0; i < faces.size(); ++i)
	{
	assert_warning( face_is_valid(faces[i]) );
	MxFace& f = face(faces[i]);

	if( f[0]==f[1] || f[1]==f[2] || f[0]==f[2] )
	    unlink_face(faces[i]);
	}
}

inline double* mxv_sub(double* r, const double* u, const double* v, const unsigned int N)
{
	for(unsigned int i = 0; i < 3; i++)
		r[i] = u[i] - v[i];

	return r;
}

void MxStdModel::compute_contraction(MxVertexID v1, MxVertexID v2,
				     MxPairContraction *conx,
				     const double *vnew)
{
	conx->v1 = v1;
	conx->v2 = v2;

	if( vnew )
	{
	mxv_sub(conx->dv1, vnew, vertex(v1), 3);
	mxv_sub(conx->dv2, vnew, vertex(v2), 3);
	}
	else
	{
	conx->dv1[X] = conx->dv1[Y] = conx->dv1[Z] = 0.0;
	conx->dv2[X] = conx->dv2[Y] = conx->dv2[Z] = 0.0;
	}

	conx->delta_faces.clear();
	conx->dead_faces.clear();


	// Mark the neighborhood of (v1,v2) such that each face is
	// tagged with the number of times the vertices v1,v2 occur
	// in it.  Possible values are 1 or 2.
	//
	mark_neighborhood(v2, 0);
	mark_neighborhood(v1, 1);
	mark_neighborhood_delta(v2, 1);


	// Now partition the neighborhood of (v1,v2) into those faces
	// which degenerate during contraction and those which are merely
	// reshaped.
	//
	partition_marked_neighbors(v1, 2, conx->delta_faces, conx->dead_faces);
	conx->delta_pivot = conx->delta_faces.size();
	partition_marked_neighbors(v2, 2, conx->delta_faces, conx->dead_faces);
}

void MxStdModel::apply_contraction(const MxPairContraction& conx)
{
	MxVertexID v1=conx.v1, v2=conx.v2;

	// Move v1 to new position
	vertex(v1).pos += conx.dv1;

	// Remove dead faces
	for(unsigned long i = 0; i < conx.dead_faces.size(); ++i)
		unlink_face(conx.dead_faces[i]);

	// Update changed faces
	for(unsigned long i = conx.delta_pivot; i < conx.delta_faces.size(); ++i)
		{
			MxFaceID fid = conx.delta_faces[i];
			face(fid).remap_vertex(v2, v1);
			neighbors(v1).push_back(fid);
		}

	// Kill v2
	vertex_mark_invalid(v2);
	neighbors(v2).clear();
}

void MxStdModel::apply_expansion(const MxPairExpansion& conx)
{
	MxVertexID v1=conx.v1, v2=conx.v2;

	vertex(v2).pos = vertex(v1).pos - conx.dv2;
	vertex(v1).pos -= conx.dv1;

	for(unsigned long i = 0; i < conx.dead_faces.size(); ++i)
	{
	MxFaceID fid = conx.dead_faces[i];
	face_mark_valid(fid);
	neighbors(face(fid)[0]).push_back(fid);
	neighbors(face(fid)[1]).push_back(fid);
	neighbors(face(fid)[2]).push_back(fid);
	}

	for(unsigned long i = conx.delta_pivot; i < conx.delta_faces.size(); ++i)
		{
	MxFaceID fid = conx.delta_faces[i];
	face(fid).remap_vertex(v1, v2);
	neighbors(v2).push_back(fid);
			face_list_t::iterator f = std::find(neighbors(v1).begin(), neighbors(v1).end(), fid);
			bool found = f != neighbors(v1).end();
			assert_warning(found);
			neighbors(v1).erase(f);
		}

	vertex_mark_valid(v2);
}


void MxStdModel::contract(MxVertexID v1, MxVertexID v2,
			  const double *vnew, MxPairContraction *conx)
{
	compute_contraction(v1, v2, conx);
	conx->dv1 = vnew - vertex(v1).pos;
	conx->dv2 = vnew - vertex(v2).pos;
	apply_contraction(*conx);
}

void MxStdModel::compute_contraction(MxFaceID fid, MxFaceContraction *conx)
{
	const MxFace& f = face(fid);

	conx->f = fid;
	conx->dv1[X] = conx->dv1[Y] = conx->dv1[Z] = 0.0;
	conx->dv2[X] = conx->dv2[Y] = conx->dv2[Z] = 0.0;
	conx->dv3[X] = conx->dv3[Y] = conx->dv3[Z] = 0.0;

	conx->delta_faces.clear();
	conx->dead_faces.clear();


	mark_neighborhood(f[0], 0);
	mark_neighborhood(f[1], 0);
	mark_neighborhood(f[2], 0);

	mark_neighborhood(f[0], 1);
	mark_neighborhood_delta(f[1], +1);
	mark_neighborhood_delta(f[2], +1);

	fmark(fid, 0);		// don't include f in dead_faces

	partition_marked_neighbors(f[0], 2, conx->delta_faces, conx->dead_faces);
	partition_marked_neighbors(f[1], 2, conx->delta_faces, conx->dead_faces);
	partition_marked_neighbors(f[2], 2, conx->delta_faces, conx->dead_faces);
}

void MxStdModel::contract(MxVertexID v1, MxVertexID v2, MxVertexID v3,
			  const double *vnew,
			  face_list_t& changed)
{
	mark_neighborhood(v1, 0);
	mark_neighborhood(v2, 0);
	mark_neighborhood(v3, 0);
	changed.clear();
	collect_unmarked_neighbors(v1, changed);
	collect_unmarked_neighbors(v2, changed);
	collect_unmarked_neighbors(v3, changed);

	// Move v1 to vnew
	vertex(v1)[0] = vnew[X];
	vertex(v1)[1] = vnew[Y];
	vertex(v1)[2] = vnew[Z];

	// Replace occurrences of v2 & v3 with v1
	remap_vertex(v2, v1);
	remap_vertex(v3, v1);

	remove_degeneracy(changed);
}

void MxStdModel::contract(MxVertexID v1, const vertex_list_t& rest,
			  const double *vnew, face_list_t& changed)
{
	// Collect all effected faces
	mark_neighborhood(v1, 0);
	for(unsigned long i = 0; i < rest.size(); ++i)
		mark_neighborhood(rest[i], 0);

	changed.clear();

	collect_unmarked_neighbors(v1, changed);
	for(unsigned long i = 0; i < rest.size(); ++i)
		collect_unmarked_neighbors(rest[i], changed);

	// Move v1 to vnew
	vertex(v1)[0] = vnew[X];
	vertex(v1)[1] = vnew[Y];
	vertex(v1)[2] = vnew[Z];

	// Replace all occurrences of vi with v1
	for(unsigned long i = 0; i < rest.size(); ++i)
		remap_vertex(rest[i], v1);

	remove_degeneracy(changed);
}


