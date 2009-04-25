// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "helpers.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace gts
{

/////////////////////////////////////////////////////////////////////////////
// mesh_area

class mesh_area :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	mesh_area(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_area(init_owner(*this) + init_name("area") + init_label(_("Mesh area")) + init_description(_("Mesh area")) + init_value(0.0))
	{
		m_input_mesh.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_area.make_slot()));

		m_area.set_update_slot(sigc::mem_fun(*this, &mesh_area::execute));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_area,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9a340565, 0x47498847, 0x8979d2a1, 0x25f072d9),
				"GTSMeshArea",
				_("Calculates input mesh area"),
				"Metrics",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_area;

	void execute(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Area)
	{
		Area = 0;

		k3d::mesh* const mesh = m_input_mesh.pipeline_value();
		if(!mesh)
			return;

		for(k3d::mesh::primitives_t::const_iterator primitive = mesh->primitives.begin(); primitive != mesh->primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron)
				continue;

			const gts_ptr<GtsSurface> gts_surface = convert(*mesh, *polyhedron);
			Area += gts_surface_area(gts_surface);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// mesh_area

k3d::iplugin_factory& mesh_area_factory()
{
	return mesh_area::get_factory();
}

} // namespace gts

} // namespace module

