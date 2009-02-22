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
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

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
	for (connections_t::iterator connection = m_changed_connections.begin(); connection != m_changed_connections.end(); ++connection)
	{
		connection->second.disconnect();
		m_deleted_connections[connection->first].disconnect();
	}
	m_changed_connections.clear();
	m_deleted_connections.clear();
	delete m_cache;
}

void sds_cache::visit_surface(const k3d::uint_t Level, k3d::sds::ipatch_surface_visitor& Visitor)
{
	return_if_fail(m_cache);
	m_cache->visit_surface(Level, Visitor);
}

void sds_cache::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	if (m_cache && m_selection_changed)
	{
		m_selection_changed = false;
		return;
	}
	k3d::mesh::bools_t point_selection(Mesh.points->size(), false);
	for(k3d::uint_t i = 0; i != m_indices.size(); ++i)
	{
		point_selection[m_indices[i]] = true;
	}
	const boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
	return_if_fail(polyhedron);
	
	k3d::mesh::selection_t face_selections(polyhedron->face_selections.size(), 1.0);
	
	if (!m_cache)
	{
		k3d::log() << debug << "SDS: Creating new SDS cache with " << m_levels << " levels" << std::endl;
		m_cache = new k3d::sds::catmull_clark_subdivider(m_levels);
		m_cache->create_mesh(Mesh, face_selections, m_mesh, Painter);
	}
	m_cache->update_mesh(Mesh, face_selections, m_mesh, Painter);
}

void sds_cache::on_schedule(k3d::inode* Painter) 
{
	register_painter(Painter);
	delete m_cache;
	m_cache = 0;
	m_selection_changed = false;
}
void sds_cache::on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
{
	register_painter(Painter);
	m_indices = Hint->changed_points;
}
void sds_cache::on_schedule(k3d::hint::selection_changed* Hint, k3d::inode* Painter)
{
	register_painter(Painter);
	m_indices.clear();
	m_selection_changed = true;
}

void sds_cache::level_changed(k3d::ihint* Hint)
{
	// search the highest level requested by the clients
	k3d::uint_t levels = 0;
	k3d::iproperty* highest = 0;
	for (sds_cache::levels_t::iterator level_it = m_level_properties.begin(); level_it != m_level_properties.end(); ++level_it)
	{
		const k3d::int32_t new_level = boost::any_cast<const k3d::int32_t>((*level_it)->property_internal_value());
		if (new_level > levels)
		{
			levels = new_level;
			highest = *level_it;
		}
	}
	if (!m_cache || m_levels != levels)
	{
		m_levels = levels;
		schedule(highest->property_node());
	}
}

void sds_cache::register_painter(k3d::inode* Painter)
{
	k3d::iproperty* property = k3d::property::get(*Painter, "levels");
	if (property)
	{
		if (m_changed_connections.find(Painter) == m_changed_connections.end())
			m_changed_connections[Painter] = property->property_changed_signal().connect(sigc::mem_fun(*this, &sds_cache::level_changed));
		if (m_deleted_connections.find(Painter) == m_deleted_connections.end())
			m_deleted_connections[Painter] = Painter->deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &sds_cache::remove_painter), Painter));
		if (m_level_properties.insert(property).second)
		{
			level_changed(0);
		}
	}
	else
	{
		k3d::log() << error << "sds_cache: failed to register property \"levels\"" << std::endl;
	}
}

void sds_cache::remove_painter(k3d::inode* Painter)
{
	m_level_properties.erase(k3d::property::get(*Painter, "levels"));
	m_changed_connections[Painter].disconnect();
	m_deleted_connections[Painter].disconnect();
	m_changed_connections.erase(Painter);
	m_deleted_connections.erase(Painter);
}

} // namespace opengl

} // namespace painters

} // namespace module
