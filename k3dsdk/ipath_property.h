#ifndef K3DSDK_IPATH_PROPERTY_H
#define K3DSDK_IPATH_PROPERTY_H

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
		\brief Declares ipath_property, an abstract interface for discovering information about a property that stores filesystem paths
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <string>
#include <vector>

namespace k3d
{

/// Abstract interface for properties that represent external filesystem resources (typically used to alter presentation)
class ipath_property :
	public virtual iunknown
{
public:
	/// Enumerates the mode of the underlying file
	typedef enum
	{
		READ,
		WRITE,
	} mode_t;

	/// Enumerates how the external resource should be referenced - by absolute path, relative path, or inline
	typedef enum
	{
		ABSOLUTE_REFERENCE,
		RELATIVE_REFERENCE,
		INLINE_REFERENCE
	} reference_t;

	/// Defines a "pattern filter" that the UI layer can use to filter files using GLOB style syntax
	struct pattern_filter
	{
		pattern_filter()
		{
		}

		pattern_filter(const std::string& Name, const std::string& Pattern) :
			name(Name),
			pattern(Pattern)
		{
		}

		/// Stores a human-readable name for the pattern
		std::string name;
		/// Stores s pattern using GLOB syntax
		std::string pattern;
	};

	typedef std::vector<pattern_filter> pattern_filters_t;
	
	/// Returns the path "mode", which indicates whether the path will be used for an input or an output file
	virtual const mode_t property_path_mode() = 0;
	/// Returns the path "type", used to categorize the purpose of the path and store most-recent-used paths
	virtual const std::string property_path_type() = 0;

	/// Returns the path "reference", which indicates how the external resource will be referenced - by absolute path, relative path, or inline
	virtual const reference_t property_path_reference() = 0;
	virtual void set_property_path_reference(const reference_t) = 0;
	/// Defines a signal that will be emitted anytime the path reference is modified
	typedef sigc::signal<void> path_reference_changed_signal_t;
	virtual path_reference_changed_signal_t& property_path_reference_changed_signal() = 0;

	/// Returns a collection of pattern filters to be (optionally) used by the UI when prompting the user for a file
	virtual const pattern_filters_t pattern_filters() = 0;
	
protected:
	ipath_property() {}
	ipath_property(const ipath_property&) {}
	ipath_property& operator=(const ipath_property&) { return *this; }
	virtual ~ipath_property() {}
};

std::ostream& operator<<(std::ostream&, const ipath_property::reference_t&);
std::istream& operator>>(std::istream&, ipath_property::reference_t&);

} // namespace k3d

#endif // !K3DSDK_IPATH_PROPERTY_H

