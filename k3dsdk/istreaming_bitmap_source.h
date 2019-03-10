#ifndef K3DSDK_ISTREAMING_BITMAP_SOURCE_H
#define K3DSDK_ISTREAMING_BITMAP_SOURCE_H

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

#include <k3dsdk/bitmap.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>

#include <boost/version.hpp>
#if BOOST_VERSION < 106900
#include <boost/gil/gil_all.hpp>
#else
#include <boost/gil.hpp>
#endif

namespace k3d
{

class inode;

/// Abstract interface for an object that collects and distributes profiling data for the K-3D visualization pipeline
class istreaming_bitmap_source :
	public virtual iunknown
{
public:
	/// Defines a standard pixel as floating-point RGBA
	typedef boost::gil::pixel<boost::gil::bits32f, boost::gil::rgba_layout_t> pixel;
	/// Defines a standard bitmap as half-precision floating-point RGBA
	typedef boost::gil::image<pixel, false> bitmap;
	/// Defines a standard bitmap as half-precision floating-point RGBA
	typedef bitmap::const_view_t bucket;
	/// Define storage for a pixel coordinate
	typedef bitmap::coord_t coordinate;

	/// Connects a slot that will be called when streaming begins
	virtual sigc::connection connect_bitmap_start_signal(const sigc::slot<void, coordinate, coordinate>& Slot) = 0;
	/// Connects a slot that will be called when streaming begins
	virtual sigc::connection connect_bitmap_bucket_signal(const sigc::slot<void, coordinate, coordinate, const bucket&>& Slot) = 0;
	/// Connects a slot that will be called when streaming finishes
	virtual sigc::connection connect_bitmap_finish_signal(const sigc::slot<void>& Slot) = 0;

protected:
	istreaming_bitmap_source() {}
	istreaming_bitmap_source(const istreaming_bitmap_source& Other) : iunknown(Other) {}
	istreaming_bitmap_source& operator=(const istreaming_bitmap_source&) { return *this; }
	virtual ~istreaming_bitmap_source() {}
};

} // namespace k3d

#endif // !K3DSDK_ISTREAMING_BITMAP_SOURCE_H

