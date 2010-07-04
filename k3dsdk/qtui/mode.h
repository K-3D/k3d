#ifndef K3DSDK_QTUI_MODE_H
#define K3DSDK_QTUI_MODE_H

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

class QGraphicsScene;

namespace k3d
{

class idocument;

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// mode

/// Abstract base class for Modes - objects that control user-interaction by
/// combining rendering, interaction, and business logic in one place.
class mode :
	public QObject
{
	Q_OBJECT;
public:
	virtual ~mode();

	virtual void enable(idocument& Document, QGraphicsScene& Scene) = 0;

protected:
	mode();
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_MODE_H

