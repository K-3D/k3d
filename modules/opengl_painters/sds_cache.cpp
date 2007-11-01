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

#include "sds_cache.h"

namespace module
{

namespace opengl
{

namespace painters
{

////////////
// sds_cache
////////////

sds_cache::~sds_cache()
{
	// disconnect these, so they no longer point into freed memory
	for (size_t i = 0; i != m_connections.size(); ++i)
		m_connections[i].disconnect();
}

void sds_cache::level_changed()
{
	// search the highest level requested by the clients
	levels = 0;
	for (sds_cache::levels_t::iterator level_it = m_levels.begin(); level_it != m_levels.end(); ++level_it)
	{
		const long new_level = boost::any_cast<const long>((*level_it)->property_value());
		levels = new_level > levels ? new_level : levels;
	}
	m_scheduled = true;
}

void sds_cache::register_property(k3d::iproperty* LevelProperty)
{
	return_if_fail(LevelProperty);
	if (m_levels.insert(LevelProperty).second)
	{
		m_connections.push_back(LevelProperty->property_deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &sds_cache::remove_property), LevelProperty)));
		level_changed();
	}
}

void sds_cache::remove_property(k3d::iproperty* LevelProperty)
{
	m_levels.erase(LevelProperty);
}

void sds_cache::on_execute(const k3d::mesh& Mesh)
{
	cache.set_new_addresses(Mesh);
	if (levels > 0 && levels != cache.levels())
	{
		k3d::log() << debug << "SDS: Setting new level to " << levels << std::endl;
		cache.set_input(&Mesh);
		cache.set_levels(levels);
		update = true;
	}
	if (update)
	{
		cache.update(indices);
	}
	update = false;
	levels = 0;
}

} // namespace opengl

} // namespace painters

} // namespace module
