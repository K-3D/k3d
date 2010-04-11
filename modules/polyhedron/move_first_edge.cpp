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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/// Handles negative numbers the way I'd expect it to
static k3d::int32_t mymod(const k3d::int32_t a, const k3d::int32_t b)
{
	k3d::int32_t result = a;

	while(result < 0)
		result += b;
	while(result >= b)
		result -= b;

	return result;
}

/////////////////////////////////////////////////////////////////////////////
// move_first_edge_implementation

class move_first_edge_implementation :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	move_first_edge_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("distance")) + init_description(_("Face distance")) + init_value(0UL) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_distance.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::int32_t distance = m_distance.pipeline_value();

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			k3d::mesh::counts_t counts;
			k3d::polyhedron::create_loop_edge_count_lookup(*polyhedron, counts);

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(!polyhedron->face_selections[face])
					continue;

				const k3d::uint_t loop = polyhedron->face_first_loops[face];
				const k3d::uint_t loop_distance = mymod(distance, counts[loop]);

				for(k3d::uint_t i = 0; i != loop_distance; ++i)
					polyhedron->loop_first_edges[loop] = polyhedron->clockwise_edges[polyhedron->loop_first_edges[loop]];
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
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
				"Polyhedron",
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

} // namespace polyhedron

} // namespace module


