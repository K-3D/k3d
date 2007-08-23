#ifndef SUBDIVISION_ALGORITHMS_H
#define SUBDIVISION_ALGORITHMS_H

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

/** \file Functions for SDS.
		\author Bart Janssens <bart.janssens@lid.kviv.be>
*/

#include <k3dsdk/mesh.h>

#include <boost/multi_array.hpp>

#include <cmath>
#include <map>
#include <set>
#include <vector>

namespace k3d
{

namespace sds
{

/// Encapsulates a point position
typedef k3d::point3 position_t;
typedef std::vector<position_t*> positions_t;
typedef std::vector<const position_t*> const_positions_t;

/// Exception thrown when the cache turns out to be invalid (despite point and face-count check)
class cache_invalid_exception {};

/// Super class for points that can be invalidated
class valid_point
{
public:
	valid_point() :
		m_valid(false)
	{
	}

	void invalidate()
	{
		m_valid = false;
	}

	position_t vertex;

protected:
	bool m_valid;
};

class sds_point;
class face_vertex;
class edge_vertex;

typedef std::vector<sds_point*> sdspoints_t;
typedef std::vector<edge_vertex*> edgevertices_t;
typedef std::vector<face_vertex*> facevertices_t;

/// A descendant from a point in the original unsubdivided mesh
class sds_point :
	public valid_point
{
public:
	sds_point() : m_border_set(0) {}

	/// recalculate position if needed
	void update();

	/// add the influence of an edge point for a border point
	void add_border_position(const position_t& position)
	{
		vertex = m_border_set == 0 ? position : vertex + position;
		++m_border_set;
		m_border_set %= 2;
	}

	/// edge-neighbours
	sdspoints_t corners;

	/// position in the original mesh
	position_t original;

	/// centroids of neighbour faces
	facevertices_t face_vertices;

private:
	int m_border_set;
};

/// an SDS edge vertex
class edge_vertex : public valid_point
{
public:
	edge_vertex()
	{
		face_vertices[0] = 0;
		face_vertices[1] = 0;
		corners[0] = 0;
		corners[1] = 0;
	}

	/// recalculate if needed
	void update();

	/// face vertex on the other side of fv
	face_vertex* other(face_vertex* fv)
	{
		if (face_vertices[0] == fv)
			return face_vertices[1];
		if (face_vertices[1] == fv)
			return face_vertices[0];
		return 0;
	}

	/// neighbouring corners
	sds_point* corners[2];

	/// neighbouring face vertices
	face_vertex* face_vertices[2];
};


typedef boost::multi_array<position_t*, 2> point_array;

class patch_border;
class sds_mipmap;

/// The corner point of a patch
class patch_corner
{
public:
	patch_corner(const position_t& Parent) : m_parent(Parent), m_next_level(0), m_valid(false), m_normal_valid(false), m_nurbs_valid(false), m_smooth(false), m_boundary_left(0), m_boundary_right(0)
	{
		m_point = new position_t(0,0,0);
		m_nurbs_position = new position_t(0,0,0);
	}

	~patch_corner()
	{
		delete m_next_level;
		delete m_point;
		delete m_nurbs_position;
	}

	/// Next level
	patch_corner* next_level()
	{
		if (m_next_level == 0)
			m_next_level = new patch_corner(position());
		return m_next_level;
	}

	/// Recalculate position
	void update();

	/// Recalculate normals
	void update_normal();

	/// Recalculate nurbs position
	void update_nurbs();

	/// PCCM corner smoothing
	void smooth();

	/// Get position
	position_t& position()
	{
		return *m_point;
	}

	/// Get position in the case of a NURBS patch
	position_t& nurbs_position()
	{
		return *m_nurbs_position;
	}

	/// Register point needed for mesh boundary calculation
	void register_boundary_point(position_t* Point)
	{
		if (!m_boundary_left)
			m_boundary_left = Point;
		else if (!m_boundary_right)
			m_boundary_right = Point;
	}

