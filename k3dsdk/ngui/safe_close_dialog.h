#ifndef K3DSDK_NGUI_SAFE_CLOSE_DIALOG_H
#define K3DSDK_NGUI_SAFE_CLOSE_DIALOG_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include <k3dsdk/types.h>

#include <string>
#include <vector>

namespace Gtk { class Window; }

namespace k3d
{

namespace ngui
{

class unsaved_document;
	
/// Provides a standard "safe close" dialog for closing windows with unsaved data.
namespace safe_close_dialog
{

/// Encapsulates an entry in the safe close dialog
class entry
{
public:
	entry() :
		document(0),
		save(true)
	{
	}
	
	entry(unsaved_document* Document) :
		document(Document),
		save(true)
	{
	}

	unsaved_document* document;
	bool_t save;
};

/// Defines a collection of entries in the safe close dialog
typedef std::vector<entry> entries_t;
	
/** Prompts the user with a modal dialog and returns returns Gtk::RESPONSE_NONE (cancel closing),
 * Gtk::RESPONSE_CLOSE (close without saving), Gtk::RESPONSE_CANCEL (cancel closing), or
 * Gtk::RESPONSE_OK (save before closing)
 */
	
int run(Gtk::Window& Parent, const string_t& Title);

/** Prompts the user with a modal dialog and returns returns Gtk::RESPONSE_NONE (cancel closing),
 * Gtk::RESPONSE_CLOSE (close without saving), Gtk::RESPONSE_CANCEL (cancel closing), or
 * Gtk::RESPONSE_OK (save before closing)
 */
	
int run(Gtk::Window& Parent, entries_t& UnsavedDocuments);

} // namespace safe_close_dialog

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_SAFE_CLOSE_DIALOG_H

