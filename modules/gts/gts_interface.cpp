// K-3D
// Copyright (c) 2004-2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Implements GTS interface
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "gts_interface.h"

#include <set>
#include <utility>

namespace libk3dgts
{

namespace detail
{

k3d::legacy::mesh::points_t points;
edges_t edges;
triangles_t triangles;

void get_vertex(GtsPoint* p, gpointer* data)
{
	double x = static_cast<double>(p->x);
	double y = static_cast<double>(p->y);
	double z = static_cast<double>(p->z);
	points.push_back(new k3d::legacy::point(x, y, z));

	// Hash for edges indices
	g_hash_table_insert(static_cast<GHashTable*>(data[1]), p, GUINT_TO_POINTER(++(*((guint*) data[0]))));
}

void get_edge(GtsSegment* s, gpointer* data)
{
	edge_t edge;
	edge.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[1]), s->v1)));
	edge.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[1]), s->v2)));
	edges.push_back(edge);

	// Hash for triangles indices
	g_hash_table_insert(static_cast<GHashTable*>(data[2]), s, GUINT_TO_POINTER(++(*((guint*) data[0]))));
}

void get_face(GtsTriangle* t, gpointer* data)
{
	triangle_t triangle;
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e1)));
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e2)));
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e3)));
	triangles.push_back(triangle);
}

} // namespace detail

bool copy_surface(GtsSurface* Surface, k3d::legacy::mesh& Mesh)
{
	// Clear global arrays ...
	detail::points.clear();
	detail::edges.clear();
	detail::triangles.clear();

	// Extract vertices, edges and triangles ...
	gpointer data[3];
	guint n;
	GHashTable *vindex, *eindex;
	data[0] = &n;
	data[1] = vindex = g_hash_table_new(0, 0);
	data[2] = eindex = g_hash_table_new(0, 0);

	n = 0;
	gts_surface_foreach_vertex(Surface, (GtsFunc)detail::get_vertex, data);
	n = 0;
	gts_surface_foreach_edge(Surface, (GtsFunc)detail::get_edge, data);
	gts_surface_foreach_face(Surface, (GtsFunc)detail::get_face, data);
	g_hash_table_destroy(vindex);
	g_hash_table_destroy(eindex);

	// Create K-3D mesh
	Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
	k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

	// Transform points ...
	k3d::legacy::mesh::points_t::iterator p_end(detail::points.end());
	for(k3d::legacy::mesh::points_t::iterator p = detail::points.begin(); p != p_end; ++p)
	{
		double x = (*p)->position[0];
		double y = (*p)->position[1];
		double z = (*p)->position[2];

		// Transform from GTS to K-3D rigth-handed system
		(*p)->position[0] = -x;
		(*p)->position[1] = z;
		(*p)->position[2] = -y;

		// Add point to k3d::legacy::mesh
		Mesh.points.push_back(*p);
	}

	// Create triangles ...
	triangles_t::const_iterator t_end(detail::triangles.end());
	for(triangles_t::const_iterator t = detail::triangles.begin(); t != t_end; ++t)
	{
		edge_t gts_edge1 = detail::edges[(*t)[0] - 1];
		edge_t gts_edge2 = detail::edges[(*t)[1] - 1];
		//edge_t gts_edge3 = detail::edges[(*t)[2] - 1];

		unsigned long edge1_first = gts_edge1[0] - 1;
		unsigned long edge1_second = gts_edge1[1] - 1;
		unsigned long edge2_first = gts_edge2[0] - 1;
		unsigned long edge2_second = gts_edge2[1] - 1;
		//unsigned long edge3_first = gts_edge3[0] - 1;
		//unsigned long edge3_second = gts_edge3[1] - 1;

		// Determine order of vertices
		unsigned long p[3];
		if(edge1_first == edge2_first)
		{
			p[0] = edge1_second;
			p[1] = edge1_first;
			p[2] = edge2_second;
		}
		else if(edge1_first == edge2_second)
		{
			p[0] = edge1_second;
			p[1] = edge1_first;
			p[2] = edge2_first;
		}
		else if(edge1_second == edge2_first)
		{
			p[0] = edge1_first;
			p[1] = edge1_second;
			p[2] = edge2_second;
		}
		else
		// edge1_second == edge2_second
		{
			p[0] = edge1_first;
			p[1] = edge1_second;
			p[2] = edge2_first;
		}

		// Create a triangle ...
		k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(detail::points[p[0]]);
		k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(detail::points[p[1]]);
		k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(detail::points[p[2]]);

		// Invert face (because of coordinate system switch)
		edge1->face_clockwise = edge3;
		edge3->face_clockwise = edge2;
		edge2->face_clockwise = edge1;

		k3d::legacy::face* const face = new k3d::legacy::face(edge1, 0);
		return_val_if_fail(face, false);

		polyhedron.faces.push_back(face);
	}

	return_val_if_fail(k3d::legacy::is_valid(polyhedron), false);
	//assert_warning(k3d::legacy::is_solid(polyhedron));

	k3d::legacy::set_companions(polyhedron);

	return true;
}

