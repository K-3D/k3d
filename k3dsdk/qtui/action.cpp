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
#include <k3dsdk/qtui/action.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/share.h>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// action

action::action(QObject* Parent, const sigc::slot<void>& Slot) :
	base(Parent)
{
	initialize(Slot);
}

action::action(const QString& Text, QObject* Parent, const sigc::slot<void>& Slot) :
	base(Text, Parent)
{
	initialize(Slot);
}

action::action(const QIcon& Icon, const QString& Text, QObject* Parent, const sigc::slot<void>& Slot) :
	base(Icon, Text, Parent)
{
	initialize(Slot);
}

action* action::create(iplugin_factory& Factory, QObject* Parent, const sigc::slot<void>& Slot)
{
	action* const result = new action(Parent, Slot);

	switch(Factory.quality())
	{
	case k3d::iplugin_factory::EXPERIMENTAL:
		result->setText(QString("%1 (Experimental)").arg(k3d::convert<QString>(Factory.name())));
		break;
	case k3d::iplugin_factory::DEPRECATED:
		result->setText(QString("%1 (Deprecated)").arg(k3d::convert<QString>(Factory.name())));
		break;
	case k3d::iplugin_factory::STABLE:
		result->setText(k3d::convert<QString>(Factory.name()));
		break;
	}
	result->setToolTip(k3d::convert<QString>(Factory.short_description()));
	result->setIcon(QIcon((k3d::share_path() / k3d::filesystem::generic_path("qtui/icons/" + Factory.name() + ".svg")).native_filesystem_string().c_str()));
	result->setIconVisibleInMenu(true);

	return result;
}

void action::initialize(const sigc::slot<void>& Slot)
{
	connect(this, SIGNAL(triggered(bool)), this, SLOT(on_triggered(bool)));
	triggered_signal.connect(sigc::hide(Slot));
}

void action::on_triggered(bool checked)
{
	triggered_signal.emit(checked);
}

} // namespace qtui

} // namespace k3d

