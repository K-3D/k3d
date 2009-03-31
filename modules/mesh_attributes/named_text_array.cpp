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

/** \file named_text_array.cpp
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\created Feb 25, 2009
*/

#include <k3d-i18n-config.h>

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh_attributes
{

class named_text_array :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
	typedef k3d::typed_array<k3d::string_t> strings_t;
public:
	named_text_array(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_array_name(init_owner(*this) + init_name("array_name") + init_label(_("Array Name")) + init_description(_("Name of the output array")) + init_value(std::string("string_data"))),
		m_default_value(init_owner(*this) + init_name("array_name") + init_label(_("Default Value")) + init_description(_("Default value, associated with unselected items")) + init_value(std::string("default"))),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Value")) + init_description(_("Value, associated with selected items")) + init_value(std::string("text_data")))
		{
			m_array_name.changed_signal().connect(make_reset_mesh_slot());
			m_default_value.changed_signal().connect(make_reset_mesh_slot());
			m_value.changed_signal().connect(make_reset_mesh_slot());
		}
	
	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output));
		
		const k3d::string_t array_name = m_array_name.pipeline_value();
		const k3d::string_t value = m_value.pipeline_value();
		if(!polyhedron->uniform_data.lookup<strings_t>(array_name))
		{
			polyhedron->uniform_data.create(array_name, new strings_t(polyhedron->face_first_loops.size(), m_default_value.pipeline_value()));
		}
		strings_t& string_data = *(polyhedron->uniform_data.writable<strings_t>(array_name));
		for(k3d::uint_t face = 0; face != polyhedron->face_first_loops.size(); ++face)
		{
			if(polyhedron->face_selections[face])
				string_data[face] = value;
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<named_text_array,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xf4d7f4d5, 0x3a40e51a, 0xfaf269bb, 0x1b68fdfe),
				"NamedTextArray",
				_("Creates or updates a named array with string data"),
				"Named Arrays",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_array_name;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_default_value;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_value;
};
	
k3d::iplugin_factory& named_text_array_factory()
{
	return named_text_array::get_factory();
}

} // namespace mesh_attributes

} // namespace module
