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

#include <k3dsdk/ipipeline_profiler.h>
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
}

void sds_cache::visit_surface(const k3d::mesh::primitive* Polyhedron, const k3d::uint_t Level, k3d::sds::ipatch_surface_visitor& Visitor)
{
	m_caches[Polyhedron].visit_surface(Level, Visitor);
}

void sds_cache::visit_boundary(const k3d::mesh::primitive* Polyhedron, const k3d::uint_t Level, k3d::sds::ipatch_boundary_visitor& Visitor)
{
	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(*Polyhedron));
	m_caches[Polyhedron].visit_boundary(*polyhedron, Level, Visitor);
}

void sds_cache::visit_corners(const k3d::mesh::primitive* Polyhedron, const k3d::uint_t Level, k3d::sds::ipatch_corner_visitor& Visitor)
{
	m_caches[Polyhedron].visit_corners(Level, Visitor);
}

void sds_cache::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	if (m_caches.size() && m_selection_changed)
	{
		m_selection_changed = false;
		return;
	}
	k3d::mesh::bools_t point_selection(Mesh.points->size(), false);
	for(k3d::uint_t i = 0; i != m_indices.size(); ++i)
	{
		point_selection[m_indices[i]] = true;
	}

	k3d::mesh input_with_normals = Mesh;
	const k3d::mesh::points_t& points = *input_with_normals.points;
	m_point_count = points.size();
	m_edge_counts.clear();
	m_face_counts.clear();
	for(k3d::mesh::primitives_t::const_iterator primitive = input_with_normals.primitives.begin(); primitive != input_with_normals.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(!polyhedron.get() || !k3d::polyhedron::is_sds(*polyhedron))
			continue;
		m_edge_counts[primitive->get()] = polyhedron->edge_points.size();
		m_face_counts[primitive->get()] = polyhedron->face_first_loops.size();
		// Calculate the normals on the input
		if(Painter) Painter->document().pipeline_profiler().start_execution(*Painter, "Calculate Normals");
		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = polyhedron->face_first_loops.size();
		k3d::mesh::normals_t uniform_normals(polyhedron->face_first_loops.size());
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
			uniform_normals[face] = k3d::normalize(k3d::polyhedron::normal(polyhedron->edge_points, polyhedron->clockwise_edges, *input_with_normals.points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]));
		k3d::mesh::normals_t& vertex_normals = input_with_normals.vertex_data.create("sds_normals", new k3d::mesh::normals_t(points.size()));
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
			const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ;)
				{
					vertex_normals[polyhedron->edge_points[edge]] += uniform_normals[face];
	
					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
		if(Painter) Painter->document().pipeline_profiler().finish_execution(*Painter, "Calculate Normals");
		
		k3d::mesh::selection_t face_selections(polyhedron->face_selections.size(), 1.0);
		
		if(m_caches.find(primitive->get()) == m_caches.end())
		{
			k3d::log() << debug << "SDS: Creating new SDS cache with " << m_levels << " levels" << std::endl;
			m_caches[primitive->get()].set_levels(m_levels);
			m_caches[primitive->get()].create_mesh(points, *polyhedron, face_selections, Painter);
		}
		m_caches[primitive->get()].update_mesh(points, *polyhedron, input_with_normals.vertex_data, face_selections, Painter);
	}
}

void sds_cache::on_schedule(k3d::inode* Painter) 
{
	register_painter(Painter);
	m_caches.clear();
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
	if (m_caches.empty() || m_levels != levels)
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
