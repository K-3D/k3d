// K3D
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
*/

#include "plugin_factory.h"
#include "result.h"

#include <boost/tokenizer.hpp>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// plugin_factory

plugin_factory::plugin_factory(const uuid& ClassID, const std::string& Name, const std::string& ShortDescription, const std::string& Categories, const quality_t Quality, const metadata_t& Metadata) :
	m_class_id(ClassID),
	m_name(Name),
	m_short_description(ShortDescription),
	m_quality(Quality),
	m_metadata(Metadata)
{
	assert(m_class_id != k3d::uuid::null());

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
	boost::char_separator<char> separator;
	tokenizer_t tokenizer(Categories, separator);
	std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(m_categories));
}

const uuid& plugin_factory::class_id()
{
	return m_class_id;
}

const std::string plugin_factory::name()
{
	return m_name;
}

const std::string plugin_factory::short_description()
{
	return m_short_description;
}

const iplugin_factory::categories_t& plugin_factory::categories()
{
	return m_categories;
}

k3d::iplugin_factory::quality_t plugin_factory::quality()
{
	return m_quality;
}

const k3d::iplugin_factory::metadata_t& plugin_factory::metadata()
{
	return m_metadata;
}

std::ostream& operator<<(std::ostream& Stream, const iplugin_factory::quality_t& RHS)
{
	switch(RHS)
	{
		case iplugin_factory::STABLE:
			Stream << "stable";
			break;
		case iplugin_factory::EXPERIMENTAL:
			Stream << "experimental";
			break;
		case iplugin_factory::DEPRECATED:
			Stream << "deprecated";
			break;
		default:
			assert_not_reached();
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, iplugin_factory::quality_t& RHS)
{
	std::string c;
	Stream >> c;

	if("stable" == c)
		RHS = iplugin_factory::STABLE;
	else if("experimental" == c)
		RHS = iplugin_factory::EXPERIMENTAL;
	else if("deprecated" == c)
		RHS = iplugin_factory::DEPRECATED;
	else
		log() << error << error << "Could not extract value [" << c << "]" << std::endl;
		
	return Stream;
}

} // namespace k3d


