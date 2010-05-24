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
#include <k3dsdk/plugin.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/plugin.h>

#include <QMetaType>
#include <QScriptContext>
#include <QScriptEngine>
#include <QScriptValue>
#include <QStringList>

Q_DECLARE_METATYPE(k3d::iunknown*);

namespace k3d
{

namespace qtui
{

namespace script
{

namespace plugin
{

namespace factory
{

static QScriptValue convert(QScriptEngine* Engine, const k3d::plugin::factory::collection_t& Factories)
{
	QScriptValue result = Engine->newArray();

	for(k3d::uint_t i = 0; i != Factories.size(); ++i)
		result.setProperty(i, Engine->toScriptValue(static_cast<k3d::iunknown*>(Factories[i])));

	return result;
}

static QScriptValue lookup(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 0:
			return convert(Engine, k3d::plugin::factory::lookup());
		case 2:
			return convert(Engine, k3d::plugin::factory::lookup(Context->argument(0).toString().toAscii().data(), Context->argument(1).toString().toAscii().data()));
	}
}

} // namespace factory

static QScriptValue create(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 1:
			return Engine->toScriptValue(k3d::plugin::create(Context->argument(0).toString().toAscii().data()));
	}

	return QScriptValue();
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue factory = Engine->newObject();
	factory.setProperty("lookup", Engine->newFunction(factory::lookup));
	
	QScriptValue plugin = Engine->newObject();
	plugin.setProperty("factory", factory);
	plugin.setProperty("create", Engine->newFunction(create));

	Namespace.setProperty("plugin", plugin);
}

} // namespace plugin

} // namespace script

} // namespace qtui

} // namespace k3d

