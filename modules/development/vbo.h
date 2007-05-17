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
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh.h>

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

/// Keep track of face data, separated in polygons, quadrilaterals and triangles
class face
{
public:
	face();
	virtual ~face();
	/// Convert the faces in Polyhedra to the internal representation
	void convert(const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra);
	/// Draw smooth faces with global indices from start to end (end included)
	void draw_smooth_range(const size_t Start, const size_t End) const;
	/// Draw flat faces with global indices from start to end (end included)
	void draw_flat_range(const size_t Start, const size_t End) const;
	/// Bind flat points VBO. Returns true if there are flat points
	bool bind_flat_points() const;
	/// Draw the faces for selection
	void draw_selection() const;
	/// True if the given Edge is sharp
	virtual bool is_sharp(const size_t Edge, const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra) = 0;
	/// Update the normals and faces using the given set of TransformedPoints
	void update(const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra, const k3d::dev::mesh::indices_t& TransformedPoints);
private:
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
	k3d::dev::mesh::indices_t m_last_transformed_points;
	/// Set of points affected by the last transformation
	std::set<size_t> m_affected_points;
	/// Face corners for the affected smooth faces
	global_corners_t m_affected_smooth_corners;
	/// Face corners for the affected flat faces
	global_corners_t m_affected_flat_corners;
	/// Collect the corner indices m_affected_faces and put them in Corners.
	void get_face_points(global_corners_t& Corners, bool Flat);
};

/// Flat shaded faces
class flat_face : public face
{
	virtual bool is_sharp(const size_t Edge, const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra);
};

/// Smooth shaded faces
class smooth_face : public face
{
	virtual bool is_sharp(const size_t Edge, const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra);
};

/// Normal calculation based on edge settings
class edge_face : public face
{
	virtual bool is_sharp(const size_t Edge, const k3d::dev::mesh::points_t& Points, const k3d::dev::mesh::polyhedra_t& Polyhedra);
};

/// Keep track of cached data for the VBO mesh painters
template<class key_t, class data_t> class painter_cache
{
	/// Collection of data associated with different meshes
	typedef std::map<const key_t, data_t*> data_collection_t;
	typedef std::map<const k3d::idocument*, painter_cache<key_t, data_t> > instances_t;
	/// Mesh <-> painter bindings
	typedef std::set<const k3d::gl::imesh_painter*> painter_set_t;
	typedef std::map<const key_t, painter_set_t> painters_t;

	public:

		/// Get the VBO cache associated with Document
		static painter_cache& instance(const k3d::idocument& Document)
		{
			typename instances_t::iterator instance = m_instances.find(&Document);
			if (instance == m_instances.end())
				instance = (m_instances.insert(std::make_pair(&Document, painter_cache()))).first;
			return instance->second;
		}

		/// Get the data associated with Key, or NULL if there is none.
		data_t* get_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data != m_data.end())
				return data->second;
			return 0;
		}

		/// Create new data for the given key if it doesn't already exist
		data_t* create_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data == m_data.end())
				data = m_data.insert(std::make_pair(Key, new data_t())).first;
			return data->second;
		}

		/// Free the data if any for the given key
		void remove_data(const key_t& Key)
		{
			typename data_collection_t::iterator data = m_data.find(Key);
			if (data != m_data.end())
			{
				delete data->second;
				m_data.erase(data);
			}
		}

		/// Register a painter as being associated with Key
		void register_painter(const key_t Key, const k3d::gl::imesh_painter* Painter)
		{
			typename painters_t::iterator painters = m_painters.find(Key);
			if (painters == m_painters.end())
				painters = m_painters.insert(std::make_pair(Key, painter_set_t())).first;
			painters->second.insert(Painter);
		}
		
		/// Remove a painter. Frees GFX card memory when a key has no painters associated.
		void remove_painter(const k3d::gl::imesh_painter* Painter)
		{
			for (typename painters_t::iterator painters = m_painters.begin(); painters != m_painters.end(); ++painters)
			{
				painters->second.erase(Painter);
				if (painters->second.empty())
					remove_data(painters->first);
			}
		}

	private:

		/// The managed data
		data_collection_t m_data;
		/// The painters associated with the keys
		painters_t m_painters;

		painter_cache() {}
		static instances_t m_instances;
};

template<class T1, class T2> std::map<const k3d::idocument*, painter_cache<T1, T2> > painter_cache<T1, T2>::m_instances;

//////////////
// Convenience functions  and types for functionality used by several mesh painters
/////////////

/// Used to keep track of selection data, ensures k3d "old style" selection constraints (no overlap, ...)
typedef std::vector<k3d::mesh_selection::record> selection_records_t;

/// Convert the array selection in SelectionArray to continuous blocks of maximal size in Selection
void array_to_selection(const k3d::dev::mesh::selection_t& SelectionArray, selection_records_t& Selection);

/// Insert Points in the VBO representing the point positions, if it doesn't exist
void create_point_vbo(const boost::shared_ptr<const k3d::dev::mesh::points_t>& Points, painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>& Cache);

/// Updates the VBO for Points at the given indices
void update_point_vbo(const boost::shared_ptr<const k3d::dev::mesh::points_t>& Points, painter_cache<boost::shared_ptr<const k3d::dev::mesh::points_t>, vbo>& Cache, const k3d::dev::mesh::indices_t& Indices);

/// Derives deformed or transformed faces and their neighbours from the points listed in Points and puts them in DeformedFaces. All points belonging to a transformed or deformed face are placed into AffectedPoints.
void get_deformed_faces(const k3d::dev::mesh::polyhedra_t& Polyhedra, const k3d::dev::mesh::indices_t& Points, std::list<size_t>& AffectedFaces, std::set<size_t>& AffectedPoints);

/// Clean VBO state. Call before and after VBO command sequence to ensure OpenGL state consistancy across painters
void clean_vbo_state();

}


#endif
