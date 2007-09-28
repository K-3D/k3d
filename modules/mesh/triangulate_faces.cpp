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
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/utility.h>

#include "helpers.h"

#include <iterator>

namespace libk3dmesh
{

namespace detail
{

struct do_triangulate
{
	do_triangulate(const bool WithHolesOnly) :
		with_holes_only(WithHolesOnly)
	{
	}

	bool operator()(k3d::legacy::face* Face)
	{
		if(!Face->selection_weight)
			return false;

		if(with_holes_only && !Face->holes.size())
			return false;

		if(helpers::edge_number(Face->first_edge) > 3)
			return true;

		return false;
	}

	const bool with_holes_only;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// triangulate_faces_implementation

class triangulate_faces_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	triangulate_faces_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_holes_only(init_owner(*this) + init_name("holes_only") + init_label(_("Holes only")) + init_description(_("Triangulate only faces that contain holes")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_holes_only.changed_signal().connect(make_reset_mesh_slot());
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
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);

		const bool holes_only = m_holes_only.pipeline_value();

		// For each polyhedron ...
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron_iterator = Mesh.polyhedra.begin(); polyhedron_iterator != Mesh.polyhedra.end(); ++polyhedron_iterator)
		{
			k3d::legacy::polyhedron& polyhedron = **polyhedron_iterator;

			// Triangulate faces that are not triangles
			k3d::legacy::polyhedron::faces_t selected_faces;
			k3d::copy_if(polyhedron.faces.begin(), polyhedron.faces.end(), std::inserter(selected_faces, selected_faces.end()), detail::do_triangulate(holes_only));

			// For each face ...
			for(k3d::legacy::polyhedron::faces_t::iterator face = selected_faces.begin(); face != selected_faces.end(); ++face)
			{
				k3d::legacy::polyhedron::faces_t faces;
				faces.push_back(*face);

				k3d::legacy::triangulate(faces, polyhedron.faces, Mesh.points);

				polyhedron.faces.erase(std::remove(polyhedron.faces.begin(), polyhedron.faces.end(), *face), polyhedron.faces.end());
				delete *face;
			}

			// Set companions
			k3d::legacy::set_companions(polyhedron);
			assert_warning(k3d::legacy::is_valid(polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<triangulate_faces_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x3796ae29, 0xbcc84a5c, 0xb6bae620, 0x87a9b1bd),
				"TriangulateFaces",
				_("Converts input faces into triangles"),
				"Polygons",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_holes_only;
};

/////////////////////////////////////////////////////////////////////////////
// triangulate_faces_factory

k3d::iplugin_factory& triangulate_faces_factory()
{
	return triangulate_faces_implementation::get_factory();
}

} // namespace libk3dmesh

