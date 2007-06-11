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
#include <k3dsdk/i18n.h>
#include <k3dsdk/itransform_array_2d.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>

#include <boost/multi_array.hpp>
#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// array_2d_implementation

class array_2d_implementation :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	array_2d_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_2d*>(0)),
		m_count1(init_owner(*this) + init_name("count1") + init_label(_("Count 1")) + init_description(_("Number of mesh copies")) + init_value(5) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0L))),
		m_count2(init_owner(*this) + init_name("count2") + init_label(_("Count 2")) + init_description(_("Number of mesh copies")) + init_value(5) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(0L)))
	{
		m_layout.changed_signal().connect(make_reset_mesh_slot());
		m_count1.changed_signal().connect(make_reset_mesh_slot());
		m_count2.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		if(k3d::itransform_array_2d* const layout = m_layout.value())
		{
			const unsigned long count1 = m_count1.value();
			const unsigned long count2 = m_count2.value();

			for(unsigned long i = 0; i != count1; ++i)
			{
				for(unsigned long j = 0; j != count2; ++j)
				{
					// Make a copy of the input geometry ...
					const unsigned long first_new_point = Mesh.points.size();
					k3d::legacy::deep_copy(InputMesh, Mesh);

					// Apply offsets ...
					const k3d::matrix4 matrix = layout->get_element(i, count1, j, count2);
					for(unsigned long i = first_new_point; i != Mesh.points.size(); ++i)
						Mesh.points[i]->position = matrix * Mesh.points[i]->position;
				}
			}
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<array_2d_implementation,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x4818d995, 0xf8664006, 0xba02ded4, 0x8cb9defb),
				"MeshArray2D",
				_("Makes copies of a mesh along two dimensions"),
				"Mesh Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::itransform_array_2d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count1;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count2;
};

/////////////////////////////////////////////////////////////////////////////
// array_2d_factory

k3d::iplugin_factory& array_2d_factory()
{
	return array_2d_implementation::get_factory();
}

} // namespace libk3dmesh


