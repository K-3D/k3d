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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// merge_mesh_implementation

class merge_mesh_implementation :
	public k3d::legacy::mesh_source<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_source<k3d::persistent<k3d::node> > base;

public:
	merge_mesh_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Input mesh 1")) + init_description(_("First mesh to merge")) + init_value<k3d::legacy::mesh*>(0)),
		m_input_mesh2(init_owner(*this) + init_name("input_mesh2") + init_label(_("Input mesh 2")) + init_description(_("Second mesh to merge")) + init_value<k3d::legacy::mesh*>(0)),
		m_same_polyhedron(init_owner(*this) + init_name("same_polyhedron") + init_label(_("Same polyhedron")) + init_description(_("Merge meshes in a single polyhedron")) + init_value(true))
	{
		m_input_mesh1.changed_signal().connect(make_reset_mesh_slot());
		m_input_mesh2.changed_signal().connect(make_reset_mesh_slot());
		m_same_polyhedron.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input_mesh1 = m_input_mesh1.value();
		const k3d::legacy::mesh* const input_mesh2 = m_input_mesh2.value();

		// Create output geometry ...
		if(input_mesh1)
			k3d::legacy::deep_copy(*input_mesh1, Mesh);

		if(input_mesh2)
			k3d::legacy::deep_copy(*input_mesh2, Mesh);

		while(m_same_polyhedron.value() && Mesh.polyhedra.size() > 1)
		{
			// Merge two first polyhedra
			k3d::legacy::polyhedron* polyhedron1 = Mesh.polyhedra[0];
			k3d::legacy::polyhedron* polyhedron2 = Mesh.polyhedra[1];

			polyhedron1->faces.insert(polyhedron1->faces.end(), polyhedron2->faces.begin(), polyhedron2->faces.end());
			Mesh.polyhedra.erase(std::find(Mesh.polyhedra.begin(), Mesh.polyhedra.end(), polyhedron2));
			polyhedron2->faces.clear();
			delete polyhedron2;
		}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_mesh_implementation > factory(
			k3d::uuid(0x50aef311, 0xf5264da2, 0x9c5995e8, 0xdc2e4ddf),
			"MergeMesh",
			_("Merges two meshes into one"),
			"Mesh",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh2;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_same_polyhedron;
};

/////////////////////////////////////////////////////////////////////////////
// merge_mesh_factory

k3d::iplugin_factory& merge_mesh_factory()
{
	return merge_mesh_implementation::get_factory();
}

} // namespace libk3dmesh

