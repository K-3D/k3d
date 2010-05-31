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
#include <k3dsdk/qtui/script/color.h>
#include <k3dsdk/qtui/script/console.h>
#include <k3dsdk/qtui/script/iunknown.h>
#include <k3dsdk/qtui/script/log.h>
#include <k3dsdk/qtui/script/matrix4.h>
#include <k3dsdk/qtui/script/model.h>
#include <k3dsdk/qtui/script/plugin.h>
#include <k3dsdk/share.h>

#include <QScriptEngine>

namespace k3d
{

namespace qtui
{

namespace script
{

namespace model
{

static QScriptValue share_path(QScriptContext* Context, QScriptEngine* Engine)
{
	return Engine->toScriptValue(k3d::convert<QString>(k3d::share_path().native_utf8_string()));
}

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue proxy = Engine->newObject();
	proxy.setProperty("share_path", Engine->newFunction(share_path), QScriptValue::PropertyGetter);

	color::setup(Engine, proxy);
	console::setup(Engine, proxy);
	iunknown::setup(Engine, proxy);
	log::setup(Engine, proxy);
	matrix4::setup(Engine, proxy);
	plugin::setup(Engine, proxy);

	Namespace.setProperty("k3d", proxy);
}

} // namespace model

} // namespace script

} // namespace qtui

} // namespace k3d

