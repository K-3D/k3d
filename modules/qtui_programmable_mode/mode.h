#ifndef MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H
#define MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H

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

#include <k3dsdk/data.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/qtui/mode.h>
#include <k3dsdk/types.h>

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
	public k3d::qtui::mode,
	public k3d::property_collection
{
	Q_OBJECT;

	typedef k3d::qtui::mode base;

public:
	mode();

	void enable(k3d::idocument& Document, QGraphicsScene& Scene);

	static k3d::iplugin_factory& get_factory();

private Q_SLOTS:
	void on_script_changed();
	void on_load();
	void on_edit();
	void on_save();
	void on_reload();
	void on_scene_rect_changed(const QRectF& Rect);

private:
	k3d::idocument* document;
	QGraphicsScene* scene;
	k3d_data(k3d::string_t, k3d::data::immutable_name, change_signal, no_undo, local_storage, no_constraint, script_property, no_serialization) script;
	boost::scoped_ptr<QScriptEngine> script_engine;
	QGraphicsProxyWidget* edit_menu_proxy;
};

} // namespace programmable

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_PROGRAMMABLE_MODE_MODE_H
