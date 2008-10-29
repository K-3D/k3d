#ifndef K3DSDK_OPTIONS_POLICY_H
#define K3DSDK_OPTIONS_POLICY_H

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
		\brief Declares k3d::istorage, an abstract interface for global user option storage (could be a file, could be gconf, could be the Windoze registry, etc)
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

namespace k3d
{

// Forward declarations
namespace xml { class element; }

namespace options
{

/// Abstract interface for global user option storage
class istorage :
	public virtual iunknown
{
public:
	/// Returns the stored user option data
	virtual xml::element& tree() = 0;
	/// Commits any modifications to the stored user option data to persistent storage
	virtual void commit() = 0;

protected:
	istorage() {}
	istorage(const istorage&) {}
	istorage& operator = (const istorage&) { return *this; }
	virtual ~istorage() {}
};

/// Trivial implementation of k3d::istorage that serializes data to an XML file
class file_storage :
	public istorage
{
public:
	file_storage(const filesystem::path& File);
	~file_storage();

	xml::element& tree();
	void commit();

private:
	class implementation;
	implementation* const m_implementation;
};

/// Sets the istorage implementation to be used for storing user options ... this must be called once at startup before any other sdk calls are made
void set_storage(istorage& OptionStorage);

} // namespace options

} // namespace k3d

#endif // !K3DSDK_OPTIONS_POLICY_H

