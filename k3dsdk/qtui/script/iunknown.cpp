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

#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/log.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/iunknown.h>

#include <QMetaType>
#include <QScriptEngine>
#include <QScriptValue>

#include <cassert>

class QScriptContext;

typedef k3d::iunknown* piunknown;
Q_DECLARE_METATYPE(k3d::iunknown*);

namespace k3d
{

namespace qtui
{

namespace script
{

namespace iunknown
{

static QScriptValue plugin_factory_name(QScriptContext* Context, QScriptEngine* Engine)
{
	k3d::iplugin_factory* const factory = dynamic_cast<k3d::iplugin_factory*>(qscriptvalue_cast<k3d::iunknown*>(Context->thisObject()));
	return k3d::convert<QString>(factory->name());
}

static QScriptValue toScriptValue(QScriptEngine* Engine, const piunknown& Value)
{
	QScriptValue result = Engine->newVariant(QVariant::fromValue(Value));

	if(k3d::iplugin_factory* const factory = dynamic_cast<k3d::iplugin_factory*>(Value))
	{
		result.setProperty("name", Engine->newFunction(plugin_factory_name), QScriptValue::PropertyGetter);
	}

	return result;
}

static void fromScriptValue(const QScriptValue& Object, piunknown& Value)
{
	Value = qvariant_cast<k3d::iunknown*>(Object.toVariant());
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	qScriptRegisterMetaType(Engine, toScriptValue, fromScriptValue);
}

} // namespace iunknown

} // namespace script

} // namespace qtui

} // namespace k3d

