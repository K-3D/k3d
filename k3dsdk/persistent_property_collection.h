#ifndef K3DSDK_PERSISTENT_PROPERTY_COLLECTION_H
#define K3DSDK_PERSISTENT_PROPERTY_COLLECTION_H

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

#include "ipersistent.h"
#include "ipersistent_collection.h"
#include "types.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// persistent_property_collection

/// Automates serialization of properties
class persistent_property_collection :
	public ipersistent_collection
{
public:
	void enable_serialization(const string_t& Name, ipersistent& Property);
	void disable_serialization(ipersistent& Property);
	const std::vector<std::pair<string_t, ipersistent*> > persistent_objects();

protected:
	persistent_property_collection();
	virtual ~persistent_property_collection();

	void save(xml::element& Element, const ipersistent::save_context& Context);
	void load(xml::element& Element, const ipersistent::load_context& Context);

private:
	persistent_property_collection(const persistent_property_collection&);
	persistent_property_collection& operator=(const persistent_property_collection&);

	typedef std::vector<string_t> names_t;
	typedef std::vector<ipersistent*> properties_t;

	names_t m_names;
	properties_t m_properties;
};

} // namespace k3d

#endif // !K3DSDK_PERSISTENT_PROPERTY_COLLECTION_H

