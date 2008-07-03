#ifndef VBO_H
#define VBO_H

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

/** \file Helper functions and classes to manage a global data cache for the mesh painters
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <list>
#include <map>
#include <set>

#include <k3dsdk/gl.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/subdivision_surface/k3d_sds_binding.h>

#include "cached_triangulation.h"
#include "painter_cache.h"
#include "sds_cache.h"
#include "selection_cache.h"

namespace module
{

namespace opengl
{

namespace painters
{

typedef std::map<size_t, size_t> indexmap_t;
typedef std::multimap<size_t, size_t> indexmultimap_t;
typedef std::list<size_t> indexlist_t;

/// Exception for VBO-related errors
class vbo_exception : public std::runtime_error
{
	typedef std::runtime_error base;
public:
	vbo_exception(const std::string& Message) : base("VBO error: " + Message) {} 
};

/// Convenience wrapper for OpenGL vertex buffer objects
class vbo
{
public:
	vbo();
	~vbo();
	/// Implicit conversion so we can use a vbo instance as an argument to OpenGL functions
	operator GLuint() const;
private:
	vbo(const vbo&);
	vbo& operator=(const vbo&);

	GLuint m_name;
};

/// Keep track of point position data in a VBO
class point_vbo : public scheduler
{
public:
	point_vbo(const k3d::mesh* const Mesh) : m_vbo(0) {}
	/// Bind the internal VBO for usage by the array drawing commands
	void bind();
protected:
	/// Implements the scheduling phase of a point position update
	void on_schedule(k3d::inode* Painter);
	void on_schedule(k3d::hint::mesh_geometry_changed_t* Hint, k3d::inode* Painter);
	
	/// Executes the point position update
	virtual void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter);
private:
	/// Stores the data itself
	vbo* m_vbo;
	
	/// Stores the modified point indices provided by the hint
	k3d::mesh::indices_t m_indices;
};

/// Keep track of edge indices in a VBO
class edge_vbo : public scheduler
{
public:
	edge_vbo(const k3d::mesh* const Mesh) : m_vbo(0) {}
	/// Bind the internal VBO for usage by the array drawing commands
	void bind();
protected:
	void on_schedule(k3d::inode* Painter);
	/// Creates the edge vbo
	virtual void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter);
private:
	/// Stores the data itself
	vbo* m_vbo;
};

/// VBOs used to paint a triangulated mesh
class triangle_vbo : public scheduler
{
public:
	triangle_vbo(const k3d::mesh* const Mesh) : m_point_vbo(0), m_index_vbo(0), m_normal_vbo(0), m_mesh(Mesh) {}
	~triangle_vbo()
	{
		delete m_point_vbo;
		delete m_normal_vbo;
		delete m_index_vbo;
	}
	
	/// Bind the buffers for drawing
	void bind();
	
	/// Draw faces with original mesh indices Start to End
	void draw_range(k3d::uint_t Start, k3d::uint_t End, k3d::inode* Painter);
protected:
	void on_schedule(k3d::inode* Painter);
	void on_schedule(k3d::hint::mesh_geometry_changed_t* Hint, k3d::inode* Painter);
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter);
private:
	vbo* m_point_vbo;
	vbo* m_index_vbo;
	vbo* m_normal_vbo;
	k3d::mesh::indices_t m_indices;
	// For each triangle corner, store the face it belongs to (used for flat normal calculation)
	k3d::mesh::indices_t m_corner_to_face;
	const k3d::mesh* const m_mesh;
};

/// Keep track of the SDS VBOs per level
template<class component_t> class sds_vbo : public scheduler
{
public:
	sds_vbo(const k3d::mesh* const Mesh) : m_mesh(Mesh) {}
	
	~sds_vbo()
	{
		for (typename connections_t::iterator connection = m_changed_connections.begin(); connection != m_changed_connections.end(); ++connection)
		{
			connection->second.disconnect();
			m_deleted_connections[connection->first].disconnect();
		}
		m_changed_connections.clear();
		m_deleted_connections.clear();
	}
	
	/// Binds the vbo's at the given level
	void bind(k3d::uint_t Level, k3d::inode* Painter)
	{
		cache(Level, Painter).bind();
	}
	
	/// Get the cache at the requested level
	component_t& cache(k3d::uint_t Level, k3d::inode* Painter)
	{
		std::pair<typename caches_t::iterator, bool> result = m_caches.insert(std::make_pair(Level, component_t()));
		if (result.second)
			result.first->second.update(*m_mesh, Level, get_data<sds_cache>(m_mesh, Painter).cache());
		return m_caches[Level];
	}
	
protected:
	
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
	{
		return_if_fail(Painter);
		k3d::int32_t level = k3d::property::pipeline_value<k3d::int32_t>(*Painter, "levels");
		m_caches.insert(std::make_pair(level, component_t())).first->second.update(Mesh, level, get_data<sds_cache>(&Mesh, Painter).cache());
	}
	
	void on_schedule(k3d::inode* Painter) 
	{
		clear();
		register_painter(Painter);
		schedule_data<sds_cache>(m_mesh, 0, Painter);
	}
	void on_schedule(k3d::hint::mesh_geometry_changed_t* Hint, k3d::inode* Painter)
	{
		register_painter(Painter);
		schedule_data<sds_cache>(m_mesh, Hint, Painter);
	}
	void on_schedule(k3d::hint::selection_changed_t* Hint, k3d::inode* Painter)
	{
		register_painter(Painter);
		schedule_data<sds_cache>(m_mesh, Hint, Painter);
	}
private:
	void clear(k3d::ihint* Hint = 0)
	{
		m_caches.clear();
	}
	
	void register_painter(k3d::inode* Painter)
	{
		k3d::iproperty* property = k3d::property::get(*Painter, "levels");
		if (property)
		{
			if (m_changed_connections.find(Painter) == m_changed_connections.end())
				m_changed_connections[Painter] = property->property_changed_signal().connect(sigc::mem_fun(*this, &sds_vbo::clear));
			if (m_deleted_connections.find(Painter) == m_deleted_connections.end())
				m_deleted_connections[Painter] = Painter->deleted_signal().connect(sigc::bind(sigc::mem_fun(*this, &sds_vbo::remove_painter), Painter));
		}
		else
		{
			k3d::log() << error << "sds_cache: failed to register property \"levels\"" << std::endl;
		}
	}
	
	void remove_painter(k3d::inode* Painter)
	{
		m_changed_connections[Painter].disconnect();
		m_deleted_connections[Painter].disconnect();
		m_changed_connections.erase(Painter);
		m_deleted_connections.erase(Painter);
		clear();
	}
	
	// store connections for safe deletion of cache
	typedef std::map<k3d::inode*, sigc::connection> connections_t;
	connections_t m_changed_connections; // connections to changed_signals
	connections_t m_deleted_connections; // connections to deleted_signals
	typedef std::map<k3d::uint_t, component_t> caches_t;
	caches_t m_caches; // stores an sds_vbo cache per level
	const k3d::mesh* const m_mesh;
};

/// Cache SDS face VBOs
class sds_face_vbo
{
public:
	sds_face_vbo() : m_point_vbo(0), m_index_vbo(0), m_normal_vbo(0) {}
	~sds_face_vbo()
	{
		delete m_point_vbo;
		delete m_normal_vbo;
		delete m_index_vbo;
	}
	
	/// Update the cache using the supplied sds cache
	void update(const k3d::mesh& Mesh, const k3d::uint_t Level, k3d::sds::k3d_sds_cache& Cache);
	
	/// Bind the VBOs
	void bind();
	
	/// Start indices for the faces
	k3d::mesh::indices_t face_starts;
	
	/// Length of the index buffer
	k3d::uint_t index_size;
	
	/// Indicates if a face is on the boundary
	k3d::mesh::bools_t boundary_faces;
	
private:
	vbo* m_point_vbo;
	vbo* m_index_vbo;
	vbo* m_normal_vbo;
	k3d::mesh::indices_t m_companions;
	k3d::mesh::bools_t m_boundary_edges;
};

/// Cache SDS edge VBOs
class sds_edge_vbo
{
public:
	sds_edge_vbo() : m_point_vbo(0) {}
	~sds_edge_vbo()
	{
		delete m_point_vbo;
	}
	
	/// Update the cache using the supplied sds cache
	void update(const k3d::mesh& Mesh, const k3d::uint_t Level, k3d::sds::k3d_sds_cache& Cache);
	
	/// Bind the VBOs
	void bind();
	
	/// Start indices for the edges
	k3d::mesh::indices_t edge_starts;
	
	/// Length of the index buffer
	k3d::uint_t index_size;
	
private:
	vbo* m_point_vbo;
};

/// Cache SDS point VBOs
class sds_point_vbo
{
public:
	sds_point_vbo() : m_point_vbo(0) {}
	~sds_point_vbo()
	{
		delete m_point_vbo;
	}
	
	/// Update the cache using the supplied sds cache
	void update(const k3d::mesh& Mesh, const k3d::uint_t Level, k3d::sds::k3d_sds_cache& Cache);
	
	/// Bind the VBOs
	void bind();
	
	/// Length of the index buffer
	k3d::uint_t index_size;
	
private:
	vbo* m_point_vbo;
};

//////////////
// Convenience functions  and types for functionality used by several mesh painters
/////////////

/// Clean VBO state. Call before and after VBO command sequence to ensure OpenGL state consistancy across painters
void clean_vbo_state();

} // namespace painters

} // namespace opengl

} // namespace module

#endif
