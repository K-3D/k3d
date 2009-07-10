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
#include <k3dsdk/basic_math.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

#include <set>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// cap_hole_implementation

class cap_hole_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	cap_hole_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_cap_polyhedra(init_owner(*this) + init_name("cap_polyhedra") + init_label(_("Cap polyhedra")) + init_description(_("Cap holes (missing faces) in polyhedra")) + init_value(true)),
		m_cap_faces(init_owner(*this) + init_name("cap_faces") + init_label(_("Cap faces")) + init_description(_("Cap holes in faces")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_cap_polyhedra.changed_signal().connect(make_reset_mesh_slot());
		m_cap_faces.changed_signal().connect(make_reset_mesh_slot());
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
		k3d::geometry::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		// For each polyhedron ...
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			if(!(*polyhedron)->faces.size())
				continue;

			const bool cap_polyhedra = m_cap_polyhedra.pipeline_value();
			const bool cap_faces = m_cap_faces.pipeline_value();

			k3d::imaterial* material = (*polyhedron)->faces.front()->material;

			// Get the set of all selected edges ...
			std::set<k3d::legacy::split_edge*> selected_edges;
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; )
				{
					if(edge->selection_weight > 0)
						selected_edges.insert(edge);

					edge = edge->face_clockwise;
					if(edge == (*face)->first_edge)
						break;
				}

			if(selected_edges.empty())
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
					for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; )
					{
						selected_edges.insert(edge);

						edge = edge->face_clockwise;
						if(edge == (*face)->first_edge)
							break;
					}

			// If we aren't going to cap polyhedra, remove all face loops from the edge list ...
			if(!cap_polyhedra)
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; )
					{
						selected_edges.erase(edge);

						edge = edge->face_clockwise;
						if(edge == (*face)->first_edge)
							break;
					}
				}
			}

			// If we aren't going to cap faces, remove all face holes from the edge list ...
			if(!cap_faces)
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					for(k3d::legacy::face::holes_t::const_iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
					{
						for(k3d::legacy::split_edge* edge = *hole; edge; )
						{
							selected_edges.erase(edge);

							edge = edge->face_clockwise;
							if(edge == *hole)
								break;
						}
					}
				}
			}

			// For each edge ...
			for(std::set<k3d::legacy::split_edge*>::iterator e = selected_edges.begin(); e != selected_edges.end(); ++e)
			{
				// Skip interior edges ...
				if((*e)->companion)
					continue;

				// Get the entire loop ...
				std::vector<k3d::legacy::split_edge*> old_edges;
				for(k3d::legacy::split_edge* edge = *e; edge; )
				{
					old_edges.push_back(edge);

					edge = next_edge(edge);

					if(edge == *e)
						break;
				}

				// Create new edges ...
				std::vector<k3d::legacy::split_edge*> new_edges;
				for(unsigned long i = 0; i != old_edges.size(); ++i)
				{
					k3d::legacy::split_edge* const old_edge = old_edges[(old_edges.size() - i) % old_edges.size()];
					k3d::legacy::split_edge* const new_edge = new k3d::legacy::split_edge(old_edge->face_clockwise->vertex);

					k3d::legacy::join_edges(*new_edge, *old_edge);
					new_edges.push_back(new_edge);
				}

				// Create a new face ...
				k3d::legacy::loop_edges(new_edges.begin(), new_edges.end());
				(*polyhedron)->faces.push_back(new k3d::legacy::face(new_edges.front(), material));
			}

			assert_warning(is_valid(**polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	/// Returns the next exterior edge (if any) in *counter-clockwise* order
	static k3d::legacy::split_edge* next_edge(k3d::legacy::split_edge* const Edge)
	{
		return_val_if_fail(Edge, 0);
		return_val_if_fail(!Edge->companion, 0);

		k3d::legacy::split_edge* edge = Edge->face_clockwise;
		return_val_if_fail(edge != Edge, 0);

		while(edge)
		{
			if(!edge->companion)
				return edge;

			edge = edge->companion->face_clockwise;
		}

		return 0;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cap_hole_implementation,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd9900dbf, 0x3d854a36, 0xbd05a3eb, 0x1379a89a),
				"CapHoles",
				_("Caps (fills) holes in a polyhedron"),
				"Polygon",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap_polyhedra;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_cap_faces;
};

/////////////////////////////////////////////////////////////////////////////
// cap_hole_factory

k3d::iplugin_factory& cap_hole_factory()
{
	return cap_hole_implementation::get_factory();
}

} // namespace mesh

} // namespace module

