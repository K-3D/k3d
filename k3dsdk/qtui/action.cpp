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

#include <k3dsdk/qtui/action.h>

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