	/// Register face vertex
	void add_face_vertex(position_t* Point)
	{
		return_if_fail(m_face_vertices.size() == 0 || m_face_vertices.back() != Point);
		m_face_vertices.push_back(Point);
	}

	/// Register corner
	void add_corner(position_t* Point)
	{
		m_corners.push_back(Point);
	}

	/// Register edge vertex (needed for NURBS)
	void add_edge_vertex(position_t* Point)
	{
		m_edge_vertices.push_back(Point);
	}

	/// invalidate
	void invalidate()
	{
		m_valid = false;
	}

	position_t& normal()
	{
		return m_normal;
	}

	void add_normal(position_t* Vector)
	{
		m_normals.push_back(Vector);
	}

	/// Register points needed for NURBS corner smoothing
	void add10(position_t* p)
	{
		m_Q10.push_back(p);
	}
	void add20(position_t* p)
	{
		m_Q20.push_back(p);
	}
	void add40(position_t* p)
	{
		m_Q40.push_back(p);
	}
	void add11(position_t* p)
	{
		m_Q11.push_back(p);
	}
	void add12(position_t* p)
	{
		m_Q12.push_back(p);
	}
	void add21(position_t* p)
	{
		m_Q21.push_back(p);
	}
	void add14(position_t* p)
	{
		m_Q14.push_back(p);
	}
	void add41(position_t* p)
	{
		m_Q41.push_back(p);
	}

private:
	const position_t& m_parent;
	patch_corner* m_next_level;
	bool m_valid;
	bool m_normal_valid;
	bool m_nurbs_valid;
	bool m_smooth;
	position_t* m_boundary_left;
	position_t* m_boundary_right;
	positions_t m_face_vertices;
	positions_t m_corners;
	positions_t m_edge_vertices;
	position_t* m_point;
	position_t* m_nurbs_position;
	positions_t m_Q10;
	positions_t m_Q20;
	positions_t m_Q40;
	positions_t m_Q11;
	positions_t m_Q12;
	positions_t m_Q21;
	positions_t m_Q14;
	positions_t m_Q41;
	position_t m_normal;
	positions_t m_normals;

};

/// The border between 2 patches
class patch_border
{
public:
	patch_border(position_t** Parent, int Psize, patch_corner* Top, patch_corner* Bottom, const int Level);

	~patch_border();

	/// The size of this border
	int size() const
	{
		return m_size;
	}

	/// Get position at index i
	position_t* at(int i)
	{
		return_val_if_fail(i >= 0, 0);
		return_val_if_fail(i < m_size, 0);
		return m_points[i];
	}

	/// Get position at index i. Index can be inverted to go from back to start if Start == last point of parent
	position_t* at(int i, const position_t* const Start)
	{
		if (Start == m_parent[m_psize - 1])
		{
			return at(m_size-1-i);
		}
		return at(i);
	}

	/// Get normal at index i. Index can be inverted to go from back to start if Start == last point of parent
	position_t* at_normal(int i, const position_t* const Start)
	{
		if (Start == m_parent[m_psize - 1])
		{
			return m_normals[m_size-1-i];
		}
		return m_normals[i];
	}

	/// Parent position at index i
	position_t* const at_parent(int i) const
	{
		return_val_if_fail(i >= 0, 0);
		return_val_if_fail(i <= m_psize, 0);
		return m_parent[i];
	}

	/// The ith nurbs point, relative from Start
	position_t* at_nurbs(int i, const position_t* const Start)
	{
		return_val_if_fail(i >= 0, 0);
		return_val_if_fail(i <= m_nsize, 0);
		if (Start == m_points[m_size - 1])
		{
			return m_nurbs[m_nsize-1-i];
		}
		return m_nurbs[i];
	}

	/// Register a face vertex at corner
	void register_face_vertex(position_t* Corner, position_t* FaceVertex);

