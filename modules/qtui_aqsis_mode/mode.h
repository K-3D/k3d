#ifndef MODULES_QTUI_AQSIS_MODE_MODE_H
#define MODULES_QTUI_AQSIS_MODE_MODE_H

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

#include "thread.h"

#include <k3dsdk/module.h>
#include <k3dsdk/qtui/mode.h>

#include <boost/scoped_ptr.hpp>

#include <QImage>

class QGraphicsPixmapItem;

namespace module
{

namespace qtui
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// mode

/// Sets-up a default mode for use when no other mode is in-effect.
class mode :
	public k3d::qtui::mode,
	public k3d::iunknown
{
	Q_OBJECT;

	typedef k3d::qtui::mode base;

public:
	mode();

	void enable(k3d::idocument& Document, QGraphicsScene& Scene);

	static k3d::iplugin_factory& get_factory();

public Q_SLOTS:
	void on_bitmap_start(int Width, int Height);
	void on_bitmap_bucket(int XOffset, int YOffset, const k3d::istreaming_bitmap_source::bucket* Bucket);
	void on_bitmap_finish();

private:
	QImage image;
	QGraphicsPixmapItem* pixmap_item;
	boost::scoped_ptr<module::qtui::aqsis::thread> render_engine;
};

} // namespace aqsis

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_AQSIS_MODE_MODE_H
