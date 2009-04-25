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
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
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
	public k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > >
{
	typedef k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > > base;

public:
	select_companion(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_keep_original_selection(init_owner(*this) + init_name("keep_original_selection") + init_label(_("Keep Original Selection")) + init_description(_("Keep the original selection, rather than selecting only the companions")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
      k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_keep_original_selection.changed_signal().connect(k3d::hint::converter<
      k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
    const k3d::bool_t keep_selection = m_keep_original_selection.pipeline_value();
      
    k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
      boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
      if(!polyhedron)
        continue;
	
      const k3d::mesh::selection_t original_edge_selections = polyhedron->edge_selections;
  	
      k3d::mesh::bools_t boundary_edges;
      k3d::mesh::indices_t companions;
      k3d::polyhedron::create_edge_adjacency_lookup(polyhedron->edge_points, polyhedron->clockwise_edges, boundary_edges, companions);
    
      std::fill(polyhedron->edge_selections.begin(), polyhedron->edge_selections.end(), 0.0);
       
      const k3d::uint_t edge_begin = 0;
      const k3d::uint_t edge_end = edge_begin + polyhedron->edge_selections.size(); 
      for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
      {
        if(original_edge_selections[edge])
        {
          polyhedron->edge_selections[companions[edge]] = original_edge_selections[edge];

          if(keep_selection)
            polyhedron->edge_selections[edge] = original_edge_selections[edge];
        }
      }
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
};

/////////////////////////////////////////////////////////////////////////////
// select_companion_factory

k3d::iplugin_factory& select_companion_factory()
{
	return select_companion::get_factory();
}

} // namespace selection

} // namespace module