	/// Invalidate data
	void invalidate()
	{
		m_valid = false;
		position_t zero(0,0,0);
		for (int i = 0; i < m_size; ++i)
			*(m_points[i]) = zero;
		m_top->invalidate();
		m_bottom->invalidate();
	}

	/// Recalculate positions
	void update(int Level);

	/// Recalculate normals
	void update_normals();

	/// Recalculate corner normals
	void update_corner_normals()
	{
		m_top->update_normal();
		m_bottom->update_normal();
	}

	/// Recalculate nurbs control points
	void update_nurbs(int Level);

	/// Register mipmap
	void register_mipmap(sds_mipmap* map);

	/// Register points needed for position calculation if this border is part of the mesh boundary
	void register_boundary_points()
	{
		if (m_right)
			return;
		m_bottom->register_boundary_point(m_points[m_size-2]);
		m_top->register_boundary_point(m_points[1]);
		if (m_next_level)
			m_next_level->register_boundary_points();
	}

	/// next level
	patch_border& next_level()
	{
		if (m_next_level == 0)
		{
			m_next_level = new patch_border(m_points, m_size, m_top->next_level(), m_bottom->next_level(), m_level+1);
		}
		return *m_next_level;
	}

	/// Register points for PCCM corner smoothing
	void add11(position_t* Corner, position_t* p)
	{
		if (Corner == &m_top->nurbs_position())
		{
			m_top->add11(p);
		}
		else if (Corner == &m_bottom->nurbs_position())
		{
			m_bottom->add11(p);
		}
		else
		{
			return_if_fail(false);
		}
	}
	void add12(position_t* Corner, position_t* p)
	{
		if (Corner == &m_top->nurbs_position())
		{
			m_top->add12(p);
		}
		else if (Corner == &m_bottom->nurbs_position())
		{
			m_bottom->add12(p);
		}
		else
		{
			return_if_fail(false);
		}
	}
	void add21(position_t* Corner, position_t* p)
	{
		if (Corner == &m_top->nurbs_position())
		{
			m_top->add21(p);
		}
		else if (Corner == &m_bottom->nurbs_position())
		{
			m_bottom->add21(p);
		}
		else
		{
			return_if_fail(false);
		}
	}
	void add14(position_t* Corner, position_t* p)
	{
		if (Corner == &m_top->nurbs_position())
		{
			m_top->add14(p);
		}
		else if (Corner == &m_bottom->nurbs_position())
		{
			m_bottom->add14(p);
		}
		else
		{
			return_if_fail(false);
		}
	}
	void add41(position_t* Corner, position_t* p)
	{
		if (Corner == &m_top->nurbs_position())
		{
			m_top->add41(p);
		}
		else if (Corner == &m_bottom->nurbs_position())
		{
			m_bottom->add41(p);
		}
		else
		{
			return_if_fail(false);
		}
	}

private:
	position_t** m_parent;
	int m_psize;
	patch_corner* m_top;
	patch_corner* m_bottom;
	int m_level;
	bool m_valid;
	bool m_normals_valid;
	bool m_nurbs_valid;
	int m_size;
	int m_nsize;
	position_t** m_points;
	position_t** m_nurbs;
	position_t** m_normals;
	sds_mipmap* m_left;
	sds_mipmap* m_right;
	patch_border* m_next_level;
};

/// The interior points for a face
class sds_mipmap
{
public:
	sds_mipmap(patch_border& Top, patch_border& Right, patch_border& Bottom, patch_border& Left, const point_array& ParentPoints, int Level, int Size);

	~sds_mipmap();

	/// Generate level
	void generate(int Level)
	{
		if (Level > m_level+1)
		{
			return_if_fail(m_next_level);
			m_next_level->generate(Level);
			return;
		}
		else if (Level == m_level + 1)
		{
			m_next_level = new sds_mipmap(m_top.next_level(), m_right.next_level(), m_bottom.next_level(), m_left.next_level(), m_points, Level, static_cast<int>(std::pow(2.0, static_cast<double>(Level))+1));
		}
	}

