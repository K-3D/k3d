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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/node.h>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// select_points_above_number

class select_points_above_number :
	public k3d::mesh_selection_modifier<k3d::node>
{
	typedef k3d::mesh_selection_modifier<k3d::node> base;

public:
	select_points_above_number(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_index(init_owner(*this) + init_name("index") + init_label(_("Point Index")) + init_description(_("Point Index")) + init_value(0L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_index.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(Output.point_selection)
		{
			k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
			std::fill(point_selection.begin(), point_selection.end(), 0.0);

			const k3d::int32_t index = m_index.pipeline_value();
      const k3d::int32_t npoints = point_selection.size();
      for(k3d::int32_t pt_idx = 0; pt_idx != npoints; ++pt_idx)
      {
        if(pt_idx >= index)
          point_selection[pt_idx] = 1.0;
      }
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_points_above_number,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x01c131dc, 0x821c4bf1, 0xba13bb1e, 0xc73af3a5),
				"SelectPointsAboveNumber",
				"Selects all points with an index above or equal to the given number",
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_index;
};

/////////////////////////////////////////////////////////////////////////////
// select_points_above_number_factory

k3d::iplugin_factory& select_points_above_number_factory()
{
	return select_points_above_number::get_factory();
}

} // namespace selection

} // namespace module

