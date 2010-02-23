// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>
#include <k3dsdk/utility.h>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// weld_points

class weld_points :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	weld_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("Distance")) + init_description(_("Maximum distance between points")) + init_value(0.001) + init_step_increment(0.0001) + init_units(typeid(k3d::measurement::distance)) + init_constraint(constraint::minimum<k3d::double_t>(0.0)))
	{
		m_distance.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	struct map_point_indices
	{
		map_point_indices(const k3d::mesh::indices_t PointMap) :
			point_map(PointMap)
		{
		}

		void operator()(const k3d::string_t&, const k3d::table&, const k3d::string_t& ArrayName, k3d::pipeline_data<k3d::array>& Array)
		{
			if(Array->get_metadata_value(k3d::metadata::key::domain()) != k3d::metadata::value::point_indices_domain())
				return;

			k3d::uint_t_array* const array = dynamic_cast<k3d::uint_t_array*>(&Array.writable());
			if(!array)
			{
				k3d::log() << error << "array [" << ArrayName << "] must be a k3d::uint_t_array." << std::endl;
				return;
			}

			const k3d::uint_t begin = 0;
			const k3d::uint_t end = begin + array->size();
			for(k3d::uint_t i = begin; i != end; ++i)
				(*array)[i] = point_map[(*array)[i]];
		}

		const k3d::mesh::indices_t& point_map;	
	};

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		if(!Output.points)
			return;
		k3d::mesh::points_t& points = Output.points.writable();

		// Begin by creating an identity map from each mesh point to itself ...
		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + points.size();
		k3d::mesh::indices_t point_map(points.size());
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			point_map[point] = point;

		// Update the point map to eliminate "duplicate" points ... warning: this is O(N^2)!!!
		k3d::uint_t weld_points_count = 0;
		const k3d::double_t distance = m_distance.pipeline_value();
		for(k3d::uint_t point1 = point_begin; point1 != point_end; ++point1)
		{
			// Skip points that have already been weld_pointsed ...
			if(point_map[point1] != point1)
				continue;

			for(k3d::uint_t point2 = point1 + 1; point2 != point_end; ++point2)
			{
				// Skip points that have already been weld_pointsed ...
				if(point_map[point2] != point2)
					continue;

				const k3d::vector3 delta = points[point2] - points[point1];
				if(std::fabs(delta[0]) < distance && std::fabs(delta[1]) < distance && std::fabs(delta[2]) < distance)
				{
					++weld_points_count;
					point_map[point2] = point1;
				}
			}
		}

		// If we didn't find any points to weld_points, we're done ...
		if(!weld_points_count)
			return;

		// Remap primitive points so that they no longer reference "duplicate" points ... 
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
			k3d::mesh::visit_arrays(primitive->writable(), map_point_indices(point_map));

		// Delete lefover unused points ...
		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<weld_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xacfb8148, 0x4f404ca1, 0x937564e3, 0x2977984c),
				"WeldPoints",
				_("Eliminates points with duplicate 3D coordinates"),
				"Mesh",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_distance;
};

/////////////////////////////////////////////////////////////////////////////
// weld_points_factory

k3d::iplugin_factory& weld_points_factory()
{
	return weld_points::get_factory();
}

} // namespace mesh

} // namespace module

