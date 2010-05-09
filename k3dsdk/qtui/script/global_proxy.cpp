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
#include <k3dsdk/qtui/script/color_proxy.h>
#include <k3dsdk/qtui/script/global_proxy.h>
#include <k3dsdk/qtui/script/log_proxy.h>
#include <k3dsdk/share.h>

#include <QScriptEngine>

namespace k3d
{

namespace qtui
{

namespace script
{

/////////////////////////////////////////////////////////////////////////////
// global_proxy

void global_proxy::setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue result = Engine->newQObject(new global_proxy(), QScriptEngine::ScriptOwnership);
	Engine->globalObject().setProperty("k3d", result);

	log::setup(Engine, result);
	color::setup(Engine, result);
}

global_proxy::global_proxy()
{
	setObjectName("k3d");
}

const QString global_proxy::share_path() const
{
	return k3d::convert<QString>(k3d::share_path().native_utf8_string());
}

} // namespace script

} // namespace qtui

} // namespace k3d

