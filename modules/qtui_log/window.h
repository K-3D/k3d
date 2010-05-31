#ifndef MODULES_QTUI_LOG_WINDOW_H
#define MODULES_QTUI_LOG_WINDOW_H

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

#include "ui_window.h"

#include <k3dsdk/log_control.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/application_widget.h>
#include <k3dsdk/signal_system.h>

#include <QMainWindow>

namespace k3d { class iplugin_factory; }

namespace module
{

namespace qtui
{

namespace log
{

class window :
	public QMainWindow,
	public k3d::iunknown,
	public sigc::trackable
{
	Q_OBJECT;

	typedef QMainWindow base;

public:
	window();

	void on_log_message(const time_t Timestamp, const k3d::log_level_t Level, const std::string& Message);

	static k3d::iplugin_factory& get_factory();

private:
	Ui::QTUILog ui;
	k3d::qtui::application_widget application_widget;
};

} // namespace log

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_LOG_WINDOW_H

