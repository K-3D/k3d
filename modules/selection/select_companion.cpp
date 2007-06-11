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
// select_companion

/** \todo Decide whether this plugin is needed at all */
class select_companion :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	select_companion(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::replace_selection(m_mesh_selection.value(), Mesh);

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			std::set<k3d::legacy::split_edge*> selected_edges;
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					if(edge->selection_weight)
						selected_edges.insert(edge);

					if(edge->face_clockwise == (*face)->first_edge)
						break;
				}
			}

			for(std::set<k3d::legacy::split_edge*>::iterator edge = selected_edges.begin(); edge != selected_edges.end(); ++edge)
			{
				if((*edge)->companion)
				{
					(*edge)->companion->selection_weight = 1.0;

					(*edge)->selection_weight = 0.0;
				}
				else
				{
					(*edge)->selection_weight = 1.0;
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
};

/////////////////////////////////////////////////////////////////////////////
// select_companion_factory

k3d::iplugin_factory& select_companion_factory()
{
	return select_companion::get_factory();
}

} // namespace libk3dselection
