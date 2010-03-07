#ifndef MODULES_ADVANCED_OPENGL_PAINTERS_SDS_CACHE_H
#define MODULES_ADVANCED_OPENGL_PAINTERS_SDS_CACHE_H

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

/** \file Common SDS painter classes
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */


#include <k3dsdk/gl.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/property.h>
#include <k3dsdk/subdivision_surface/catmull_clark.h>

#include <boost/ptr_container/ptr_map.hpp>
#include <map>

#include "painter_cache.h"

namespace module
{

namespace opengl
{

namespace painters
{

class sds_cache : public cached_polyhedron_data<std::pair<k3d::uint_t, const k3d::mesh::primitive* const>, k3d::sds::catmull_clark_subdivider>
{
	typedef cached_polyhedron_data<std::pair<k3d::uint_t, const k3d::mesh::primitive* const>, k3d::sds::catmull_clark_subdivider> base;
public:
	sds_cache(const std::pair<k3d::uint_t, const k3d::mesh::primitive* const> Key, k3d::iproperty::changed_signal_t& ChangedSignal) : base(Key, ChangedSignal) {}
protected:
	void on_topology_changed(k3d::sds::catmull_clark_subdivider& Output, const k3d::mesh& InputMesh);
	void on_selection_changed(k3d::sds::catmull_clark_subdivider& Output, const k3d::mesh& InputMesh) {}
	void on_geometry_changed(k3d::sds::catmull_clark_subdivider& Output, const k3d::mesh& InputMesh, const k3d::mesh::indices_t& ChangedPoints);
private:
	const k3d::uint_t polyhedron_idx(const k3d::mesh& InputMesh);
	void calculate_normals(k3d::mesh& Output, const k3d::mesh& Input);
};


} // namespace opengl

} // namespace painters

} // namespace module

#endif // !MODULES_ADVANCED_OPENGL_PAINTERS_SDS_CACHE_H
