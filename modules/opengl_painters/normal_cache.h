#ifndef NORMAL_CACHE_H_
#define NORMAL_CACHE_H_

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

/** \file Cache normals
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3dsdk/mesh.h>
#include "painter_cache.h"

namespace k3d { class icamera; }

namespace module
{

namespace opengl
{

namespace painters
{

class normal_cache : public scheduler
{
public:
	normal_cache(const k3d::mesh* const Mesh) : m_mesh(Mesh) {}
	/// Return the point ("smooth") normals
	const k3d::mesh::normals_t& point_normals(k3d::inode* Painter);
	/// Return the face ("flat") normals
	const k3d::mesh::normals_t& face_normals(k3d::inode* Painter);
protected:
	/// Scheduler implementation
	void on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter);
	void on_schedule(k3d::inode* Painter);
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter);
private:
	const k3d::mesh* const m_mesh;
};

/// Returns true if Point is on a backfacing component
bool backfacing(const k3d::point3& Point, k3d::icamera& Camera, const k3d::normal3& Normal);

} // namespace painters

} // namespace opengl

} // namespace module

#endif /*NORMAL_CACHE_H_*/
