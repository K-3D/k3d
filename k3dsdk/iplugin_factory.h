#ifndef K3DSDK_IPLUGIN_FACTORY_H
#define K3DSDK_IPLUGIN_FACTORY_H

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
	\brief Declares iplugin_factory, an abstract factory interface for plugin objects
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "uuid.h"

#include <iosfwd>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

namespace k3d
{

/// Abstract factory interface for plugin objects
class iplugin_factory :
	public virtual iunknown
{
public:
	/// Marks a plugin "stable", "experimental", or "deprecated", typically used to provide appropriate warnings in the UI
	typedef enum
	{
		STABLE,
		EXPERIMENTAL,
		DEPRECATED
	} quality_t;

	/// Defines a collection of "categories" used to group plugins within the UI
	typedef std::vector<std::string> categories_t;
	/// Defines a collection of interfaces implemented by the underlying plugin instance
	typedef std::vector<const std::type_info*> interfaces_t;
	/// Defines an arbitrary collection of name-value pair metadata describing the underlying plugin instance
	typedef std::map<std::string, std::string> metadata_t;

	/// Returns a guaranteed-unique factory ID
	virtual const k3d::uuid& factory_id() = 0;
	/// Returns the internal name for a plugin
	virtual const std::string name() = 0;
	/// Returns a short, human-readable description of the plugin
	virtual const std::string short_description() = 0;
	/// Returns a default category for ordering this plugin within the UI
	virtual const categories_t& categories() = 0;
	/// Returns the plugin quality (stable or experimental)
	virtual quality_t quality() = 0;
	/// Returns true iff a plugin implements the requested interface
	virtual bool implements(const std::type_info& InterfaceType) = 0;
	/// Returns the set of interfaces implemented by the plugin 
	virtual const interfaces_t interfaces() = 0;
	/// Returns metadata describing the plugin
	virtual metadata_t metadata() = 0;

protected:
	iplugin_factory() {}
	iplugin_factory(const iplugin_factory&) {}
	iplugin_factory& operator = (const iplugin_factory&) { return *this; }
	virtual ~iplugin_factory() {}
};

std::ostream& operator<<(std::ostream&, const iplugin_factory::quality_t&);
std::istream& operator>>(std::istream&, iplugin_factory::quality_t&);

} // namespace k3d

#endif // K3DSDK_IPLUGIN_FACTORY_H

