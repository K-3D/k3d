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
*/

#include "properties_ri.h"

namespace k3d
{

std::ostream& operator<<(std::ostream& Stream, const irenderman_property::parameter_type_t Value)
{
	switch(Value)
		{
			case irenderman_property::ATTRIBUTE:
				Stream << "attribute";
				break;
			case irenderman_property::OPTION:
				Stream << "option";
				break;
		}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, irenderman_property::parameter_type_t Value)
{
	std::string s;
	Stream >> s;

	if("attribute" == s)
		Value = irenderman_property::ATTRIBUTE;
	else if("option" == s)
		Value = irenderman_property::OPTION;
	else
		log() << error << "Could not extract value [" << s << "]" << std::endl;

	return Stream;
}

} // namespace k3d


