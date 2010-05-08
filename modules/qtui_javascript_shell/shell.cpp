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

#include "shell.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/engine.h>

#include <boost/assign/list_of.hpp>

#include <QDateTime>

namespace module
{

namespace qtui
{

namespace javascript
{

shell::shell() :
	application_widget(*this)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);

	script_engine.reset(k3d::qtui::script::engine());

	// Redirect script output to the console ...
	QScriptValue custom_print = script_engine->newFunction(&shell::print, 1);
	custom_print.setData(script_engine->newQObject(this));
	script_engine->globalObject().setProperty("print", custom_print);

	prompt();
}

k3d::iplugin_factory& shell::get_factory()
{
	static k3d::application_plugin_factory<shell> factory(
		k3d::uuid(0x36e6340d, 0x36456f5f, 0x2e68eeb8, 0x970050e5),
		"QTUIJavaScriptShell",
		_("Provides an interactive JavaScript shell."),
		"QTUI",
		k3d::iplugin_factory::EXPERIMENTAL,
		boost::assign::map_list_of("qtui:component-type", "dialog")
		);

	return factory;
}

void shell::on_console_execute(const QString& Command)
{
	QScriptValue result = script_engine->evaluate(Command);

	if(result.isError())
		ui.console->print_html("<span style='color: #f00'>" + result.toString() + "</span>");
	else if(!result.isUndefined())
		ui.console->print_html("<span style='color: #00f'>" + result.toString() + "</span>");
	
	prompt();
}

void shell::on_shell_clear_activated()
{
	ui.console->clear();
	prompt();
}

void shell::prompt()
{
	ui.console->print_html("<span style='color: #ccc'>&gt;&gt;&gt;</span> ");
}

QScriptValue shell::print(QScriptContext* Context, QScriptEngine* Engine)
{
	QScriptValue argument = Context->argument(0);
	if(!argument.isString())
		return Context->throwError(QScriptContext::TypeError, "print(): expected string argument");

	qobject_cast<shell*>(Context->callee().data().toQObject())->ui.console->print_html("<span style='color: #00f'>" + argument.toString() + "</span>");
	return QScriptValue();
}

} // namespace javascript

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::javascript::shell::get_factory());
K3D_MODULE_END

