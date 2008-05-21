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
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/types_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iterator>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// sds_corner_implementation

class sds_corner_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	sds_corner_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sharpness(init_owner(*this) + init_name("sharpness") + init_label(_("Sharpness")) + init_description(_("Controls the sharpness of subdivision surface corners")) + init_value(0.0))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_sharpness.changed_signal().connect(make_update_mesh_slot());
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
	}

	void on_update_mesh(const k3d::legacy::mesh& Source, k3d::legacy::mesh& Target)
	{
		return_if_fail(Source.points.size() == Target.points.size());

		const k3d::ri::real sharpness = m_sharpness.pipeline_value();

		for(unsigned long i = 0; i != Target.points.size(); ++i)
		{
			if(Source.points[i]->selection_weight)
				Target.points[i]->tags["corner"] = sharpness;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_corner_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xded03869, 0x283e4531, 0x9de22a9c, 0xe763d7c6),
				"SDSCorner",
				_("Adds variable-sharpness corners to SDS surfaces"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_sharpness;
};

/////////////////////////////////////////////////////////////////////////////
// sds_corner_factory

k3d::iplugin_factory& sds_corner_factory()
{
	return sds_corner_implementation::get_factory();
}

} // namespace mesh

} // namespace module


