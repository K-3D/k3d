// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "normal_cache.h"
#include "utility.h"

#include <k3dsdk/hints.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/transform.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

namespace detail
{

/// Calclate the normal for the given global face index (cumulative index over all polyhedra)
const k3d::normal3 normal(const k3d::uint_t GlobalFace, const k3d::mesh& Mesh)
{
	k3d::uint_t faces_end = 0;
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(!polyhedron.get())
			continue;
		faces_end += polyhedron->face_first_loops.size();
		if(GlobalFace < faces_end)
		{
			const k3d::uint_t face = GlobalFace - (faces_end - polyhedron->face_first_loops.size());
			return k3d::polyhedron::normal(polyhedron->edge_points, polyhedron->clockwise_edges, *Mesh.points, polyhedron->loop_first_edges[polyhedron->face_first_loops[face]]);
		}
	}
}

typedef std::set<k3d::uint_t> index_set_t;
void get_affected_points(const k3d::mesh& Mesh, const k3d::uint_t GlobalFace, index_set_t& AffectedPoints)
{
	k3d::uint_t faces_end = 0;
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(!polyhedron.get())
			continue;
		faces_end += polyhedron->face_first_loops.size();
		if(GlobalFace < faces_end)
		{
			const k3d::uint_t face = GlobalFace - (faces_end - polyhedron->face_first_loops.size());
			const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
			const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					AffectedPoints.insert(polyhedron->edge_points[edge]);
					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
			return;
		}
	}
}

/// Traverse polygonal mesh, visiting faces, loops, and points.
template<typename FunctorT>
void traverse_polyhedra(const k3d::mesh& Mesh, FunctorT& Functor)
{
	k3d::uint_t face_offset = 0;
	for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
	{
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
		if(!polyhedron.get())
			continue;

		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::indices_t& face_first_loops = polyhedron->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = polyhedron->face_loop_counts;
		const k3d::mesh::indices_t& loop_first_edges = polyhedron->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = polyhedron->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = polyhedron->clockwise_edges;
		
		const k3d::uint_t face_count = face_first_loops.size();
		for(k3d::uint_t poly_face = 0; poly_face != face_count; ++poly_face)
		{
			const k3d::uint_t face = poly_face + face_offset;
			Functor.on_face_start(face);
			const k3d::uint_t loop_begin = face_first_loops[face];
			const k3d::uint_t loop_end = loop_begin + face_loop_counts[face];
	
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				Functor.on_loop(loop);
				const k3d::uint_t first_edge = loop_first_edges[loop];
	
				for(k3d::uint_t edge = first_edge; ; )
				{
					Functor.on_edge(edge, edge_points[edge], edge_points[clockwise_edges[edge]], points[edge_points[edge]], points[edge_points[clockwise_edges[edge]]]);
					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
			Functor.on_face_end(face);
		}
		face_offset += face_count;
	}
}

class face_normals : public scheduler 
{
public:
	face_normals(const k3d::mesh* const Mesh) : m_normal(0,0,0)
	{
	}
	
	void on_face_start(const k3d::uint_t Face)
	{
		m_face = Face;
	}
	
	void on_face_end(const k3d::uint_t Face)
	{
		if(m_normal.length())
		{
			f_normals.push_back(k3d::normalize(m_normal));
		}
		else
		{
			k3d::log() << debug << "bad normal on face " << m_face << std::endl;
			f_normals.push_back(k3d::normal3(0,0,1));
		}
		m_normal = k3d::normal3(0,0,0);
	}
	
	void on_loop(const k3d::uint_t Loop) {}
	
	void on_edge(const k3d::uint_t Edge, const k3d::uint_t StartPointIndex, const k3d::uint_t EndPointIndex, const k3d::point3& StartPoint, const k3d::point3& EndPoint)
	{
		m_normal[0] += (StartPoint[1] + EndPoint[1]) * (EndPoint[2] - StartPoint[2]);
		m_normal[1] += (StartPoint[2] + EndPoint[2]) * (EndPoint[0] - StartPoint[0]);
		m_normal[2] += (StartPoint[0] + EndPoint[0]) * (EndPoint[1] - StartPoint[1]);
		point_to_faces[StartPointIndex].push_back(m_face);
	}
	
	/// Per face normals
	k3d::mesh::normals_t f_normals;
	/// For each point, the faces it belongs to
	std::vector<k3d::mesh::indices_t> point_to_faces;
	/// Indices of modified points
	k3d::mesh::indices_t indices;
	
protected:
	void on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
	{	
		if (indices.empty()) // Only set indices once (they are cleared upon execute()
		{
			indices = Hint->changed_points;
		}
	}
	
	void on_schedule(k3d::inode* Painter)
	{
		f_normals.clear();
		point_to_faces.clear();
		indices.clear();
	}
	
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
	{
		// Resize arrays and initialize normals if the topology changed
		if (point_to_faces.empty())
		{
			point_to_faces.resize(Mesh.points->size());
			detail::traverse_polyhedra(Mesh, *this);
		}
		for (k3d::uint_t i = 0; i != indices.size(); ++i)
		{
			const k3d::mesh::indices_t& faces = point_to_faces[indices[i]];
			for (k3d::uint_t j = 0; j != faces.size(); ++j)
			{
				f_normals[faces[j]] = detail::normal(faces[j], Mesh);
			}
		}
		indices.clear();
	}
	
private:
	k3d::normal3 m_normal;
	k3d::uint_t m_face;
};

class point_normals : public scheduler
{
public:
	point_normals(const k3d::mesh* const Mesh) {}
	
	k3d::mesh::normals_t p_normals;
	k3d::mesh::indices_t indices;
	
protected:
	void on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
	{	
		if (indices.empty()) // Only set indices once (they are cleared upon execute()
		{
			indices = Hint->changed_points;
		}
	}
	
	void on_schedule(k3d::inode* Painter)
	{
		p_normals.clear();
		indices.clear();
	}
	
	void on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
	{
		if(!has_non_empty_polyhedra(Mesh))
			return;
		face_normals& f_normals = get_data<face_normals>(&Mesh, Painter);
		// Resize arrays and initialize normals if the topology changed
		if (p_normals.empty())
		{
			k3d::uint_t point_count = Mesh.points->size();
			p_normals.resize(point_count, k3d::normal3(0.0,0.0,0.0));
			for (k3d::uint_t i = 0; i != point_count; ++i)
			{
				const k3d::mesh::indices_t& faces = f_normals.point_to_faces[i];
				for (k3d::uint_t j = 0; j != faces.size(); ++j)
					p_normals[i] += f_normals.f_normals[faces[j]];
				p_normals[i] /= faces.size();
			}
		}
		
		const k3d::mesh::points_t& points = *Mesh.points;
		
		// not only the moved points, but all points belonging to deformed faces need to be updated
		detail::index_set_t affected_points;
		for (k3d::uint_t i = 0; i != indices.size(); ++i)
		{
			const k3d::mesh::indices_t& faces = f_normals.point_to_faces[indices[i]];
			for (k3d::uint_t j = 0; j != faces.size(); ++j)
			{
				k3d::uint_t face = faces[j];
				detail::get_affected_points(Mesh, face, affected_points);
			}
		}
		for (std::set<k3d::uint_t>::iterator point = affected_points.begin(); point != affected_points.end(); ++point)
		{
			p_normals[*point] = k3d::normal3(0,0,0);
			const k3d::mesh::indices_t& faces = f_normals.point_to_faces[*point];
			for (k3d::uint_t j = 0; j != faces.size(); ++j)
			{
				p_normals[*point] += f_normals.f_normals[faces[j]];
			}
			p_normals[*point] = k3d::normalize(p_normals[*point]);
		}
		indices.clear();
	}
};

} // namespace detail

