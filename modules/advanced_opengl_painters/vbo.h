#ifndef MODULES_ADVANCED_OPENGL_PAINTERS_VBO_H
#define MODULES_ADVANCED_OPENGL_PAINTERS_VBO_H

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

/** \file Helper functions and classes to manage a global data cache for the mesh painters
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3dsdk/gl.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/properties.h>

#include "painter_cache.h"

#include <stdexcept>

namespace module
{

namespace opengl
{

namespace painters
{

/// Exception for VBO-related errors
class vbo_exception : public std::runtime_error
{
	typedef std::runtime_error base;
public:
	vbo_exception(const std::string& Message) : base("VBO error: " + Message) {}
};

/// Convenience wrapper for OpenGL vertex buffer objects
class vbo
{
public:
	vbo();
	~vbo();
	/// Implicit conversion so we can use a vbo instance as an argument to OpenGL functions
	operator GLuint() const;
private:
	vbo(const vbo&);
	vbo& operator=(const vbo&);

	GLuint m_name;
};

/// Bind a vertex buffer VBO, and enable the required OpenGL state
void bind_vertex_buffer(const vbo& VBO);

/// Bind a texture coordinate VBO, and enable the required OpenGL state
void bind_texture_buffer(const vbo& VBO);

/// Disable all VBOs
void clean_vbo_state();

//////////////////////////////////////////
// point_vbo
//////////////////////////////////////////

struct point_data
{
	vbo points;
	vbo selection;
};

/// Cached object for point data stored in VBOs
class point_vbo : public cached_polyhedron_data<const k3d::mesh::points_t* const, point_data>
{
	typedef const k3d::mesh::points_t* const key_t;
	typedef cached_polyhedron_data<key_t, point_data> base;
public:
	point_vbo(const key_t Key, k3d::iproperty::changed_signal_t& ChangedSignal) : base(Key, ChangedSignal) {}
private:
	void on_topology_changed(point_data& Output, const k3d::mesh& InputMesh);
	void on_selection_changed(point_data& Output, const k3d::mesh& InputMesh);
	void on_geometry_changed(point_data& Output, const k3d::mesh& InputMesh, const k3d::mesh::indices_t& ChangedPoints);
};

} // namespace painters

} // namespace opengl

} // namespace module

#endif // !MODULES_ADVANCED_OPENGL_PAINTERS_VBO_H
