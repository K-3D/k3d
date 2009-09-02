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

#include "sds_cache.h"

#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

////////////
// sds_cache
////////////

void sds_cache::on_topology_changed(k3d::sds::catmull_clark_subdivider& Output, const k3d::mesh& InputMesh)
{
	k3d::mesh input_with_normals;
	calculate_normals(input_with_normals, InputMesh);
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(input_with_normals, *input_with_normals.primitives[polyhedron_idx(InputMesh)]));
	return_if_fail(polyhedron);
	return_if_fail(k3d::polyhedron::is_sds(*polyhedron));
	Output.set_levels(m_key.first);
	k3d::mesh::selection_t face_selections(polyhedron->face_selections.size(), 1.0);
	Output.create_mesh(*input_with_normals.points, *polyhedron, face_selections);
	Output.update_mesh(*input_with_normals.points, *polyhedron, input_with_normals.point_attributes, face_selections);
}

void sds_cache::on_geometry_changed(k3d::sds::catmull_clark_subdivider& Output, const k3d::mesh& InputMesh, const k3d::mesh::indices_t& ChangedPoints)
{
	k3d::mesh input_with_normals;
	calculate_normals(input_with_normals, InputMesh);
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(input_with_normals, *input_with_normals.primitives[polyhedron_idx(InputMesh)]));
	return_if_fail(polyhedron.get());
	return_if_fail(k3d::polyhedron::is_sds(*polyhedron));
	k3d::mesh::selection_t face_selections(polyhedron->face_selections.size(), 1.0);
	Output.update_mesh(*input_with_normals.points, *polyhedron, input_with_normals.point_attributes, face_selections);
}

const k3d::uint_t sds_cache::polyhedron_idx(const k3d::mesh& InputMesh)
{
	for(k3d::uint_t i = 0; i != InputMesh.primitives.size(); ++i)
	{
		if(InputMesh.primitives[i] == m_key.second)
		{
			return i;
		}
	}
	throw std::runtime_error("polyhedron not found!");
}

void sds_cache::calculate_normals(k3d::mesh& Output, const k3d::mesh& Input)
{
	Output = Input;
	const k3d::mesh::points_t& points = *Output.points;
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Output, *Output.primitives[polyhedron_idx(Input)]));
	return_if_fail(polyhedron.get());
	return_if_fail(k3d::polyhedron::is_sds(*polyhedron));
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = polyhedron->face_first_loops.size();
	k3d::mesh::normals_t uniform_normals(polyhedron->face_first_loops.size());
	for(k3d::uint_t face = face_begin; face != face_end; ++face)
		uniform_normals[face] = k3d::normalize(k3d::polyhedron::normal(polyhedron->vertex_points, polyhedron->clockwise_edges, *Output.points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]));
	k3d::mesh::normals_t& vertex_normals = Output.point_attributes.create("sds_normals", new k3d::mesh::normals_t(points.size()));
	for(k3d::uint_t face = face_begin; face != face_end; ++face)
	{
		const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
		const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
			for(k3d::uint_t edge = first_edge; ;)
			{
				vertex_normals[polyhedron->vertex_points[edge]] += uniform_normals[face];

				edge = polyhedron->clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}
}

} // namespace opengl

} // namespace painters

} // namespace module
