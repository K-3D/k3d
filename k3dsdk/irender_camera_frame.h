#ifndef K3DSDK_IRENDER_CAMERA_FRAME_H
#define K3DSDK_IRENDER_CAMERA_FRAME_H

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

class icamera;
namespace filesystem { class path; }
	
/// Abstract interface implemented by objects that can render a still image using a camera
class irender_camera_frame :
	public virtual k3d::iunknown
{
public:
	virtual bool render_camera_frame(icamera& Camera, const filesystem::path& OutputImage, const bool ViewCompletedImage) = 0;

protected:
	irender_camera_frame() {}
	irender_camera_frame(const irender_camera_frame&) {}
	irender_camera_frame& operator = (const irender_camera_frame&) { return *this; }
	virtual ~irender_camera_frame() {}
};

} // namespace k3d

#endif // K3DSDK_IRENDER_CAMERA_FRAME_H

