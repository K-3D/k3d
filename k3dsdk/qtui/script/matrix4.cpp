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

#include <k3dsdk/algebra.h>

#include <k3dsdk/log.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/script/matrix4.h>

#include <QMetaType>
#include <QScriptEngine>
#include <QScriptValue>

class QScriptContext;

Q_DECLARE_METATYPE(k3d::matrix4);
Q_DECLARE_METATYPE(k3d::matrix4*);

namespace k3d
{

namespace qtui
{

namespace script
{

namespace matrix4
{

static QScriptValue row_major(QScriptContext* Context, QScriptEngine* Engine)
{
	const k3d::matrix4 value = qscriptvalue_cast<k3d::matrix4>(Context->thisObject());
	QVariantList result;
	for(k3d::uint_t i = 0; i != 4; ++i)
	{
		for(k3d::uint_t j = 0; j != 4; ++j)
		{
			result << value[i][j];
		}
	}
	return Engine->toScriptValue(result);
}

static QScriptValue column_major(QScriptContext* Context, QScriptEngine* Engine)
{
	const k3d::matrix4 value = qscriptvalue_cast<k3d::matrix4>(Context->thisObject());
	QVariantList result;
	for(k3d::uint_t j = 0; j != 4; ++j)
	{
		for(k3d::uint_t i = 0; i != 4; ++i)
		{
			result << value[i][j];
		}
	}
	return Engine->toScriptValue(result);
}

static QScriptValue toString(QScriptContext* Context, QScriptEngine* Engine)
{
	const k3d::matrix4 value = qscriptvalue_cast<k3d::matrix4>(Context->thisObject());
	std::ostringstream buffer;
	buffer << "matrix4(" << value << ")";
	return QString(buffer.str().c_str());
}

static QScriptValue constructor(QScriptContext* Context, QScriptEngine* Engine)
{
	QScriptValue result;
	switch(Context->argumentCount())
	{
		case 0:
			result = Engine->toScriptValue(k3d::matrix4());
			break;
		case 1:
			result = Engine->toScriptValue(k3d::identity3() * Context->argument(0).toNumber());
			break;
/*
		case 3:
			result = Engine->toScriptValue(k3d::matrix4(Context->argument(0).toNumber(), Context->argument(1).toNumber(), Context->argument(2).toNumber()));
*/
			break;
		default:
			return Context->throwError(QScriptContext::TypeError, "0, 1, or 3 numeric arguments required.");
	}

	result.setPrototype(Context->callee().property("prototype"));
	return result;
}

static QScriptValue scale(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 1:
			return Engine->toScriptValue(k3d::scale3(Context->argument(0).toNumber()));
		case 3:
			return Engine->toScriptValue(k3d::scale3(Context->argument(0).toNumber(), Context->argument(1).toNumber(), Context->argument(2).toNumber()));
		default:
			return Context->throwError(QScriptContext::TypeError, "1 or 3 numeric arguments required.");
	}
}

static QScriptValue translate(QScriptContext* Context, QScriptEngine* Engine)
{
	switch(Context->argumentCount())
	{
		case 3:
			return Engine->toScriptValue(k3d::translate3(Context->argument(0).toNumber(), Context->argument(1).toNumber(), Context->argument(2).toNumber()));
		default:
			return Context->throwError(QScriptContext::TypeError, "3 numeric arguments required.");
	}
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	QScriptValue prototype = Engine->newObject();
	prototype.setProperty("column_major", Engine->newFunction(column_major), QScriptValue::PropertyGetter);
	prototype.setProperty("row_major", Engine->newFunction(row_major), QScriptValue::PropertyGetter);
/*
	prototype.setProperty("blue", Engine->newFunction(blue), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
*/
	prototype.setProperty("toString", Engine->newFunction(toString));
	
	Engine->setDefaultPrototype(qMetaTypeId<k3d::matrix4>(), prototype);

	QScriptValue constructor = Engine->newFunction(matrix4::constructor, prototype);
	constructor.setProperty("scale", Engine->newFunction(scale));
	constructor.setProperty("translate", Engine->newFunction(translate));

	Namespace.setProperty("matrix4", constructor);
}

} // namespace matrix4

} // namespace script

} // namespace qtui

} // namespace k3d

