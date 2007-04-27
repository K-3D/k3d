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

MxStdSlim::MxStdSlim(MxStdModel *m0)
{
	m = m0;

	// Externally visible variables
	placement_policy = MX_PLACE_OPTIMAL;
	weighting_policy = MX_WEIGHT_AREA;
	boundary_weight = 1000.0;
	compactness_ratio = 0.0;
	meshing_penalty = 1.0;
	local_validity_threshold = 0.0;
	vertex_degree_limit = 24;
	will_join_only = false;

	valid_faces = 0;
	valid_verts = 0;

	for(unsigned long i = 0; i < m->face_count(); ++i)
		if(m->face_is_valid(i))
			valid_faces++;

	for(unsigned long i = 0; i < m->vert_count(); ++i)
		if(m->vertex_is_valid(i))
			valid_verts++;
}


