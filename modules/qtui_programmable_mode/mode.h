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

#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/mode.h>

#include <QGraphicsProxyWidget>
#include <QScriptEngine>

#include <boost/scoped_ptr.hpp>

namespace k3d { class iplugin_factory; }
class QGraphicsProxyWidget;

namespace module
{

namespace qtui
{

namespace programmable
{

/////////////////////////////////////////////////////////////////////////////
// mode

/// Provides a programmable edit mode, mainly intended for development.
class mode :
	public QObject,
	public k3d::qtui::mode,
	public k3d::iunknown
{
	Q_OBJECT

	typedef k3d::qtui::mode base;

public:
	mode();

	void enable(QGraphicsScene& Scene);

	static k3d::iplugin_factory& get_factory();

private Q_SLOTS:
	void on_load();
	void on_edit();
	void on_save();
	void on_reload();
	void on_scene_rect_changed(const QRectF& Rect);

private:
	QGraphicsScene* scene;
	QString program;
	boost::scoped_ptr<QScriptEngine> script_engine;
	boost::scoped_ptr<QGraphicsProxyWidget> edit_menu_proxy;
};

} // namespace programmable

} // namespace qtui

} // namespace module