namespace detail
{

/// std::pair equivalent that maintains the order of its members
template<typename T1, typename T2>
class ordered_pair;

template<typename T1, typename T2>
bool operator<(const ordered_pair<T1,T2>& lhs, const ordered_pair<T1,T2>& rhs);

template<typename T1, typename T2>
class ordered_pair :
	public std::pair<T1, T2>
{
public:
	typedef T1 first_type;
	typedef T2 second_type;

	T1 first;
	T2 second;

	explicit ordered_pair()
	{
	}

	explicit ordered_pair(const T1& First, const T2& Second) :
		first(First < Second ? First : Second),
		second(First < Second ? Second : First)
	{
	}

	explicit ordered_pair(const k3d::legacy::split_edge* Edge) :
		first(Edge->vertex < Edge->face_clockwise->vertex ? Edge->vertex : Edge->face_clockwise->vertex),
		second(Edge->vertex < Edge->face_clockwise->vertex ? Edge->face_clockwise->vertex : Edge->vertex)
	{
	}

	friend bool operator< <>(const ordered_pair& lhs, const ordered_pair& rhs);
};

template<typename T1, typename T2>
bool operator<(const ordered_pair<T1,T2>& lhs, const ordered_pair<T1,T2>& rhs)
{
	if(lhs.first != rhs.first)
		return lhs.first < rhs.first;

	return lhs.second < rhs.second;
}

/// Test to see if a mesh is all triangles
bool triangle_test(const k3d::legacy::mesh& Mesh)
{
	k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron;
	for(polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
	{
		k3d::legacy::polyhedron::faces_t::iterator face;
		for(face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			k3d::legacy::split_edge* first = (*face)->first_edge;
			// Skip empty faces
			if(!first)
				continue;

			k3d::legacy::split_edge* current_edge = first->face_clockwise;
			// Skip one-edged faces
			if(!current_edge)
				continue;

			unsigned long edge_number = 1;

			while(current_edge != first)
			{
				edge_number++;
				current_edge = current_edge->face_clockwise;
			}

			if(edge_number != 3)
				return false;
		}
	}

	return true;
}

typedef ordered_pair<k3d::legacy::point*, k3d::legacy::point*> ordered_edge_t;
typedef std::set<ordered_edge_t> ordered_edges_t;
typedef std::vector<ordered_edge_t> edge_triangle_t;
typedef std::vector<edge_triangle_t> edge_triangles_t;

} // namespace detail

GtsSurface* gts_surface(const k3d::legacy::mesh& Mesh)
{
	// Process only if object is triangulated ...
	if(!detail::triangle_test(Mesh))
		return 0;

	// Create a mapping of points-to-one-based-indices ...
	std::map<k3d::legacy::point*, unsigned long> point_map;
	k3d::legacy::mesh::points_t::const_iterator p_end(Mesh.points.end());
	for(k3d::legacy::mesh::points_t::const_iterator point = Mesh.points.begin(); point != p_end; ++point)
		point_map[*point] = point_map.size();

	// Build the collection of edges ...
	detail::ordered_edges_t edges;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			k3d::legacy::split_edge* first_edge = (*face)->first_edge;
			k3d::legacy::split_edge* edge = first_edge;
			do
			{
				edges.insert(detail::ordered_edge_t(edge));

				edge = edge->face_clockwise;
			}
			while(edge != first_edge);
		}

