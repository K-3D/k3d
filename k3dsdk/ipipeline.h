#ifndef K3DSDK_IPIPELINE_H
#define K3DSDK_IPIPELINE_H

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

#include "ihint.h"
#include "signal_system.h"

#include <map>
#include <vector>

namespace k3d
{

class iproperty;

/// Abstract interface for maintaining a collection of dependencies among properties ... a "pipeline"
class ipipeline :
	public virtual iunknown
{
public:
	/// Defines a dependency between two properties
	typedef std::pair<iproperty*, iproperty*> dependency_t;
	/// Defines a set of dependencies between properties - the map key is the dependent property and must not be NULL.  The map value is the property it depends upon, and may be NULL.
	typedef std::map<iproperty*, iproperty*> dependencies_t;

	/// Updates dependencies between a set of properties
	virtual void set_dependencies(dependencies_t& Dependencies, ihint* Hint = 0) = 0;

	/// Returns the dependency for a given property - may return NULL for no dependency
	virtual iproperty* dependency(iproperty& Target) = 0;

	/// Returns the set of all dependencies between properties
	virtual const dependencies_t& dependencies() = 0;

	/// Defines a signal that will be emitted whenever a set of dependencies are altered
	typedef sigc::signal<void, const dependencies_t&> dependency_signal_t;
	virtual dependency_signal_t& dependency_signal() = 0;

protected:
	ipipeline() {}
	ipipeline(const ipipeline& Other) : iunknown(Other) {}
	ipipeline& operator=(const ipipeline&) { return *this; }
	virtual ~ipipeline() {}
};

} // namespace k3d

#endif // !K3DSDK_IPIPELINE_H

