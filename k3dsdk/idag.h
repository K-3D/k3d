#ifndef K3DSDK_IDAG_H
#define K3DSDK_IDAG_H

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

#include "iunknown.h"
#include "signal_system.h"

#include <map>
#include <vector>

namespace k3d
{

// Forward declarations
class iproperty;

/// Abstract interface for a Directed Acyclic Graph of dependencies between document plugin properties
class idag :
	public virtual iunknown
{
public:
	/// Defines a dependency between two properties
	typedef std::pair<iproperty*, iproperty*> dependency_t;
	/// Defines a set of dependencies between properties - the map key is the dependent property and must not be NULL.  The map value is the property it depends upon, and may be NULL.
	typedef std::map<iproperty*, iproperty*> dependencies_t;
	/// Defines a collection of properties
	typedef std::vector<iproperty*> properties_t;

	/// Updates dependencies between a set of properties
	virtual void set_dependencies(dependencies_t& Dependencies) = 0;

	/// Returns the dependency for a given property - may return NULL for no dependency
	virtual iproperty* dependency(iproperty& Target) = 0;

	/// Returns the set of all dependencies between properties
	virtual const dependencies_t& dependencies() = 0;

	/// Defines a signal that will be emitted whenever a set of dependencies are altered
	typedef sigc::signal<void, const dependencies_t&> dependency_signal_t;
	virtual dependency_signal_t& dependency_signal() = 0;

protected:
	idag() {}
	idag(const idag&) {}
	idag& operator=(const idag&) { return *this; }
	virtual ~idag() {}
};



} // namespace k3d

#endif // !K3DSDK_IDAG_H


