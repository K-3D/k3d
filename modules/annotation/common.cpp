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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "common.h"

#include <k3dsdk/log.h>
#include <k3dsdk/types.h>

#include <iostream>

namespace module
{

namespace annotation
{

std::ostream& operator << (std::ostream& Stream, const alignment_t& Value)
{
	switch(Value)
	{
		case LEFT:
			Stream << "left";
			break;
		case CENTER:
			Stream << "center";
			break;
		case RIGHT:
			Stream << "right";
			break;
		case JUSTIFY:
			Stream << "justify";
			break;
	}

	return Stream;
}

std::istream& operator >> (std::istream& Stream, alignment_t& Value)
{
	k3d::string_t text;
	Stream >> text;

	if(text == "left")
		Value = LEFT;
	else if(text == "center")
		Value = CENTER;
	else if(text == "right")
		Value = RIGHT;
	else if(text == "justify")
		Value = JUSTIFY;
	else
		k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

	return Stream;
}

const k3d::ienumeration_property::enumeration_values_t& alignment_values()
{
	static k3d::ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Left", "left", "Left-justify text."));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Center", "center", "Center-justify text."));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Right", "right", "Right-justify text."));
		values.push_back(k3d::ienumeration_property::enumeration_value_t("Justify", "justify", "Justify text."));
	}

	return values;
}

} // namespace annotation

} // namespace module

