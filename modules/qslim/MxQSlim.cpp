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
		\brief Surface simplification using quadric error metrics
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "MxQSlim.h"
#include "MxTriangle.h"

#include <algorithm>

typedef MxQuadric3 Quadric;

void MxQSlim::initialize()
{
	collect_quadrics();
	if(boundary_weight > 0)
		constrain_boundaries();
}

void MxQSlim::collect_quadrics()
{
	quadrics.resize(m->vert_count());
	for(unsigned long j = 0; j < quadrics.size(); j++)
		quadrics[j].clear();

	for(MxFaceID i=0; i<m->face_count(); i++)
		{
			MxFace& f = m->face(i);

			Vec3 v1(m->vertex(f[0]));
			Vec3 v2(m->vertex(f[1]));
			Vec3 v3(m->vertex(f[2]));

			Vec4 p = (weighting_policy==MX_WEIGHT_RAWNORMALS) ?
			triangle_raw_plane(v1, v2, v3):
			triangle_plane(v1, v2, v3);
			Quadric Q(p[X], p[Y], p[Z], p[W], m->compute_face_area(i));

			switch(weighting_policy)
				{
					case MX_WEIGHT_ANGLE:
						for(unsigned long j = 0; j < 3; j++)
							{
								Quadric Q_j = Q;
								Q_j *= m->compute_corner_angle(i, j);
								quadrics[f[j]] += Q_j;
							}
					break;
					case MX_WEIGHT_AREA:
					case MX_WEIGHT_AREA_AVG:
						Q *= Q.area();
						// no break: fallthrough
					default:
						quadrics[f[0]] += Q;
						quadrics[f[1]] += Q;
						quadrics[f[2]] += Q;
					break;
				}
		}
}

void MxQSlim::discontinuity_constraint(MxVertexID i, MxVertexID j, const face_list_t& faces)
{
	for(unsigned long f = 0; f < faces.size(); ++f)
		{
	Vec3 org(m->vertex(i)), dest(m->vertex(j));
	Vec3 e = dest - org;

	Vec3 n;
	m->compute_face_normal(faces[f], n);

	Vec3 n2 = e ^ n;
	n2.Normalize();

	MxQuadric3 Q(n2, -(n2*org));
	Q *= boundary_weight;

	if(weighting_policy == MX_WEIGHT_AREA ||
		weighting_policy == MX_WEIGHT_AREA_AVG)
	{
		Q.set_area(norm2(e));
		Q *= Q.area();
	}

	quadrics[i] += Q;
	quadrics[j] += Q;
		}
}

void MxQSlim::constrain_boundaries()
{
	for(MxVertexID i = 0; i < m->vert_count(); ++i)
	{
		vertex_list_t star;
		m->collect_vertex_star(i, star);

		for(unsigned long j = 0; j < star.size(); j++)
			if(i < star[j])
			{
				face_list_t faces;
				m->collect_edge_neighbors(i, star[j], faces);
				if(faces.size() == 1)
					discontinuity_constraint(i, star[j], faces);
			}
	}
}

///////////////////////////////////////////////////////////////////////////
//
// IMPORTANT NOTE:  The check_xxx functions assume that the local
//                  neighborhoods have been marked so that each face
//                  is marked with the number of involved vertices it has.
//

MxEdgeQSlim::MxEdgeQSlim(MxStdModel& _m) :
	MxQSlim(_m)
{
	edge_links.resize(_m.vert_count());
}

MxEdgeQSlim::~MxEdgeQSlim()
{
	// Delete everything remaining in the heap
	//for(unsigned long i = 0; i < heap->size(); ++i)
	//	delete (edge_info*)heap->item(i);
}

double MxEdgeQSlim::check_local_compactness(unsigned long v1, unsigned long/*v2*/,
					    const double *vnew)
{
	const face_list_t& N1 = m->neighbors(v1);
	double c_min = 1.0;

	for(unsigned long i = 0; i < N1.size(); ++i)
	if( m->face_mark(N1[i]) == 1 )
	{
	    const MxFace& f = m->face(N1[i]);
	    Vec3 f_after[3];
	    for(unsigned long j=0; j<3; j++)
		f_after[j] = (f[j]==v1)?Vec3(vnew):Vec3(m->vertex(f[j]));

	    double c=triangle_compactness(f_after[0], f_after[1], f_after[2]);

	    if( c < c_min ) c_min = c;
	}

	return c_min;
}

