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

#include "axis.h"
#include "log.h"
#include "result.h"

#include <iostream>

namespace k3d
{

const ienumeration_property::enumeration_values_t& axis_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t("X Axis", "x", "X Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("Y Axis", "y", "Y Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("Z Axis", "z", "Z Axis"));
	}
	
	return values;
}

const ienumeration_property::enumeration_values_t& signed_axis_values()
{
	static ienumeration_property::enumeration_values_t values;
	if(values.empty())
	{
		values.push_back(ienumeration_property::enumeration_value_t("-X Axis", "-x", "-X Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("+X Axis", "+x", "+X Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("-Y Axis", "-y", "-Y Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("+Y Axis", "+y", "+Y Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("-Z Axis", "-z", "-Z Axis"));
		values.push_back(ienumeration_property::enumeration_value_t("+Z Axis", "+z", "+Z Axis"));
	}
	
	return values;
}

std::ostream& operator << (std::ostream& Stream, const axis& Value)
{
	switch(Value)
	{
		case X:
			Stream << "x";
			break;
		case Y:
			Stream << "y";
			break;
		case Z:
			Stream << "z";
			break;
	}

	return Stream;
}

std::istream& operator >> (std::istream& Stream, axis& Value)
{
	char c;
	Stream >> c;

	if('x' == c)
		Value = X;
	else if('y' == c)
		Value = Y;
	else if('z' == c)
		Value = Z;
	else
		log() << error << k3d_file_reference << ": could not extract value [" << c << "]" << std::endl;

	return Stream;
}

std::ostream& operator << (std::ostream& Stream, const signed_axis& Value)
{
	switch(Value)
	{
		case NX:
			Stream << "-x";
			break;
		case PX:
			Stream << "+x";
			break;
		case NY:
			Stream << "-y";
			break;
		case PY:
			Stream << "+y";
			break;
		case NZ:
			Stream << "-z";
			break;
		case PZ:
			Stream << "+z";
			break;
	}

	return Stream;
}

std::istream& operator >> (std::istream& Stream, signed_axis& Value)
{
	std::string c;
	Stream >> c;

	if("-x" == c)
		Value = NX;
	else if("+x" == c)
		Value = PX;
	else if("-y" == c)
		Value = NY;
	else if("+y" == c)
		Value = PY;
	else if("-z" == c)
		Value = NZ;
	else if("+z" == c)
		Value = PZ;
	else
		log() << error << error << k3d_file_reference << ": could not extract value [" << c << "]" << std::endl;

	return Stream;
}

} // namespace k3d

