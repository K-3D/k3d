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
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/painter_cache.h>

#include <subdivision_surface/k3d_sds_binding.h>

namespace libk3ddevelopment
{

typedef std::map<size_t, size_t> indexmap_t;
typedef std::multimap<size_t, size_t> indexmultimap_t;
typedef std::list<size_t> indexlist_t;

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

/// Storage for selection data
typedef std::vector<k3d::mesh_selection::record> selection_records_t;

/// Keep track of component selections
class component_selection : public k3d::scheduler
{
public:
	/// Provide access to the stored selection records
	const selection_records_t& records() const
	{
		return m_selection_records;
	}
protected:
	/// Executes the selection update based on the mesh array selection
	virtual void on_execute(const k3d::mesh& Mesh);

	selection_records_t m_selection_records;
	boost::shared_ptr<const k3d::mesh::selection_t> m_selection_array;
};

/// Implement component_selection::on_schedule for a point selection
class point_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.point_selection;
	}
};

/// Implement component_selection::on_schedule for an edge selection
class edge_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.polyhedra->edge_selection;
	}
};


/// Implement component_selection::on_schedule for a face selection
class face_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.polyhedra->face_selection;
	}
};

/// Keep track of point position data in a VBO
class point_vbo : public k3d::scheduler
{
public:
	point_vbo() : m_vbo(0) {}
	/// Bind the internal VBO for usage by the array drawing commands
	void bind();
protected:
	/// Implements the scheduling phase of a point position update
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint);
	
	/// Executes the point position update
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	/// Stores the data itself
	vbo* m_vbo;
	
	/// Stores the modified point indices provided by the hint
	k3d::mesh::indices_t m_indices;
};

/// Keep track of edge indices in a VBO
class edge_vbo : public k3d::scheduler
{
public:
	edge_vbo() : m_vbo(0) {}
	/// Bind the internal VBO for usage by the array drawing commands
	void bind();
protected:
	/// Creates the edge vbo
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	/// Stores the data itself
	vbo* m_vbo;
};

/// Keep track of face data, separated in polygons, quadrilaterals and triangles
class face : public k3d::scheduler
{
public:
	face();
	virtual ~face();
	/// Draw smooth faces with global indices from start to end (end included)
	void draw_smooth_range(const size_t Start, const size_t End) const;
	/// Draw flat faces with global indices from start to end (end included)
	void draw_flat_range(const size_t Start, const size_t End) const;
	/// Bind flat points VBO. Returns true if there are flat points
	bool bind_flat_points() const;
	/// Draw the faces for selection
	void draw_selection() const;
	
	/// Scheduler implementation
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint);
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	/// Convert the faces in Polyhedra to the internal representation
	void convert(const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra);
	/// Update the normals and faces using the given set of TransformedPoints
	void update(const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra, const k3d::mesh::indices_t& TransformedPoints);
	typedef std::pair<size_t, size_t> range_t;
	typedef std::map<size_t, std::vector<size_t> > global_corners_t;
	/// return local start and end indices as a pair. End is converted to one past the corresponding index 
	range_t local_indices(size_t Start, size_t End, const indexmap_t& Map) const;
	/// VBO names for face indices
	GLuint m_smooth_polys, m_smooth_quads, m_smooth_tris, m_flat_polys, m_flat_quads, m_flat_tris;
	/// VBO for duplicated points belonging to "flat" faces
	GLuint m_flat_points;
	/// VBOs for normals
	GLuint m_smooth_normals, m_flat_normals;
	/// Mapping between global and local indices
	indexmap_t m_smooth_polymap, m_smooth_quadmap, m_smooth_trimap, m_flat_polymap, m_flat_quadmap, m_flat_trimap;
	/// Sizes for each polygon
	std::vector<size_t> m_smooth_polycounts;
	std::vector<size_t> m_flat_polycounts;
	/// Links between flat and smooth points, defined for smooth points belonging to flat faces
	indexmultimap_t m_smooth_to_flat, m_flat_to_smooth;
	/// Link old points to their flat duplicates (needed for position updates)
	indexmultimap_t m_position_link;
	/// List of faces affected by the last transformation
	indexlist_t m_affected_faces;
	/// Points used in the last update (used to check if this set has changed)
	k3d::mesh::indices_t m_last_transformed_points;
	/// Set of points affected by the last transformation
	std::set<size_t> m_affected_points;
	/// Face corners for the affected smooth faces
	global_corners_t m_affected_smooth_corners;
	/// Face corners for the affected flat faces
	global_corners_t m_affected_flat_corners;
	/// Collect the corner indices m_affected_faces and put them in Corners.
	void get_face_points(global_corners_t& Corners, bool Flat);
	/// True if the given Edge is sharp
	virtual bool is_sharp(const size_t Edge, const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra) = 0;
	/// Stores the modified point indices provided by the hint
	k3d::mesh::indices_t m_indices;
};

/// Flat shaded faces
class flat_face : public face
{
private:
	virtual bool is_sharp(const size_t Edge, const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra);
};

/// Smooth shaded faces
class smooth_face : public face
{
private:
	virtual bool is_sharp(const size_t Edge, const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra);
};

/// Normal calculation based on edge settings
class edge_face : public face
{
private:
	virtual bool is_sharp(const size_t Edge, const k3d::mesh::points_t& Points, const k3d::mesh::polyhedra_t& Polyhedra);
};

/// Common SDS cache functionality
class sds_cache : public k3d::scheduler
{
public:
	sds_cache() : levels(2) {}
	
	/// Notify the cache that one of the registered painters changed level
	void level_changed();
	
	/// Register a level property
	void register_property(k3d::iproperty* LevelProperty);
	
	/// Remove a level property
	void remove_property(k3d::iproperty* LevelProperty);
	
	int levels;
private:
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_levels;
};

/// Encapsulates a VBO SDS cache in a k3d::scheduler
class sds_vbo_cache : public sds_cache
{
public:
	sds_vbo_cache() : regenerate(true), update(true), update_selection(true) {}
	
	k3d::sds::k3d_vbo_sds_cache cache;
	bool regenerate;
	bool update;
	bool update_selection;
protected:
	/// Scheduler implementation
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_levels;
};

/// Encapsulates a basic SDS cache in a k3d::scheduler
class sds_gl_cache : public sds_cache
{
public:
	sds_gl_cache() : update(true) {}
	
	k3d::sds::k3d_basic_opengl_sds_cache cache;
	bool update;
protected:
	/// Scheduler implementation
	virtual void on_execute(const k3d::mesh& Mesh);
private:
	typedef std::set<k3d::iproperty*> levels_t;
	levels_t m_levels;
};

//////////////
// Convenience functions  and types for functionality used by several mesh painters
/////////////

/// Convert the array selection in SelectionArray to continuous blocks of maximal size in Selection
void array_to_selection(const k3d::mesh::selection_t& SelectionArray, selection_records_t& Selection);

/// Derives deformed or transformed faces and their neighbours from the points listed in Points and puts them in DeformedFaces. All points belonging to a transformed or deformed face are placed into AffectedPoints.
void get_deformed_faces(const k3d::mesh::polyhedra_t& Polyhedra, const k3d::mesh::indices_t& Points, std::list<size_t>& AffectedFaces, std::set<size_t>& AffectedPoints);

/// Clean VBO state. Call before and after VBO command sequence to ensure OpenGL state consistancy across painters
void clean_vbo_state();

}


#endif
