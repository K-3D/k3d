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

#include <k3dsdk/log.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/log.h>

#include <QScriptContext>
#include <QScriptEngine>
#include <QScriptValue>

namespace k3d
{

namespace qtui
{

namespace script
{

namespace log
{

static QScriptValue critical(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::log() << k3d::critical << Context->argument(0).toString().toAscii().data() << std::endl;
	return QScriptValue();
}

static QScriptValue debug(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::log() << k3d::debug << Context->argument(0).toString().toAscii().data() << std::endl;
	return QScriptValue();
}

static QScriptValue error(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::log() << k3d::error << Context->argument(0).toString().toAscii().data() << std::endl;
	return QScriptValue();
}

static QScriptValue info(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::log() << k3d::info << Context->argument(0).toString().toAscii().data() << std::endl;
	return QScriptValue();
}

static QScriptValue warning(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::log() << k3d::warning << Context->argument(0).toString().toAscii().data() << std::endl;
	return QScriptValue();
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue proxy = Engine->newObject();
	proxy.setProperty("critical", Engine->newFunction(critical));
	proxy.setProperty("debug", Engine->newFunction(debug));
	proxy.setProperty("error", Engine->newFunction(error));
	proxy.setProperty("info", Engine->newFunction(info));
	proxy.setProperty("warning", Engine->newFunction(warning));

	Namespace.setProperty("log", proxy);
}

} // namespace log

} // namespace script

} // namespace qtui

} // namespace k3d

