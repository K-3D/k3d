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

/** \file
		\brief Implements the GTSWriter K-3D object, which exports GTS *.gts files
		\author Tom Browder (tbrowder@home.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "helpers.h"

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_write_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <k3dsdk/fstream.h>

#include <map>
#include <set>

namespace libk3dgeometry
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

typedef std::vector<k3d::legacy::point*> PointCollection;
typedef std::map<k3d::legacy::point*, unsigned long> PointMap;
typedef ordered_pair<k3d::legacy::point*, k3d::legacy::point*> Edge;
typedef std::set<Edge> EdgeCollection;
typedef std::map<Edge, unsigned long> EdgeMap;
typedef std::vector<Edge> Triangle;
typedef std::vector<Triangle> TriangleCollection;

void write_gts_mesh(k3d::legacy::mesh& Mesh, k3d::inode& Object, std::ostream& Stream)
{
	// Process only if object is triangulated ...
	if(!detail::triangle_test(Mesh))
		return;

	// Get the collection of points ...
	k3d::legacy::mesh::points_t points;
	std::copy(Mesh.points.begin(), Mesh.points.end(), std::back_inserter(points));

	// Create a mapping of points-to-one-based-indices ...
	PointMap pointmap;
	for(PointCollection::iterator point = points.begin(); point != points.end(); ++point)
		pointmap[*point] = pointmap.size();

	// Build the collection of edges ...
	EdgeCollection edges;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
			{
				k3d::legacy::split_edge* first_edge = (*face)->first_edge;
				k3d::legacy::split_edge* edge = first_edge;
				do
					{
						edges.insert(Edge(edge));

						edge = edge->face_clockwise;
					}
				while(edge != first_edge);
			}

	// Create a mapping of edges-to-one-based-indices ...
	EdgeMap edgemap;
	for(EdgeCollection::iterator edge = edges.begin(); edge != edges.end(); ++edge)
		edgemap[*edge] = edgemap.size();

	// Build triangle collection ...
	TriangleCollection triangle_collection;
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); polyhedron++)
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); face++)
		{
			Triangle triangle;

			k3d::legacy::split_edge* first = (*face)->first_edge;
			// Skip empty faces
			if(!first)
				continue;

			triangle.push_back(Edge(first));

			k3d::legacy::split_edge* current_edge = first->face_clockwise;
			// Skip one-edged faces
			if(!current_edge)
				continue;

			triangle.push_back(Edge(current_edge));

			current_edge = current_edge->face_clockwise;
			if(current_edge)
				if(current_edge->face_clockwise == first)
					{
						triangle.push_back(Edge(current_edge));
						triangle_collection.push_back(triangle);
					}
		}

	// Write some comments
	Stream << "# object: " << Object.name() << std::endl;
	Stream << "# num points [" << points.size() << "] num edges [" << edges.size() << "] num triangles [" << triangle_collection.size() << "]" << std::endl;

	// Data header
	Stream << points.size() << " " << edges.size() << " " << triangle_collection.size() << std::endl;

	// Write points
	Stream << "# points" << std::endl;
	for(PointCollection::iterator point = points.begin(); point != points.end(); ++point)
		{
			// Transform coords back to GTS convention
			const k3d::point3 coords = (*point)->position;
			Stream << -coords[0] << " " << -coords[2] << " " << coords[1] << std::endl;
		}

	// Write edges
	Stream << "# edges" << std::endl;
	for(EdgeCollection::iterator edge = edges.begin(); edge != edges.end(); ++edge)
		Stream << pointmap[edge->first] << " " << pointmap[edge->second] << std::endl;

	// Write triangles
	Stream << "# triangles" << std::endl;
	for(TriangleCollection::iterator triangle = triangle_collection.begin(); triangle != triangle_collection.end(); ++triangle)
		{
			for(Triangle::iterator edge = triangle->begin(); edge != triangle->end(); ++edge)
				Stream << edgemap[*edge] << " ";
			Stream << std::endl;
		}
}

/////////////////////////////////////////////////////////////////////////////
// gts_writer_implementation

class gts_writer_implementation :
	public k3d::ifile_format,
	public k3d::idocument_write_format,
	public k3d::ideletable
{
public:
	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& FilePath)
	{
		return k3d::filesystem::extension(FilePath).lowercase().raw() == ".gts";
	}

	bool write_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<gts_writer_implementation, k3d::interface_list<k3d::idocument_write_format> > factory(
			k3d::uuid(0xc6bdb531, 0x17a74c0a, 0x99db8c94, 0x38195da7),
			"GTSWriter",
			_("GNU Triangulated Surface ( .gts )"),
			"GeometryWriter");

		return factory;
	}
};

bool gts_writer_implementation::write_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Writing " << FilePath.native_console_string() << " with " << factory().name() << std::endl;

	// (GTS format is not a real one : it has been designed to store surfaces
	// as sets of points, edges and triangles, only;
	// each surface item can have optional arbitrary values, but no other
	// scene information can be found in a native GTS file)

	// Try to open the output file
	k3d::filesystem::ofstream file(FilePath);
	return_val_if_fail(file.good(), false);
	file << "# Written by K-3D" << std::endl;

	// Get the set of available meshes
	detail::mesh_instances_t meshes;
	detail::get_mesh_instances(Document, meshes);

	// Write meshes to the file
	for(detail::mesh_instances_t::iterator mesh = meshes.begin(); mesh != meshes.end(); ++mesh)
		write_gts_mesh(*mesh->first, *mesh->second, file);

	return true;
}

k3d::iplugin_factory& gts_writer_factory()
{
	return gts_writer_implementation::get_factory();
}

} // namespace libk3dgeometry

