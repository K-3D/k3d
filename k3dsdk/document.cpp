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
		\brief Provides a concrete implementation of idocument, which encapsulates an open K-3D document
		\author Tim Shead (tshead@k-3d.com)
		\author Dan Erikson (derikson@montana.com)
*/

#include "k3d-i18n-config.h"
#include "k3d-version-config.h"

#include "application.h"
#include "classes.h"
#include "command_node.h"
#include "data.h"
#include "dependencies.h"
#include "document.h"
#include "ideletable.h"
#include "idocument.h"
#include "idocument_plugin_factory.h"
#include "imaterial.h"
#include "inode.h"
#include "inode_collection.h"
#include "iplugin_factory.h"
#include "iselectable.h"
#include "node_name_map.h"
#include "persistent.h"
#include "pipeline.h"
#include "pipeline_profiler.h"
#include "plugins.h"
#include "property_collection.h"
#include "signal_slots.h"
#include "string_cast.h"
#include "string_modifiers.h"
#include "utility.h"
#include "xml.h"
using namespace k3d::xml;

#include <sigc++/bind.h>

#include "fstream.h"

#include <fstream>
#include <iterator>
#include <memory>

namespace k3d
{

namespace detail
{
	
/////////////////////////////////////////////////////////////////////////////
// state_recorder_implementation

/// Provides a concrete implementation of istate_recorder
class state_recorder_implementation :
	public istate_recorder
{
public:
	state_recorder_implementation() :
		m_current_node(0),
		m_newest_node(0),
		m_last_saved_node(0)
	{
	}

	~state_recorder_implementation()
	{
		// Delete all nodes ...
		for(nodes_t::iterator node = m_root_nodes.begin(); node != m_root_nodes.end(); ++node)
			delete_node(*node);

		if(m_current_recording.get())
			log() << warning << "Still recording undo/redo data at shutdown, this indicates an undo/redo leak" << std::endl;
	}

	void delete_node(node* const Node)
	{
		if(!Node)
			return;

		for(nodes_t::iterator node = Node->children.begin(); node != Node->children.end(); ++node)
			delete_node(*node);
		delete Node;
	}

	void start_recording(std::auto_ptr<state_change_set> ChangeSet, const char* const Context)
	{
		if(!ChangeSet.get())
		{
			log() << error << "start_recording() attempt with NULL changeset.  Context: " << Context << std::endl;
			return;
		}
		
		if(m_current_recording.get())
		{
			log() << warning << "Forcing termination of unfinished changeset.  Context: " << m_current_context << std::endl;
			std::auto_ptr<state_change_set> changeset = stop_recording(Context);
			commit_change_set(changeset, "Unfinished changeset", Context);
		}

		m_current_recording = ChangeSet;
		m_current_context = Context;
	}

	state_change_set* current_change_set()
	{
		return m_current_recording.get();
	}

	std::auto_ptr<state_change_set> stop_recording(const char* const Context)
	{
		if(!m_current_recording.get())
		{
			log() << error << "stop_recording() attempt with NULL changeset.  Context: " << Context << std::endl;
			return m_current_recording;
		}
		
		// Let the world know that recording is finished ...
		m_recording_done_signal.emit();

		// Now that recording is complete, get rid of leftover connections ...
		sigc::signal<void>::slot_list_type slots = m_recording_done_signal.slots();
		slots.erase(slots.begin(), slots.end());

		return m_current_recording;
	}

	void commit_change_set(std::auto_ptr<state_change_set> ChangeSet, const std::string& Label, const char* const Context)
	{
		if(!ChangeSet.get())
		{
			log() << error << "commit_change_set() attempt with NULL changeset.  Context: " << Context << std::endl;
			return;
		}

		if(!ChangeSet->undo_count() && !ChangeSet->redo_count())
		{
			log() << error << "discarding empty changeset [" << Label << "].  Context: " << Context << std::endl;
			return;
		}
		
		std::string label = Label;
		if(label.empty())
		{
			log() << warning << "committing anonymous changeset.  Context: " << Context << std::endl;
			label = "Unnamed changeset";
		}

		// Create a new node in the hierarchy, branching it from the current node if necessary ...
		m_newest_node = new node(label, ChangeSet.release(), m_current_node);

		if(m_current_node)
			m_current_node->children.push_back(m_newest_node);
		else
			m_root_nodes.push_back(m_newest_node);

		m_current_node = m_newest_node;

		m_node_added_signal.emit(m_newest_node);
		m_current_node_changed_signal.emit();
	}

	const nodes_t& root_nodes()
	{
		return m_root_nodes;
	}

	const node* current_node()
	{
		return m_current_node;
	}

	const node* newest_node()
	{
		return m_newest_node;
	}

	const node* last_saved_node()
	{
		return m_last_saved_node;
	}

	void mark_saved()
	{
		m_last_saved_node = m_current_node;
		m_last_saved_node_changed_signal.emit();
	}

	void set_current_node(const node* const Node)
	{
		m_current_node = const_cast<node*>(Node);
		m_current_node_changed_signal.emit();
	}

	sigc::connection connect_recording_done_signal(const sigc::slot<void>& Slot)
	{
		return m_recording_done_signal.connect(Slot);
	}
	
	sigc::connection connect_node_added_signal(const sigc::slot<void, const node*>& Slot)
	{
		return m_node_added_signal.connect(Slot);
	}
	
	sigc::connection connect_current_node_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_current_node_changed_signal.connect(Slot);
	}
	
