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

/** \file
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

namespace module
{

namespace mesh
{

namespace detail
{

/// Handles negative numbers the way I'd expect it to
long mymod(const long a, const long b)
{
	long result = a;

	while(result < 0)
		result += b;
	while(result >= b)
		result -= b;

	return result;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// move_first_edge_implementation

class move_first_edge_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	move_first_edge_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("distance")) + init_description(_("Face distance")) + init_value(0UL) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_distance.changed_signal().connect(make_reset_mesh_slot());
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
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Mesh);

		const long distance = m_distance.pipeline_value();

		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				if(!(*face)->selection_weight)
					continue;

				std::vector<k3d::legacy::split_edge*> edges;
				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					edges.push_back(edge);
					if(edge->face_clockwise == (*face)->first_edge)
						break;
				}

				(*face)->first_edge = edges[detail::mymod(distance, edges.size())];
			}
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<move_first_edge_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x81c766ed, 0x2cb943c7, 0x91e9e680, 0xeba9bc4b),
				"MoveFirstEdge",
				_("Changes the first edge for selected polygons"),
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_distance;
};

/////////////////////////////////////////////////////////////////////////////
// move_first_edge_factory

k3d::iplugin_factory& move_first_edge_factory()
{
	return move_first_edge_implementation::get_factory();
}

} // namespace mesh

} // namespace module


