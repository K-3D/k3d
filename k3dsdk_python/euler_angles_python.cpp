// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "euler_angles_python.h"

#include <boost/python.hpp>
using namespace boost::python;

#include <sstream>

namespace k3d
{

namespace python
{

const k3d::euler_angles euler_angles_init(k3d::euler_angles::AngleOrder order, double a, double b, double c)
{
	return k3d::euler_angles(k3d::radians(a), k3d::radians(b), k3d::radians(c), order);
}

int euler_angles_len(const k3d::euler_angles& LHS)
{
	return 3;
}

double euler_angles_getitem(const k3d::euler_angles& LHS, int item)
{
	return k3d::degrees(LHS[item]);
}

void euler_angles_setitem(k3d::euler_angles& LHS, int item, double value)
{
	LHS[item] = k3d::radians(value);
}

const std::string euler_angles_str(const k3d::euler_angles& LHS)
{
	std::stringstream buffer;
	buffer << "(\"";

	switch(LHS.order)
	{
		case k3d::euler_angles::XYZstatic:
			buffer << "xyz";
			break;
		case k3d::euler_angles::XYXstatic:
			buffer << "xyx";
			break;
		case k3d::euler_angles::XZYstatic:
			buffer << "xzy";
			break;
		case k3d::euler_angles::XZXstatic:
			buffer << "xzx";
			break;
		case k3d::euler_angles::YZXstatic:
			buffer << "yzx";
			break;
		case k3d::euler_angles::YZYstatic:
			buffer << "yzy";
			break;
		case k3d::euler_angles::YXZstatic:
			buffer << "yxz";
			break;
		case k3d::euler_angles::YXYstatic:
			buffer << "yxy";
			break;
		case k3d::euler_angles::ZXYstatic:
			buffer << "zxy";
			break;
		case k3d::euler_angles::ZXZstatic:
			buffer << "zxz";
			break;
		case k3d::euler_angles::ZYXstatic:
			buffer << "zyx";
			break;
		case k3d::euler_angles::ZYZstatic:
			buffer << "zyz";
			break;
		case k3d::euler_angles::ZYXrotating:
			buffer << "zyx_rotating";
			break;
		case k3d::euler_angles::XYXrotating :
			buffer << "xyx_rotating";
			break;
		case k3d::euler_angles::YZXrotating:
			buffer << "yzx_rotating";
			break;
		case k3d::euler_angles::XZXrotating :
			buffer << "xzx_rotating";
			break;
		case k3d::euler_angles::XZYrotating:
			buffer << "xzy_rotating";
			break;
		case k3d::euler_angles::YZYrotating :
			buffer << "yzy_rotating";
			break;
		case k3d::euler_angles::ZXYrotating:
			buffer << "zxy_rotating";
			break;
		case k3d::euler_angles::YXYrotating :
			buffer << "yxy_rotating";
			break;
		case k3d::euler_angles::YXZrotating:
			buffer << "yxz_rotating";
			break;
		case k3d::euler_angles::ZXZrotating :
			buffer << "zxz_rotating";
			break;
		case k3d::euler_angles::XYZrotating:
			buffer << "xyz_rotating";
			break;
		case k3d::euler_angles::ZYZrotating:
			buffer << "zyz_rotating";
			break;
	}

	buffer << "\", " << k3d::degrees(LHS[0]) << ", " << k3d::degrees(LHS[1]) << ", " << k3d::degrees(LHS[2]);
	buffer << ")";

	return buffer.str();
}

const k3d::euler_angles euler_angles_add(const k3d::euler_angles& LHS, const k3d::euler_angles& RHS)
{
	if(LHS.order != RHS.order)
		throw std::invalid_argument("argument order must match");


	return k3d::euler_angles(LHS[0] + RHS[0], LHS[1] + RHS[1], LHS[2] + RHS[2], LHS.order);
}

const k3d::euler_angles euler_angles_multiply(const k3d::euler_angles& LHS, const double RHS)
{
	return k3d::euler_angles(LHS[0] * RHS, LHS[1] * RHS, LHS[2] * RHS, LHS.order);
}

void enum_euler_angles_order()
{
	enum_<k3d::euler_angles::AngleOrder>("angle_order")
		.value("xyz", k3d::euler_angles::XYZstatic)
		.value("xyz", k3d::euler_angles::XYZstatic)
		.value("xyx", k3d::euler_angles::XYXstatic)
		.value("xzy", k3d::euler_angles::XZYstatic)
		.value("xzx", k3d::euler_angles::XZXstatic)
		.value("yzx", k3d::euler_angles::YZXstatic)
		.value("yzy", k3d::euler_angles::YZYstatic)
		.value("yxz", k3d::euler_angles::YXZstatic)
		.value("yxy", k3d::euler_angles::YXYstatic)
		.value("zxy", k3d::euler_angles::ZXYstatic)
		.value("zxz", k3d::euler_angles::ZXZstatic)
		.value("zyx", k3d::euler_angles::ZYXstatic)
		.value("zyz", k3d::euler_angles::ZYZstatic)
		.value("zyx_rotating", k3d::euler_angles::ZYXrotating)
		.value("xyx_rotating", k3d::euler_angles::XYXrotating)
		.value("yzx_rotating", k3d::euler_angles::YZXrotating)
		.value("xzx_rotating", k3d::euler_angles::XZXrotating)
		.value("xzy_rotating", k3d::euler_angles::XZYrotating)
		.value("yzy_rotating", k3d::euler_angles::YZYrotating)
		.value("zxy_rotating", k3d::euler_angles::ZXYrotating)
		.value("yxy_rotating", k3d::euler_angles::YXYrotating)
		.value("yxz_rotating", k3d::euler_angles::YXZrotating)
		.value("zxz_rotating", k3d::euler_angles::ZXZrotating)
		.value("xyz_rotating", k3d::euler_angles::XYZrotating)
		.value("zyz_rotating", k3d::euler_angles::ZYZrotating)
		.attr("__module__") = "k3d";
}

void export_euler_angles()
{
	class_<k3d::euler_angles>("euler_angles")
		.def_readwrite("order", &k3d::euler_angles::order)
		.def("__len__", euler_angles_len)
		.def("__getitem__", euler_angles_getitem)
		.def("__setitem__", euler_angles_setitem)
		.def("__str__", euler_angles_str)
		.def("__add__", euler_angles_add)
		.def("__mul__", euler_angles_multiply);

	enum_euler_angles_order();
}

} // namespace python

} // namespace k3d