double MxEdgeQSlim::check_local_inversion(unsigned long v1,unsigned long/*v2*/,const double *vnew)
{
	double Nmin = 1.0;
	const face_list_t& N1 = m->neighbors(v1);

	for(unsigned long i = 0; i < N1.size(); ++i)
	if( m->face_mark(N1[i]) == 1 )
	{
	    const MxFace& f = m->face(N1[i]);
	    Vec3 n_before;
	    m->compute_face_normal(N1[i], n_before);

	    Vec3 f_after[3];
	    for(unsigned long j=0; j<3; j++)
		f_after[j] = (f[j]==v1)?Vec3(vnew):Vec3(m->vertex(f[j]));

	    double delta = n_before *
		triangle_normal(f_after[0], f_after[1], f_after[2]);

	    if( delta < Nmin ) Nmin = delta;
	}

	return Nmin;
}

unsigned long MxEdgeQSlim::check_local_validity(unsigned long v1, unsigned long /*v2*/, const double *vnew)

{
	const face_list_t& N1 = m->neighbors(v1);
	unsigned long nfailed = 0;

	for(unsigned long i = 0; i < N1.size(); ++i)
	if( m->face_mark(N1[i]) == 1 )
	{
	    MxFace& f = m->face(N1[i]);
	    unsigned long k = f.find_vertex(v1);
	    unsigned long x = f[(k+1)%3];
	    unsigned long y = f[(k+2)%3];

		Vec3 d_yx = m->vertex(y) - m->vertex(x); // d_yx = y-x
		Vec3 d_vx = m->vertex(v1) - m->vertex(x); // d_vx = v-x
		Vec3 d_vnew = vnew - m->vertex(x); // d_vnew = vnew-x

		Vec3 f_n = d_yx ^ d_vx;
		Vec3 n = f_n ^ d_yx; // n = ((y-x)^(v-x))^(y-x)
		n.Normalize();

	    // assert(d_vx * n > -FEQ_EPS );
	    if(d_vnew * n < local_validity_threshold * (d_vx * n))
		nfailed++;
	}

	return nfailed;
}

unsigned long MxEdgeQSlim::check_local_degree(unsigned long v1, unsigned long v2, const double *)
{
	const face_list_t& N1 = m->neighbors(v1);
	const face_list_t& N2 = m->neighbors(v2);
	unsigned long i;
	unsigned long degree = 0;

	// Compute the degree of the vertex after contraction
	for(i=0; i<N1.size(); i++)
	if( m->face_mark(N1[i]) == 1 )
	    degree++;

	for(i=0; i<N2.size(); i++)
	if( m->face_mark(N2[i]) == 1 )
	    degree++;


	if( degree > vertex_degree_limit )
	return degree - vertex_degree_limit;
	else
	return 0;
}

