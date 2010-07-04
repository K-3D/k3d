#ifndef MODULES_QTUI_JAVASCRIPT_SHELL_SHELL_H
#define MODULES_QTUI_JAVASCRIPT_SHELL_SHELL_H

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

#include "ui_shell.h"

#include <k3dsdk/log_control.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/qtui/application_widget.h>
#include <k3dsdk/signal_system.h>

#include <boost/scoped_ptr.hpp>

#include <QMainWindow>
#include <QScriptEngine>

namespace k3d { class iplugin_factory; }

namespace module
{

namespace qtui
{

namespace javascript
{

class shell :
	public QMainWindow,
	public k3d::iunknown,
	public sigc::trackable
{
	Q_OBJECT;

	typedef QMainWindow base;

public:
	shell();

	static k3d::iplugin_factory& get_factory();

public Q_SLOTS:
	void on_console_execute(const QString& Command);
	void on_shell_clear_activated();

private:
	static QScriptValue print(QScriptContext* Context, QScriptEngine* Engine);
	void prompt();

	Ui::QTUIJavaScriptShell ui;
	k3d::qtui::application_widget application_widget;
	boost::scoped_ptr<QScriptEngine> script_engine;
};

} // namespace javascript

} // namespace qtui

} // namespace module

#endif // !MODULES_QTUI_JAVASCRIPT_SHELL_SHELL_H

