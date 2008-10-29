#ifndef K3DSDK_SNAPPABLE_DETAIL_H
#define K3DSDK_SNAPPABLE_DETAIL_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include "isnappable.h"

namespace k3d
{

/// Provides a boilerplate implementation of k3d::isnappable for objects that can be interactively "snapped"
class snappable_detail :
	public isnappable
{
public:
	virtual ~snappable_detail();

	const snap_sources_t snap_sources();
	const snap_targets_t snap_targets();

	void add_snap_source(isnap_source* const SnapSource);
	void add_snap_target(isnap_target* const SnapTarget);
	
	void save(xml::element& Element, const ipersistent::save_context& Context);
	void load(xml::element& Element, const ipersistent::load_context& Context);

private:
	snap_sources_t m_snap_sources;
	snap_targets_t m_snap_targets;
};

} // namespace k3d

#endif // !K3DSDK_SNAPPABLE_DETAIL_H