	// Create a mapping of edges-to-one-based-indices ...
	std::map<detail::ordered_edge_t, unsigned long> edge_map;
	for(detail::ordered_edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
		edge_map[*edge] = edge_map.size();

	// Build triangle collection ...
	detail::edge_triangles_t triangle_collection;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			detail::edge_triangle_t triangle;

			k3d::legacy::split_edge* first = (*face)->first_edge;
			// Skip empty faces
			if(!first)
				continue;

			triangle.push_back(detail::ordered_edge_t(first));

			k3d::legacy::split_edge* current_edge = first->face_clockwise;
			// Skip one-edged faces
			if(!current_edge)
				continue;

			triangle.push_back(detail::ordered_edge_t(current_edge));

			current_edge = current_edge->face_clockwise;
			if(current_edge)
				if(current_edge->face_clockwise == first)
				{
					triangle.push_back(detail::ordered_edge_t(current_edge));
					triangle_collection.push_back(triangle);
				}
		}

	// Create new GTS surface
	GtsSurface* surface = gts_surface_new(gts_surface_class(), gts_face_class(), gts_edge_class(), gts_vertex_class());
	return_val_if_fail(surface, 0);

	// Add points
	GtsVertex** gts_vertices = static_cast<GtsVertex**>(g_malloc((Mesh.points.size() + 1) * sizeof(GtsVertex*)));
	guint n = 0;
	for(k3d::legacy::mesh::points_t::const_iterator point = Mesh.points.begin(); point != p_end; ++point)
	{
		// Transform coords back to GTS convention
		const k3d::point3 coords = (*point)->position;
		GtsObject* new_vertex = gts_object_new (GTS_OBJECT_CLASS (surface->vertex_class));
		GtsVertex* v = GTS_VERTEX (new_vertex);

		v->p.x = -coords[0];
		v->p.y = -coords[2];
		v->p.z = coords[1];

		gts_vertices[n++] = v;
	}

	// Add edges
	GtsEdge** gts_edges = static_cast<GtsEdge**>(g_malloc((edges.size() + 1) * sizeof(GtsEdge*)));
	n = 0;
	for(detail::ordered_edges_t::iterator edge = edges.begin(); edge != edges.end(); ++edge)
	{
		guint p1 = point_map[edge->first];
		guint p2 = point_map[edge->second];
		GtsEdge* new_edge = gts_edge_new (surface->edge_class, gts_vertices[p1 - 1], gts_vertices[p2 - 1]);

		gts_edges[n++] = new_edge;
	}

	// Add triangles
	for(detail::edge_triangles_t::iterator triangle = triangle_collection.begin(); triangle != triangle_collection.end(); ++triangle)
	{
		assert_warning(triangle->size() == 3);

		guint s1 = edge_map[(*triangle)[0]];
		guint s2 = edge_map[(*triangle)[1]];
		guint s3 = edge_map[(*triangle)[2]];

		// Invert face (because of coordinate system switch)
		GtsFace* new_face = gts_face_new (surface->face_class,
			gts_edges[s1 - 1], gts_edges[s3 - 1], gts_edges[s2 - 1]);

		gts_surface_add_face (surface, new_face);
	}

	g_free (gts_vertices);
	g_free (gts_edges);

	return surface;
}

