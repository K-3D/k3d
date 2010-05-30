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

#include <k3dsdk/imeta_object.h>

namespace k3d
{

const boost::any imeta_object::execute(const string_t& Method)
{
	std::vector<boost::any> arguments;
	return execute(Method, arguments);
}

const boost::any imeta_object::execute(const string_t& Method, const boost::any& Argument1)
{
	std::vector<boost::any> arguments;
	arguments.push_back(Argument1);
	return execute(Method, arguments);
}

const boost::any imeta_object::execute(const string_t& Method, const boost::any& Argument1, const boost::any& Argument2)
{
	std::vector<boost::any> arguments;
	arguments.push_back(Argument1);
	arguments.push_back(Argument2);
	return execute(Method, arguments);
}

const boost::any imeta_object::execute(const string_t& Method, const boost::any& Argument1, const boost::any& Argument2, const boost::any& Argument3)
{
	std::vector<boost::any> arguments;
	arguments.push_back(Argument1);
	arguments.push_back(Argument2);
	arguments.push_back(Argument3);
	return execute(Method, arguments);
}

} // namespace k3d

