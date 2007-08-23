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

#include "snap_target.h"

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////////
// snap_target

snap_target::snap_target(const std::string& Label, const position_slot_t& PositionSlot, const orientation_slot_t& OrientationSlot) :
	m_label(Label),
	m_position_slot(PositionSlot),
	m_orientation_slot(OrientationSlot)
{
}
	
const std::string snap_target::label()
{
	return m_label;
}

const isnap_target::groups_t snap_target::groups()
{
	return groups_t();
}

bool snap_target::target_position(const point3& Position, point3& TargetPosition)
{
	return m_position_slot(Position, TargetPosition);
}

bool snap_target::target_orientation(const point3& Position, vector3& TargetLook, vector3& TargetUp)
{
	return m_orientation_slot(Position, TargetLook, TargetUp);
}

} // namespace k3d

