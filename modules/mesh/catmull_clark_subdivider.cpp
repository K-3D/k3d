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
		\brief Modifier object that takes a mesh as input and returns the Catmull-Clark subdivision of this mesh.
		\author Bart Janssens <bart.janssens@polytechnic.be>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <subdivision_surface/k3d_sds_binding.h>

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivider

class catmull_clark_subdivider :
		public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	catmull_clark_subdivider(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_material(dynamic_cast<k3d::imaterial*>(k3d::default_material(Document))),
		m_level(init_owner(*this) + init_name("level") + init_label(_("Level")) + init_description(_("Subdivision level")) + init_constraint(constraint::minimum(1L)) + init_value(1) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_level.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::replace_selection(m_mesh_selection.value(), const_cast<k3d::legacy::mesh&>(InputMesh));
		// Set levels -before- input
//		m_sds_cache.set_levels(m_level.value());
//		m_sds_cache.set_input(&InputMesh);
//
//		m_sds_cache.update();
//		m_sds_cache.output(&Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<catmull_clark_subdivider,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xdc73f6e3, 0x744b4b51, 0xac535307, 0xc82d261c),
				"CatmullClark",
				_("Generates a Catmull-Clark subdivision mesh"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d::imaterial* m_material;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_level;

	//k3d::sds::k3d_mesh_sds_cache m_sds_cache;
};

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivision_factory

k3d::iplugin_factory& catmull_clark_subdivision_factory()
{
	return libk3dmesh::catmull_clark_subdivider::get_factory();
}

} // namespace libk3dmesh

