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
// sds_crease_implementation

class sds_crease_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	sds_crease_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sharpness(init_owner(*this) + init_name("sharpness") + init_label(_("Sharpness")) + init_description(_("Controls the sharpness of subdivision surface creases")) + init_value(0.0))
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
		const k3d::ri::real sharpness = m_sharpness.pipeline_value();

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Target.polyhedra.begin(); polyhedron != Target.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::split_edge* edge = (*face)->first_edge;
				do
				{
					if(edge->selection_weight)
					{
						edge->tags["crease"] = sharpness;
						if (edge->companion)
							edge->companion->tags["crease"] = sharpness;
					}

					edge = edge->face_clockwise;
				}
				while(edge != (*face)->first_edge);
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sds_crease_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x78b79c12, 0x4c174732, 0xa67d6317, 0x6dbec1fb),
				"SDSCrease",
				_("Adds variable-sharpness creases to SDS surfaces"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_sharpness;
};

/////////////////////////////////////////////////////////////////////////////
// sds_crease_factory

k3d::iplugin_factory& sds_crease_factory()
{
	return sds_crease_implementation::get_factory();
}

} // namespace mesh

} // namespace module