namespace detail
{

/// GTS Vertex overloading

#define GTS_MYVERTEX(obj)	GTS_OBJECT_CAST (obj, detail::GtsMyVertex, detail::gts_myvertex_class ())
#define GTS_MYVERTEX_CLASS(klass) GTS_OBJECT_CLASS_CAST (klass, detail::GtsMyVertexClass, detail::gts_myvertex_class())
#define GTS_IS_MYVERTEX(obj)	(gts_object_is_from_class (obj, detail::gts_myvertex_class ()))

// Define structure for myvertex which is derived from vertex and adds number variable
struct _GtsMyVertex
{
	GtsVertex parent;
	gulong number;
};

// Default data class definition following standard
struct _GtsMyVertexClass
{
	GtsVertexClass parent_class;
};

// Create aliases for myvertex data and myvertex data class
typedef struct _GtsMyVertex          GtsMyVertex;
typedef struct _GtsMyVertexClass     GtsMyVertexClass;

// Prototype for myvertex data class function following standard
GtsMyVertexClass * gts_myvertex_class (void);

// Define an init function for myvertex data class
static void myvertex_init (GtsMyVertex * myvertex)
{
	myvertex->number = 0;
}

// gts_myvertex_class:
// define internal function to create myvertex data class following standard
GtsMyVertexClass* gts_myvertex_class(void)
{
	static GtsMyVertexClass* klass = 0;

	if(klass == 0)
	{
		GtsObjectClassInfo myvertex_info = {
			"GtsMyVertex",
			sizeof (GtsMyVertex),
			sizeof (GtsMyVertexClass),
			(GtsObjectClassInitFunc) 0,
			(GtsObjectInitFunc) myvertex_init,
			(GtsArgSetFunc) 0,
			(GtsArgGetFunc) 0
			};

		klass = static_cast<GtsMyVertexClass*>(gts_object_class_new (GTS_OBJECT_CLASS (gts_vertex_class ()), &myvertex_info));
	}

	return klass;
}

/// GTS Edge overloading

#define GTS_MYEDGE(obj)	GTS_OBJECT_CAST (obj, detail::GtsMyEdge, detail::gts_myedge_class ())
#define GTS_MYEDGE_CLASS(klass) GTS_OBJECT_CLASS_CAST (klass, detail::GtsMyEdgeClass, detail::gts_myedge_class())
#define GTS_IS_MYEDGE(obj)	(gts_object_is_from_class (obj, detail::gts_myedge_class ()))

// Define structure for myedge which is derived from edge and adds number variable
struct _GtsMyEdge
{
	GtsEdge parent;
	gulong number;
};

// Default data class definition following standard
struct _GtsMyEdgeClass
{
	GtsEdgeClass parent_class;
};

// Create aliases for myedge data and myedge data class
typedef struct _GtsMyEdge          GtsMyEdge;
typedef struct _GtsMyEdgeClass     GtsMyEdgeClass;

// Prototype for myedge data class function following standard
GtsMyEdgeClass* gts_myedge_class(void);

// Define an init function for myedge data class
static void myedge_init (GtsMyEdge * myedge)
{
	myedge->number = 0;
}

// gts_myedge_class:
// define internal function to create myedge data class following standard
GtsMyEdgeClass* gts_myedge_class(void)
{
	static GtsMyEdgeClass* klass = 0;

	if (klass == 0)
	{
		GtsObjectClassInfo myedge_info = {
			"GtsMyEdge",
			sizeof (GtsMyEdge),
			sizeof (GtsMyEdgeClass),
			(GtsObjectClassInitFunc) 0,
			(GtsObjectInitFunc) myedge_init,
			(GtsArgSetFunc) 0,
			(GtsArgGetFunc) 0
			};

		klass = static_cast<GtsMyEdgeClass*>(gts_object_class_new (GTS_OBJECT_CLASS (gts_edge_class ()), &myedge_info));
	}

	return klass;
}

/// GTS Face overloading

#define GTS_MYFACE(obj)		GTS_OBJECT_CAST (obj, detail::GtsMyFace, detail::gts_myface_class ())
#define GTS_MYFACE_CLASS(klass)	GTS_OBJECT_CLASS_CAST (klass, detail::GtsMyFaceClass, detail::gts_myface_class())
#define GTS_IS_MYFACE(obj)	(gts_object_is_from_class (obj, detail::gts_myface_class ()))

// Define structure for myface which is derived from face and adds more control variables
struct _GtsMyFace
{
	GtsFace parent;
	gulong number;
	gulong polyhedron_number;
	gulong polygon_number;
	gulong polygon_size;
};

// Default data class definition following standard
struct _GtsMyFaceClass
{
	GtsFaceClass parent_class;
};

// Create aliases for myface data and myface data class
typedef struct _GtsMyFace          GtsMyFace;
typedef struct _GtsMyFaceClass     GtsMyFaceClass;

// Prototype for myface data class function following standard
GtsMyFaceClass * gts_myface_class (void);

// Define an init function for myface data class
static void myface_init (GtsMyFace * myface)
{
	myface->number = 0;
	myface->polyhedron_number = 0;
	myface->polygon_number = 0;
	myface->polygon_size = 0;
}

// gts_myface_class:
// define internal function to create myface data class following standard
GtsMyFaceClass* gts_myface_class (void)
{
	static GtsMyFaceClass* klass = 0;

	if(klass == 0)
	{
		GtsObjectClassInfo myface_info = {
			"GtsMyFace",
			sizeof (GtsMyFace),
			sizeof (GtsMyFaceClass),
			(GtsObjectClassInitFunc) 0,
			(GtsObjectInitFunc) myface_init,
			(GtsArgSetFunc) 0,
			(GtsArgGetFunc) 0
			};

		klass = static_cast<GtsMyFaceClass*>(gts_object_class_new (GTS_OBJECT_CLASS (gts_face_class ()), &myface_info));
	}

	return klass;
}

} // namespace detail

