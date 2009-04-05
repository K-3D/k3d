#ifndef K3DSDK_NGUI_UNSAVED_DOCUMENT_H
#define K3DSDK_NGUI_UNSAVED_DOCUMENT_H

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

#include <k3dsdk/types.h>

namespace k3d
{

namespace ngui
{

//////////////////////////////////////////////////////////////////////////////////////////
// unsaved_document

/// Abstract interface class for UI components that may encapsulate an unsaved document
class unsaved_document
{
public:
	/// Returns true iff there are unsaved changes to the underlying document.
	virtual const bool_t unsaved_changes() = 0;
	/// Return a human-readable title for the underlying document.
	virtual const string_t unsaved_document_title() = 0;
	/// Save the underlying document, returning true if it was saved successfully.  Implementations may need to the prompt the user for a filename, and should return false if the user cancels file selection.
	virtual const bool_t save_unsaved_changes() = 0;

protected:
	unsaved_document() {}
	unsaved_document(const unsaved_document&) {}
	unsaved_document& operator=(const unsaved_document&) { return *this; }
	virtual ~unsaved_document() {}
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_UNSAVED_DOCUMENT_H	

