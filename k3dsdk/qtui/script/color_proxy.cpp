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

#include <k3dsdk/color.h>

#include <k3dsdk/log.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/color_proxy.h>

#include <QMetaType>
#include <QScriptEngine>
#include <QScriptValue>

class QScriptContext;

Q_DECLARE_METATYPE(k3d::color);
Q_DECLARE_METATYPE(k3d::color*);

namespace k3d
{

namespace qtui
{

namespace script
{

namespace color
{

static QScriptValue red(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 0:
			return qscriptvalue_cast<k3d::color*>(Context->thisObject())->red;
		case 1:
			qscriptvalue_cast<k3d::color*>(Context->thisObject())->red = Context->argument(0).toNumber();
	}
	return QScriptValue();
}

static QScriptValue green(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 0:
			return qscriptvalue_cast<k3d::color*>(Context->thisObject())->green;
		case 1:
			qscriptvalue_cast<k3d::color*>(Context->thisObject())->green = Context->argument(0).toNumber();
	}
	return QScriptValue();
}

static QScriptValue blue(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 0:
			return qscriptvalue_cast<k3d::color*>(Context->thisObject())->blue;
		case 1:
			qscriptvalue_cast<k3d::color*>(Context->thisObject())->blue = Context->argument(0).toNumber();
	}
	return QScriptValue();
}

static QScriptValue toString(QScriptContext* Context, QScriptEngine* Engine)
{
	const k3d::color value = qscriptvalue_cast<k3d::color>(Context->thisObject());
	return QString("{red:%1, green:%2, blue:%3}").arg(value.red).arg(value.green).arg(value.blue);
}

static QScriptValue constructor(QScriptContext* Context, QScriptEngine* Engine)
{
	QScriptValue result;
	switch(Context->argumentCount())
	{
		case 0:
			result = Engine->toScriptValue(k3d::color(0, 0, 0));
			break;
		case 1:
			result = Engine->toScriptValue(k3d::color(Context->argument(0).toNumber(), Context->argument(0).toNumber(), Context->argument(0).toNumber()));
			break;
		case 3:
			result = Engine->toScriptValue(k3d::color(Context->argument(0).toNumber(), Context->argument(1).toNumber(), Context->argument(2).toNumber()));
			break;
		default:
			result = Context->throwError(QScriptContext::TypeError, "0, 1, or 3 numeric arguments required.");
			break;
	}

	result.setPrototype(Context->callee().property("prototype"));
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue prototype = Engine->newObject();
	prototype.setProperty("red", Engine->newFunction(red), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
	prototype.setProperty("green", Engine->newFunction(green), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
	prototype.setProperty("blue", Engine->newFunction(blue), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
	prototype.setProperty("toString", Engine->newFunction(toString));

	Engine->setDefaultPrototype(qMetaTypeId<k3d::color>(), prototype);

	Namespace.setProperty("color", Engine->newFunction(constructor, prototype));
}

} // namespace color

} // namespace script

} // namespace qtui

} // namespace k3d