GtsSurface* gts_polygonal_surface(const k3d::legacy::mesh& Mesh, unsigned long& PolyhedronIndex)
{
	k3d::legacy::mesh::points_t new_points;
	std::copy(Mesh.points.begin(), Mesh.points.end(), std::back_inserter(new_points));

	// Triangulate polyhedra ...
	detail::ordered_edges_t edge_collection;
	detail::edge_triangles_t triangle_collection;
	std::vector<unsigned long> triangles_polyhedron;
	std::vector<unsigned long> triangles_polygon;
	std::vector<unsigned long> triangles_sizes;
	unsigned long polygon_number = 0;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron_iterator = Mesh.polyhedra.begin(); polyhedron_iterator != Mesh.polyhedra.end(); ++polyhedron_iterator)
	{
		k3d::legacy::polyhedron& polyhedron = **polyhedron_iterator;

		// Process each face ...
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); face++)
		{
			k3d::legacy::polyhedron::faces_t faces;
			faces.push_back(*face);

			k3d::legacy::split_edge* current_edge = (*face)->first_edge;
			while(current_edge && current_edge != (*face)->first_edge)
			{
				current_edge = current_edge->face_clockwise;
			}

			// Triangulate ...
			k3d::legacy::polyhedron::faces_t new_faces;
			k3d::legacy::triangulate(faces, new_faces, new_points);

			// For each face ...
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = new_faces.begin(); face != new_faces.end(); face++)
			{
				detail::edge_triangle_t triangle;
				k3d::legacy::split_edge* current_edge = (*face)->first_edge;
				assert_warning(current_edge);
				triangle.push_back(detail::ordered_edge_t(current_edge));

				current_edge = current_edge->face_clockwise;
				assert_warning(current_edge);
				triangle.push_back(detail::ordered_edge_t(current_edge));

				current_edge = current_edge->face_clockwise;
				assert_warning(current_edge && current_edge->face_clockwise == (*face)->first_edge);
				triangle.push_back(detail::ordered_edge_t(current_edge));

				triangle_collection.push_back(triangle);
				triangles_polyhedron.push_back(PolyhedronIndex);
				triangles_polygon.push_back(polygon_number);
				triangles_sizes.push_back(new_faces.size());

				// Save edges for mapping
				std::copy(triangle.begin(), triangle.end(), std::inserter(edge_collection, edge_collection.end()));
			}

			// Update counter
			polygon_number++;
		}

		// Update counter
		PolyhedronIndex++;
	}

	// Create new GTS surface
	GtsSurface* surface = gts_surface_new(gts_surface_class(), GTS_FACE_CLASS (detail::gts_myface_class()), GTS_EDGE_CLASS (detail::gts_myedge_class()), GTS_VERTEX_CLASS (detail::gts_myvertex_class()));
	return_val_if_fail(surface, 0);

	// Add points
	GtsVertex** gts_vertices = static_cast<GtsVertex**>(g_malloc((new_points.size() + 1) * sizeof(GtsVertex*)));
	std::map<k3d::legacy::point*, unsigned long> point_map;
	guint n = 0;
	gulong vertex_number = 1;
	k3d::legacy::mesh::points_t::const_iterator p_end(new_points.end());
	for(k3d::legacy::mesh::points_t::const_iterator point = new_points.begin(); point != p_end; ++point)
	{
		// Transform coords back to GTS convention
		const k3d::point3 coords = (*point)->position;
		GtsObject* new_vertex = gts_object_new (GTS_OBJECT_CLASS (surface->vertex_class));
		detail::GtsMyVertex* myvertex = GTS_MYVERTEX(new_vertex);
		myvertex->number = vertex_number++;
		GtsVertex* v = GTS_VERTEX (new_vertex);

		v->p.x = -coords[0];
		v->p.y = -coords[2];
		v->p.z = coords[1];

		gts_vertices[n++] = v;

		// Create a mapping of points-to-one-based-indices ...
		point_map[*point] = n;
	}

	// Create a mapping of edges-to-one-based-indices ...
	std::map<detail::ordered_edge_t, unsigned long> edge_map;
	for(detail::ordered_edges_t::iterator edge = edge_collection.begin(); edge != edge_collection.end(); ++edge)
		edge_map[*edge] = edge_map.size();

	// Add edges
	GtsEdge** gts_edges = static_cast<GtsEdge**>(g_malloc((edge_collection.size() + 1) * sizeof(GtsEdge*)));
	n = 0;
	gulong edge_number = 1;
	for(detail::ordered_edges_t::iterator edge = edge_collection.begin(); edge != edge_collection.end(); ++edge)
	{
		guint p1 = point_map[edge->first];
		guint p2 = point_map[edge->second];
		GtsEdge* new_edge = gts_edge_new (surface->edge_class, gts_vertices[p1 - 1], gts_vertices[p2 - 1]);

		gts_edges[n++] = new_edge;

		detail::GtsMyEdge* myedge = GTS_MYEDGE(new_edge);
		myedge->number = edge_number++;
	}

	// Add triangles
	gulong face_number = 1;
	unsigned long index = 0;
	for(detail::edge_triangles_t::iterator triangle = triangle_collection.begin(); triangle != triangle_collection.end(); ++triangle)
	{
		assert_warning(triangle->size() == 3);

		guint s1 = edge_map[(*triangle)[0]];
		guint s2 = edge_map[(*triangle)[1]];
		guint s3 = edge_map[(*triangle)[2]];

		// Invert face (because of coordinate system switch)
		GtsFace* new_face = gts_face_new (surface->face_class,
			gts_edges[s1 - 1], gts_edges[s3 - 1], gts_edges[s2 - 1]);

		detail::GtsMyFace* myface = GTS_MYFACE(new_face);
		myface->number = face_number++;
		myface->polyhedron_number = triangles_polyhedron[index];
		myface->polygon_number = triangles_polygon[index];
		myface->polygon_size = triangles_sizes[index];

		gts_surface_add_face (surface, new_face);
	}

	g_free (gts_vertices);
	g_free (gts_edges);

	return surface;
}

