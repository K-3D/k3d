// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imesh_selection_algorithm.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// make_edge_selection

class make_edge_selection :
	public k3d::imesh_selection_algorithm
{
	typedef k3d::imesh_selection_algorithm base;

public:
	const k3d::selection::set create_mesh_selection(const k3d::mesh& Mesh)
	{
		k3d::selection::set results;

		boost::scoped_ptr<k3d::geometry::primitive_selection::storage> primitive_selection(k3d::geometry::primitive_selection::create(results));

		// For each primitive in the mesh ...
		k3d::uint_t primitive = 0;
		for(k3d::mesh::primitives_t::const_iterator p = Mesh.primitives.begin(); p != Mesh.primitives.end(); ++p, ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh, **p));
			if(polyhedron)
			{
				// Convert point and face selections to edge selections ...
				const k3d::uint_t face_begin = 0;
				const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
				for(k3d::uint_t face = face_begin; face != face_end; ++face)
				{
					const k3d::uint_t face_loop_begin = polyhedron->face_first_loops[face];
					const k3d::uint_t face_loop_end = face_loop_begin + polyhedron->face_loop_counts[face];
					for(k3d::uint_t face_loop = face_loop_begin; face_loop != face_loop_end; ++face_loop)
					{
						const k3d::uint_t first_edge = polyhedron->loop_first_edges[face_loop];
						for(k3d::uint_t edge = first_edge; ; )
						{
							if(polyhedron->face_selections[face])
								{
								k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::EDGE, edge, edge+1, 1.0);
								}
							else if((*Mesh.point_selection)[polyhedron->vertex_points[edge]] || (*Mesh.point_selection)[polyhedron->vertex_points[polyhedron->clockwise_edges[edge]]])
								{
								k3d::geometry::primitive_selection::append(*primitive_selection, primitive, primitive+1, k3d::selection::EDGE, edge, edge+1, 1.0);
								}

							edge = polyhedron->clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}
					}
				} 

				continue;
			}
		}

		return results;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<make_edge_selection,
			k3d::interface_list<k3d::imesh_selection_algorithm> > factory(
				k3d::uuid(0x5ad75f8f, 0x294732de, 0xd793dfae, 0x230175a9),
				"MakeEdgeSelection",
				_("Converts primitive selections to edge selections."),
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_edge_selection_factory

k3d::iplugin_factory& make_edge_selection_factory()
{
	return make_edge_selection::get_factory();
}

} // namespace selection

} // namespace module