	/// Mark mipmap as invalid
	void invalidate()
	{
		m_valid = false;
		m_top.invalidate();
		m_right.invalidate();
		m_bottom.invalidate();
		m_left.invalidate();
		if (m_next_level)
			m_next_level->invalidate();
	}

	/// Recalculate positions for this mipmap, exploiting the array structure.
	void update(int Level);

	/// update border
	void update_border(patch_border* border);

	/// update nurbs
	void update_nurbs(int Level);

	/// Point at index i relative for the border
	const position_t& at_neighbour(const int i, patch_border* Border);

	/// Points at level
	const point_array& points(int Level) const
	{
		if (Level == m_level)
			return m_points;
		return_val_if_fail(m_next_level, m_points);
		return m_next_level->points(Level);
	}

	/// Nurbs control points at level
	const point_array& nurbs(int Level) const
	{
		if (Level == m_level)
			return m_nurbs;
		return_val_if_fail(m_next_level, m_nurbs);
		return m_next_level->nurbs(Level);
	}

	/// Nurbs knots at level
	float* const knots(int Level) const
	{
		if (Level == m_level)
			return m_knots;
		return_val_if_fail(m_next_level, m_knots);
		return m_next_level->knots(Level);
	}

	/// Normals at level
	const point_array& normals(int Level) const
	{
		if (Level == m_level)
			return m_normals;
		return_val_if_fail(m_next_level, m_normals);
		return m_next_level->normals(Level);
	}

	/// Recalculate normals.
	void update_normals(int Level);

	/// Update borders
	void update_borders(int Level)
	{
		if (Level == m_level)
		{
			m_top.update(Level);
			m_right.update(Level);
			m_bottom.update(Level);
			m_left.update(Level);
			return;
		}
		return_if_fail(m_next_level);
		m_next_level->update_borders(Level);
	}

	/// Update border normals
	void update_border_normals(int Level)
	{
		if (Level == m_level)
		{
			m_top.update_normals();
			m_right.update_normals();
			m_bottom.update_normals();
			m_left.update_normals();
			return;
		}
		return_if_fail(m_next_level);
		m_next_level->update_border_normals(Level);
	}

	/// Update corner normals
	void update_corner_normals(int Level)
	{
		if (Level == m_level)
		{
			m_top.update_corner_normals();
			m_right.update_corner_normals();
			m_bottom.update_corner_normals();
			m_left.update_corner_normals();
			return;
		}
		return_if_fail(m_next_level);
		m_next_level->update_corner_normals(Level);
	}

private:
	patch_border& m_top;
	patch_border& m_right;
	patch_border& m_bottom;
	patch_border& m_left;
	const point_array& m_parent_points;
	int m_level;
	int m_size;
	bool m_valid;
	bool m_normals_valid;
	bool m_nurbs_valid;
	point_array m_points;
	point_array m_normals;
	point_array m_nurbs;
	sds_mipmap* m_next_level;
	float* m_knots;
	int m_ksize;
};

/// A centroid of a face of the original mesh
class face_vertex :
	public valid_point
{
public:
	face_vertex()
	{}

	~face_vertex()
	{
		for (unsigned long i = 0; i < mipmaps.size(); ++i)
			delete mipmaps[i];
	}

	/// recalculate if needed, using corners of Face, and invalidate child mipmaps.
	void update(const_positions_t& Points);

	/// Update mipmaps
	void update_mipmaps(int Level)
	{
		for (unsigned long i = 0; i < mipmaps.size(); ++i)
			mipmaps[i]->update(Level);
	}

	/// update nurbs
	void update_nurbs(int Level)
	{
		for (unsigned long i = 0; i < mipmaps.size(); ++i)
			mipmaps[i]->update_nurbs(Level);
	}

	/// corners of the face
	sdspoints_t corners;

	/// edge vertices for all edges of the face
	edgevertices_t edge_vertices;

	/// mipmaps of all quads with this face as parent
	typedef std::vector<sds_mipmap*> mipmaps_t;
	mipmaps_t mipmaps;
};

/// Corner smoothing algorithm for PCCM. See J???rg Peters' PCCM paper for an explanation of the calculations.
class corner_smoothing
{
	// FIXME: Broken somewhere, resulting in points at infinity
public:
	static corner_smoothing& get_instance();