	sigc::connection connect_last_saved_node_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_last_saved_node_changed_signal.connect(Slot);
	}

private:
	/// Stores the current change set
	std::auto_ptr<state_change_set> m_current_recording;
	/// Stores the context for the current change set
	const char* m_current_context;
	/// Stores the root node(s) (if any)
	nodes_t m_root_nodes;
	/// Stores a reference to the current node (if any)
	node* m_current_node;
	/// Stores a reference to the most-recently-added node (if any)
	node* m_newest_node;
	/// Stores a reference to the most-recently-saved node (if any)
	node* m_last_saved_node;

	sigc::signal<void> m_recording_done_signal;
	sigc::signal<void, const node*> m_node_added_signal;
	sigc::signal<void> m_current_node_changed_signal;
	sigc::signal<void> m_last_saved_node_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// node_collection_implementation

class node_collection_implementation :
	public inode_collection
{
public:
	node_collection_implementation(istate_recorder& StateRecorder) :
		m_state_recorder(StateRecorder)
	{
	}

	~node_collection_implementation()
	{
	}

	void add_nodes(const nodes_t& Nodes)
	{
		// Ensure no NULLs creep in ...
		nodes_t nodes(Nodes);
		nodes.erase(std::remove(nodes.begin(), nodes.end(), static_cast<inode*>(0)), nodes.end());
		if(nodes.size() != Nodes.size())
			log() << warning << "NULL node cannot be inserted into node collection and will be ignored" << std::endl;

		// We want to emit a signal whenever an node's name changes ...
		for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			(*node)->name_changed_signal().connect(sigc::bind(m_rename_node_signal.make_slot(), *node));

		// If we're recording undo/redo data, record the new state ...
		if(m_state_recorder.current_change_set())
		{
			m_state_recorder.current_change_set()->record_old_state(new remove_nodes_container(*this, nodes));
			m_state_recorder.current_change_set()->record_new_state(new add_nodes_container(*this, nodes));
		}

		// Make the change and notify observers ...
		m_nodes.insert(m_nodes.end(), nodes.begin(), nodes.end());
		m_add_nodes_signal.emit(nodes);
	}

	const inode_collection::nodes_t& collection()
	{
		return m_nodes;
	}

	void remove_nodes(const nodes_t& Nodes)
	{
		// Ensure no NULLs creep in ...
		nodes_t nodes(Nodes);
		nodes.erase(std::remove(nodes.begin(), nodes.end(), static_cast<inode*>(0)), nodes.end());
		if(nodes.size() != Nodes.size())
			log() << warning << "NULL node will be ignored" << std::endl;

		// If we're recording undo/redo data, record the new state ...
		if(m_state_recorder.current_change_set())
		{
			m_state_recorder.current_change_set()->record_old_state(new add_nodes_container(*this, nodes));
			m_state_recorder.current_change_set()->record_new_state(new remove_nodes_container(*this, nodes));
		}

		// Make the change and notify observers ...
		for(nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			(*node)->deleted_signal().emit();
			m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), *node), m_nodes.end());
		}
		m_remove_nodes_signal.emit(nodes);

	}

	add_nodes_signal_t& add_nodes_signal()
	{
		return m_add_nodes_signal;
	}

	remove_nodes_signal_t& remove_nodes_signal()
	{
		return m_remove_nodes_signal;
	}

	rename_node_signal_t& rename_node_signal()
	{
		return m_rename_node_signal;
	}

	void on_close_document()
	{
		// Give nodes a chance to shut down ...
		for(inode_collection::nodes_t::iterator node = m_nodes.begin(); node != m_nodes.end(); ++node)
		{
			(*node)->deleted_signal().emit();
		}

		// Zap nodes ...
		for(inode_collection::nodes_t::iterator node = m_nodes.begin(); node != m_nodes.end(); ++node)
			delete dynamic_cast<ideletable*>(*node);
	}

