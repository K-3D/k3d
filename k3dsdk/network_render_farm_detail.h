#ifndef K3DSDK_NETWORK_RENDER_FARM_DETAIL_H
#define K3DSDK_NETWORK_RENDER_FARM_DETAIL_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "inetwork_render_farm.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// network_render_farm_implementation

class network_render_farm_implementation :
	public inetwork_render_farm
{
public:
	network_render_farm_implementation(const filesystem::path& OptionsPath);
	~network_render_farm_implementation();

	inetwork_render_job& create_job(const std::string& JobName);
	void start_job(inetwork_render_job& Job);

private:
	class implementation;
	implementation* const m_implementation;
};

/// Call this once at startup to register the global singleton render farm implementation - note: does not take control of the given object's lifetime.
void set_network_render_farm(inetwork_render_farm& RenderFarm);

} // namespace k3d

#endif // K3DSDK_NETWORK_RENDER_FARM_DETAIL_H

