#ifndef MXQSLIM_INCLUDED
#define MXQSLIM_INCLUDED

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

#include "MxStdSlim.h"
#include "MxQMetric3.h"

#include "MxHeap.h"

#include <vector>

class MxQSlim :
	public MxStdSlim
{
protected:
	Heap* heap;
	std::vector<MxQuadric3> quadrics;

	void discontinuity_constraint(MxVertexID, MxVertexID, const face_list_t&);
	void collect_quadrics();
	void constrain_boundaries();

public:
	MxQSlim(MxStdModel& _m) :
		MxStdSlim(&_m)
	{
		heap = new Heap(64);
		quadrics.resize(_m.vert_count());
	}

	virtual ~MxQSlim() {}

	virtual void initialize();

	const MxQuadric3& vertex_quadric(MxVertexID v) { return quadrics[v]; }
};

class MxEdgeQSlim :
	public MxQSlim
{
private:
	class edge_info :
		public MxEdge, public Heapable
	{
	public:
		double vnew[3];
	};

	typedef std::vector<edge_info*> edge_list;
	std::vector<edge_list> edge_links;

	// Temporary variables used by methods
	vertex_list_t star, star2;

protected:
	double check_local_compactness(unsigned long v1, unsigned long v2, const double *vnew);
	double check_local_inversion(unsigned long v1, unsigned long v2, const double *vnew);
	unsigned long check_local_validity(unsigned long v1, unsigned long v2, const double *vnew);
	unsigned long check_local_degree(unsigned long v1, unsigned long v2, const double *vnew);
	void apply_mesh_penalties(edge_info*);
	void create_edge(MxVertexID i, MxVertexID j);
	void collect_edges();

	void compute_target_placement(edge_info*);
	void finalize_edge_update(edge_info*);

	virtual void compute_edge_info(edge_info*);
	virtual void update_pre_contract(const MxPairContraction&);
	virtual void update_post_contract(const MxPairContraction&);
	virtual void update_pre_expand(const MxPairContraction&);
	virtual void update_post_expand(const MxPairContraction&);

public:
	MxEdgeQSlim(MxStdModel&);
	virtual ~MxEdgeQSlim();

	void initialize();
	void initialize(const std::vector<MxEdge>& edges, unsigned long count);
	bool decimate(unsigned long target);

	void apply_contraction(const MxPairContraction& conx);
	void apply_expansion(const MxPairContraction& conx);
};

class MxFaceQSlim :
	public MxQSlim
{
private:
	class tri_info :
		public Heapable
	{
	public:
		MxFaceID f;
		double vnew[3];
	};

	std::vector<tri_info> f_info;

	void compute_face_info(MxFaceID);

public:
	MxFaceQSlim(MxStdModel&);

	void initialize();
	bool decimate(unsigned long target);
};

#endif // MXQSLIM_INCLUDED

