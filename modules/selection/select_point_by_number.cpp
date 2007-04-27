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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/new_mesh_selection_modifier.h>
#include <k3dsdk/shared_pointer.h>

namespace libk3dselection
{

/////////////////////////////////////////////////////////////////////////////
// select_point_by_number

class select_point_by_number :
	public k3d::dev::mesh_selection_modifier
{
	typedef k3d::dev::mesh_selection_modifier base;

public:
	select_point_by_number(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_index(init_owner(*this) + init_name("index") + init_label(_("Point Index")) + init_description(_("Point Index")) + init_value(0L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_index.changed_signal().connect(make_update_mesh_slot());
	}

	void on_select_mesh(const k3d::dev::mesh& Input, k3d::dev::mesh& Output)
	{
		if(Output.point_selection)
		{
			k3d::dev::mesh::selection_t& point_selection = *k3d::make_unique(Output.point_selection);
			std::fill(point_selection.begin(), point_selection.end(), 0.0);

			const unsigned long index = m_index.value();
			if(index < point_selection.size())
				point_selection[index] = 1.0;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_point_by_number,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xded6d3a7, 0x65fc4d6d, 0x8b7afff7, 0xdd0785ad),
				"SelectPointByNumber",
				"Selects a point from the input mesh by its index number",
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_index;
};

/////////////////////////////////////////////////////////////////////////////
// select_point_by_number_factory

k3d::iplugin_factory& select_point_by_number_factory()
{
	return select_point_by_number::get_factory();
}

} // namespace libk3dselection

