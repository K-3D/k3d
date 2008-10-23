#ifndef MODULES_OBJ_IO_OBJ_PARSER_H
#define MODULES_OBJ_IO_OBJ_PARSER_H

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

#include <iosfwd>
#include <k3dsdk/mesh.h>

namespace k3d { class normal3; }
namespace k3d { class point4; }
namespace k3d { class texture3; }

namespace module
{

namespace obj
{

namespace io
{

/// Template design pattern class for parsing Wavefront (.obj) files
class obj_parser
{
public:
	/// Parse an input stream as an OBJ file, executing events based on the file contents
	void parse(std::istream& Stream);

private:
	/// @{
	/// @name Override these methods in a derived class to handle the given parsing events

	/// \note All indices are zero-based, regardless of the contents of the OBJ file

	virtual void on_curve_surface_end();
	virtual void on_curve_surface_type(const k3d::string_t& Type);
	virtual void on_degree(const k3d::uint_t& UDegree, const k3d::uint_t& VDegree);
	virtual void on_face(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates);
	virtual void on_group(const k3d::string_t& Name);
	virtual void on_line(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates);
	virtual void on_material_library(const k3d::string_t& Name);
	virtual void on_normal_coordinates(const k3d::normal3& Normal);
	virtual void on_object(const k3d::string_t& Name);
	virtual void on_parameter(const k3d::string_t& Direction, const k3d::mesh::knots_t& Knots);
	virtual void on_points(const k3d::mesh::indices_t& VertexCoordinates);
	virtual void on_surface(const k3d::double_t& S0, const k3d::double_t& S1, const k3d::double_t& T0, const k3d::double_t& T1, const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates);
	virtual void on_texture_coordinates(const k3d::texture3& Texture);
	virtual void on_use_material(const k3d::string_t& Name);
	virtual void on_vertex_coordinates(const k3d::point4& Vertex);

	/// @}
};

/// Concrete implementation of obj_parser that does pretty-printing of the file contents for debugging
class print_obj :
	public obj_parser
{
private:
	void on_curve_surface_end();
	void on_curve_surface_type(const k3d::string_t& Type);
	void on_degree(const k3d::uint_t& UDegree, const k3d::uint_t& VDegree);
	void on_face(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates);
	void on_group(const k3d::string_t& Name);
	void on_line(const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates);
	void on_material_library(const k3d::string_t& Name);
	void on_normal_coordinates(const k3d::normal3& Normal);
	void on_object(const k3d::string_t& Name);
	void on_parameter(const k3d::string_t& Direction, const k3d::mesh::knots_t& Knots);
	void on_points(const k3d::mesh::indices_t& VertexCoordinates);
	void on_surface(const k3d::double_t& S0, const k3d::double_t& S1, const k3d::double_t& T0, const k3d::double_t& T1, const k3d::mesh::indices_t& VertexCoordinates, const k3d::mesh::indices_t& TextureCoordinates, const k3d::mesh::indices_t& NormalCoordinates);
	void on_texture_coordinates(const k3d::texture3& Texture);
	void on_use_material(const k3d::string_t& Name);
	void on_vertex_coordinates(const k3d::point4& Vertex);
};

} // namespace io

} // namespace obj

} // namespace module

#endif // !MODULES_OBJ_IO_OBJ_PARSER_H