	/// Factor r
	position_t r(const positions_t& Q40);

	/// add h_i
	void add_h(position_t r, positions_t& Q40, positions_t& Q41, positions_t& Q14);

	/// new Q10
	void q10(positions_t& Q10, const position_t& Q00);

	/// new Q20
	void q20(positions_t& Q20, const positions_t& Q40, const positions_t& Q10, const position_t& Q00);

	/// new Q11
	void q11(positions_t& Q11, const positions_t& Q10, const positions_t& Q40, const positions_t& Q20);

	/// Q21 and Q12
	void q12(positions_t& Q21, positions_t& Q12, const positions_t& Q20, const positions_t& Q20_orig);
private:
	typedef boost::multi_array<double, 2> matrix;
	typedef std::vector<matrix*> matrices_t;
	corner_smoothing() {}
	/// Apply matrix A
	void A(unsigned long n, positions_t& Q10);
	/// Apply matrix B
	void B(unsigned long n, positions_t& Q);
	/// Matrix multiplication
	void mat_mult(matrix& A, positions_t& p);

	int powi(int i)
	{
		if ((i % 2) == 0)
			return 1;
		return -1;
	}
	static corner_smoothing* m_Instance;
	matrices_t m_A;
	matrices_t m_B;
};

/// Retains the first level of SDS and the actual input mesh.
template<typename mesh_type> class cache_input
{
public:
	virtual ~cache_input() {}
	
	/// update the first level using the latest input and place all updated face_vertices in updated_maps. If all is true, everything gets updated. First update selected face_vertices using corner point info, then update all edge_vertices and finally all sds_vertices using the update() methods in those classes.
	virtual void update(bool all, facevertices_t& updated_maps) = 0;

	/// Set the input. This depends on the client of course. Returns false if the new input is incompatible with the cache (i.e. different point or faces count).
	virtual bool set_input(const mesh_type* Input) = 0;

	/// The face vertices. Every one of these corresponds to an original face of the mesh. The client must make sure they are properly connected using edge_vertices and sds_vertices.
	facevertices_t& face_vertices()
	{
		return m_face_vertices;
	}

protected:
	/// The face vertices created by the first step.
	facevertices_t m_face_vertices;

};

/// Implementation of the higher levels of the SDS cache. Takes face_vertex as input, and generates and updates the required sds_mipmaps, where each sds_mipmap corresponds to a quad of the one time subdivided mesh.
class higher_level_cache
{
public:
	higher_level_cache(const unsigned long Levels, const bool UseNURBS, facevertices_t& Faces);
	~higher_level_cache();

	/// Update point positions for all faces given in UpdatedMaps
	void update(facevertices_t& UpdatedMaps);

	/// Get level number
	const unsigned long levels() const
	{
		return m_levels;
	}

	const bool use_nurbs() const
	{
		return m_use_nurbs;
	}

private:
	const unsigned long m_levels;
	const bool m_use_nurbs;

	typedef std::map<valid_point*, patch_border*> inner_bordermap_t;
	typedef std::map<valid_point*, inner_bordermap_t*> bordermap_t;
	typedef std::map<valid_point*, patch_corner*> cornermap_t;
	bordermap_t m_borders;
	std::vector<point_array*> m_patch_parents;
	std::vector<position_t**> m_border_parents;
	cornermap_t m_patch_corners;

	// get or create the border line with given start and end points
	patch_border* get_border(valid_point* Start, valid_point* End);

	// only retrieve the border from the map
	patch_border* retrieve_border(valid_point* Start, valid_point* End, bool recurse = true);

	// generate cache up to level
	void generate(const int level, facevertices_t& Faces);

