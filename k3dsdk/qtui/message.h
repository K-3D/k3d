#ifndef K3DSDK_QTUI_MESSAGE_H
#define K3DSDK_QTUI_MESSAGE_H

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

class QString;

namespace k3d
{

namespace qtui
{

/// Displays an informational message in a modal dialog box
void message(const QString& Message, const QString& SecondaryMessage);
/// Displays a warning message in a modal dialog box
void warning_message(const QString& Message, const QString& SecondaryMessage);
/// Displays an error message in a modal dialog box
void error_message(const QString& Message, const QString& SecondaryMessage);

/*
/// Warns the user before creating an experimental or deprecated plugin, and gives them a chance to cancel.  Returns true iff the user decided to cancel.
bool_t cancel_plugin(iplugin_factory& Factory);
*/

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_MESSAGE_H

