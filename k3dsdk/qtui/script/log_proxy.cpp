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
#include <k3dsdk/qtui/script/log_proxy.h>

#include <QScriptValue>

namespace k3d
{

namespace qtui
{

namespace script
{

namespace log
{

/////////////////////////////////////////////////////////////////////////////
// proxy

proxy::proxy(QObject* Parent) :
	QObject(Parent)
{
	setObjectName("log");
}

void proxy::critical(const QString& Message)
{
	k3d::log() << k3d::critical << Message.toAscii().data() << std::endl;
}

void proxy::debug(const QString& Message)
{
	k3d::log() << k3d::debug << Message.toAscii().data() << std::endl;
}

void proxy::error(const QString& Message)
{
	k3d::log() << k3d::error << Message.toAscii().data() << std::endl;
}

void proxy::info(const QString& Message)
{
	k3d::log() << k3d::info << Message.toAscii().data() << std::endl;
}

void proxy::warning(const QString& Message)
{
	k3d::log() << k3d::warning << Message.toAscii().data() << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// setup

void setup(QScriptEngine* Engine, QScriptValue Namespace)
{
	proxy* const result = new proxy(Namespace.toQObject());
}

} // namespace log

} // namespace script

} // namespace qtui

} // namespace k3d

