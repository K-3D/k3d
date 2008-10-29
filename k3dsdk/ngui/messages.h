#ifndef K3DSDK_NGUI_MESSAGES_H
#define K3DSDK_NGUI_MESSAGES_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include <k3dsdk/ustring.h>

#include <string>
#include <vector>

namespace libk3dngui
{

/// Displays an informational message in a modal dialog box
void message(const std::string& Message, const std::string& SecondaryMessage = "");
/// Displays a warning message in a modal dialog box
void warning_message(const std::string& Message, const std::string& SecondaryMessage = "");
/// Displays an error message in a modal dialog box
void error_message(const std::string& Message, const std::string& SecondaryMessage = "");
/**
	 \brief Prompts the user to choose an option from a modal dialog box
	 \param Message text displayed by the message box.
	 \param DefaultOption one-based index of the option that is selected by default.  If DefaultOption is 0, then no option is selected by default.
	 \return one-based index of the option selected by the user, or "0" if a choice was not made (e.g. user clicked WM "close" button)
*/
unsigned int query_message(const std::string& Message, const unsigned int DefaultOption, const std::vector<std::string>& Options);
/// Displays an information "nag" message in a modal dialog box
void nag_message(const std::string& Type, const k3d::ustring& Message, const k3d::ustring& SecondaryMessage = k3d::ustring());

} // namespace libk3dngui

#endif // !K3DSDK_NGUI_MESSAGES_H

