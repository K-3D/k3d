#ifndef MODULES_QTUI_NODE_LIST_PANEL_H
#define MODULES_QTUI_NODE_LIST_PANEL_H

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

#include "ui_panel.h"

#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/node_list_model.h>
#include <k3dsdk/qtui/panel.h>
#include <k3dsdk/signal_system.h>

#include <QWidget>

namespace k3d { class iplugin_factory; }

namespace module
{

namespace qtui
{

namespace node_list
{

class panel :
	public QWidget,
	public k3d::qtui::panel,
	public k3d::iunknown,
	public sigc::trackable
{
	Q_OBJECT;

	typedef QWidget base;

public:
	panel();

	void initialize(k3d::idocument& Document);

	static k3d::iplugin_factory& get_factory();

public Q_SLOTS:

private:
	Ui::QTUINodeListPanel ui;
	k3d::idocument* document;
	k3d::qtui::node_list_model model;
};

} // namespace node_list

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_NODE_LIST_PANEL_H

