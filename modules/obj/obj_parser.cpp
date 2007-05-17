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
#include <k3dsdk/vectors.h>

#include <iostream>
#include <iterator>

namespace libk3dobj
{

//////////////////////////////////////////////////////////////////////////////////////////
// obj_parser

void obj_parser::parse(std::istream& Stream)
{
	size_t line_count = 0;
        std::string line_buffer;
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
                std::string keyword;
                std::istringstream line_stream(line_buffer);
                line_stream >> keyword;

		if(keyword == "cstype")
		{
			std::string type;
			line_stream >> type;

			on_curve_type(type);
		}
		else if(keyword == "f")
		{
			k3d::mesh::indices_t points;
			k3d::mesh::indices_t texture_points;
			k3d::mesh::indices_t normals; 

			size_t index;
			char separator;
			while(line_stream.good())
			{
				try
				{
					line_stream >> index; // Vertex position index (decrement, 1-based indices!)
					points.push_back(--index);
					if (line_stream.peek() != '/') // assume we have only vertex position indices
						continue;
					if (!line_stream.good())
						throw 1;
					line_stream >> separator;
					if (!line_stream.good() || separator != '/')
						throw 2;
					
					if (line_stream.peek() != '/')
					{
						line_stream >> index; // Optional texture vertex index
						texture_points.push_back(--index);
					}
					if (!line_stream.good())
						throw 3;
					line_stream >> separator;
					if (!line_stream.good() || separator != '/')
						throw 4;
					
					if (line_stream.peek() != '/')
					{
						line_stream >> index; // Optional normal vector index
						normals.push_back(--index);
					}
				}
				catch (int e)
				{
					k3d::log() << warning << "Error " << e << " reading obj file at line " << line_count << std::endl;
					break;
				}
			}
			
			if (points.size() < 3) // specs require a minimum of 3 vertices per face
			{
				k3d::log() << warning << "Obj file contained a face with less than 3 vertices at line " << line_count << std::endl;
			}
			else
			{ 
				on_face(points, texture_points, normals);
			}
		}
		else if(keyword == "g")
		{
			std::string name;
			line_stream >> name;

			on_group(name);
		}
 		else if(keyword == "l")
		{
			k3d::mesh::indices_t points;

			size_t point;
			for(line_stream >> point; line_stream; line_stream >> point)
				points.push_back(point);

			on_line(points);
		}
		else if(keyword == "mtllib")
		{
			std::string name;
			line_stream >> name;

			on_material_library(name);
		}
		else if(keyword == "o")
		{
			std::string name;
			line_stream >> name;

			on_object(name);
		}
		else if(keyword == "p")
		{
			k3d::mesh::indices_t points;

			size_t point;
			for(line_stream >> point; line_stream; line_stream >> point)
				points.push_back(point);

			on_points(points);
		}
		else if(keyword == "usemtl")
		{
			std::string name;
			line_stream >> name;

			on_use_material(name);
		}
    else if(keyword == "v")
          {
            k3d::point4 v(0, 0, 0, 1);
            line_stream >> v[0] >> v[1] >> v[2] >> v[3];

						on_vertex(v);
          }
    else if(keyword == "vn")
		{
      k3d::normal3 vn(0, 0, 0);
      line_stream >> vn[0] >> vn[1] >> vn[2];

      on_normal_vertex(vn);
		}
    else if(keyword == "vt")
    {
            k3d::point3 vt(0, 0, 0);
            line_stream >> vt[0] >> vt[1] >> vt[2];

						on_texture_vertex(vt);
    }
		else
		{
			k3d::log() << error << "Line: " << line_count << " - unsupported keyword [" << keyword << "] will be ignored" << std::endl;
		}
	}
}

void obj_parser::on_curve_type(const std::string& Type)
{
}

void obj_parser::on_face(const k3d::mesh::indices_t& Points, const k3d::mesh::indices_t& TexturePoints, const k3d::mesh::indices_t& Normals)
{
}

void obj_parser::on_group(const std::string& Name)
{
}

void obj_parser::on_line(const k3d::mesh::indices_t& Points)
{
}

void obj_parser::on_material_library(const std::string& Name)
{
}

void obj_parser::on_object(const std::string& Name)
{
}

void obj_parser::on_points(const k3d::mesh::indices_t& Points)
{
}

void obj_parser::on_use_material(const std::string& Name)
{
}

void obj_parser::on_vertex(const k3d::point4& Vertex)
{
}

void obj_parser::on_normal_vertex(const k3d::normal3& NormalVertex)
{
}

