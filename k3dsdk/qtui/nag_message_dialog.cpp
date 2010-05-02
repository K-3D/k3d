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

#include <k3dsdk/batch_mode.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/nag_message_dialog.h>
#include <k3dsdk/qtui/options.h>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// nag_message_dialog

nag_message_dialog::nag_message_dialog(const QString& Message, const QString& SecondaryMessage)
{
	ui.setupUi(this);
	ui.icon->setPixmap(QIcon::fromTheme("dialog-information").pixmap(48));
	ui.message->setText(Message);
	ui.secondaryMessage->setText(SecondaryMessage);
}

const bool_t nag_message_dialog::show_message() const
{
	return ui.showMessage->isChecked();
}

void nag_message_dialog::nag(const string_t& Type, const ustring& Message, const ustring& SecondaryMessage)
{
	if(batch_mode())
		return;

	if(!options::nag(Type))
		return;

	nag_message_dialog dialog(convert<QString>(Message), convert<QString>(SecondaryMessage));
	dialog.exec();
	options::enable_nag(Type, dialog.show_message());
}

} // namespace qtui

} // namespace k3d