private:
	class add_nodes_container :
		public istate_container
	{
	public:
		add_nodes_container(inode_collection& Collection, const inode_collection::nodes_t& Nodes) :
			m_collection(Collection),
			m_nodes(Nodes)
		{
		}

		~add_nodes_container()
		{
		}

		void restore_state()
		{
			m_collection.add_nodes(m_nodes);
		}

	private:
		inode_collection& m_collection;
		const inode_collection::nodes_t m_nodes;
	};

	class remove_nodes_container :
		public istate_container
	{
	public:
		remove_nodes_container(inode_collection& Collection, const inode_collection::nodes_t& Nodes) :
			m_collection(Collection),
			m_nodes(Nodes)
		{
		}

		~remove_nodes_container()
		{
		}

		void restore_state()
		{
			m_collection.remove_nodes(m_nodes);
		}

	private:
		inode_collection& m_collection;
		const inode_collection::nodes_t m_nodes;
	};

	/// Provides storage for undo/redo information
	istate_recorder& m_state_recorder;
	/// Provides undo-able storage for a collection of nodes
	inode_collection::nodes_t m_nodes;
	/// Signal for notifying observers when nodes are added to the collection
	add_nodes_signal_t m_add_nodes_signal;
	/// Signal for notifying observers when nodes are removed from the collection
	remove_nodes_signal_t m_remove_nodes_signal;
	/// Signal for notifying observers when an node's name is changed
	rename_node_signal_t m_rename_node_signal;
};

/////////////////////////////////////////////////////////////////////////////
// public_document_implementation

/// Encapsulates an open K-3D document
class public_document_implementation :
	public idocument,
	public command_node::implementation,
	public property_collection,
	public sigc::trackable
{
public:
	public_document_implementation(istate_recorder& StateRecorder, inode_collection& Nodes, ipipeline& Pipeline) :
		command_node::implementation("document", 0),
		property_collection(),
		m_state_recorder(StateRecorder),
		m_nodes(Nodes),
		m_pipeline(Pipeline),
		m_path(init_owner(*this) + init_name("path") + init_label(_("Document Path")) + init_description(_("Document Path")) + init_value(filesystem::path())),
		m_title(init_owner(*this) + init_name("title") + init_label(_("Document Title")) + init_description(_("Document Title")) + init_value(k3d::ustring()))
	{
 		// Automatically add nodes to the unique node name collection
 		m_nodes.add_nodes_signal().connect(sigc::mem_fun(m_unique_node_names, &node_name_map::add_nodes));
	}

	~public_document_implementation()
	{
		// Note: our close signal gets called by our owner - yes, it's a hack
	}

	idocument& document()
	{
		return *this;
	}

	inode_collection& nodes()
	{
		return m_nodes;
	}

	ipipeline& pipeline()
	{
		return m_pipeline;
	}

	ipipeline_profiler& pipeline_profiler()
	{
		return m_pipeline_profiler;
	}
	
	istate_recorder& state_recorder()
	{
		return m_state_recorder;
	}

 	inode_name_map& unique_node_names()
 	{
 		return m_unique_node_names;
 	}

	iproperty& path()
	{
		return m_path;
	}

	iproperty& title()
	{
		return m_title;
	}

	idocument::close_signal_t& close_signal()
	{
		return m_close_signal;
	}

private:
	/// Notifies observers that the document is being closed
	close_signal_t m_close_signal;

	/// Records changes made by the user for Undo / Redo purposes
	istate_recorder& m_state_recorder;
	/// Stores document nodes ...
	inode_collection& m_nodes;
	/// Stores a reference to an implementation of ipipeline
	ipipeline& m_pipeline;
	/// Stores a pipeline profiler object
	k3d::pipeline_profiler m_pipeline_profiler;
 	/// Stores unique node names
 	node_name_map m_unique_node_names;

	/// Stores the full document filepath (if any)
	k3d_data(filesystem::path, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_path;
	/// Stores the document title (if any)
	k3d_data(ustring, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_title;
};

/// This is a real abortion, but it solves our interdependency problems among state recorder, pipeline, and property collection implementations
class document_implementation
{
public:
	document_implementation() :
		m_state_recorder(new state_recorder_implementation()),
		m_nodes(new node_collection_implementation(*m_state_recorder)),
		m_pipeline(new pipeline(m_state_recorder)),
		m_document(new public_document_implementation(*m_state_recorder, *m_nodes, *m_pipeline))
	{
	}

	~document_implementation()
	{
		m_document->close_signal().emit();

		// Completely remove all pipeline connections so we don't waste time updating individual properties
		m_pipeline->clear();
		m_nodes->on_close_document();

		delete m_document;
		delete m_pipeline;
		delete m_nodes;
		delete m_state_recorder;
	}

	state_recorder_implementation* const m_state_recorder;
	node_collection_implementation* const m_nodes;
	pipeline* const m_pipeline;
	public_document_implementation* const m_document;

private:
	document_implementation(const document_implementation&);
	document_implementation& operator=(const document_implementation&);
};

typedef std::vector<document_implementation*> documents_t;

documents_t& documents()
{
	static documents_t g_documents;
	return g_documents;
}

} // namespace detail

idocument* create_document()
{
	std::auto_ptr<detail::document_implementation> document(new detail::document_implementation());
	detail::documents().push_back(document.get());
	return document.release()->m_document;
}

void close_document(idocument& Document)
{
	for(detail::documents_t::iterator document = detail::documents().begin(); document != detail::documents().end(); ++document)
	{
		if((*document)->m_document == &Document)
		{
			delete *document;
			detail::documents().erase(document);

			return;
		}
	}

	log() << error << "close_document(): could not find document to destroy" << std::endl;
}

} // namespace k3d

