#ifndef K3DSDK_IAPPLICATION_H
#define K3DSDK_IAPPLICATION_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "iplugin_factory_collection.h"
#include "iunknown.h"
#include "signal_system.h"

#include <string>
#include <vector>

namespace k3d
{

class idocument;

/// Abstract interface for the global K-3D Application object
class iapplication :
	public virtual iunknown
{
public:
	/// Exit K-3D if safe to do so
	virtual bool exit() = 0;

	/// Creates a new K-3D document (could return NULL)
	virtual idocument* create_document() = 0;
	/// Closes an open K-3D document
	virtual void close_document(idocument& Document) = 0;

	/// A collection of idocument objects
	typedef std::vector<idocument*> document_collection_t;
	/// Returns the collection of open documents
	virtual const document_collection_t documents() = 0;

	/// Returns the collection of available plugin factories
	/** \deprecated Use k3d::plugin::factory::lookup() instead. */
	virtual const iplugin_factory_collection::factories_t& plugins() = 0;

	// Signals

	/// Defines a signal emitted to display progress messages to the user during application startup
	typedef sigc::signal<void, const std::string&> startup_message_signal_t;
	virtual startup_message_signal_t& startup_message_signal() = 0;

	/// Connects a slot to a signal emitted when the application is closing
	virtual sigc::connection connect_close_signal(const sigc::slot<void>& Slot) = 0;

	/// Connects a slot to a signal emitted when an existing document is closed
	virtual sigc::connection connect_close_document_signal(const sigc::slot<void, idocument&>& Slot) = 0;

protected:
	iapplication() {}
	iapplication(const iapplication& Other) : iunknown(Other) {}
	iapplication& operator = (const iapplication&) { return *this; }
	virtual ~iapplication() {}
};

} // namespace k3d

#endif // !K3DSDK_IAPPLICATION_H

