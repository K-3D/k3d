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
	\author Tim Shead (tshead@k-3d.com) parts of code
	\author Joaquin Duo (joaduo at lugmen org ar)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iuser_property.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// morph_points

class morph_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	morph_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_user_property_changed_signal.connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		std::vector<const k3d::mesh::points_t*> inputs;
		std::vector<k3d::double_t> amounts;
		
		//Search for the Meshes targets and its morph percent multiplier
		const k3d::iproperty_collection::properties_t& properties = k3d::node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(!dynamic_cast<k3d::iuser_property*>(*property))
				continue;
			
			if((**property).property_type() == typeid(k3d::mesh*))
			{
				if(k3d::mesh* const mesh = k3d::property::pipeline_value<k3d::mesh*>(**property))
				{
					if(const k3d::mesh::points_t* const points = mesh->points.get())
						inputs.push_back(points);
				}
			}
			else if((**property).property_type() == typeid(k3d::double_t))
			{
				amounts.push_back(k3d::property::pipeline_value<k3d::double_t>(**property));
			}
			else
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
			}
		}

		if(inputs.size() != amounts.size())
		{
			k3d::log() << error << "mismatch in number of morph inputs and amounts." << std::endl;
			return;
		}

		OutputPoints = InputPoints;

		const k3d::uint_t input_begin = 0;
		const k3d::uint_t input_end = input_begin + inputs.size();
		for(k3d::uint_t i = input_begin; i != input_end; ++i)
		{
			const k3d::mesh::points_t& input_points = *inputs[i];
			const k3d::double_t amount = amounts[i];

			const k3d::uint_t point_begin = 0;
			const k3d::uint_t point_end = point_begin + std::min(OutputPoints.size(), input_points.size());
			for(k3d::uint_t point = point_begin; point != point_end; ++point)
				OutputPoints[point] += PointSelection[point] * amount * (input_points[point] - InputPoints[point]);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<morph_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xc52c53c2, 0x3fca4d28, 0x88db7096, 0xea60f819),
				"MorphPoints",
				_("Applies morph to different targets, interpolating between target and source position"),
				"Deformation",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d::user_property_changed_signal m_user_property_changed_signal;
	
};

/////////////////////////////////////////////////////////////////////////////
// morph_points_factory

k3d::iplugin_factory& morph_points_factory()
{
	return morph_points::get_factory();
}

} // namespace deformation

} // namespace module

