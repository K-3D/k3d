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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "fstream.h"
#include "log.h"
#include "resolutions.h"
#include "share.h"
#include "xml.h"

namespace k3d
{

const resolutions_t& resolutions()
{
	static resolutions_t results;
	if(results.empty())
	{
		try
		{
			const filesystem::path path = share_path() / filesystem::generic_path("resolutions.k3d");
			filesystem::ifstream stream(path);

			k3d::xml::element xml("k3dml");
			stream >> xml;

			k3d::xml::element& xml_resolutions = xml.safe_element("application").safe_element("resolutions");

			for(k3d::xml::element::elements_t::iterator xml_resolution = xml_resolutions.children.begin(); xml_resolution != xml_resolutions.children.end(); ++xml_resolution)
			{
				if(xml_resolution->name != "resolution")
					continue;

				const std::string name = k3d::xml::attribute_text(*xml_resolution, "name");
				const std::string description = k3d::xml::attribute_text(*xml_resolution, "description");
				const unsigned long width = k3d::xml::attribute_value<unsigned long>(*xml_resolution, "width", 320);
				const unsigned long height = k3d::xml::attribute_value<unsigned long>(*xml_resolution, "height", 240);
				results.push_back(resolution(name, description, width, height));
			}
		}
		catch(std::exception& e)
		{
			log() << error << k3d_file_reference << ": " << e.what() << std::endl;
		}
		catch(...)
		{
			log() << error << k3d_file_reference << ": Unknown exception" << std::endl;
		}
	}

	return results;
}

const ienumeration_property::enumeration_values_t& resolution_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t("<Custom>", "", "Custom resolution"));
		const resolutions_t& resolutions = k3d::resolutions();
		for(resolutions_t::const_iterator resolution = resolutions.begin(); resolution != resolutions.end(); ++resolution)
		{
			const double ratio = static_cast<double>(resolution->width) / static_cast<double>(resolution->height);
			const std::string label = string_cast(boost::format("%1% (%2%:1)") % resolution->name % ratio);
			const std::string value = resolution->name;
			const std::string description = resolution->description;
			values.push_back(ienumeration_property::enumeration_value_t(label, value, description));
		}
	}

	return values;
}

} // namespace k3d


