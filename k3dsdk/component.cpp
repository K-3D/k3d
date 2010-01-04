// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/component.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>

#include <iostream>

namespace k3d
{

const ilist_property<string_t>::values_t& component_values()
{
	static ilist_property<string_t>::values_t values;
	if(values.empty())
	{
		values.push_back("constant");
		values.push_back("curve");
		values.push_back("edge");
		values.push_back("face");
		values.push_back("parameter");
		values.push_back("patch");
		values.push_back("surface");
		values.push_back("vertex");
	}
	
	return values;
}

} // namespace k3d

