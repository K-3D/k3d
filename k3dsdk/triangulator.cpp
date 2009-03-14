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
#include "polyhedron.h"
#include "sgi_tesselator.h"
#include "triangulator.h"
#include <set>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////////
// triangulator::implementation

class triangulator::implementation
{
public:
	implementation(triangulator& Owner) :
		owner(Owner),
		tessellator(sgiNewTess())
	{
		// Note: specifying an edge flag callback ensures that we don't receive triangle fans or triangle strips

		sgiTessCallback(tessellator, GLU_TESS_BEGIN_DATA, reinterpret_cast<callback_t>(&raw_begin_callback));
		sgiTessCallback(tessellator, GLU_TESS_COMBINE_DATA, reinterpret_cast<callback_t>(&raw_combine_callback));
//		sgiTessCallback(tessellator, GLU_TESS_EDGE_FLAG, reinterpret_cast<callback_t>(&raw_edge_flag_callback));
		sgiTessCallback(tessellator, GLU_TESS_VERTEX_DATA, reinterpret_cast<callback_t>(&raw_vertex_callback));
		sgiTessCallback(tessellator, GLU_TESS_END_DATA, reinterpret_cast<callback_t>(&raw_end_callback));
		sgiTessCallback(tessellator, GLU_TESS_ERROR_DATA, reinterpret_cast<callback_t>(&raw_error_callback));
	}

	~implementation()
	{
		sgiDeleteTess(tessellator);
	}

	void process(const mesh& Mesh, const polyhedron::const_primitive& Polyhedron)
	{
		const uint_t face_begin = 0;
		const uint_t face_end = face_begin + Polyhedron.face_first_loops.size();
		for(uint_t face = face_begin; face != face_end; ++face)
		{
			process(
				*Mesh.points,
				Polyhedron.face_first_loops,
				Polyhedron.face_loop_counts,
				Polyhedron.loop_first_edges,
				Polyhedron.edge_points,
				Polyhedron.clockwise_edges,
				face);
		}
	}


	void process(
		const mesh::points_t& Points,
		const mesh::indices_t& FaceFirstLoops,
		const mesh::counts_t& FaceLoopCounts,
		const mesh::indices_t& LoopFirstEdges,
		const mesh::indices_t& EdgePoints,
		const mesh::indices_t& ClockwiseEdges,
		const uint_t Face)
	{
		owner.start_face(Face);

		vertex_edges.resize(Points.size());

		sgiTessBeginPolygon(tessellator, this);

		const uint_t loop_begin = FaceFirstLoops[Face];
		const uint_t loop_end = loop_begin + FaceLoopCounts[Face];

		for(uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			sgiTessBeginContour(tessellator);

			const uint_t first_edge = LoopFirstEdges[loop];
			for(uint_t edge = first_edge; ; )
			{
				vertex_edges[EdgePoints[edge]] = edge;

				sgiTessVertex(tessellator, const_cast<double_t*>(Points[EdgePoints[edge]].n), const_cast<void*>(reinterpret_cast<void*>(EdgePoints[edge])));

				edge = ClockwiseEdges[edge];
				if(edge == first_edge)
					break;
			}

			sgiTessEndContour(tessellator);
		}

		sgiTessEndPolygon(tessellator);

		owner.finish_face(Face);
	}

	void begin_callback(GLenum Mode)
	{
		mode = Mode;
		vertex_count = 0;
		flip_strip = false;
	}

