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
#include "utility_python.h"

#include <k3dsdk/algebra.h>

#include <boost/python.hpp>
using namespace boost::python;

#include <sstream>

namespace k3d
{

namespace python
{

/*
euler_angles::euler_angles(const k3d::euler_angles& EulerAngles) :
	base(EulerAngles)
{
}

euler_angles::euler_angles(const euler_angles::AngleOrder Order, const double A, const double B, const double C) :
	base(radians(A), radians(B), radians(C), Order)
{
}

const double euler_angles::getitem(const int Item) const
{
	if(Item < 0 || Item > 2)
		throw std::out_of_range("index out-of-range");

	return degrees((*this)[Item]);
}

void euler_angles::setitem(const int Item, const double Value)
{
	if(Item < 0 || Item > 2)
		throw std::out_of_range("index out-of-range");

	(*this)[Item] = radians(Value);
}

const std::string euler_angles::str() const
{
	std::stringstream buffer;
	buffer << "(\"";

	switch(order)
	{
		case euler_angles::XYZstatic:
			buffer << "xyz";
			break;
		case euler_angles::XYXstatic:
			buffer << "xyx";
			break;
		case euler_angles::XZYstatic:
			buffer << "xzy";
			break;
		case euler_angles::XZXstatic:
			buffer << "xzx";
			break;
		case euler_angles::YZXstatic:
			buffer << "yzx";
			break;
		case euler_angles::YZYstatic:
			buffer << "yzy";
			break;
		case euler_angles::YXZstatic:
			buffer << "yxz";
			break;
		case euler_angles::YXYstatic:
			buffer << "yxy";
			break;
		case euler_angles::ZXYstatic:
			buffer << "zxy";
			break;
		case euler_angles::ZXZstatic:
			buffer << "zxz";
			break;
		case euler_angles::ZYXstatic:
			buffer << "zyx";
			break;
		case euler_angles::ZYZstatic:
			buffer << "zyz";
			break;
		case euler_angles::ZYXrotating:
			buffer << "zyx_rotating";
			break;
		case euler_angles::XYXrotating :
			buffer << "xyx_rotating";
			break;
		case euler_angles::YZXrotating:
			buffer << "yzx_rotating";
			break;
		case euler_angles::XZXrotating :
			buffer << "xzx_rotating";
			break;
		case euler_angles::XZYrotating:
			buffer << "xzy_rotating";
			break;
		case euler_angles::YZYrotating :
			buffer << "yzy_rotating";
			break;
		case euler_angles::ZXYrotating:
			buffer << "zxy_rotating";
			break;
		case euler_angles::YXYrotating :
			buffer << "yxy_rotating";
			break;
		case euler_angles::YXZrotating:
			buffer << "yxz_rotating";
			break;
		case euler_angles::ZXZrotating :
			buffer << "zxz_rotating";
			break;
		case euler_angles::XYZrotating:
			buffer << "xyz_rotating";
			break;
		case euler_angles::ZYZrotating:
			buffer << "zyz_rotating";
			break;
	}

	buffer << "\", " << degrees((*this)[0]) << ", " << degrees((*this)[1]) << ", " << degrees((*this)[2]);
	buffer << ")";

	return buffer.str();
}

const euler_angles operator+(const euler_angles& LHS, const euler_angles& RHS)
{
	if(LHS.order != RHS.order)
		throw std::invalid_argument("argument order must match");


	return k3d::euler_angles(LHS[0] + RHS[0], LHS[1] + RHS[1], LHS[2] + RHS[2], LHS.order);
}

const euler_angles operator*(const euler_angles& LHS, const double RHS)
{
	return k3d::euler_angles(LHS[0] * RHS, LHS[1] * RHS, LHS[2] * RHS, LHS.order);
}

const euler_angles operator*(const double LHS, const euler_angles& RHS)
{
	return RHS * LHS;
}
*/

void define_class_euler_angles()
{
	scope outer = class_<k3d::euler_angles>("euler_angles",
		"Encodes a change in orientation as three rotations around mutally-perpendicular axes.", no_init)
		.def(init<k3d::double_t, k3d::double_t, k3d::double_t, k3d::euler_angles::AngleOrder>())
		.def_readwrite("order", &k3d::euler_angles::order)
		.def("__len__", &utility::constant_len_len<k3d::euler_angles, 3>)
		.def("__getitem__", &utility::constant_len_get_item<k3d::euler_angles, 3, k3d::double_t>)
		.def("__setitem__", &utility::constant_len_set_item<k3d::euler_angles, 3, k3d::double_t>)
//		.def(self + self)
//		.def(self * double())
//		.def(double() * self)
		.def(self_ns::str(self))
		;

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
		.attr("__module__") = "k3d"
		;
}

} // namespace python

} // namespace k3d

