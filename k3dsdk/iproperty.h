#ifndef K3DSDK_IPROPERTY_H
#define K3DSDK_IPROPERTY_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
#include "types.h"

#include <boost/any.hpp>

namespace k3d
{

class inode;

/// Abstract interface for a name-value pair with fixed type (note - if you want to write to a property, query for iwritable_property and be prepared for it to fail for read-only properties)
class iproperty :
	public virtual iunknown
{
public:
	virtual ~iproperty() {}

	/// Returns the property name, which will be used as a unique identifier for programmatic access to the property (i.e. through scripting)
	virtual const string_t property_name() = 0;
	/// Returns a human-readable lable for the property, which should be localized for display in UI code
	virtual const string_t property_label() = 0;
	/// Returns a human-readable "one-liner" description of the property, which should be localized for display in the UI (e.g. as a tooltip)
	virtual const string_t property_description() = 0;
	/// Returns the property type
	virtual const std::type_info& property_type() = 0;
	/// Returns the value stored internally by the property.  Note that most code that uses properties should retrieve the pipeline value instead.
	virtual const boost::any property_internal_value() = 0;
	/// Returns the property value, accounting for any connections in the pipeline.  The result may-or-may-not be the same as the internal value.
	virtual const boost::any property_pipeline_value() = 0;
	/// Returns a reference to the object that owns the property (if any)
	virtual inode* property_node() = 0;
	/// Defines a signal that will be emitted if the property value changes.  The signal includes a pointer to an optional "hint" object that may provide additional information about what changed.
	typedef sigc::signal<void, ihint*> changed_signal_t;
	virtual changed_signal_t& property_changed_signal() = 0;
	/// Defines a signal that will be emitted when the property is destroyed
	typedef sigc::signal<void> deleted_signal_t;
	virtual deleted_signal_t& property_deleted_signal() = 0;

	/// Returns this property's pipeline dependency, if any.  Note: there may be dependency cycles, don't use this to perform lookups directly, use k3d::property_lookup() instead
	virtual iproperty* property_dependency() = 0;
	/// Sets this property's pipeline dependency.  Note: never call this directly, use a k3d::pipeline object to manage pipeline dependencies
	virtual void property_set_dependency(iproperty*) = 0;

protected:
	iproperty() {}
	iproperty(const iproperty& Other) : iunknown(Other) {}
	iproperty& operator = (const iproperty&) { return *this; }
};

} // namespace k3d

#endif // !K3DSDK_IPROPERTY_H

