#ifndef K3DSDK_RENDER_STATE_RI_H
#define K3DSDK_RENDER_STATE_RI_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
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

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "types_ri.h"

namespace k3d
{

class iprojection;
class inetwork_render_frame;
class matrix4;
	
namespace ri
{

class istream;
class ishader_collection;
	
/// Used to pass (potentially) useful rendering state from the renderer to the object being rendered
class render_state
{
public:
	render_state(inetwork_render_frame& Frame, istream& Stream, ishader_collection& Shaders, iprojection& Projection, const render_context_t RenderContext, const sample_times_t& SampleTimes, const unsigned_integer SampleIndex, const matrix4& CameraMatrix) :
		frame(Frame),
		stream(Stream),
		shaders(Shaders),
		projection(Projection),
		render_context(RenderContext),
		sample_times(SampleTimes),
		sample_index(SampleIndex),
		camera_matrix(CameraMatrix)
	{
	}

	inetwork_render_frame& frame;
	istream& stream;
	ishader_collection& shaders;
	iprojection& projection;
	render_context_t render_context;
	sample_times_t sample_times;
	unsigned_integer sample_index;
	matrix4 camera_matrix;
};

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_RENDER_STATE_RI_H