	void combine_callback(GLdouble Coords[3], void* VertexData[4], GLfloat Weight[4], void** OutputData)
	{
		const k3d::point3 coordinates(Coords[0], Coords[1], Coords[2]);

		uint_t vertices[4];
		vertices[0] = reinterpret_cast<uint_t>(VertexData[0]);
		vertices[1] = reinterpret_cast<uint_t>(VertexData[1]);
		vertices[2] = reinterpret_cast<uint_t>(VertexData[2]);
		vertices[3] = reinterpret_cast<uint_t>(VertexData[3]);

		uint_t edges[4];
		edges[0] = vertex_edges[vertices[0]];
		edges[1] = vertex_edges[vertices[1]];
		edges[2] = vertex_edges[vertices[2]];
		edges[3] = vertex_edges[vertices[3]];

		double_t weights[4];
		weights[0] = Weight[0];
		weights[1] = Weight[1];
		weights[2] = Weight[2];
		weights[3] = Weight[3];

		uint_t new_vertex = 0;
		owner.add_vertex(coordinates, vertices, edges, weights, new_vertex);

		while(vertex_edges.size() <= new_vertex)
			vertex_edges.push_back(0);

		*OutputData = reinterpret_cast<void*>(new_vertex);
	}

	void vertex_callback(void* VertexData)
	{
		vertices[std::min(static_cast<uint_t>(2), vertex_count)] = reinterpret_cast<uint_t>(VertexData);

		if(++vertex_count > 2)
		{
			uint_t triangle_vertices[3];
			switch(mode)
			{
				case GL_TRIANGLE_FAN:
					triangle_vertices[0] = vertices[0];
					triangle_vertices[1] = vertices[1];
					triangle_vertices[2] = vertices[2];

					vertices[1] = vertices[2];
					break;
				case GL_TRIANGLE_STRIP:
					if(flip_strip)
					{
						triangle_vertices[0] = vertices[2];
						triangle_vertices[1] = vertices[1];
						triangle_vertices[2] = vertices[0];
					}
					else
					{
						triangle_vertices[0] = vertices[0];
						triangle_vertices[1] = vertices[1];
						triangle_vertices[2] = vertices[2];
					}

					flip_strip = !flip_strip;
					vertices[0] = vertices[1];
					vertices[1] = vertices[2];
					break;
				case GL_TRIANGLES:
					triangle_vertices[0] = vertices[0];
					triangle_vertices[1] = vertices[1];
					triangle_vertices[2] = vertices[2];

					vertex_count = 0;
					break;
			}

			uint_t triangle_edges[3];
			triangle_edges[0] = vertex_edges[triangle_vertices[0]];
			triangle_edges[1] = vertex_edges[triangle_vertices[1]];
			triangle_edges[2] = vertex_edges[triangle_vertices[2]];
					
			owner.add_triangle(triangle_vertices, triangle_edges);
		}
	}

	void end_callback()
	{
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

	GLenum mode;
	uint_t vertex_count;
	uint_t vertices[3];
	bool flip_strip;
	std::vector<uint_t> vertex_edges;
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

void triangulator::process(const mesh& SourceMesh, const polyhedron::const_primitive& Polyhedron)
{
	start_processing(SourceMesh);
	m_implementation->process(SourceMesh, Polyhedron);
	finish_processing(SourceMesh);
}

void triangulator::process(
	const mesh::points_t& Points,
	const mesh::indices_t& FaceFirstLoops,
	const mesh::counts_t& FaceLoopCounts,
	const mesh::indices_t& LoopFirstEdges,
	const mesh::indices_t& EdgePoints,
	const mesh::indices_t& ClockwiseEdges,
	const uint_t Face)
{
	m_implementation->process(
		Points,
		FaceFirstLoops,
		FaceLoopCounts,
		LoopFirstEdges,
		EdgePoints,
		ClockwiseEdges,
		Face);
}

void triangulator::start_processing(const mesh& SourceMesh)
{
}

void triangulator::start_face(const uint_t Face)
{
}

void triangulator::add_vertex(const point3& Coordinates, uint_t Vertices[4], uint_t Edges[4], double_t Weights[4], uint_t& NewVertex)
{
}

void triangulator::add_triangle(uint_t Points[3], uint_t Edges[3])
{
}

void triangulator::finish_face(const uint_t Face)
{
}

void triangulator::finish_processing(const mesh& SourceMesh)
{
}

} // namespace k3d

