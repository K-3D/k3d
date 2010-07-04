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
#include <k3dsdk/qtui/script/engine.h>
#include <k3dsdk/qtui/script/model.h>

#include <QScriptEngine>
#include <QStringList>

namespace k3d
{

namespace qtui
{

namespace script
{

/////////////////////////////////////////////////////////////////////////////
// engine

QScriptEngine* engine()
{
	QScriptEngine* const script_engine = new QScriptEngine();

	script_engine->importExtension("qt");
	script_engine->importExtension("qt.core");
	script_engine->importExtension("qt.gui");
	script_engine->importExtension("qt.network");
	script_engine->importExtension("qt.opengl");
	script_engine->importExtension("qt.phonon");
	script_engine->importExtension("qt.sql");
	script_engine->importExtension("qt.svg");
	script_engine->importExtension("qt.webkit");
	script_engine->importExtension("qt.xml");
	script_engine->importExtension("qt.xmlpatterns");

	model::setup(script_engine, script_engine->globalObject());

	return script_engine;
}

} // namespace script

} // namespace qtui

} // namespace k3d