void obj_parser::on_texture_vertex(const k3d::point3& TextureVertex)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// print_obj

void print_obj::on_curve_type(const std::string& Type)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Type << std::endl;
}

void print_obj::on_face(const k3d::mesh::indices_t& Points, const k3d::mesh::indices_t& TexturePoints, const k3d::mesh::indices_t& Normals)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " ";
	std::copy(Points.begin(), Points.end(), std::ostream_iterator<size_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_group(const std::string& Name)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Name << std::endl;
}

void print_obj::on_line(const k3d::mesh::indices_t& Points)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " ";
	std::copy(Points.begin(), Points.end(), std::ostream_iterator<size_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_material_library(const std::string& Name)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Name << std::endl;
}

void print_obj::on_object(const std::string& Name)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Name << std::endl;
}

void print_obj::on_points(const k3d::mesh::indices_t& Points)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " ";
	std::copy(Points.begin(), Points.end(), std::ostream_iterator<size_t>(k3d::log(), " "));
	k3d::log() << std::endl;
}

void print_obj::on_use_material(const std::string& Name)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Name << std::endl;
}

void print_obj::on_vertex(const k3d::point4& Vertex)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << Vertex << std::endl;
}

void print_obj::on_normal_vertex(const k3d::normal3& NormalVertex)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << NormalVertex << std::endl;
}

void print_obj::on_texture_vertex(const k3d::point3& TextureVertex)
{
	k3d::log() << debug << __PRETTY_FUNCTION__ << " " << TextureVertex << std::endl;
}

} // namespace libk3dobj

