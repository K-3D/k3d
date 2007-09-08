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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/mesh_modifier.h>

#include <iterator>

namespace module
{

namespace point_group
{

/////////////////////////////////////////////////////////////////////////////
// make_point_group

class make_point_group :
	public k3d::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	make_point_group(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!k3d::validate_points(Input))
			return;

		const size_t point_count = Input.points->size();

		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.vertex_data = Input.vertex_data;

		k3d::mesh::point_groups_t* const point_groups = new k3d::mesh::point_groups_t();
		k3d::mesh::indices_t* const first_points = new k3d::mesh::indices_t(1, 0);
		k3d::mesh::counts_t* const point_counts = new k3d::mesh::counts_t(1, point_count);
		k3d::mesh::materials_t* const materials = new k3d::mesh::materials_t(1, static_cast<k3d::imaterial*>(0));
		k3d::mesh::indices_t* const points = new k3d::mesh::indices_t(point_count);
		for(size_t i = 0; i != point_count; ++i)
			(*points)[i] = i;

		point_groups->first_points.reset(first_points);
		point_groups->point_counts.reset(point_counts);
		point_groups->materials.reset(materials);
		point_groups->points.reset(points);

		Output.point_groups.reset(point_groups);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<make_point_group,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x3e4086fe, 0x0246f9ab, 0xeef01f8a, 0xf5cf5cff),
				"MakePointGroup",
				_("Converts input gprims into a single point group"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// make_point_group_factory

k3d::iplugin_factory& make_point_group_factory()
{
	return make_point_group::get_factory();
}

} // namespace point_group

} // namespace module


