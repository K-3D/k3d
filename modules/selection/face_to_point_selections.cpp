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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// face_to_point_selections

class face_to_point_selections :
	public k3d::mesh_selection_modifier<k3d::node>
{
	typedef k3d::mesh_selection_modifier<k3d::node> base;

public:
	face_to_point_selections(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_keep_point_selection(init_owner(*this) + init_name("keep_point_selection") + init_label(_("Keep Point Selection")) + init_description(_("Keep the original point selection, just adding points of selected faces.")) + init_value(false)),
		m_keep_face_selection(init_owner(*this) + init_name("keep_face_selection") + init_label(_("Keep Face Selection")) + init_description(_("Keep the original face selection.")) + init_value(false))
	{
		m_keep_point_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));

		m_keep_face_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
		if(!m_keep_point_selection.pipeline_value())
			point_selection.assign(point_selection.size(), 0.0);
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			if((*primitive)->type != "polyhedron")
				continue;

			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron)
				continue;
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = polyhedron->face_selections.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(!polyhedron->face_selections[face])
					continue;
				const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
				const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
				for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
					for(k3d::uint_t edge = first_edge; ;)
					{
						point_selection[polyhedron->edge_points[edge]] = polyhedron->face_selections[face];

						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}
			if(!m_keep_face_selection.pipeline_value())
				polyhedron->face_selections.assign(face_end, 0.0);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_to_point_selections,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x7a01c414, 0x834071c5, 0xae544194, 0x2861defc),
				"FaceToPointSelections",
				_("Selects the points belonging to selected faces"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_keep_point_selection;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_keep_face_selection;
};

/////////////////////////////////////////////////////////////////////////////
// face_to_point_selections_factory

k3d::iplugin_factory& face_to_point_selections_factory()
{
	return face_to_point_selections::get_factory();
}

} // namespace selection

} // namespace module

