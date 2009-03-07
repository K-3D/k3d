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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/serialization_xml.h>

#include <iterator>

namespace module
{

namespace mesh_instance
{

/////////////////////////////////////////////////////////////////////////////
// frozen_mesh

class frozen_mesh :
	public k3d::node,
	public k3d::imesh_sink,
	public k3d::imesh_source,
	public k3d::imesh_storage
{
	typedef k3d::node base;

public:
	frozen_mesh(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")))
	{
		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &frozen_mesh::execute));
	}

	void execute(const std::vector<k3d::ihint*>& Hints, k3d::mesh& Output)
	{
		if(const k3d::mesh* const input = m_input_mesh.pipeline_value())
			k3d::mesh::deep_copy(*input, Output);
	}

	k3d::iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	k3d::iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void save(k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		base::save(Element, Context);

		if(k3d::mesh* const mesh = m_output_mesh.internal_value())
		{
			k3d::xml::element& xml_mesh = Element.append(k3d::xml::element("mesh_arrays"));
			k3d::xml::save(*mesh, xml_mesh, Context);
		}
	}

	void load(k3d::xml::element& Element, const k3d::ipersistent::load_context& Context)
	{
		base::load(Element, Context);

		// Create a new mesh ...
		k3d::mesh* const mesh = new k3d::mesh();
		m_output_mesh.reset(mesh);

		// Load the stored mesh data ...
		k3d::xml::element* xml_mesh = k3d::xml::find_element(Element, "mesh_arrays");
		if(xml_mesh)
		{
			k3d::xml::load(*mesh, *xml_mesh, Context);
		}
		else
		{ // try the legacy mesh loader
			xml_mesh = k3d::xml::find_element(Element, "mesh");
			if(!xml_mesh)
				return;
			k3d::legacy::mesh legacy_mesh;
			k3d::xml::load(legacy_mesh, *xml_mesh, Context);
			*mesh = legacy_mesh;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<frozen_mesh,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
			k3d::classes::FrozenMesh(),
			"FrozenMesh",
			_("Freezes its input for manual editing"),
			"Mesh",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh;
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_mesh;
};

/////////////////////////////////////////////////////////////////////////////
// frozen_mesh_factory

k3d::iplugin_factory& frozen_mesh_factory()
{
	return frozen_mesh::get_factory();
}

} // namespace mesh_instance

} // namespace module

