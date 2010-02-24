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

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Wladyslaw Strugala (fizws@julia.univ.gda.pl)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include "obj_parser.h"

#include <k3dsdk/algebra.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/log.h>
#include <k3dsdk/texture3.h>

#include <iostream>
#include <iterator>
#include <stdexcept>

namespace module
{

namespace obj
{

namespace io
{

namespace detail
{

/// Converts one-based indices and negative indices to zero-based indices
k3d::uint_t zero_based_index(const int Index, const k3d::uint_t& CurrentCount)
{
	if(Index > 0)
		return Index - 1;
	
	if(Index < 0)
		return CurrentCount + Index;

	k3d::log() << error << "Invalid zero index" << std::endl;
	return 0;
}

void read_vertices(std::istream& Stream, k3d::mesh::indices_t& VertexCoordinates, const k3d::uint_t& VertexCount)
{
	int vertex_coordinate;
	for(Stream >> vertex_coordinate; Stream; Stream >> vertex_coordinate)
		VertexCoordinates.push_back(zero_based_index(vertex_coordinate, VertexCount));
}

void read_vertices(std::istream& Stream, k3d::mesh::indices_t& VertexCoordinates, k3d::mesh::indices_t& TextureCoordinates, const k3d::uint_t& VertexCount, const k3d::uint_t& TextureCount)
{
	int vertex_coordinate;
	for(Stream >> vertex_coordinate; Stream; Stream >> vertex_coordinate)
	{
		VertexCoordinates.push_back(zero_based_index(vertex_coordinate, VertexCount));
		if(Stream.peek() == '/')
		{
			char separator;
			int texture_coordinate;
			Stream >> separator >> texture_coordinate;
			if(Stream)
				TextureCoordinates.push_back(zero_based_index(texture_coordinate, TextureCount));
		}
	}

	// Sanity check - the number of texture coordinates must equal the number of vertices if nonzero
	if(TextureCoordinates.size() && TextureCoordinates.size() != VertexCoordinates.size())
		throw std::runtime_error("inconsistent use of texture coordinates");
}

void read_vertices(std::istream& Stream, k3d::mesh::indices_t& VertexCoordinates, k3d::mesh::indices_t& TextureCoordinates, k3d::mesh::indices_t& NormalCoordinates, const k3d::uint_t& VertexCount, const k3d::uint_t& TextureCount, const k3d::uint_t& NormalCount)
{
	int vertex_coordinate;
	for(Stream >> vertex_coordinate; Stream; Stream >> vertex_coordinate)
	{
		VertexCoordinates.push_back(zero_based_index(vertex_coordinate, VertexCount));
		if(Stream.peek() == '/')
		{
			char separator;
			int texture_coordinate;
			Stream >> separator >> texture_coordinate;
			if(Stream)
			{
				TextureCoordinates.push_back(zero_based_index(texture_coordinate, TextureCount));
				if(Stream.peek() == '/')
				{
					char separator;
					int normal_coordinate;
					Stream >> separator >> normal_coordinate;
					if(Stream)
						NormalCoordinates.push_back(zero_based_index(normal_coordinate, NormalCount));
				}
			}
		}
	}

	// Sanity check - the number of texture coordinates must equal the number of vertices if nonzero
	if(TextureCoordinates.size() && TextureCoordinates.size() != VertexCoordinates.size())
		throw std::runtime_error("inconsistent use of texture coordinates");

	// Sanity check - the number of normal coordinates must equal the number of vertices if nonzero
	if(NormalCoordinates.size() && NormalCoordinates.size() != VertexCoordinates.size())
		throw std::runtime_error("inconsistent use of normal coordinates");
}

} // namespace detail

//////////////////////////////////////////////////////////////////////////////////////////
// obj_parser

void obj_parser::parse(std::istream& Stream)
{
	k3d::uint_t line_count = 0; // Track the number of lines parsed
	k3d::uint_t v_count = 0; // Track the number of vertex coordinates parsed
	k3d::uint_t vn_count = 0; // Track the number of vertex normal coordinates parsed
	k3d::uint_t vt_count = 0; // Track the number of vertex texture coordinates parsed

	try
	{
		k3d::string_t line_buffer;
		for(k3d::getline(Stream, line_buffer); Stream; k3d::getline(Stream, line_buffer))
		{
			++line_count;

			// Skip blank lines ...
			if(line_buffer.empty())
				continue;

			// Skip comments ...
			if(line_buffer[0] == '#')
				continue;

			// Start looking for keywords ...
			k3d::string_t keyword;
			std::istringstream line_stream(line_buffer);
			line_stream >> keyword;

			if(keyword == "cstype")
			{
				k3d::string_t type;
				line_stream >> type;

				on_curve_surface_type(type);
			}
			else if(keyword == "deg")
			{
				k3d::uint_t u_degree = 0;
				k3d::uint_t v_degree = 0;
				line_stream >> u_degree >> v_degree;

				on_degree(u_degree, v_degree);
			}
			else if(keyword == "end")
			{
				on_curve_surface_end();
			}
			else if(keyword == "f")
			{
				k3d::mesh::indices_t vertex_coordinates;
				k3d::mesh::indices_t texture_coordinates;
				k3d::mesh::indices_t normal_coordinates; 
				detail::read_vertices(line_stream, vertex_coordinates, texture_coordinates, normal_coordinates, v_count, vt_count, vn_count);
			
				if(vertex_coordinates.size() < 3)
					throw std::runtime_error("face contains less than three vertices");

				on_face(vertex_coordinates, texture_coordinates, normal_coordinates);
			}
			else if(keyword == "g")
			{
				k3d::string_t name;
				line_stream >> name;

				on_group(name);
			}
			else if(keyword == "l")
			{
				k3d::mesh::indices_t vertex_coordinates;
				k3d::mesh::indices_t texture_coordinates;
				detail::read_vertices(line_stream, vertex_coordinates, texture_coordinates, v_count, vt_count);

				on_line(vertex_coordinates, texture_coordinates);
			}
			else if(keyword == "mtllib")
			{
				k3d::string_t name;
				line_stream >> name;

				on_material_library(name);
			}
			else if(keyword == "o")
			{
				k3d::string_t name;
				line_stream >> name;

				on_object(name);
			}
			else if(keyword == "p")
			{
				k3d::mesh::indices_t vertex_coordinates;
				detail::read_vertices(line_stream, vertex_coordinates, v_count);

				on_points(vertex_coordinates);
			}
			else if(keyword == "parm")
			{
				k3d::string_t direction;
				line_stream >> direction;

				k3d::double_t knot;
				k3d::mesh::knots_t knots;
				for(line_stream >> knot; line_stream; line_stream >> knot)
					knots.push_back(knot);

				on_parameter(direction, knots);
			}
			else if(keyword == "surf")
			{
				k3d::double_t s0, s1, t0, t1;
				line_stream >> s0 >> s1 >> t0 >> t1;

				k3d::mesh::indices_t vertex_coordinates;
				k3d::mesh::indices_t texture_coordinates;
				k3d::mesh::indices_t normal_coordinates; 
				detail::read_vertices(line_stream, vertex_coordinates, texture_coordinates, normal_coordinates, v_count, vt_count, vn_count);

				on_surface(s0, s1, t0, t1, vertex_coordinates, texture_coordinates, normal_coordinates);
			}
			else if(keyword == "usemtl")
			{
				k3d::string_t name;
				line_stream >> name;

				on_use_material(name);
			}
			else if(keyword == "v")
			{
				++v_count;

				k3d::point4 v(0, 0, 0, 1);
				line_stream >> v[0] >> v[1] >> v[2] >> v[3];

				on_vertex_coordinates(v);
			}
			else if(keyword == "vn")
			{
				++vn_count;

				k3d::normal3 vn(0, 0, 0);
				line_stream >> vn[0] >> vn[1] >> vn[2];

				on_normal_coordinates(vn);
			}
			else if(keyword == "vt")
			{
				++vt_count;

				k3d::texture3 vt(0, 0, 0);
				line_stream >> vt[0] >> vt[1] >> vt[2];

				on_texture_coordinates(vt);
			}
			else
			{
				k3d::log() << error << "unsupported keyword [" << keyword << "] at line " << line_count << " will be ignored" << std::endl;
			}
		}
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "error parsing wavefront obj at line " << line_count << ": " << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "error parsing wavefront obj at line " << line_count << ": unknown exception" << std::endl;
	}
}

void obj_parser::on_curve_surface_end()
{
}

void obj_parser::on_curve_surface_type(const k3d::string_t& Type)
{
}

void obj_parser::on_degree(const k3d::uint_t& UDegree, const k3d::uint_t& VDegree)
{
}

void obj_parser::on_face(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates)
{
}

void obj_parser::on_group(const k3d::string_t& Name)
{
}

void obj_parser::on_line(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates)
{
}

void obj_parser::on_material_library(const k3d::string_t& Name)
{
}

void obj_parser::on_normal_coordinates(const k3d::normal3& Normal)
{
}

void obj_parser::on_object(const k3d::string_t& Name)
{
}

void obj_parser::on_parameter(const k3d::string_t& Direction, const k3d::mesh::knots_t& Knots)
{
}

void obj_parser::on_points(const k3d::mesh::indices_t& VertexCoordinates)
{
}

void obj_parser::on_surface(const k3d::double_t& S0, const k3d::double_t& S1, const k3d::double_t& T0, const k3d::double_t& T1, const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates)
{
}

void obj_parser::on_texture_coordinates(const k3d::texture3& Texture)
{
}

void obj_parser::on_use_material(const k3d::string_t& Name)
{
}

void obj_parser::on_vertex_coordinates(const k3d::point4& Vertex)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// print_obj

void print_obj::on_curve_surface_end()
{
	k3d::log() << info << "on_curve_surface_end" << std::endl;
}

void print_obj::on_curve_surface_type(const k3d::string_t& Type)
{
	k3d::log() << info << "on_curve_surface_type " << Type << std::endl;
}

void print_obj::on_degree(const k3d::uint_t& UDegree, const k3d::uint_t& VDegree)
{
	k3d::log() << info << "on_degree " << UDegree << " " << VDegree << std::endl;
}

void print_obj::on_face(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates)
{
	k3d::log() << info << "on_face ";
	std::copy(VertexCoordinates.begin(), VertexCoordinates.end(), std::ostream_iterator<k3d::uint_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_group(const k3d::string_t& Name)
{
	k3d::log() << info << "on_group " << Name << std::endl;
}

void print_obj::on_line(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates)
{
	k3d::log() << info << "on_line ";
	std::copy(VertexCoordinates.begin(), VertexCoordinates.end(), std::ostream_iterator<k3d::uint_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_material_library(const k3d::string_t& Name)
{
	k3d::log() << info << "on_material_library " << Name << std::endl;
}

void print_obj::on_normal_coordinates(const k3d::normal3& Normal)
{
	k3d::log() << info << "on_normal_coordinates " << Normal << std::endl;
}

void print_obj::on_object(const k3d::string_t& Name)
{
	k3d::log() << info << "on_object " << Name << std::endl;
}

void print_obj::on_parameter(const k3d::string_t& Direction, const k3d::mesh::knots_t& Knots)
{
	k3d::log() << info << "on_parameter " << Direction << " " << std::endl;
	std::copy(Knots.begin(), Knots.end(), std::ostream_iterator<k3d::double_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_points(const k3d::mesh::indices_t& VertexCoordinates)
{
	k3d::log() << info << "on_points ";
	std::copy(VertexCoordinates.begin(), VertexCoordinates.end(), std::ostream_iterator<k3d::uint_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_surface(const k3d::double_t& S0, const k3d::double_t& S1, const k3d::double_t& T0, const k3d::double_t& T1, const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates)
{
	k3d::log() << info << "on_surface " << S0 << " " << S1 << " " << T0 << " " << T1 << " ";
	std::copy(VertexCoordinates.begin(), VertexCoordinates.end(), std::ostream_iterator<k3d::uint_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_texture_coordinates(const k3d::texture3& Texture)
{
	k3d::log() << info << "on_texture_coordinates " << Texture << std::endl;
}

void print_obj::on_use_material(const k3d::string_t& Name)
{
	k3d::log() << info << "on_use_material " << Name << std::endl;
}

void print_obj::on_vertex_coordinates(const k3d::point4& Vertex)
{
	k3d::log() << info << "on_vertex_coordinates " << Vertex << std::endl;
}

} // namespace io

} // namespace obj

} // namespace module