/*
#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/material.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/property.h>
#include <k3dsdk/share.h>
#include <k3dsdk/string_cast.h>

#include <k3dsdk/fstream.h>

namespace libk3dgeometry
{

namespace detail
{

// Declarations
typedef std::map<std::string, k3d::imaterial*> obj_materials_t;
const std::string default_material_name = "default";
bool load_mtl(k3d::idocument& Document, const k3d::filesystem::path& Path, obj_materials_t& Materials);

typedef std::vector<k3d::point4> positions_t;
bool load_bspline(k3d::idocument& Document, const bool rational, k3d::filesystem::ifstream& file, unsigned long& LineCount, k3d::legacy::mesh& Mesh, k3d::imaterial* Material, positions_t& Positions, const unsigned long CurrentSize);


/// Provides lookup of objects by one-based and negative (relative to the end of current sequence) index
unsigned long lookup(const long Index, const unsigned long PointsSize, const unsigned long CurrentSize)
{
	return_val_if_fail(Index, 0);

	if(Index > 0)
	{
		// Vertices are indexed starting from 1
		const unsigned long offset = static_cast<unsigned long>(Index);
		return_val_if_fail(offset <= PointsSize, false);
		return offset - 1;
	}

	// Index < 0, reference is backwards from current position
	const unsigned long offset = static_cast<unsigned long>(-Index);
	return_val_if_fail(offset <= CurrentSize, false);
	return CurrentSize - offset;
}

bool load_obj_file(k3d::legacy::mesh& Mesh, k3d::idocument& Document, const boost::filesystem::path& Path, const std::string& TextureU, const std::string& TextureV, const std::string& TextureW)
{
	if(Path.empty())
		return false;

	k3d::log() << info << "Loading .obj file: " << Path.native_file_string() << std::endl;

	k3d::filesystem::ifstream file(Path);
	if(!file)
	{
		k3d::log() << error << k3d_file_reference << ": error opening [" << Path.native_file_string() << "]" << std::endl;
		return false;
	}

	// Keep track of vertex coordinates as they're loaded
	std::vector<k3d::point4> positions;
	// Keep track of texture coordinates as they're loaded
	std::vector<k3d::point3> texture_coordinates;
	// Keep track of vertex normals as they're loaded
	std::vector<k3d::point3> normals;

	// Keep track of new materials
	obj_materials_t materials;
	materials[default_material_name] = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));

	// First pass : load points (since they can be defined after primitives) and materials
	std::string line_buffer;
	for(k3d::getline(file, line_buffer); file; k3d::getline(file, line_buffer))
	{
		// Skip blank lines ...
		if(line_buffer.empty())
			continue;

		// Skip comments ...
		if(line_buffer[0] == '#')
			continue;

		// Start looking for keywords ...
		std::string keyword;
		std::istringstream line_stream(line_buffer);
		line_stream >> keyword;

		if(keyword == "v")
		{
			k3d::point4 v(0, 0, 0, 1);
			line_stream >> v[0] >> v[1] >> v[2] >> v[3];

			positions.push_back(v);

			Mesh.points.push_back(new k3d::legacy::point(k3d::point3(-v[0], v[1], v[2])));
		}
		else if(keyword == "vt")
		{
			k3d::point3 vt(0, 0, 0);
			line_stream >> vt[0] >> vt[1] >> vt[2];

			texture_coordinates.push_back(vt);
		}
		else if(keyword == "vn")
		{
			k3d::point3 vn(0, 0, 0);
			line_stream >> vn[0] >> vn[1] >> vn[2];

			normals.push_back(vn);
		}
		else if(keyword == "mtllib")
		{
			std::string mtl_name;
			for(line_stream >> mtl_name; line_stream; line_stream >> mtl_name)
			{
				if(!mtl_name.size())
					continue;

				// Append parent .obj path (.mtl file name may contain a path too)
				try
				{
					boost::filesystem::path mtl_path(mtl_name, k3d::filesystem::get_native());
					load_mtl(Document, Path.branch_path() / mtl_path.leaf(), materials);
				}
				catch(std::exception& e)
				{
					k3d::log() << error << e.what() << std::endl;
				}
			}
		}
	}

	file.close();

	// Second pass : load everything else
	k3d::filesystem::ifstream file2(Path);
	return_val_if_fail(file2, false);

	// Keep track of the current material
	k3d::imaterial* current_material = materials[default_material_name];

	// Keep track of the current polyhedron
	k3d::legacy::polyhedron* current_polyhedron = 0;

	// Keep track of current group name
	std::string current_group_name("");

	unsigned long line_count = 0;
	unsigned long v_index = 0;
	unsigned long vt_index = 0;
	unsigned long vn_index = 0;
	for(k3d::getline(file2, line_buffer); file2; k3d::getline(file2, line_buffer))
	{
		++line_count;

		// Skip blank lines ...
		if(line_buffer.empty())
			continue;

		// Skip comments ...
		if(line_buffer[0] == '#')
			continue;

		// Start looking for keywords ...
		std::string keyword;
		std::istringstream line_stream(line_buffer);
		line_stream >> keyword;

		if(keyword == "g")
		{
			line_stream >> current_group_name;

			// If a material has this group name, apply material
			if(materials.find(current_group_name) != materials.end())
				current_material = materials[current_group_name];
		}
		else if(keyword == "v")
		{
			++v_index;
		}
		else if(keyword == "vt")
		{
			++vt_index;
		}
		else if(keyword == "vn")
		{
			++vn_index;
		}
		else if(keyword == "o")
		{
			std::string object_name;
			line_stream >> object_name;
			k3d::log() << info << "Loading object " << object_name << std::endl;

			if(current_polyhedron)
				k3d::legacy::set_companions(*current_polyhedron);

			current_polyhedron = 0;
		}
		else if(keyword == "p")
		{
			typedef std::vector<unsigned long> vertices_t;

			std::string vertex;
			vertices_t vertices;
			for(line_stream >> vertex; line_stream; line_stream >> vertex)
			{
				const long index = k3d::from_string<long>(vertex, 0);
				return_val_if_fail(index, false);

				vertices.push_back(lookup(index, positions.size(), v_index));
			}

			k3d::legacy::point_group* const point_group = new k3d::legacy::point_group();
			point_group->material = current_material;
			Mesh.point_groups.push_back(point_group);

			for(vertices_t::const_iterator v = vertices.begin(); v != vertices.end(); ++v)
				point_group->points.push_back(Mesh.points[*v]);
		}
		else if(keyword == "l")
		{
			typedef std::vector<unsigned long> vertices_t;

			std::string vertex;
			vertices_t vertices;
			for(line_stream >> vertex; line_stream; line_stream >> vertex)
			{
				long v = 0; char s1 = 0; long vt = 0;
				std::istringstream vertex_stream(vertex);
				vertex_stream >> v >> s1 >> vt;
				return_val_if_fail(v, false);

				vertices.push_back(lookup(v, positions.size(), v_index));
			}

			k3d::legacy::linear_curve_group* const linear_curve_group = new k3d::legacy::linear_curve_group();
			linear_curve_group->material = current_material;
			Mesh.linear_curve_groups.push_back(linear_curve_group);

			k3d::legacy::linear_curve* const linear_curve = new k3d::legacy::linear_curve();
			linear_curve_group->curves.push_back(linear_curve);

			for(vertices_t::const_iterator v = vertices.begin(); v != vertices.end(); ++v)
				linear_curve->control_points.push_back(Mesh.points[*v]);
		}
		else if(keyword == "f")
		{
			if(!current_polyhedron)
			{
				current_polyhedron = new k3d::legacy::polyhedron();
				Mesh.polyhedra.push_back(current_polyhedron);
			}

			std::vector<k3d::legacy::split_edge*> edges;

			std::string vertex;
			for(line_stream >> vertex; line_stream; line_stream >> vertex)
			{
				long v = 0; char s1 = 0; long vt = 0; char s2 = 0; long vn = 0;
				std::istringstream vertex_stream(vertex);
				vertex_stream >> v >> s1;
				if(vertex_stream.peek() == '/')
					vertex_stream >> s2 >> vn;
				else
					vertex_stream >> vt >> s2 >> vn;

				return_val_if_fail(v, false);

				k3d::legacy::split_edge* const edge = new k3d::legacy::split_edge(Mesh.points[lookup(v, positions.size(), v_index)]);
				if(vt)
				{
					const k3d::point3 texture = texture_coordinates[lookup(vt, texture_coordinates.size(), vt_index)];
					edge->facevarying_data[TextureU] = texture[0];
					edge->facevarying_data[TextureV] = 1 - texture[1];
					edge->facevarying_data[TextureW] = texture[2];
				}

				edges.push_back(edge);
			}

			std::reverse(edges.begin(), edges.end());
			k3d::legacy::loop_edges(edges.begin(), edges.end());

			k3d::legacy::face* const face = new k3d::legacy::face(edges.front(), current_material);
			current_polyhedron->faces.push_back(face);
		}
		else if(keyword == "usemtl")
		{
			std::string material_name;
			line_stream >> material_name;

			current_material = materials[default_material_name];
			if(materials.find(material_name) != materials.end())
				current_material = materials[material_name];
			else
				k3d::log() << error << k3d_file_reference << "material " << material_name << " not found." << std::endl;
		}
		else if(keyword == "mtllib")
		{
			// Materials were loaded in first pass
		}
		else if(keyword == "cstype")
		{
			std::string type;
			line_stream >> type;

			bool rational = false;
			if(type == "rat")
			{
				rational = true;
				line_stream >> type;
			}

			if(type == "bspline")
			{
				load_bspline(Document, rational, file2, line_count, Mesh, current_material, positions, v_index);
			}
			else
				k3d::log() << debug << "cstype not supported : " << type << std::endl;
		}
		else
		{
			// Not gonna handle this baby ...
			k3d::log() << warning << Path.native_console_string() << " line " << line_count << ": unsupported keyword will be ignored: " << line_buffer << std::endl;
		}
	}

	if(current_polyhedron)
		k3d::legacy::set_companions(*current_polyhedron);

	return true;
}

bool load_mtl(k3d::idocument& Document, const k3d::filesystem::path& Path, obj_materials_t& Materials)
{
	// Open mtl file ...
	k3d::filesystem::ifstream file(Path);
	if(!file)
	{
		k3d::log() << error << k3d_file_reference << ": error opening material [" << Path.native_console_string() << "]" << std::endl;
		return false;
	}

	// For tests
	k3d::log() << info << "Loading MTL file : " << Path.native_console_string() << std::endl;

	// Current processed material
	std::string material_name;
	k3d::inode* current_material = 0;
	k3d::inode* current_shader = 0;

	// Parse the mtl file ...
	std::string line_buffer;
	for(k3d::getline(file, line_buffer); file; k3d::getline(file, line_buffer))
	{
		// Skip empty lines ...
		if(!line_buffer.size())
			continue;

		// Skip comments ...
		if('#' == *line_buffer.begin())
			continue;

		// Extract a record type ...
		std::string keyword;
		std::istringstream line_stream(line_buffer);
		line_stream >> keyword;

		if(keyword == "newmtl")
		{
			line_stream >> material_name;

			if(!material_name.size())
			{
				material_name = default_material_name;
				k3d::log() << debug << "empty material name in file " << Path.native_console_string() << " - using default material" << std::endl;
			}

			if(Materials.find(material_name) == Materials.end())
			{
				current_material = k3d::create_plugin<k3d::inode>(k3d::classes::RenderManMaterial(), Document, material_name);
				return_val_if_fail(current_material, false);

				current_shader = k3d::create_plugin<k3d::inode>(k3d::classes::RenderManSurfaceShader(), Document, material_name + " shader");
				return_val_if_fail(current_shader, false);

				assert_warning(k3d::set_value(*current_shader, "shader_path", k3d::share_path() / k3d::filesystem::generic_path("shaders/k3d_plastic.sl")));
				assert_warning(k3d::set_value(*current_material, "surface_shader", current_shader));

				k3d::imaterial* mtl_material = dynamic_cast<k3d::imaterial*>(current_material);
				return_val_if_fail(mtl_material, false);

				Materials[material_name] = mtl_material;
			}
		}
		else if(keyword == "Ka")
		{
			k3d::color color;
			line_stream >> color;

			//assert_warning(k3d::set_value(*current_shader, "ambientcolor", color));
		}
		else if(keyword == "Kd")
		{
			k3d::color color;
			line_stream >> color;

			if(current_material)
				assert_warning(k3d::set_value(*current_material, "color", color));
		}
		else if(keyword == "Ks")
		{
			k3d::color color;
			line_stream >> color;

			if(current_shader)
				assert_warning(k3d::set_value(*current_shader, "specularcolor", color));
		}
		else if(keyword == "Ns")
		{
			double shininess;
			line_stream >> shininess;

			// Ns range is 0..1000
			if(shininess > 1000 || shininess < 0)
				shininess = 1000;

			// Now to range 0..1
			shininess /= 1000;

			//assert_warning(k3d::set_value(*current_shader, "shininess", color));
		}
		else if(keyword == "illum")
		{
			double luminance;
			line_stream >> luminance;

			k3d::log() << "Unhandled illuminance " << luminance << std::endl;
		}
		else if(keyword == "map_Kd")
		{
			// Material texture
			std::string mtlKdTextureFileName;
			line_stream >> mtlKdTextureFileName;

			k3d::log() << "Unhandled map_Kd " << mtlKdTextureFileName << std::endl;
			// - the same texture file can be used for many materials !
		}
		else
		{
			k3d::log() << "Unknown .mtl material record: " << keyword << " will be ignored" << std::endl;
		}
	}

	return true;
}

void get_obj_doubles(std::istringstream& Stream, std::vector<double>& List)
{
	while(true)
	{
		double value;
		Stream >> value;

		if(Stream.fail())
			return;

		List.push_back(value);
	}
}

bool load_bspline(k3d::idocument& Document, const bool rational, k3d::filesystem::ifstream& File, unsigned long& LineCount, k3d::legacy::mesh& Mesh, k3d::imaterial* Material, positions_t& Positions, const unsigned long CurrentSize)
{
	k3d::legacy::nupatch* current_nupatch = 0;

	std::string freeform_type("");
	unsigned long u_degree;
	unsigned long v_degree;

	std::string line_buffer;
	for(k3d::getline(File, line_buffer); File; k3d::getline(File, line_buffer))
	{
		++LineCount;

		std::string keyword;
		std::istringstream line_stream(line_buffer);
		line_stream >> keyword;

		if(keyword == "end")
			break;
		else if(keyword == "deg")
		{
			line_stream >> u_degree;
			line_stream >> v_degree;
		}
		else if(keyword == "surf")
		{
			freeform_type = "surf";

			if(current_nupatch)
				assert_warning(is_valid(*current_nupatch));

			current_nupatch = new k3d::legacy::nupatch();
			return_val_if_fail(current_nupatch, false);

			current_nupatch->material = Material;
			Mesh.nupatches.push_back(current_nupatch);

			current_nupatch->u_order = u_degree + 1;
			current_nupatch->v_order = v_degree + 1;

			double s0, s1, t0, t1;
			line_stream >> s0 >> s1 >> t0 >> t1;

			while(true)
			{
				long index;
				line_stream >> index;
				if(line_stream.fail())
					break;

				const unsigned long point_index = lookup(index, Positions.size(), CurrentSize);
				k3d::legacy::point* const position = Mesh.points[point_index];
				assert_warning(position);

				current_nupatch->control_points.push_back(k3d::legacy::nupatch::control_point(position, Positions[point_index][3]));
			}
		}
		else if(keyword == "parm")
		{
			if(freeform_type == "surf")
			{
				std::string type;
				line_stream >> type;
				if(type == "u")
					get_obj_doubles(line_stream, current_nupatch->u_knots);
				else if(type == "v")
					get_obj_doubles(line_stream, current_nupatch->v_knots);
				else
					k3d::log() << debug << "OBJ reader: Unknown cstype '" << type << "'" << std::endl;
			}
		}
		else
			k3d::log() << debug << "OBJ reader: unhandled free-form item '" << keyword << "'" << std::endl;
	}

	if(current_nupatch)
		return_val_if_fail(is_valid(*current_nupatch), false);

	return true;
}

} // namespace detail

} // namespace libk3dgeometry
*/

