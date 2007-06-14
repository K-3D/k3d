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

#include "aspect_ratios.h"
#include "fstream.h"
#include "log.h"
#include "share.h"
#include "xml.h"

namespace k3d
{

const aspect_ratios_t& aspect_ratios()
{
	static aspect_ratios_t results;
	if(results.empty())
	{
		try
		{
			const filesystem::path path = share_path() / filesystem::generic_path("aspect_ratios.k3d");
			k3d::filesystem::ifstream stream(path);

			xml::element xml("k3dml");
			stream >> xml;

			xml::element& xml_aspect_ratios = xml.safe_element("application").safe_element("aspectratios");

			for(xml::element::elements_t::iterator xml_aspect_ratio = xml_aspect_ratios.children.begin(); xml_aspect_ratio != xml_aspect_ratios.children.end(); ++xml_aspect_ratio)
			{
				if(xml_aspect_ratio->name != "aspectratio")
					continue;

				const std::string name = xml::attribute_text(*xml_aspect_ratio, "name");
				const std::string description = xml::attribute_text(*xml_aspect_ratio, "description");
				const double value = xml::attribute_value<double>(*xml_aspect_ratio, "value", 1.0);

				results.push_back(aspect_ratio(name, description, value));
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

const ienumeration_property::enumeration_values_t& aspect_ratio_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t("<Custom>", "", "Custom aspect ratio"));
		const aspect_ratios_t ratios = aspect_ratios();
		for(aspect_ratios_t::const_iterator ratio = ratios.begin(); ratio != ratios.end(); ++ratio)
		{
			const std::string label = string_cast(boost::format("%1% (%2%:1)") % ratio->name % ratio->value);
			const std::string value = ratio->name;
			const std::string description = ratio->description;
			values.push_back(ienumeration_property::enumeration_value_t(label, value, description));
		}
	}

	return values;
}

} // namespace k3d