void MxEdgeQSlim::apply_mesh_penalties(edge_info* info)
{
	unsigned long i;

	const face_list_t& N1 = m->neighbors(info->v1);
	const face_list_t& N2 = m->neighbors(info->v2);

	// Set up the face marks as the check_xxx() functions expect.
	for(i=0; i<N2.size(); i++) m->face_mark(N2[i], 0);
	for(i=0; i<N1.size(); i++) m->face_mark(N1[i], 1);
	for(i=0; i<N2.size(); i++) m->face_mark(N2[i], m->face_mark(N2[i])+1);

	double base_error = info->heap_key();
	double bias = 0.0;

	// Check for excess over degree bounds.
	unsigned long max_degree = std::max(N1.size(), N2.size());
	if( max_degree > vertex_degree_limit )
	bias += (max_degree-vertex_degree_limit) * meshing_penalty * 0.001;

#if ALTERNATE_DEGREE_BIAS
	// ??BUG:  This code was supposed to be a slight improvement over
	//         the earlier version above.  But it performs worse.
	//         Should check into why sometime.
	//
	unsigned long degree_excess = check_local_degree(info->v1, info->v2, info->vnew);
	if( degree_excess )
	bias += degree_excess * meshing_penalty;
#endif

	// Local validity checks
	//
	unsigned long nfailed = check_local_validity(info->v1, info->v2, info->vnew);
	nfailed += check_local_validity(info->v2, info->v1, info->vnew);
	if( nfailed )
	bias += nfailed*meshing_penalty;

	if( compactness_ratio > 0.0 )
	{
	double c1_min=check_local_compactness(info->v1, info->v2, info->vnew);
	double c2_min=check_local_compactness(info->v2, info->v1, info->vnew);
	double c_min = std::min(c1_min, c2_min);

	// !!BUG: There's a small problem with this: it ignores the scale
	//        of the errors when adding the bias.  For instance, enabling
	//        this on the cow produces bad results.  I also tried
	//        += (base_error + FEQ_EPS) * (2-c_min), but that works
	//        poorly on the flat planar thing.  A better solution is
	//        clearly needed.
	//
	//  NOTE: The prior heuristic was
	//        if( ratio*cmin_before > cmin_after ) apply penalty;
	//
	if( c_min < compactness_ratio )
	    bias += (1-c_min);
	}

#if USE_OLD_INVERSION_CHECK
	double Nmin1 = check_local_inversion(info->v1, info->v2, info->vnew);
	double Nmin2 = check_local_inversion(info->v2, info->v1, info->vnew);
	if( std::min(Nmin1, Nmin2) < 0.0 )
	bias += meshing_penalty;
#endif

	info->heap_key(base_error - bias);
}

void MxEdgeQSlim::compute_target_placement(edge_info* info)
{
	MxVertexID i=info->v1, j=info->v2;

	const Quadric &Qi=quadrics[i], &Qj=quadrics[j];

	Quadric Q = Qi;  Q += Qj;
	double e_min;

	if( placement_policy==MX_PLACE_OPTIMAL &&
	Q.optimize(&info->vnew[X], &info->vnew[Y], &info->vnew[Z]) )
	{
	e_min = Q.evaluate(info->vnew);
	}
	else
	{
	Vec3 vi(m->vertex(i)), vj(m->vertex(j));
	Vec3 best;

	if( placement_policy>=MX_PLACE_LINE && Q.optimize(best, vi, vj) )
	    e_min = Q.evaluate(best);
	else
	{
	    double ei = Q.evaluate(vi), ej = Q.evaluate(vj);

	    if( ei < ej ) { e_min = ei; best = vi; }
	    else          { e_min = ej; best = vj; }

	    if( placement_policy>=MX_PLACE_ENDORMID )
	    {
		Vec3 mid = (vi+vj)/2;
		double e_mid = Q.evaluate(mid);

		if( e_mid < e_min ) { e_min = e_mid; best = mid; }
	    }
	}

	info->vnew[X] = best[X];
	info->vnew[Y] = best[Y];
	info->vnew[Z] = best[Z];
	}

	if( weighting_policy == MX_WEIGHT_AREA_AVG )
	e_min /= Q.area();

	info->heap_key(-e_min);
}

void MxEdgeQSlim::finalize_edge_update(edge_info* info)
{
	if(meshing_penalty > 1)
		apply_mesh_penalties(info);

	if(info->is_in_heap())
		heap->update(info);
	else
		heap->insert(info);
}


void MxEdgeQSlim::compute_edge_info(edge_info* info)
{
	compute_target_placement(info);

	finalize_edge_update(info);
}

void MxEdgeQSlim::create_edge(MxVertexID i, MxVertexID j)
{
	edge_info* info = new edge_info;

	edge_links[i].push_back(info);
	edge_links[j].push_back(info);

	info->v1 = i;
	info->v2 = j;

	compute_edge_info(info);
}

void MxEdgeQSlim::collect_edges()
{

	for(MxVertexID i = 0; i < m->vert_count(); ++i)
	{
		vertex_list_t star;
		m->collect_vertex_star(i, star);

		for(unsigned long j = 0; j < star.size(); ++j)
		if(i < star[j])
			// Only add particular edge once
			create_edge(i, star[j]);
	}
}

