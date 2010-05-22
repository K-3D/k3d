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

#include "dialog.h"

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/qtui/application_widget.h>
#include <k3dsdk/signal_system.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>
#include <sstream>

#include <aqsis/math/region.h>
#include <libs/core/ddmanager/iddmanager.h>

namespace module
{

namespace qtui
{

namespace aqsis
{

/////////////////////////////////////////////////////////////////////////////
// dialog

dialog::dialog() :
	application_widget(*this)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);

	render_engine.reset(new module::qtui::aqsis::thread());
	QObject::connect(render_engine.get(), SIGNAL(open_display(int, int)), this, SLOT(on_open_display(int, int)), Qt::BlockingQueuedConnection);
	QObject::connect(render_engine.get(), SIGNAL(display_bucket(const QRect&, const Aqsis::IqChannelBuffer*)), this, SLOT(on_display_bucket(const QRect&, const Aqsis::IqChannelBuffer*)), Qt::BlockingQueuedConnection);
	render_engine->start();
}

void dialog::on_open_display(int Width, int Height)
{
	image = QImage(Width, Height, QImage::Format_RGB32);
	image.fill(QColor(128, 128, 128).rgba());
	ui.image->setPixmap(QPixmap::fromImage(image));
}

void dialog::on_display_bucket(const QRect& Region, const Aqsis::IqChannelBuffer* Buffer)
{
	const int index = Buffer->getChannelIndex("rgb");

	for(int y = 0; y != Buffer->height(); ++y)
	{
		for(int x = 0; x != Buffer->width(); ++x)
		{
			Aqsis::IqChannelBuffer::TqConstChannelPtr values = (*Buffer)(x, y, index);
			image.setPixel(x + Region.left(), y + Region.top(), QColor::fromRgbF(values[0], values[1], values[2]).rgb());
		}
	}

	ui.image->setPixmap(QPixmap::fromImage(image));
}

k3d::iplugin_factory& dialog::get_factory()
{
	static k3d::application_plugin_factory<dialog> factory(
		k3d::uuid(0xa467cfdd, 0x2545d997, 0x07ecf8a2, 0x042455fb),
		"QTUIAqsisDialog",
		_("Displays output from an embedded Aqsis RenderMan engine."),
		"QTUI Dialog",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "dialog"));

	return factory;
}

} // namespace aqsis

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::aqsis::dialog::get_factory());
K3D_MODULE_END

