#ifndef K3DSDK_EXPLICIT_SNAP_SOURCE_H
#define K3DSDK_EXPLICIT_SNAP_SOURCE_H

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

#include "isnap_source.h"
#include "point3.h"
#include "vector3.h"
#include <memory>

namespace k3d
{

class explicit_snap_source :
	public isnap_source
{
public:
	explicit_snap_source(const std::string& Label, const k3d::point3& Position);
	explicit_snap_source(const std::string& Label, const k3d::point3& Position, const k3d::vector3& Look, const k3d::vector3& Up);

	void add_group(const std::string& Group);
	
	const std::string label();
	const groups_t groups();
	const point3 source_position();
	bool source_orientation(vector3& SourceLook, vector3& SourceUp);

	std::string m_label;
	k3d::point3 m_position;
	std::auto_ptr<k3d::vector3> m_look;
	std::auto_ptr<k3d::vector3> m_up;
	groups_t m_groups;
};

} // namespace k3d

#endif // K3DSDK_EXPLICIT_SNAP_SOURCE_H