void MxEdgeQSlim::initialize()
{
	MxQSlim::initialize();
	collect_edges();
}

void MxEdgeQSlim::initialize(const std::vector<MxEdge>& edges, unsigned long count)
{
	MxQSlim::initialize();
	for(unsigned long i=0; i<count; i++)
	create_edge(edges[i].v1, edges[i].v2);
}

void MxEdgeQSlim::update_pre_contract(const MxPairContraction& conx)
{
	MxVertexID v1=conx.v1, v2=conx.v2;
	star.clear();

	// Before, I was gathering the vertex "star" using:
	//      m->collect_vertex_star(v1, star);
	// This doesn't work when we initially begin with a subset of
	// the total edges.  Instead, we need to collect the "star"
	// from the edge links maintained at v1.
	for(unsigned long i = 0; i < edge_links[v1].size(); ++i)
		star.push_back(edge_links[v1][i]->opposite_vertex(v1));

	for(unsigned long i = 0; i < edge_links[v2].size(); ++i)
	{
	edge_info* e = edge_links[v2][i];
	MxVertexID u = (e->v1==v2)?e->v2:e->v1;
	assert_warning( e->v1==v2 || e->v2==v2 );
	assert_warning( u!=v2 );

	if( u==v1 || (std::find(star.begin(), star.end(), u) != star.end()) )
	{
		// This is a useless link --- kill it
		edge_list::iterator edge = std::find(edge_links[u].begin(), edge_links[u].end(), e);
		bool found = edge != edge_links[u].end();
		assert_warning(found);
		edge_links[u].erase(edge);

		heap->remove(e);
		if(u != v1) delete e; // (v1,v2) will be deleted later
	}
	else
	{
	    // Relink this to v1
	    e->v1 = v1;
	    e->v2 = u;
	    edge_links[v1].push_back(e);
	}
	}

	edge_links[v2].clear();
}

void MxEdgeQSlim::update_post_contract(const MxPairContraction& conx)
{
}

void MxEdgeQSlim::apply_contraction(const MxPairContraction& conx)
{
	// Pre-contraction update
	valid_verts--;
	valid_faces -= conx.dead_faces.size();
	quadrics[conx.v1] += quadrics[conx.v2];

	update_pre_contract(conx);

	m->apply_contraction(conx);

	update_post_contract(conx);

	// Must update edge info here so that the meshing penalties
	// will be computed with respect to the new mesh rather than the old
	for(unsigned long i = 0; i < edge_links[conx.v1].size(); ++i)
		compute_edge_info(edge_links[conx.v1][i]);
}

void MxEdgeQSlim::update_pre_expand(const MxPairContraction&)
{
}

void MxEdgeQSlim::update_post_expand(const MxPairContraction& conx)
{
	MxVertexID v1=conx.v1, v2=conx.v2;

	star.clear(); star2.clear();
	edge_links[conx.v2].clear();
	m->collect_vertex_star(conx.v1, star);
	m->collect_vertex_star(conx.v2, star2);

	unsigned long i = 0;
	while(i < edge_links[v1].size())
	{
	edge_info *e = edge_links[v1][i];
	MxVertexID u = (e->v1==v1)?e->v2:e->v1;
	assert_warning( e->v1==v1 || e->v2==v1 );
	assert_warning( u!=v1 && u!=v2 );

	bool v1_linked = std::find(star.begin(), star.end(), u) == star.end();
	bool v2_linked = std::find(star2.begin(), star2.end(), u) == star2.end();

	if( v1_linked )
	{
	    if( v2_linked )  create_edge(v2, u);
	    i++;
	}
	else
	{
	    // !! BUG: I expected this to be true, but it's not.
	    //         Need to find out why, and whether it's my
	    //         expectation or the code that's wrong.
	    // assert_warning(v2_linked);
	    e->v1 = v2;
	    e->v2 = u;
	    edge_links[v2].push_back(e);
	    edge_links[v1].erase(edge_links[v1].begin() + i);
	}

	compute_edge_info(e);
	}

	if(std::find(star.begin(), star.end(), v2) != star.end())
		// ?? BUG: Is it legitimate for there not to be an edge here ??
		create_edge(v1, v2);
}


