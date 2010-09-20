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

#include <k3dsdk/point4.h>
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/utility_gl.h>
#include <modules/qslim/MxMath.h>

namespace k3d
{

namespace gl
{

const point3 render_state::project(const point3& WorldCoords) const
{
	k3d::point3 result;
	
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLint error = gluProject(
		WorldCoords[0],
		WorldCoords[1],
		WorldCoords[2],
		modelview,
		projection,
		gl_viewport,
		&result[0],
		&result[1],
		&result[2]);
	
	return_val_if_fail(error, result);

	return result;
}

} // namespace gl

} // namespace k3d