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

#include <boost/assign/list_of.hpp>
#include <set>

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

const uint_t component_size(const mesh::primitive& Primitive, const string_t Component)
{
	static const std::set<string_t> two_sided_curves = boost::assign::list_of("linear_curve")("cubic_curve")("nurbs_curve");
	static const std::set<string_t> three_sided_patches = boost::assign::list_of("bezier_triangle_patch");
	static const std::set<string_t> four_sided_patches = boost::assign::list_of("bilinear_patch")("bicubic_patch")("nurbs_patch");
	static const std::set<string_t> four_sided_surfaces = boost::assign::list_of("cone")("cylinder")("disk")("hyperboloid")("paraboloid")("sphere")("torus");

	if(Primitive.structure.count(Component))
	{
		return Primitive.structure.find(Component)->second.row_count();
	}
	else if(Component == "constant")
	{
		return 1;
	}
	else if(Component == "parameter" && two_sided_curves.count(Primitive.type))
	{
		return Primitive.structure.find("curve")->second.row_count() * 2;
	}
	else if(Component == "parameter" && three_sided_patches.count(Primitive.type))
	{
		return Primitive.structure.find("patch")->second.row_count() * 3;
	}
	else if(Component == "parameter" && four_sided_patches.count(Primitive.type))
	{
		return Primitive.structure.find("patch")->second.row_count() * 4;
	}
	else if(Component == "parameter" && four_sided_surfaces.count(Primitive.type))
	{
		return Primitive.structure.find("surface")->second.row_count() * 4;
	}

	k3d::log() << error << "Cannot determine count for unknown attribute [" << Component << "] in [" << Primitive.type << "] primitive" << std::endl;
	return 0;
}

} // namespace k3d

