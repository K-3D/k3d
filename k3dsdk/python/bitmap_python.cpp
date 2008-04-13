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

tuple bitmap::get_pixel(const unsigned long X, const unsigned long Y)
{
	if(X >= wrapped().width())
		throw std::invalid_argument("X value out-of-range");
	if(Y >= wrapped().height())
		throw std::invalid_argument("Y value out-of-range");

	const k3d::bitmap::view_t& bitmap = boost::gil::view(wrapped());
	k3d::pixel& pixel = bitmap(X, Y);

	return boost::python::make_tuple(
		boost::gil::get_color(pixel, boost::gil::red_t()),
		boost::gil::get_color(pixel, boost::gil::green_t()),
		boost::gil::get_color(pixel, boost::gil::blue_t()),
		boost::gil::get_color(pixel, boost::gil::alpha_t()));
}

void bitmap::set_pixel(const unsigned long X, const unsigned long Y, const tuple& Pixel)
{
	if(X >= wrapped().width())
		throw std::invalid_argument("X value out-of-range");
	if(Y >= wrapped().height())
		throw std::invalid_argument("Y value out-of-range");
	if(len(Pixel) != 4)
		throw std::invalid_argument("Pixel argument must be a 4-tuple");

	const k3d::bitmap::view_t& bitmap = boost::gil::view(wrapped());
	k3d::pixel& pixel = bitmap(X, Y);

	pixel = k3d::pixel(
		boost::python::extract<double>(Pixel[0])(),
		boost::python::extract<double>(Pixel[1])(),
		boost::python::extract<double>(Pixel[2])(),
		boost::python::extract<double>(Pixel[3])());
}

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
		.def("width", &bitmap::width,
			"Returns the width of the image in pixels.")
		.def("height", &bitmap::height,
			"Returns the height of the image in pixels.")
		.def("get_pixel", &bitmap::get_pixel)
		.def("set_pixel", &bitmap::set_pixel)
		;
}

} // namespace python

} // namespace k3d

