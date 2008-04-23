// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
#include <k3d-i18n-config.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iterator>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// set_material_implementation

class set_material_implementation :
	public k3d::material_sink<k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > >
{
	typedef k3d::material_sink<k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > > base;

public:
	set_material_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_material.changed_signal().connect(make_update_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::for_each_component(Mesh, set_material(m_material.pipeline_value()));
	}
	
	class set_material
	{
	public:
		set_material(k3d::imaterial* const Material) :
			m_material(Material)
		{
		}
	
		template<typename T>
		void operator()(T& Object)
		{
			if(Object.selection_weight)
				Object.material = m_material;
		}

		void operator()(k3d::legacy::point&) { }
		void operator()(k3d::legacy::split_edge&) { }
		void operator()(k3d::legacy::linear_curve&) { }
		void operator()(k3d::legacy::cubic_curve&) { }
		void operator()(k3d::legacy::nucurve&) { }
		void operator()(k3d::legacy::polyhedron&) { }
		void operator()(k3d::legacy::mesh&) { }

	private:
		k3d::imaterial* const m_material;
	};

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<set_material_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x7743a7f2, 0x995345b9, 0x8d6dacc7, 0x3fa37029),
				"SetMaterial",
				_("Sets geometry materials"),
				"Materials",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
};

/////////////////////////////////////////////////////////////////////////////
// set_material_factory

k3d::iplugin_factory& set_material_factory()
{
	return set_material_implementation::get_factory();
}

} // namespace mesh

} // namespace module