void MxEdgeQSlim::apply_expansion(const MxPairContraction& conx)
{
	update_pre_expand(conx);

	m->apply_expansion(conx);

	//
	// Post-expansion update
	valid_verts++;
	valid_faces += conx.dead_faces.size();
	quadrics[conx.v1] -= quadrics[conx.v2];

	update_post_expand(conx);
}

bool MxEdgeQSlim::decimate(unsigned long target)
{
	MxPairContraction local_conx;

	while(valid_faces > target)
		{
			edge_info* info = (edge_info*)heap->extract();
			if(!info)
				return false;

			MxVertexID v1=info->v1;
			MxVertexID v2=info->v2;

	if( m->vertex_is_valid(v1) && m->vertex_is_valid(v2) )
	{
	    MxPairContraction& conx = local_conx;

	    m->compute_contraction(v1, v2, &conx, info->vnew);

	    if( will_join_only && conx.dead_faces.size() > 0 )
			continue;

	    apply_contraction(conx);
	}

	delete info;
		}

	return true;
}


MxFaceQSlim::MxFaceQSlim(MxStdModel& _m) :
	MxQSlim(_m)
{
}

void MxFaceQSlim::initialize()
{
	MxQSlim::initialize();

	f_info.resize(m->face_count());
	for(MxFaceID f = 0; f < m->face_count(); ++f)
		compute_face_info(f);
}

void MxFaceQSlim::compute_face_info(MxFaceID f)
{
	tri_info& info = f_info[f];
	info.f = f;

	MxVertexID i = m->face(f)[0];
	MxVertexID j = m->face(f)[1];
	MxVertexID k = m->face(f)[2];

	const Quadric& Qi = quadrics[i];
	const Quadric& Qj = quadrics[j];
	const Quadric& Qk = quadrics[k];

	Quadric Q = Qi;
	Q += Qj;
	Q += Qk;

	if( placement_policy == MX_PLACE_OPTIMAL &&
		Q.optimize(&info.vnew[X], &info.vnew[Y], &info.vnew[Z]) )
		{
			info.heap_key(-Q.evaluate(info.vnew));
		}
		else
		{
	  Vec3 v1(m->vertex(i)), v2(m->vertex(j)), v3(m->vertex(k));
	  double e1 = Q.evaluate(v1), e2 = Q.evaluate(v2), e3 = Q.evaluate(v3);

	  Vec3 best;
	  double e_min;

	  if( e1<=e2 && e1<=e3 ) { e_min=e1; best=v1; }
	  else if( e2<=e1 && e2<=e3 ) { e_min=e2; best=v2; }
	  else { e_min=e3; best=v3; }

			info.vnew[X] = best[X];
			info.vnew[Y] = best[Y];
			info.vnew[Z] = best[Z];
			info.heap_key(-e_min);
		}

	if(weighting_policy == MX_WEIGHT_AREA_AVG)
		info.heap_key(info.heap_key() / Q.area());

	if(info.is_in_heap())
		heap->update(&info);
	else
		heap->insert(&info);
}

bool MxFaceQSlim::decimate(unsigned long target)
{
	face_list_t changed;
	while(valid_faces > target)
		{
			tri_info* info = (tri_info*)heap->extract();
			if(!info)
				return false;

			MxFaceID f = info->f;
			MxVertexID v1 = m->face(f)[0],
			v2 = m->face(f)[1],
			v3 = m->face(f)[2];

			if(m->face_is_valid(f))
				{
					// Perform the actual contractions
					m->contract(v1, v2, v3, info->vnew, changed);

					// Update quadric of v1
					quadrics[v1] += quadrics[v2];
					quadrics[v1] += quadrics[v3];

					// Update valid counts
					valid_verts -= 2;
					for(unsigned long i=0; i<changed.size(); i++)
						if(!m->face_is_valid(changed[i]))
							valid_faces--;

					for(unsigned long i=0; i<changed.size(); i++)
						{
							if(m->face_is_valid(changed[i]))
								compute_face_info(changed[i]);
							else
								heap->remove(&f_info[changed[i]]);
						}
				}
		}

	return true;
}


