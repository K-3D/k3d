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
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/utility.h>

#include <boost/multi_array.hpp>

#include <iomanip>
#include <iterator>
#include <set>

namespace libk3dselection
{

/////////////////////////////////////////////////////////////////////////////
// select_clockwise

class select_clockwise :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	select_clockwise(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_number(init_owner(*this) + init_name("number") + init_label(_("number")) + init_description(_("Edge number")) + init_constraint(constraint::minimum(0L)) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_number.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Create output geometry ...
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		// For each polyhedron ...
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			std::set<k3d::legacy::split_edge*> selected_edges;
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					if(edge->selection_weight)
						selected_edges.insert(edge);

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}

			for(std::set<k3d::legacy::split_edge*>::iterator it = selected_edges.begin(); it != selected_edges.end(); ++it)
			{
				(*it)->selection_weight = 0.0;
			}
			unsigned long number = m_number.value();
			k3d::legacy::split_edge* edge = *(selected_edges.begin());
			for (unsigned long i = 0; i <= number; ++i)
				edge = edge->face_clockwise;
			edge->selection_weight = 1.0;
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_clockwise,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xea623f75, 0x19264b07, 0xbfa105a4, 0xba070bed),
				"SelectClockwise",
				"Select the clockwise edge of an edge. If multiple edges are selected the first one in the list is selected and all others are deselected",
				"Selection",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_number;
};

/////////////////////////////////////////////////////////////////////////////
// select_clockwise_factory

k3d::iplugin_factory& select_clockwise_factory()
{
	return select_clockwise::get_factory();
}

} // namespace libk3dselection

