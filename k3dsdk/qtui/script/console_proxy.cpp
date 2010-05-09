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

#include <k3dsdk/qtui/console.h>
#include <k3dsdk/qtui/script/console_proxy.h>

#include <QScriptEngine>
#include <QScriptValue>

class QScriptContext;

namespace k3d
{

namespace qtui
{

namespace script
{

namespace console
{

static QScriptValue constructor(QScriptContext* Context, QScriptEngine* Engine)
{
	return Engine->newQObject(new k3d::qtui::console::widget(), QScriptEngine::ScriptOwnership);
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	Namespace.setProperty("console", Engine->newFunction(console::constructor));
}

} // namespace console

} // namespace script

} // namespace qtui

} // namespace k3d

