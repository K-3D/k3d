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
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "algebra.h"
#include "imaterial_ri.h"
#include "irenderman_property.h"
#include "material.h"
#include "renderable_ri.h"
#include "render_state_ri.h"
#include "vectors.h"

#include <boost/array.hpp>

#include <iostream>
#include <numeric>

namespace k3d
{

namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// motion_begin

void motion_begin(const render_state& State)
{
	if(motion_blur(State))
		State.stream.RiMotionBeginV(State.sample_times);
}

/////////////////////////////////////////////////////////////////////////////
// motion_end

void motion_end(const render_state& State)
{
	if(motion_blur(State))
		State.stream.RiMotionEnd();
}

/////////////////////////////////////////////////////////////////////////////
// motion_blur

bool motion_blur(const render_state& State)
{
	return State.sample_times.size() > 1;
}

/////////////////////////////////////////////////////////////////////////////
// first_sample

bool first_sample(const render_state& State)
{
	return 0 == State.sample_index;
}

/////////////////////////////////////////////////////////////////////////////
// last_sample

bool last_sample(const render_state& State)
{
	return State.sample_index == State.sample_times.size() - 1;
}

/////////////////////////////////////////////////////////////////////////////
// convert

const matrix convert(const k3d::matrix4& Matrix)
{
	return Matrix;
}

/////////////////////////////////////////////////////////////////////////////
// setup_material

void setup_material(iunknown* const Material, const render_state& State)
{
	// If we can find a RenderMan material, let it do the work ...
	if(k3d::ri::imaterial* const ri_material = k3d::material::lookup<k3d::ri::imaterial>(Material))
	{
		ri_material->setup_renderman_material(State);
		return;
	}

	// Otherwise, we generate a default "null" material ...

	// We only generate RIB on the final sample ...
	if(!last_sample(State))
		return;

	State.stream.RiColor(k3d::color(1, 1, 1));
	State.stream.RiOpacity(k3d::color(1, 1, 1));
	State.stream.RiSurfaceV(path(), "null");
	State.stream.RiDisplacementV(path(), "null");
	State.stream.RiAtmosphereV(path(), "null");
	State.stream.RiInteriorV(path(), "null");
	State.stream.RiExteriorV(path(), "null");
}

/////////////////////////////////////////////////////////////////////////////
// set_attributes

void set_attributes(iproperty_collection& Properties, istream& Stream)
{
	const iproperty_collection::properties_t& properties = Properties.properties();

	typedef std::map<std::string, parameter_list> parameter_lists_t;
	parameter_lists_t parameter_lists;

	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		if(irenderman_property* const renderman_property = dynamic_cast<irenderman_property*>(*property))
		{
			if(renderman_property->property_parameter_list_type() != irenderman_property::ATTRIBUTE)
				continue;

			const std::string parameter_list_name = renderman_property->property_parameter_list_name();
			const std::string parameter_name = (*property)->property_name();

			parameter_lists[parameter_list_name].push_back(parameter(parameter_name, UNIFORM, 1, (*property)->property_pipeline_value()));
		}
	}

	for(parameter_lists_t::iterator parameter_list = parameter_lists.begin(); parameter_list != parameter_lists.end(); ++parameter_list)
		Stream.RiAttributeV(parameter_list->first, parameter_list->second);
}

/////////////////////////////////////////////////////////////////////////////
// set_options

/// Extracts RenderMan data from a property collection, generating corresponding calls to RiOption()
void set_options(iproperty_collection& Properties, istream& Stream)
{
	const iproperty_collection::properties_t& properties = Properties.properties();

	typedef std::map<std::string, parameter_list> parameter_lists_t;
	parameter_lists_t parameter_lists;

	for(iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
	{
		if(irenderman_property* const renderman_property = dynamic_cast<irenderman_property*>(*property))
		{
			if(renderman_property->property_parameter_list_type() != irenderman_property::OPTION)
				continue;

			const std::string parameter_list_name = renderman_property->property_parameter_list_name();
			const std::string parameter_name = (*property)->property_name();

			parameter_lists[parameter_list_name].push_back(parameter(parameter_name, UNIFORM, 1, (*property)->property_pipeline_value()));
		}
	}

	for(parameter_lists_t::iterator parameter_list = parameter_lists.begin(); parameter_list != parameter_lists.end(); ++parameter_list)
		Stream.RiOptionV(parameter_list->first, parameter_list->second);
}

} // namespace ri

} // namespace k3d

