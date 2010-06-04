#ifndef K3DSDK_QTUI_NAG_MESSAGE_H
#define K3DSDK_QTUI_NAG_MESSAGE_H

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

#include <ui_nag_message_dialog.h>
#include <k3dsdk/types.h>

namespace k3d
{

class ustring;

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// nag_message_dialog

/// Provides a mixin-class for widgets that should be closed automatically if a document is closed.
class nag_message_dialog :
	public QDialog
{
	Q_OBJECT;

	typedef QDialog base;

public:
	nag_message_dialog(const QString& Message, const QString& SecondaryMessage = "");

	const bool_t show_message() const;

	static void nag(const string_t& Type, const ustring& Message, const ustring& SecondaryMessage);

private:
	Ui::nag_message_dialog ui;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_NAG_MESSAGE_H

