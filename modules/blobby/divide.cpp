// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "detail.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// divide

class divide :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	divide(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_a(init_owner(*this) + init_name("input_a") + init_label(_("Input A")) + init_description(_("First input mesh.")) + init_value<k3d::mesh*>(0)),
		m_input_b(init_owner(*this) + init_name("input_b") + init_label(_("Input B")) + init_description(_("Second input mesh.")) + init_value<k3d::mesh*>(0))
	{
		m_input_a.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));

		m_input_b.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}
	
	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		// Get the set of input meshes ...
		detail::mesh_collection meshes;

		k3d::mesh* const input_a = m_input_a.pipeline_value();
		k3d::mesh* const input_b = m_input_b.pipeline_value();

		if(input_a)
			meshes.push_back(input_a);

		if(input_b)
			meshes.push_back(input_b);

		// Merge 'em ...
		detail::merge(meshes, m_material.pipeline_value(), k3d::blobby::DIVIDE, false, Output);
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<divide, k3d::interface_list<k3d::imesh_source> > factory(
			k3d::uuid(0x19533129, 0x6b45e7ea, 0x4a120cb8, 0x06cf305a),
			"BlobbyDivide",
			_("Combines blobbies using the Division operator."),
			"Blobby",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_a;
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_b;
};

/////////////////////////////////////////////////////////////////////////////
// divide_factory

k3d::iplugin_factory& divide_factory()
{
	return divide::get_factory();
}

} // namespace blobby

} // namespace module

