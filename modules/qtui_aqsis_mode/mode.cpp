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

#include "mode.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/plugin.h>

#include <boost/assign/list_of.hpp>

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>

namespace module
{

namespace qtui
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// mode

mode::mode() :
	pixmap_item(0)
{
}

void mode::enable(k3d::idocument& Document, QGraphicsScene& Scene)
{
	pixmap_item = new QGraphicsPixmapItem();
	Scene.addItem(pixmap_item);

	render_engine.reset(new module::qtui::aqsis::thread(k3d::plugin::create<k3d::istreaming_bitmap_source>("EmbeddedAqsisRenderManEngine")));
	QObject::connect(render_engine.get(), SIGNAL(bitmap_start(int, int)), this, SLOT(on_bitmap_start(int, int)), Qt::BlockingQueuedConnection);
	QObject::connect(render_engine.get(), SIGNAL(bitmap_bucket(int, int, const k3d::istreaming_bitmap_source::bucket*)), this, SLOT(on_bitmap_bucket(int, int, const k3d::istreaming_bitmap_source::bucket*)), Qt::BlockingQueuedConnection);
	QObject::connect(render_engine.get(), SIGNAL(bitmap_finish()), this, SLOT(on_bitmap_finish()), Qt::BlockingQueuedConnection);
	render_engine->start();
}

k3d::iplugin_factory& mode::get_factory()
{
	static k3d::application_plugin_factory<mode> factory(
		k3d::uuid(0x119ce863, 0x55493089, 0x49eb2f87, 0xb9a59431),
		"QTUIAqsisMode",
		_("Provides an edit mode that automatically previews using the Aqsis render engine."),
		"QTUI Mode",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "mode"));

	return factory;
}

void mode::on_bitmap_start(int Width, int Height)
{
	image = QImage(Width, Height, QImage::Format_ARGB32);
	image.fill(QColor(255, 255, 255, 64).rgba());
	pixmap_item->setPixmap(QPixmap::fromImage(image));
}

void mode::on_bitmap_bucket(int XOffset, int YOffset, const k3d::istreaming_bitmap_source::bucket* Bucket)
{
	for(int y = 0; y != Bucket->height(); ++y)
	{
		for(int x = 0; x != Bucket->width(); ++x)
		{
			double red = get_color(*Bucket->at(x, y), boost::gil::red_t());
			double green = get_color(*Bucket->at(x, y), boost::gil::green_t());
			double blue = get_color(*Bucket->at(x, y), boost::gil::blue_t());
			double alpha = get_color(*Bucket->at(x, y), boost::gil::alpha_t());
			image.setPixel(x + XOffset, y + YOffset, QColor::fromRgbF(red, green, blue, alpha).rgba());
		}
	}

	pixmap_item->setPixmap(QPixmap::fromImage(image));
}

void mode::on_bitmap_finish()
{
}

} // namespace aqsis

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::aqsis::mode::get_factory());
K3D_MODULE_END

