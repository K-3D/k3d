#ifndef K3DSDK_ITEXTURE_RI_H
#define K3DSDK_ITEXTURE_RI_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

namespace k3d
{

class inetwork_render_frame;
namespace filesystem { class path; }
	
namespace ri
{

class istream;
class ishader_collection;
class render_state;
	
/// Abstract interface implemented by objects that can act as texture sources while drawing RenderMan graphics
class itexture :
	public virtual iunknown
{
public:
	virtual void setup_renderman_texture(inetwork_render_frame& Frame, istream& Stream, ishader_collection& Shaders) = 0;
	virtual const filesystem::path renderman_texture_path(const render_state& State) = 0;

protected:
	itexture() {}
	itexture(const itexture& ) {}
	itexture& operator=(const itexture& ) { return *this; }
	virtual ~itexture() {}
};

} // namespace ri

} // namespace k3d

#endif // K3DSDK_ITEXTURE_RI_H

