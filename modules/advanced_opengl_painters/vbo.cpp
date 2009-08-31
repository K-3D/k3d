// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "vbo.h"

#include <k3dsdk/hints.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <boost/any.hpp>
#include <boost/scoped_ptr.hpp>

#include <map>

namespace module
{

namespace opengl
{

namespace painters
{

////////
// class vbo
///////

vbo::vbo( )
{
	glGenBuffers(1, &m_name);
}

vbo::~ vbo( )
{
	glDeleteBuffers(1, &m_name);
}

vbo::operator GLuint( ) const
{
	return m_name;
}

void validate(const vbo& VBO)
{
	if(!glIsBuffer(VBO))
	{
		throw vbo_exception("point_vbo is not a buffer!");
	}
}

void bind_vertex_buffer(const vbo& VBO)
{
	validate(VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void bind_texture_buffer(const vbo& VBO)
{
	validate(VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glTexCoordPointer(1, GL_DOUBLE, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void clean_vbo_state()
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_FOG_COORDINATE_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

///////////////////////////////
// point_vbo
///////////////////////////////

void point_vbo::on_topology_changed(point_data& Output, const k3d::mesh& InputMesh)
{
	k3d::log() << debug << "topology changed" << std::endl;
	const k3d::mesh::points_t& points = *InputMesh.points;
	glBindBuffer(GL_ARRAY_BUFFER, Output.points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points[0]) * points.size(), &points[0], GL_STATIC_DRAW);
}

void point_vbo::on_selection_changed(point_data& Output, const k3d::mesh& InputMesh)
{
	k3d::log() << debug << "selection changed" << std::endl;
	const k3d::mesh::selection_t& selection = *InputMesh.point_selection;
	glBindBuffer(GL_ARRAY_BUFFER, Output.selection);
	glBufferData(GL_ARRAY_BUFFER, sizeof(selection[0]) * selection.size(), &selection[0], GL_STATIC_DRAW);
}

void point_vbo::on_geometry_changed(point_data& Output, const k3d::mesh& InputMesh, const k3d::mesh::indices_t& ChangedPoints)
{
	const k3d::mesh::points_t& points = *InputMesh.points;
	glBindBuffer(GL_ARRAY_BUFFER, Output.points);
	if(ChangedPoints.empty()) // caller didn't tell which points changed, so update everything
	{
		k3d::log() << debug << "geometry changed, full rebuild" << std::endl;
		glBufferData(GL_ARRAY_BUFFER, sizeof(points[0]) * points.size(), &points[0], GL_STATIC_DRAW);
	}
	else
	{
		k3d::log() << debug << "geometry changed, mapping points" << std::endl;
		k3d::point3* opengl_points = static_cast<k3d::point3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
		for (size_t index = 0; index != ChangedPoints.size(); ++index)
		{
			opengl_points[ChangedPoints[index]] = points[ChangedPoints[index]];
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

} // namespace painters

} // namespace opengl

} // namespace module
