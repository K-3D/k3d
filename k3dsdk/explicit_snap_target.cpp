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

#include "explicit_snap_target.h"
#include "result.h"

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////////
// explicit_snap_target

explicit_snap_target::explicit_snap_target(const std::string& Label, const k3d::point3& Position) :
	m_label(Label),
	m_position(Position)
{
}

explicit_snap_target::explicit_snap_target(const std::string& Label, const k3d::point3& Position, const k3d::vector3& Look, const k3d::vector3& Up) :
	m_label(Label),
	m_position(Position),
	m_look(new k3d::vector3(Look)),
	m_up(new k3d::vector3(Up))
{
}

void explicit_snap_target::add_group(const std::string& Group)
{
	return_if_fail(Group.size());
	return_if_fail(std::find(m_groups.begin(), m_groups.end(), Group) == m_groups.end());

	m_groups.push_back(Group);
}

const std::string explicit_snap_target::label()
{
	return m_label;
}

const isnap_target::groups_t explicit_snap_target::groups()
{
	return m_groups;
}

bool explicit_snap_target::target_position(const point3& Position, point3& TargetPosition)
{
	TargetPosition = m_position;
	return true;
}

bool explicit_snap_target::target_orientation(const point3& Position, vector3& TargetLook, vector3& TargetUp)
{
	if(m_look.get() && m_up.get())
	{
		TargetLook = *m_look;
		TargetUp = *m_up;
		return true;
	}
	
	return false;
}

} // namespace k3d

