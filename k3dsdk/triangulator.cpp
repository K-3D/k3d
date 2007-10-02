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

#include "gl.h"
#include "mesh_operations.h"
#include "sgi_tesselator.h"
#include "triangulator.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////////
// triangulator::implementation

class triangulator::implementation
{
public:
	implementation(triangulator& Owner) :
		owner(Owner),
		tessellator(sgiNewTess()),
		current_vertex(0)
	{
		// Note: specifying an edge flag callback ensures that we don't receive triangle fans or triangle strips

		sgiTessCallback(tessellator, GLU_TESS_BEGIN_DATA, reinterpret_cast<callback_t>(&raw_begin_callback));
		sgiTessCallback(tessellator, GLU_TESS_COMBINE_DATA, reinterpret_cast<callback_t>(&raw_combine_callback));
		sgiTessCallback(tessellator, GLU_TESS_EDGE_FLAG, reinterpret_cast<callback_t>(&raw_edge_flag_callback));
		sgiTessCallback(tessellator, GLU_TESS_VERTEX_DATA, reinterpret_cast<callback_t>(&raw_vertex_callback));
		sgiTessCallback(tessellator, GLU_TESS_END_DATA, reinterpret_cast<callback_t>(&raw_end_callback));
		sgiTessCallback(tessellator, GLU_TESS_ERROR_DATA, reinterpret_cast<callback_t>(&raw_error_callback));
	}

	~implementation()
	{
		sgiDeleteTess(tessellator);
	}

	void process(const mesh& Mesh)
	{
		if(!validate_polyhedra(Mesh))
			return;

		const mesh::points_t& points = *Mesh.points;
		const mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
		const mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;

		const uint_t face_begin = 0;
		const uint_t face_end = face_begin + face_first_loops.size();

		for(uint_t face = face_begin; face != face_end; ++face)
		{
			sgiTessBeginPolygon(tessellator, this);

			const uint_t loop_begin = face_first_loops[face];
			const uint_t loop_end = loop_begin + face_loop_counts[face];

			for(uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				sgiTessBeginContour(tessellator);

				const uint_t first_edge = loop_first_edges[loop];

				for(uint_t edge = first_edge; ; )
				{
					sgiTessVertex(tessellator, const_cast<double_t*>(points[edge_points[edge]].n), const_cast<void*>(reinterpret_cast<void*>(edge_points[edge])));

					edge = clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}

				sgiTessEndContour(tessellator);
			}

			sgiTessEndPolygon(tessellator);
		}
	}

	void begin_callback(GLenum Mode)
	{
		if(Mode != GL_TRIANGLES)
			log() << error << k3d_file_reference << ": incorrect tesselation primitive type: " << Mode << std::endl;
			
		owner.on_begin();

		current_vertex = 0;
	}

	void combine_callback(GLdouble Coords[3], void* VertexData[4], GLfloat Weight[4], void** OutputData)
	{
		const k3d::point3 coordinates(Coords[0], Coords[1], Coords[2]);

		uint_t vertices[4];
		vertices[0] = reinterpret_cast<uint_t>(VertexData[0]);
		vertices[1] = reinterpret_cast<uint_t>(VertexData[1]);
		vertices[2] = reinterpret_cast<uint_t>(VertexData[2]);
		vertices[3] = reinterpret_cast<uint_t>(VertexData[3]);

		double_t weights[4];
		weights[0] = Weight[0];
		weights[1] = Weight[1];
		weights[2] = Weight[2];
		weights[3] = Weight[3];

		uint_t new_vertex;

		owner.add_vertex(coordinates, vertices, weights, new_vertex);
	}

	void vertex_callback(void* VertexData)
	{
		vertices[current_vertex] = reinterpret_cast<uint_t>(VertexData);
		current_vertex = (current_vertex + 1) % 3;

		if(0 == current_vertex)
			owner.add_triangle(vertices[0], vertices[1], vertices[2]);
	}

	void end_callback()
	{
		if(current_vertex)
			log() << error << k3d_file_reference << ": too many tesselation vertices" << std::endl;
	}

	void error_callback(GLenum ErrorNumber)
	{
		log() << error << k3d_file_reference << " " << gluErrorString(ErrorNumber) << std::endl;
	}

	static implementation* instance(void* UserData)
	{
		return reinterpret_cast<implementation*>(UserData);
	}

	static void raw_begin_callback(GLenum Mode, void* UserData)
	{
		instance(UserData)->begin_callback(Mode);
	}

	static void raw_combine_callback(GLdouble Coords[3], void* VertexData[4], GLfloat Weight[4], void** OutputData, void* UserData)
	{
		instance(UserData)->combine_callback(Coords, VertexData, Weight, OutputData);
	}

	static void raw_edge_flag_callback(GLboolean)
	{
		// Note: Even though we don't use it, specifying this edge flag callback ensures that we don't receive triangle fans or triangle strips
	}

	static void raw_vertex_callback(void* VertexData, void* UserData)
	{
		instance(UserData)->vertex_callback(VertexData);
	}

	static void raw_end_callback(void* UserData)
	{
		instance(UserData)->end_callback();
	}

	static void raw_error_callback(GLenum ErrorNumber, void* UserData)
	{
		instance(UserData)->error_callback(ErrorNumber);
	}

	triangulator& owner;
	SGItesselator* const tessellator;

	uint_t current_vertex;
	uint_t vertices[3];
};

/////////////////////////////////////////////////////////////////////////////////
// triangulator

triangulator::triangulator() :
	m_implementation(new implementation(*this))
{
}

triangulator::~triangulator()
{
	delete m_implementation;
}

void triangulator::process(const mesh& Mesh)
{
	m_implementation->process(Mesh);
}

void triangulator::add_vertex(const point3& Coordinates, uint_t Vertices[4], double_t Weights[4], uint_t& NewVertex)
{
	on_add_vertex(Coordinates, Vertices, Weights, NewVertex);
}

void triangulator::add_triangle(const uint_t Point1, const uint_t Point2, const uint_t Point3)
{
	on_add_triangle(Point1, Point2, Point3);
}

void triangulator::on_add_vertex(const point3& Coordinates, uint_t Vertices[4], double_t Weights[4], uint_t& NewVertex)
{
}

void triangulator::on_add_triangle(const uint_t Point1, const uint_t Point2, const uint_t Point3)
{
}

void triangulator::on_begin()
{
}

} // namespace k3d

