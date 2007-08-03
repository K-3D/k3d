// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "iproperty.h"
#include "istate_container.h"
#include "istate_recorder.h"
#include "log.h"
#include "pipeline.h"
#include "result.h"
#include "signal_slots.h"
#include "state_change_set.h"

#include <cassert>
#include <iostream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// pipeline::implementation

class pipeline::implementation
{
public:
	implementation(istate_recorder* StateRecorder) :
		state_recorder(StateRecorder)
	{
	}

	~implementation()
	{
		clear();
	}

	void set_dependencies(dependencies_t& Dependencies, iunknown* Hint = 0)
	{
		// Don't let any NULLs creep in ...
		if(Dependencies.erase(static_cast<iproperty*>(0)))
			log() << warning << "Cannot assign a dependency to a NULL property" << std::endl;

		// If we're recording undo/redo data, record the new state ...
		if(state_recorder && state_recorder->current_change_set())
			state_recorder->current_change_set()->record_new_state(new set_dependencies_container(*this, Dependencies));

		// Update our internal graph, keep track of the original state as we go ...
		dependencies_t old_dependencies;
		for(dependencies_t::iterator dependency = Dependencies.begin(); dependency != Dependencies.end(); ++dependency)
		{
			dependencies_t::iterator old_dependency = get_dependency(dependency->first);
			old_dependencies.insert(*old_dependency);

			old_dependency->second = dependency->second;

			m_change_connections[dependency->first].disconnect();
			if(dependency->second)
			{
				m_change_connections[dependency->first] =
					dependency->second->property_changed_signal().connect(
						signal::make_loop_safe_slot(
							dependency->first->property_changed_signal()));
			}

			dependency->first->property_set_dependency(dependency->second);
		}

		// If we're recording undo/redo data, keep track of the original state ...
		if(state_recorder && state_recorder->current_change_set())
			state_recorder->current_change_set()->record_old_state(new set_dependencies_container(*this, old_dependencies));

		// Notify observers that the pipeline has changed ...
		changed_signal.emit(Dependencies);

		// Synthesize change notifications for every property whose parent was set ...
		for(dependencies_t::iterator dependency = Dependencies.begin(); dependency != Dependencies.end(); ++dependency)
			dependency->first->property_changed_signal().emit(Hint);
	}

	void clear()
	{
		for(connections_t::iterator connection = m_change_connections.begin(); connection != m_change_connections.end(); ++connection)
			connection->second.disconnect();
		m_change_connections.clear();

		for(connections_t::iterator connection = m_delete_connections.begin(); connection != m_delete_connections.end(); ++connection)
			connection->second.disconnect();
		m_delete_connections.clear();

		dependencies.clear();
	}

	void on_property_deleted(iproperty* Property)
	{
		dependencies_t::iterator dependency = dependencies.find(Property);
		return_if_fail(dependency != dependencies.end());

		if(state_recorder && state_recorder->current_change_set())
		{
			dependencies_t old_dependencies;
			old_dependencies.insert(*dependency);
			state_recorder->current_change_set()->record_old_state(new set_dependencies_container(*this, old_dependencies));
			state_recorder->current_change_set()->record_new_state(new delete_property_container(*this, Property));
		}

		dependencies.erase(dependency);

		m_delete_connections[Property].disconnect();
		m_delete_connections.erase(Property);

		dependencies_t new_dependencies;
		for(dependencies_t::iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
		{
			if(dependency->second == Property)
			{
				dependency->first->property_set_dependency(0);
				new_dependencies.insert(std::make_pair(dependency->first, static_cast<iproperty*>(0)));
			}
		}

		if(new_dependencies.size())
			set_dependencies(new_dependencies);
	}

	dependencies_t::iterator get_dependency(iproperty* Property)
	{
		assert(Property);

		dependencies_t::iterator result = dependencies.find(Property);
		if(result == dependencies.end())
		{
			result = dependencies.insert(std::make_pair(Property, static_cast<iproperty*>(0))).first;
			m_delete_connections[Property] = Property->property_deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &implementation::on_property_deleted), Property));
		}

		return result;
	}

	/// Used to implement undo/redo state changes when pipeline connections are modified
	class set_dependencies_container :
		public istate_container
	{
	public:
		set_dependencies_container(pipeline::implementation& Pipeline, const ipipeline::dependencies_t& Dependencies) :
			m_pipeline(Pipeline),
			m_dependencies(Dependencies)
		{
		}

		~set_dependencies_container()
		{
		}

		void restore_state()
		{
			m_pipeline.set_dependencies(m_dependencies);
		}

	private:
		pipeline::implementation& m_pipeline;
		ipipeline::dependencies_t m_dependencies;
	};

	/// Used to implement undo/redo state changes when a pipeline property has been deleted
	class delete_property_container :
		public istate_container
	{
	public:
		delete_property_container(pipeline::implementation& Pipeline, iproperty* const Property) :
			m_pipeline(Pipeline),
			m_property(Property)
		{
		}

		~delete_property_container()
		{
		}

		void restore_state()
		{
			m_pipeline.on_property_deleted(m_property);
		}

	private:
		pipeline::implementation& m_pipeline;
		iproperty* const m_property;
	};

	/// Optionally records changes to the pipeline for undo/redo
	istate_recorder* const state_recorder;
	/// Stores the set of all property dependencies
	ipipeline::dependencies_t dependencies;
	/// Defines storage for per-property signal connections
	typedef std::map<iproperty*, sigc::connection> connections_t;
	/// Stores connections between property change signals (so a change to a source property is automatically passed-along to its dependent properties)
	connections_t m_change_connections;
	/// Stores connections to property delete signals (so we can clean-up when a property goes away)
	connections_t m_delete_connections;
	/// Signal that is emitted whenever the pipeline is modified
	ipipeline::dependency_signal_t changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// pipeline

pipeline::pipeline(istate_recorder* StateRecorder) :
	m_implementation(new implementation(StateRecorder))
{
}

pipeline::~pipeline()
{
	delete m_implementation;
}

void pipeline::set_dependencies(dependencies_t& Dependencies, iunknown* Hint)
{
	m_implementation->set_dependencies(Dependencies, Hint);
}

iproperty* pipeline::dependency(iproperty& Property)
{
	return Property.property_dependency();
}

const ipipeline::dependencies_t& pipeline::dependencies()
{
	return m_implementation->dependencies;
}

ipipeline::dependency_signal_t& pipeline::dependency_signal()
{
	return m_implementation->changed_signal;
}

void pipeline::clear()
{
	m_implementation->clear();
}

void pipeline::connect(iproperty& From, iproperty& To)
{
	dependencies_t dependencies;
	dependencies.insert(std::make_pair(&To, &From));
	set_dependencies(dependencies);
}

} // namespace k3d