////////////////////
// normal_cache
////////////////////

const k3d::mesh::normals_t& normal_cache::point_normals(k3d::inode* Painter)
{
	return get_data<detail::point_normals>(m_mesh, Painter).p_normals;
}

const k3d::mesh::normals_t& normal_cache::face_normals(k3d::inode* Painter)
{
	return get_data<detail::face_normals>(m_mesh, Painter).f_normals;
}

void normal_cache::on_schedule(k3d::hint::mesh_geometry_changed* Hint, k3d::inode* Painter)
{
	schedule_data<detail::face_normals>(m_mesh, Hint, Painter);
	schedule_data<detail::point_normals>(m_mesh, Hint, Painter);
}

void normal_cache::on_schedule(k3d::inode* Painter)
{
	schedule_data<detail::face_normals>(m_mesh, 0, Painter);
	schedule_data<detail::point_normals>(m_mesh, 0, Painter);
}

void normal_cache::on_execute(const k3d::mesh& Mesh, k3d::inode* Painter)
{
	// nothing needed here, everything gets executed when the normals are requested
}

bool backfacing(const k3d::point3& Point, k3d::icamera& Camera, const k3d::normal3& Normal)
{
	k3d::point3 eye = k3d::property::pipeline_value<k3d::matrix4>(Camera.transformation().transform_source_output()) * k3d::point3(0,0,0);
	try
	{
		k3d::iperspective& perspective = dynamic_cast<k3d::iperspective&>(Camera.projection());
		return ((Point - eye) * Normal) > -1e-8; // 1e-8 avoids non-deterministic behaviour
	}
	catch (std::bad_cast)
	{
		k3d::point3 target = k3d::property::pipeline_value<k3d::point3>(Camera.world_target());
		return ((target - eye) * Normal) > 1e-8;
	}
}

} // namespace painters

} // namespace opengl

} // namespace module
