#ifndef MXSTDSLIM_INCLUDED
#define MXSTDSLIM_INCLUDED

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
		\brief Core simplification interface
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#include "MxStdModel.h"

#define MX_PLACE_ENDPOINTS 0
#define MX_PLACE_ENDORMID  1
#define MX_PLACE_LINE      2
#define MX_PLACE_OPTIMAL   3

#define MX_WEIGHT_UNIFORM       0
#define MX_WEIGHT_AREA          1
#define MX_WEIGHT_ANGLE         2
#define MX_WEIGHT_AVERAGE       3
#define MX_WEIGHT_AREA_AVG      4
#define MX_WEIGHT_RAWNORMALS    5

class MxStdSlim
{
protected:
	MxStdModel *m;

public:
	unsigned int valid_verts;
	unsigned int valid_faces;

	int placement_policy;
	int weighting_policy;
	bool will_join_only;

	double boundary_weight;
	double compactness_ratio;
	double meshing_penalty;
	double local_validity_threshold;
	unsigned long vertex_degree_limit;

public:
	MxStdSlim(MxStdModel *m0);
	virtual ~MxStdSlim() {}

	virtual void initialize() = 0;
	virtual bool decimate(unsigned long) = 0;

	MxStdModel& model() { return *m; }
};

#endif // MXSTDSLIM_INCLUDED

