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

#include "bitmap_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

bitmap::bitmap() :
	base()
{
}

bitmap::bitmap(k3d::bitmap* Bitmap) :
	base(Bitmap)
{
}

bitmap::bitmap(k3d::bitmap& Bitmap) :
	base(Bitmap)
{
}

void bitmap::assign(const bitmap& Bitmap)
{
	wrapped() = Bitmap.wrapped();
}

void bitmap::reset(const unsigned long Width, const unsigned long Height)
{
	k3d::bitmap temp(Width, Height);
	wrapped().swap(temp);
}

/*
const k3d::color bitmap::get_pixel(const unsigned long X, const unsigned long Y)
{
	if(X >= wrapped().width())
		throw std::invalid_argument("x value out-of-range");
	if(Y >= wrapped().height())
		throw std::invalid_argument("y value out-of-range");

	return *(wrapped().begin() + (wrapped().width() * Y) + X);
}

void bitmap::set_pixel(const unsigned long X, const unsigned long Y, const k3d::color& Color)
{
	if(X >= wrapped().width())
		throw std::invalid_argument("x value out-of-range");
	if(Y >= wrapped().height())
		throw std::invalid_argument("y value out-of-range");

	*(wrapped().begin() + (wrapped().width() * Y) + X) = Color;
}
*/

const unsigned long bitmap::width()
{
	return wrapped().width();
}

const unsigned long bitmap::height()
{
	return wrapped().height();
}

void export_bitmap()
{
	class_<bitmap>("bitmap",
			"Stores a two-dimensional half-precision floating-point RGBA bitmap image.")
		.def("assign", &bitmap::assign,
			"Replaces the current contents with a copy of the given bitmap.\n\n"
			">>> mycopy.assign(myoriginal)\n\n")
		.def("reset", &bitmap::reset,
			"Replaces the current contents with an uninitialized bitmap with the given dimensions.\n\n"
			">>> mybitmap.reset(640, 480)\n\n")
//		.def("get_pixel", &bitmap::get_pixel)
//		.def("set_pixel", &bitmap::set_pixel)
		.def("width", &bitmap::width,
			"Returns the width of the image in pixels.")
		.def("height", &bitmap::height,
			"Returns the height of the image in pixels.")
		;
}

} // namespace python

} // namespace k3d

