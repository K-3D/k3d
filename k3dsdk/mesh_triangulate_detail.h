#ifndef K3DSDK_MESH_TRIANGULATE_DETAIL_H
#define K3DSDK_MESH_TRIANGULATE_DETAIL_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "sgi_tesselator.h"

namespace detail
{

template<typename OutputFaceIterator, typename OutputPointIterator>
class glu_triangulator_t
{
public:
	glu_triangulator_t(OutputFaceIterator OutputFaces, OutputPointIterator OutputPoints) :
		tessellator(sgiNewTess()),
		output_faces(OutputFaces),
		output_points(OutputPoints),
		current_face(0)
	{
		sgiTessCallback(tessellator, GLU_TESS_BEGIN_DATA, reinterpret_cast<callback_t>(&raw_begin));
		sgiTessCallback(tessellator, GLU_TESS_VERTEX_DATA, reinterpret_cast<callback_t>(&raw_vertex));
		sgiTessCallback(tessellator, GLU_TESS_COMBINE_DATA, reinterpret_cast<callback_t>(&raw_combine));
		sgiTessCallback(tessellator, GLU_TESS_END_DATA, reinterpret_cast<callback_t>(&raw_end));
		sgiTessCallback(tessellator, GLU_TESS_ERROR_DATA, reinterpret_cast<callback_t>(&raw_error));
	}

	~glu_triangulator_t()
	{
		sgiDeleteTess(tessellator);
	}

	void operator()(face& Face)
	{
		current_face = &Face;

		sgiTessBeginPolygon(tessellator, this);

		// Handle the main contour for the face ...
		sgiTessBeginContour(tessellator);
		for(split_edge* edge = Face.first_edge; edge && edge->face_clockwise; edge = edge->face_clockwise)
		{
			sgiTessVertex(tessellator, edge->vertex->position.n, edge->vertex);
			if(edge->face_clockwise == Face.first_edge)
				break;
		}
		sgiTessEndContour(tessellator);

		// Handle holes ...
		for(face::holes_t::const_iterator hole = Face.holes.begin(); hole != Face.holes.end(); ++hole)
		{
			sgiTessBeginContour(tessellator);
			for(split_edge* edge = *hole; edge && edge->face_clockwise; edge = edge->face_clockwise)
			{
				sgiTessVertex(tessellator, edge->vertex->position.n, edge->vertex);
				if(edge->face_clockwise == *hole)
					break;
			}
			sgiTessEndContour(tessellator);
		}

		sgiTessEndPolygon(tessellator);
	}

private:
	void begin(GLenum Mode)
	{
		mode = Mode;
		v0 = v1 = v2 = 0;
		flip_strip = false;
	}

	void vertex(void* VertexData)
	{
		if(!v0)
		{
			v0 = v1;
			v1 = v2;
		}
		v2 = reinterpret_cast<point*>(VertexData);

		if(!v0)
			return;

		std::vector<split_edge*> new_edges;
		switch(mode)
		{
			case GL_TRIANGLE_FAN:
				new_edges.push_back(new split_edge(v0));
				new_edges.push_back(new split_edge(v1));
				new_edges.push_back(new split_edge(v2));
				break;
			case GL_TRIANGLE_STRIP:
				new_edges.push_back(new split_edge(v0));
				new_edges.push_back(new split_edge(v1));
				new_edges.push_back(new split_edge(v2));
				if(flip_strip)
					std::reverse(new_edges.begin(), new_edges.end());
				break;
			case GL_TRIANGLES:
				new_edges.push_back(new split_edge(v0));
				new_edges.push_back(new split_edge(v1));
				new_edges.push_back(new split_edge(v2));
				break;
		}
		loop_edges(new_edges.begin(), new_edges.end());

		face* const new_face = new face(*current_face);
		new_face->holes.clear();
		new_face->first_edge = new_edges.front();
		*output_faces++ = new_face;

		switch(mode)
		{
			case GL_TRIANGLE_FAN:
				v1 = v2;
				break;
			case GL_TRIANGLE_STRIP:
				v0 = v1;
				v1 = v2;
				flip_strip = !flip_strip;
				break;
			case GL_TRIANGLES:
				v0 = v1 = v2 = 0;
				break;
		}
	}

	void combine(GLdouble Coords[3], void* VertexData[4], GLfloat Weight[4], void** OutputData)
	{
		point* const new_point = new point(point3(Coords[0], Coords[1], Coords[2]));
		*OutputData = new_point;
		*output_points++ = new_point;
	}

	void end()
	{
	}

	void error(GLenum ErrorNumber)
	{
		log() << k3d::error << k3d_file_reference << " " << gluErrorString(ErrorNumber) << std::endl;
	}

	static glu_triangulator_t* instance(void* UserData)
	{
		return reinterpret_cast<glu_triangulator_t*>(UserData);
	}

	static void raw_begin(GLenum Mode, void* UserData)
	{
		instance(UserData)->begin(Mode);
	}

	static void raw_vertex(void* VertexData, void* UserData)
	{
		instance(UserData)->vertex(VertexData);
	}

	static void raw_combine(GLdouble Coords[3], void* VertexData[4], GLfloat Weight[4], void** OutputData, void* UserData)
	{
		instance(UserData)->combine(Coords, VertexData, Weight, OutputData);
	}

	static void raw_end(void* UserData)
	{
		instance(UserData)->end();
	}

	static void raw_error(GLenum ErrorNumber, void* UserData)
	{
		instance(UserData)->error(ErrorNumber);
	}

	SGItesselator* const tessellator;
	OutputFaceIterator output_faces;
	OutputPointIterator output_points;

	GLenum mode;
	point* v0;
	point* v1;
	point* v2;
	bool flip_strip;

	/// Stores a reference to the "current" face so data can be copied to child triangles
	face* current_face;
};

} // namespace detail

#endif // !K3DSDK_MESH_TRIANGULATE_DETAIL_H