	// get the patch_corner associated with the Point
	patch_corner* get_patch_corner(valid_point* Point);
};

/// Generic interface to access the subdivider. This should be all the client sees.mesh_type is the mesh structure used by the client program.
template <typename mesh_type> class catmull_clark_cache
{
public:

	catmull_clark_cache() : m_first_level_cache(0), m_higher_level_cache(0), m_levels(2), m_use_nurbs(false), m_valid(false) {}

	virtual ~catmull_clark_cache()
	{
		delete m_first_level_cache;
		delete m_higher_level_cache;
	}

	/// The number of SDS levels required from 1 to ...
	void set_levels(unsigned long Levels)
	{
		return_if_fail(Levels > 0);
		if (m_levels != Levels)
			m_valid = false;
		m_levels = Levels;
	}

	/// Turn NURBS algorithm on/off
	void use_nurbs(bool UseNurbs)
	{
		if (m_use_nurbs != UseNurbs)
			m_valid = false;
		m_use_nurbs = UseNurbs;
	}

	/// Output of the subdivided mesh, which may be placed in the Output argument, or drawn directly to an output device if Output == 0.
	void output(mesh_type* Output = 0)
	{
		if (m_first_level_cache->face_vertices().size() == 0)
			return;
		if (m_use_nurbs)
		{
			client_output_nurbs(Output);
		}
		else
		{
			client_output(Output);
		}
	}

	/// Update the cache.
	void update()
	{
		return_if_fail(m_first_level_cache);
		if (m_valid)
		{
			try
			{
				update_detail(false);
			}
			catch (cache_invalid_exception& E)
			{
//				k3d::log() << debug << "Invalidating cache during update" << std::endl;
				m_valid = false;
				set_input(m_first_level_cache_mesh);
				update();
			}
		}
		else // invalid cache
		{
			delete m_higher_level_cache;
//			k3d::log() << debug << "cache reset" << std::endl;
			m_higher_level_cache = new higher_level_cache(m_levels, m_use_nurbs, m_first_level_cache->face_vertices());
			update_detail(true);
			m_valid = true;
		}
	}

	/// Set the input,using the structure of the client program
	void set_input(const mesh_type* const Input)
	{
		m_first_level_cache_mesh = Input;
		if (m_valid && m_first_level_cache && m_first_level_cache->set_input(Input))
			return;
		delete m_first_level_cache;
		m_first_level_cache = create_cache_input(Input);
		m_valid = false;
	}

protected:
	/// Create a new cache_input using the particular structure from the client program. Needs to be implemented by the client.
	virtual cache_input<mesh_type>* create_cache_input(const mesh_type* Input) = 0;
	/// Client-specific output part, which may assume to work on a valid and updated cache.
	virtual void client_output(mesh_type* Output = 0) = 0;
	virtual void client_output_nurbs(mesh_type* Output = 0) = 0;

	// cache for the first level (allowing non-quadrilateral faces)
	cache_input<mesh_type>* m_first_level_cache; // first level cache
	// input mesh
	const mesh_type* m_first_level_cache_mesh;
	// cache for levels > 1 (taking advantage of all-quadrilateral structure)
	higher_level_cache* m_higher_level_cache;
	// number of levels to use
	unsigned long m_levels;
	// calculate NURBS control points?
	bool m_use_nurbs;
	// false if the cache became invalid
	bool m_valid;
private:
	// update helper
	void update_detail(bool All)
	{
		// try to retain our sanity:
		return_if_fail(m_first_level_cache);
		return_if_fail(m_higher_level_cache);

		// update first level:
		facevertices_t updated_faces;
		m_first_level_cache->update(All, updated_faces);
		// and the higher levels:
		m_higher_level_cache->update(updated_faces);
	}	
};

position_t normal(const position_t& p1, const position_t& p2, const position_t& p3, const position_t& p4);

position_t normal(const position_t& p1, const position_t& p2, const position_t& p3);

} // namespace sds



} // namespace k3d

#endif


