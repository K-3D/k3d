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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

#include <iterator>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// smooth_shade

class smooth_shade :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	smooth_shade(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Mesh);

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			typedef std::map<k3d::legacy::split_edge*, k3d::ri::normal> edge_normal_map_t;
			edge_normal_map_t edge_normal_map;

			typedef std::map<k3d::legacy::point*, k3d::ri::normal> point_normal_map_t;
			point_normal_map_t point_normal_map;

			typedef std::map<k3d::legacy::point*, unsigned long> edge_count_map_t;
			edge_count_map_t edge_count_map;

			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				const k3d::normal3 face_normal = k3d::legacy::normal(**face);

				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					edge_normal_map[edge] = face_normal;

					point_normal_map_t::iterator point = point_normal_map.insert(std::make_pair(edge->vertex, k3d::ri::normal(0, 0, 0))).first;
					point->second += face_normal;

					edge_count_map_t::iterator edge_count = edge_count_map.insert(std::make_pair(edge->vertex, 0)).first;
					edge_count->second += 1;

					if((*face)->first_edge == edge->face_clockwise)
						break;
				}
			}

			// Finish calculating average normals for all our points ...
			point_normal_map_t::iterator point = point_normal_map.begin();
			edge_count_map_t::const_iterator count = edge_count_map.begin();
			for(; point != point_normal_map.end() && count != edge_count_map.end(); ++point, ++count)
			{
				if(count->second != 0)
					point->second /= count->second;
			}

			// Now, store normals for every edge (i.e. facevarying) ... either from the face as a whole, or the average of all adjacent faces ...
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					set_normal(*edge, point_normal_map[edge->vertex], edge_normal_map[edge], edge->facevarying_data);
					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}
	}

	template<typename T>
	void set_normal(T& Object, const k3d::ri::normal& NewNormal, const k3d::ri::normal& DefaultNormal, k3d::legacy::parameters_t& Parameters)
	{
		if(Object.selection_weight)
			Parameters["N"] = NewNormal;
		else if(0 == Parameters.count("N"))
			Parameters["N"] = DefaultNormal;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<smooth_shade,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x7eed702d, 0xba274e6b, 0xb8a5db73, 0xba9c2382),
				"SmoothShade",
				_("Simulates smooth geometry by averaging polygon normals"),
				"Texture",
				k3d::iplugin_factory::DEPRECATED);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// smooth_shade_factory

k3d::iplugin_factory& smooth_shade_factory()
{
	return smooth_shade::get_factory();
}

} // namespace mesh

} // namespace module

