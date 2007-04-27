#ifndef K3D_SDS_BINDING
#define K3D_SDS_BINDING

// K-3D SDS preview
// Copyright (c) 2005, Bart Janssens
//
// Contact: bart.janssens@lid.kviv.be
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

/**	\file K-3D interface for Catmull-Clark algorithm
		\author Bart Janssens <bart.janssens@lid.kviv.be>
 */

#include "subdivision_algorithms.h"

#include <k3dsdk/color.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/iunknown.h>

namespace k3d
{

namespace sds
{

// Keeps track of modified faces and their number in the original mesh
typedef std::map<size_t, face_vertex*> facevertices_map;

/// convert k3d::legacy::mesh to the internal sds cache structure. This one does a lot of work, it should be looked into if part of this can be moved to the generic SDS part. (doubtful, since it deals heavily with k3d::edge and k3d::legacy::face)
class k3d_cache_input : public cache_input<k3d::dev::mesh>
{
public:
	
	k3d_cache_input(boost::shared_ptr<const k3d::dev::mesh::points_t> Points, boost::shared_ptr<const k3d::dev::mesh::polyhedra_t> Polyhedra,  boost::shared_ptr<const k3d::dev::mesh::selection_t> PointSelection);
	~k3d_cache_input();

	//////
	// cache_input<k3d::dev::mesh> implementation
	//////
	bool set_input(const k3d::dev::mesh* Input);

	void update(bool all, facevertices_t& updated_maps);

	//////////
	// Extra K3D specific methods
	//////////

	/// Store cache validity status based on the supplied hint
	bool validate(k3d::iunknown* Hint);

	/// Get the faces that were modified by the last update
	facevertices_map& get_modified_faces()
	{
		return m_modified_faces;
	}
	
	/// Get the full set of faces
	facevertices_map& get_all_faces()
	{
		return m_all_faces;
	}
	
	/// True if the cache was updated. Only the first call after update() returns "true"
	bool updated()
	{
		if (m_updated)
		{
			m_updated = false;
			return true;
		}
		return false;
	}

	// True if the given face or its neighbours contain selected elements
	bool selected(size_t Face, int Recurse = 4);

private:
	bool m_hint_validated;
	bool m_updated;

	boost::shared_ptr<const k3d::dev::mesh::points_t> m_input_points;
	boost::shared_ptr<const k3d::dev::mesh::polyhedra_t> m_input_polyhedra;
	boost::shared_ptr<const k3d::dev::mesh::selection_t> m_point_selection;
	
	facevertices_map m_modified_faces;
	
	facevertices_map m_all_faces;
	
	typedef std::pair<size_t, size_t> size_pair_t;
	typedef std::map<size_pair_t, edge_vertex*> edgemap_t;
	
	// Helper function to find an edge by its point indices, regardless of their order
	edgemap_t::iterator find_edge(size_t Start, size_t End);
	
	typedef std::map<size_t, sds_point*> pointmap_t; // FIXME optimize as vector?
	edgemap_t m_edge_vertices;
	pointmap_t m_sds_vertices;
	
	size_t m_npoints;
	size_t m_nfaces;
	
	typedef std::vector<size_t> edge_to_face_t; // Map edge numbers to face numbers
	edge_to_face_t m_edge_to_face;
	typedef std::vector<size_t> companions_t; // Keep track of edge companions
	companions_t m_companions;
	
	// get or create the edge vertex belonging to Edge
	edge_vertex* get_ev(size_t StartPoint, size_t EndPoint, face_vertex* Fv);
	
	// get the corner associated with the given point
	sds_point* get_corner(size_t Point);
	
	// initialise point, face and plohedra counters
	void init_counters()
	{
		m_npoints = m_input_points->size();
		m_nfaces = m_input_polyhedra->face_first_loops->size();
	}
};

/// Base class for the caches used by k3d, handling only the input side of things.
class k3d_sds_cache_base : public catmull_clark_cache<k3d::dev::mesh>
{
protected:
	////////////
	// catmull_clark_cache<k3d::dev::mesh> implementation
	////////////
	cache_input<k3d::dev::mesh>* create_cache_input(const k3d::dev::mesh* Input)
	{
		return new k3d_cache_input(Input->points, Input->polyhedra, Input->point_selection);
	}
};

/// SDS cache that outputs to OpenGL
class k3d_opengl_sds_cache : public k3d_sds_cache_base
{
public:

	k3d_opengl_sds_cache() :
		m_n_faces(0),
		m_n_edges(0),
		m_n_points(0),
		m_n_corners(0),
		m_mesh_selection(false),
		m_update_selection(true),
		m_unselected_color(1.0,1.0,1.0),
		m_validated_hint(false)
	{
		m_first_level_cache_mesh = 0;
	}

	~k3d_opengl_sds_cache();
	
	/// (K-3D specific) draw subdivided faces ("patches")
	void draw_faces(size_t Level, bool Selected);
	
	/// (K-3D specific) draw patch borders
	void draw_borders(size_t Level, bool Selected);
	
	/// (K-3D specific) draw patch corner points
	void draw_corners(size_t Level, bool Selected);
	
	/// (K-3D specific) selection for faces
	void select_faces(size_t Level);
	
	/// (K-3D specific) selection for borders
	void select_borders(size_t Level);
	
	/// (K-3D specific) selection for corners
	void select_corners(size_t Level);
	
	/// (K-3D specific) update selection color arrays
	void update_selection();
	
	/// Regenerate VBOs if the mesh structure has changed
	void regenerate_vbos();
	
	/// Validate cache through hint
	void validate(k3d::iunknown* Hint);
	
protected:
	////////
	// catmull_clark_cache<k3d::legacy::mesh> implementation
	///////
	
	/// Equivalent to draw_faces
	void client_output(k3d::dev::mesh* Output = 0);

	void client_output_nurbs(k3d::dev::mesh* Output = 0);

private:
	// VBOs for the cached data
	std::vector<GLuint> m_point_vbos, m_face_vbos, m_normals_vbos, m_edge_vbos, m_corner_vbos;
	// Color VBOs for selection color
	std::vector<GLuint>  m_corner_color_vbos, m_edge_color_vbos, m_face_color_vbos;
	// Colored points, edges and faces
	std::vector<std::vector<size_t> > m_selected_corners, m_selected_edges, m_selected_faces;
	// Start index for the points belonging to each face vertex
	std::vector<std::vector<size_t> > m_face_indices;
	// Number of components to draw
	std::vector<size_t> m_n_faces, m_n_edges, m_n_points, m_n_corners;
	bool m_update_selection;
	// Execute the scheduled selection update
	void do_selection_update();
	// Update point positions
	void update_positions();
	// Old mesh selection state
	bool m_mesh_selection;
	// Color for unselected components
	k3d::color m_unselected_color;
	bool m_validated_hint;
};

/// Output to a k3d::legacy::mesh
class k3d_mesh_sds_cache : public k3d_sds_cache_base
{
public:
	k3d_mesh_sds_cache()
	{
		set_levels(1);
	}
protected:
	////////
	// catmull_clark_cache<k3d::legacy::mesh> implementation
	///////
	/** \todo make efficient */
	void client_output(k3d::legacy::mesh* Output = 0);

	/** \todo implement this */
	void client_output_nurbs(k3d::legacy::mesh* Output = 0) {}
};

} // namespace sds

} // namespace k3d

#endif // #ifndef K3D_SDS_BINDING
