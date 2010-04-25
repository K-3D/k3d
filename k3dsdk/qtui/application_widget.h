#ifndef K3DSDK_QTUI_APPLICATION_WIDGET_H
#define K3DSDK_QTUI_APPLICATION_WIDGET_H

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

#include <QObject>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// application_widget

/// Provides a standard base-class for dialogs that should be closed automatically if the application is closed.
class application_widget :
	public QObject
{
	Q_OBJECT;

	typedef QObject base;

public:
	application_widget(QWidget& owner);

private Q_SLOTS:
	/// Called when the application is about to close.
	void application_closing();

private:
	QWidget& owner;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_APPLICATION_WIDGET_H

