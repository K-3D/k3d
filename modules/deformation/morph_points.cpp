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

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

#include <k3dsdk/iuser_property.h>
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
		m_hundred_limit(init_owner(*this) + init_name("hundred_limit") + init_label(_("100% Limit")) + init_description(_("Morph up to 100%")) + init_value(true)),
		m_input_mesh1(init_owner(*this) + init_name("input_mesh1") + init_label(_("Target")) + init_description(_("First Morph Target. You may add more target through user properties. Add a Mesh and a Scalar")) + init_value<k3d::mesh*>(0)),
		m_input_mesh1_percent(init_owner(*this) + init_name("input_mesh1_percent") + init_label(_("Morph Percent")) + init_description(_("First Target Morph Percent. You may add more target through user properties. Add a Mesh and a Scalar")) + init_value(0.)),
		m_user_property_changed_signal(*this)
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());

		m_hundred_limit.changed_signal().connect(make_update_mesh_slot());
		m_input_mesh1.changed_signal().connect(make_update_mesh_slot());
		m_input_mesh1_percent.changed_signal().connect(make_update_mesh_slot());
		
		m_user_property_changed_signal.connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::bool_t hundred_limit = m_hundred_limit.pipeline_value();
		k3d::mesh &input_mesh1 = *m_input_mesh1.pipeline_value();
		k3d::double_t input_mesh1_percent = m_input_mesh1_percent.pipeline_value() / 100.;
		std::vector<k3d::mesh*> morph_targets;
		std::vector<k3d::double_t> morph_percents;
		
		//Search for the Meshes targets and its morph percent multiplier
		const k3d::iproperty_collection::properties_t& properties = k3d::node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		{
			if(!dynamic_cast<k3d::iuser_property*>(*property))
				continue;
			
			if((**property).property_type() == typeid(k3d::mesh*))
			{
				morph_targets.push_back( k3d::property::pipeline_value<k3d::mesh*>(**property) );
			}
			else if((**property).property_type() == typeid(k3d::double_t))
			{
				morph_percents.push_back( k3d::property::pipeline_value<k3d::double_t>(**property) / 100.);
			}
			else
			{
				k3d::log() << warning << factory().name() << ": user property [" << (**property).property_name() << "] with unsupported type [" << k3d::demangle((**property).property_type()) << "] will be ignored" << std::endl;
			}
		}

		if(hundred_limit) //do not exceed 100% limit, all target percent < -1.0;1.0 >
		{
			if(input_mesh1_percent > 1.)
				input_mesh1_percent= 1.;
			else if(input_mesh1_percent < -1.)
				input_mesh1_percent= -1.;
			
			const size_t index_end = morph_percents.size();
			for(size_t index = 0; index < index_end; ++index)
			{
				if(morph_percents[index] > 1.)
					morph_percents[index]= 1.;
				else if(morph_percents[index] < -1.)
					morph_percents[index]= -1.;
			}
		}
		
		if(&input_mesh1 && (input_mesh1_percent != 0.))
		{
			k3d::mesh::points_t& TargetPoints = const_cast<k3d::mesh::points_t&>(*input_mesh1.points);
			
			size_t point_end = OutputPoints.size() < TargetPoints.size() ? OutputPoints.size() : TargetPoints.size();
			for(size_t point = 0; point != point_end; ++point)//
			{
				OutputPoints[point] = k3d::mix(InputPoints[point], InputPoints[point] + (TargetPoints[point] - InputPoints[point])*input_mesh1_percent, PointSelection[point] );
			}
			if( TargetPoints.size() < OutputPoints.size()	)
			{
				point_end = OutputPoints.size();
				for(size_t point = TargetPoints.size(); point != point_end; ++point)//
				{
					OutputPoints[point] = InputPoints[point];
				}
			}
		}
  	else
		{
			const size_t point_end = OutputPoints.size();
			for(size_t point = 0; point != point_end; ++point)//
			{
				OutputPoints[point] = InputPoints[point];
			}
		}
		
  	k3d::uint64_t index_end = morph_percents.size() < morph_targets.size() ? morph_percents.size() : morph_targets.size();
		for(k3d::uint64_t index = 0; index < index_end; ++index)
		{
			k3d::mesh& morph_target = *morph_targets[index];
			k3d::double_t morph_percent = morph_percents[index];
			if(!&morph_target || !morph_percent)
				continue;
				
			k3d::mesh::points_t& TargetPoints = const_cast<k3d::mesh::points_t&>(*morph_target.points);
			
			const size_t point_end = OutputPoints.size() < TargetPoints.size() ? OutputPoints.size() : TargetPoints.size();
			for(size_t point = 0; point != point_end; ++point)//
			{
				OutputPoints[point] = k3d::mix(InputPoints[point], InputPoints[point] + (TargetPoints[point] - InputPoints[point])*morph_percent, PointSelection[point] )
															- InputPoints[point] + OutputPoints[point] ;
			}
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
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_hundred_limit; 
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_mesh1; //First target for user friendlines
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input_mesh1_percent; //First target for user friendlines
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