namespace detail
{

typedef std::vector<unsigned long> info_t;
info_t info_polyhedron_number;
info_t info_polygon_number;
info_t info_polygon_size;

void get_numbered_vertex(GtsMyVertex* v, gpointer* data)
{
	GtsPoint* p = GTS_POINT (v);

	double x = static_cast<double>(p->x);
	double y = static_cast<double>(p->y);
	double z = static_cast<double>(p->z);
	points.push_back(new k3d::legacy::point(x, y, z));

	// Hash for edges indices
	g_hash_table_insert(static_cast<GHashTable*>(data[1]), p, GUINT_TO_POINTER(++(*((guint*) data[0]))));
}

void get_numbered_edge(GtsMyEdge* e, gpointer* data)
{
	GtsSegment* s = GTS_SEGMENT(e);

	edge_t edge;
	edge.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[1]), s->v1)));
	edge.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[1]), s->v2)));
	edges.push_back(edge);

	// Hash for triangles indices
	g_hash_table_insert(static_cast<GHashTable*>(data[2]), s, GUINT_TO_POINTER(++(*((guint*) data[0]))));
}

void get_numbered_face(GtsMyFace* f, gpointer* data)
{
	info_polyhedron_number.push_back(f->polyhedron_number);
	info_polygon_number.push_back(f->polygon_number);
	info_polygon_size.push_back(f->polygon_size);

	GtsTriangle* t = GTS_TRIANGLE(f);

	triangle_t triangle;
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e1)));
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e2)));
	triangle.push_back(GPOINTER_TO_UINT(g_hash_table_lookup(static_cast<GHashTable*>(data[2]), t->e3)));
	triangles.push_back(triangle);
}

} // namespace detail

