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
		\brief Declares the application_implementation class, which encapsulates the running K-3D program
		\author Tim Shead (tshead@k-3d.com)
*/

#include "application_detail.h"
#include "data.h"
#include "document.h"
#include "iapplication_plugin_factory.h"
#include "ideletable.h"
#include "idocument.h"
#include "iscript_engine.h"
#include "plugins.h"
#include "result.h"
#include "signal_accumulators.h"
#include "state_change_set.h"

#include <fstream>
#include <iostream>

#ifdef	interface
#undef	interface
#endif	// interface

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// application_implementation::implementation

/// Encapsulates the running K-3D server application
class application_implementation::implementation :
	public k3d::iapplication,
	public k3d::ideletable
{
public:
	implementation(k3d::iplugin_factory_collection& Plugins) :
		m_plugins(Plugins)
	{
	}

	~implementation()
	{
		// Notify observers that we're going away ...
		m_close_signal.emit();

		// Delete any remaining documents ...
		for(document_list_t::iterator document = m_documents.begin(); document != m_documents.end(); document = m_documents.begin())
			close_document(**document);
	}

	bool exit()
	{
		return m_exit_signal.emit();
	}

	k3d::idocument* create_document()
	{
		// Create a new doc ...
		k3d::idocument* const document = k3d::create_document();
		return_val_if_fail(document, 0);

		// Add it to the doc list ...
		m_documents.push_back(document);

		return document;
	}

	void close_document(k3d::idocument& Document)
	{
		// Find the document iterator ...
		const document_list_t::iterator document = std::find(m_documents.begin(), m_documents.end(), &Document);

		// Sanity checks ...
		return_if_fail(document != m_documents.end());

		// Notify observers that the document will be closed
		m_close_document_signal.emit(Document);

		// Remove it from the document list ...
		m_documents.erase(document);

		// Delete the document ...
		k3d::close_document(Document);
	}

	const k3d::iapplication::document_collection_t documents()
	{
		document_collection_t results;
		std::copy(m_documents.begin(), m_documents.end(), std::back_inserter(results));

		return results;
	}

	k3d::iapplication::startup_message_signal_t& startup_message_signal()
	{
		return m_startup_message_signal;
	}

	sigc::connection connect_close_signal(const sigc::slot<void>& Slot)
	{
		return m_close_signal.connect(Slot);
	}

	sigc::connection connect_close_document_signal(const sigc::slot<void, idocument&>& Slot)
	{
		return m_close_document_signal.connect(Slot);
	}

	const k3d::iplugin_factory_collection::factories_t& plugins()
	{
		return m_plugins.factories();
	}

	/// Stores a reference to the collection of available plugin factories
	k3d::iplugin_factory_collection& m_plugins;
	/// Stores a collection of open documents
	typedef std::list<k3d::idocument*> document_list_t;
	/// Stores the collection of open documents
	document_list_t m_documents;
	/// Signal emitted to display progress during application startup
	startup_message_signal_t m_startup_message_signal;
	/// Signal emitted when the application is closing
	sigc::signal<void> m_close_signal;
	/// Signal emitted when an open document is closed
	sigc::signal<void, idocument&> m_close_document_signal;
	/// Signal emitted to request application close
	sigc::signal0<bool, signal::cancelable> m_exit_signal;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// application_implementation

application_implementation::application_implementation(iplugin_factory_collection& PluginFactories) :
	m_implementation(new implementation(PluginFactories))
{
}

application_implementation::~application_implementation()
{
	delete m_implementation;
}

iapplication& application_implementation::interface()
{
	return *m_implementation;
}

sigc::connection application_implementation::connect_exit_signal(const sigc::slot<bool>& Slot)
{
	return m_implementation->m_exit_signal.connect(Slot);
}

} // namespace k3d

