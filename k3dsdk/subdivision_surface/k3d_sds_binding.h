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

typedef std::vector<size_t> companions_t; // Keep track of edge companions
/// Color with alpha channel support
typedef k3d::basic_rgba<double, k3d::color_traits<double> > color_t;

/// convert k3d::legacy::mesh to the internal sds cache structure. This one does a lot of work, it should be looked into if part of this can be moved to the generic SDS part. (doubtful, since it deals heavily with k3d::edge and k3d::legacy::face)
class k3d_cache_input : public cache_input<k3d::mesh>
{
public:
	
	k3d_cache_input(boost::shared_ptr<const k3d::mesh::points_t> Points, boost::shared_ptr<const k3d::mesh::polyhedra_t> Polyhedra,  boost::shared_ptr<const k3d::mesh::selection_t> PointSelection);
	~k3d_cache_input();

	//////
	// cache_input<k3d::mesh> implementation
	//////
	bool set_input(const k3d::mesh* Input);

	void update(const indices_t& Indices, facevertices_t& updated_maps);

	//////////
	// Extra K3D specific methods
	//////////

	/// Reset mesh addresses
	void set_new_addresses(const k3d::mesh& Mesh);

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

	// True if the given face or its neighbours contain selected elements
	typedef std::set<size_t> indexset_t;
	bool selected(size_t Face, const indexset_t& Indices, int Recurse = 2);
	
	// True if none of the components in the mesh are selected
	bool no_selection();
	
	/// Return the edge companions for the original mesh
	const companions_t& companions() const
	{
		return m_companions;
	}

private:

	boost::shared_ptr<const k3d::mesh::points_t> m_input_points;
	boost::shared_ptr<const k3d::mesh::polyhedra_t> m_input_polyhedra;
	boost::shared_ptr<const k3d::mesh::selection_t> m_point_selection;
	
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

/// Visitor to traverse the SDS data
class sds_visitor
{
public:
	/// called when a point is visited
	virtual void on_point(const position_t& Point, const position_t& Normal = position_t(0,0,1)) {}
	
	/// called with the corner indices of a subfacet of the subdivided mesh
	virtual void on_subfacet(const k3d::uint_t Point1, const k3d::uint_t Point2, const k3d::uint_t Point3, const k3d::uint_t Point4) {}
	
	/// called when a new face is entered
	virtual void on_face(k3d::uint_t Face) {}
	
	/// called when a new edge is entered
	virtual void on_edge() {}
};

/// Base class for the caches used by k3d, handling the input side of things and providing traversal using visitors
class k3d_sds_cache : public catmull_clark_cache<k3d::mesh>
{
public:
	/// Visit faces
	void visit_faces(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly);
	
	/// Visit patch borders
	void visit_borders(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly);
	
	/// Visit patch corners
	void visit_corners(sds_visitor& Visitor, const k3d::uint_t Level, const bool ModifiedOnly);
	
	/// Reset mesh addresses
	void set_new_addresses(const k3d::mesh& Mesh)
	{
		if (!m_first_level_cache)
		{
			set_input(&Mesh);
			return;
		}
		m_first_level_cache_mesh = &Mesh;
		dynamic_cast<k3d_cache_input*>(m_first_level_cache)->set_new_addresses(Mesh);
	}
		
	/// Clear the list of modified faces to force recalculation of the geometry affected by a change
	void clear_modified_faces()
	{
		if (m_first_level_cache)
			dynamic_cast<k3d_cache_input*>(m_first_level_cache)->get_modified_faces().clear();
	}
	
	/// Currently set number of levels
	k3d::uint_t levels() const
	{
		return m_levels;
	}
	
protected:
	////////////
	// catmull_clark_cache<k3d::mesh> implementation
	////////////
	cache_input<k3d::mesh>* create_cache_input(const k3d::mesh* Input)
	{
		return new k3d_cache_input(Input->points, Input->polyhedra, Input->point_selection);
	}	
	void client_output(k3d::mesh* Output = 0) {}
	void client_output_nurbs(k3d::mesh* Output = 0) {}
};

/// Output to a k3d::legacy::mesh
class k3d_mesh_sds_cache : public k3d_sds_cache
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
