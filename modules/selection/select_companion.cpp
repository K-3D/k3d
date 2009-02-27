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
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/node.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <iterator>
#include <set>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// select_companion

/** \todo Decide whether this plugin is needed at all */
class select_companion :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	select_companion(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_keep_original_selection(init_owner(*this) + init_name("keep_original_selection") + init_label(_("Keep Original Selection")) + init_description(_("Keep the original selection, rather than selecting only the companions")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_keep_original_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(Input));
		if(!input_polyhedron)
			return;
		
		k3d::mesh::bools_t boundary_edges;
		k3d::polyhedron::create_edge_adjacency_lookup(input_polyhedron->edge_points, input_polyhedron->clockwise_edges, boundary_edges, m_companions);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron(k3d::polyhedron::validate(Output));
		if(!output_polyhedron)
			return;
		
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		
		return_if_fail(output_polyhedron->edge_selections.size() == m_companions.size());
		
		const k3d::bool_t keep_selection = m_keep_original_selection.pipeline_value();
		
		for(k3d::uint_t edge = 0; edge != output_polyhedron->edge_selections.size(); ++edge)
		{
			// select the companions
			if(output_polyhedron->edge_selections[edge])
			{
				output_polyhedron->edge_selections[m_companions[edge]] = output_polyhedron->edge_selections[edge];
			}
			
			// deselect original, if requested
			if(!keep_selection && edge != m_companions[edge])
				output_polyhedron->edge_selections[edge] = 0.0;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_companion,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x636a062d, 0x80fc4b53, 0x8f0149e8, 0x4de9c520),
				"SelectCompanion",
				"Select the companion of an edge. If multiple edges are selected the first one in the list is selected and all others are deselected",
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_keep_original_selection;
	k3d::mesh::indices_t m_companions;
};

/////////////////////////////////////////////////////////////////////////////
// select_companion_factory

k3d::iplugin_factory& select_companion_factory()
{
	return select_companion::get_factory();
}

} // namespace selection

} // namespace module