bool copy_polygonal_surface(GtsSurface* Surface, const k3d::legacy::mesh& OldMesh, k3d::legacy::mesh& Mesh)
{
	// Clear global arrays ...
	detail::points.clear();
	detail::edges.clear();
	detail::triangles.clear();
	detail::info_polyhedron_number.clear();
	detail::info_polygon_number.clear();
	detail::info_polygon_size.clear();

	// Extract vertices, edges and triangles ...
	gpointer data[3];
	guint n;
	GHashTable *vindex, *eindex;
	data[0] = &n;
	data[1] = vindex = g_hash_table_new(0, 0);
	data[2] = eindex = g_hash_table_new(0, 0);

	n = 0;
	gts_surface_foreach_vertex(Surface, (GtsFunc)detail::get_numbered_vertex, data);
	n = 0;
	gts_surface_foreach_edge(Surface, (GtsFunc)detail::get_numbered_edge, data);
	gts_surface_foreach_face(Surface, (GtsFunc)detail::get_numbered_face, data);
	g_hash_table_destroy(vindex);
	g_hash_table_destroy(eindex);

	// Create K-3D mesh
	Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
	k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

	// Transform points ...
	k3d::legacy::mesh::points_t::iterator p_end(detail::points.end());
	for(k3d::legacy::mesh::points_t::iterator p = detail::points.begin(); p != p_end; ++p)
	{
		double x = (*p)->position[0];
		double y = (*p)->position[1];
		double z = (*p)->position[2];

		// Transform from GTS to K-3D rigth-handed system
		(*p)->position[0] = -x;
		(*p)->position[1] = z;
		(*p)->position[2] = -y;

		// Add point to k3d::legacy::mesh
		Mesh.points.push_back(*p);
	}

	// Create triangles ...
	triangles_t::const_iterator t_end(detail::triangles.end());
	for(triangles_t::const_iterator t = detail::triangles.begin(); t != t_end; ++t)
	{
		edge_t gts_edge1 = detail::edges[(*t)[0] - 1];
		edge_t gts_edge2 = detail::edges[(*t)[1] - 1];
		//edge_t gts_edge3 = detail::edges[(*t)[2] - 1];

		unsigned long edge1_first = gts_edge1[0] - 1;
		unsigned long edge1_second = gts_edge1[1] - 1;
		unsigned long edge2_first = gts_edge2[0] - 1;
		unsigned long edge2_second = gts_edge2[1] - 1;
		//unsigned long edge3_first = gts_edge3[0] - 1;
		//unsigned long edge3_second = gts_edge3[1] - 1;

		// Determine order of vertices
		unsigned long p[3];
		if(edge1_first == edge2_first)
		{
			p[0] = edge1_second;
			p[1] = edge1_first;
			p[2] = edge2_second;
		}
		else if(edge1_first == edge2_second)
		{
			p[0] = edge1_second;
			p[1] = edge1_first;
			p[2] = edge2_first;
		}
		else if(edge1_second == edge2_first)
		{
			p[0] = edge1_first;
			p[1] = edge1_second;
			p[2] = edge2_second;
		}
		else
		// edge1_second == edge2_second
		{
			p[0] = edge1_first;
			p[1] = edge1_second;
			p[2] = edge2_first;
		}

		// Create a triangle ...
		k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(detail::points[p[0]]);
		k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(detail::points[p[1]]);
		k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(detail::points[p[2]]);

		// Invert face (because of coordinate system switch)
		edge1->face_clockwise = edge3;
		edge3->face_clockwise = edge2;
		edge2->face_clockwise = edge1;

		k3d::legacy::face* const face = new k3d::legacy::face(edge1, 0);
		return_val_if_fail(face, false);

		polyhedron.faces.push_back(face);
	}

	return_val_if_fail(k3d::legacy::is_valid(polyhedron), false);
	//assert_warning(k3d::legacy::is_solid(polyhedron));

	k3d::legacy::set_companions(polyhedron);

	return true;
}

} // namespace libk3dgts

