#ifndef K3DSDK_SL_H
#define K3DSDK_SL_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares the Shading Language interface, which returns information (including parsed and cached metadata) about a RenderMan Shading Language shader
		\author Tim Shead (tshead@k-3d.com)
*/

#include "path.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace k3d
{

namespace sl
{

/// Encapsulates an argument to a Pixar RenderMan Shading Language shader
class argument
{
public:
	/// Enumerates Shading Language storage classes
	typedef enum
	{
		UNIFORM,
		VARYING,
	} storage_class_t;

	/// Enumerates Shading Language data types
	typedef enum
	{
		FLOAT,
		STRING,
		POINT,
		VECTOR,
		NORMAL,
		HPOINT,
		MATRIX,
		COLOR,
	} type_t;

	/// Enumerates "extended" data types
	typedef enum
	{
		EX_FLOAT,
			EX_TIME,
			EX_ANGLE,
			EX_DISTANCE,
			EX_AREA,
			EX_VOLUME,
			EX_MASS,
			EX_FORCE,
			EX_PRESSURE,
		EX_STRING,
			EX_TEXTURE,
			EX_SPACE,
		EX_POINT,
		EX_VECTOR,
		EX_NORMAL,
		EX_HPOINT,
		EX_MATRIX,
		EX_COLOR,
	} extended_type_t;

	/// Stores the argument name
	std::string name;
	/// Stores the argument label (human-readable string)
	std::string label;
	/// Stores the argument description (human-readable string)
	std::string description;
	/// Stores the Shading Language storage class
	storage_class_t storage_class;
	/// Stores the Shading Language data type
	type_t type;
	/// Stores the extended data type
	extended_type_t extended_type;
	/// Stores the number of array elements for this argument
	unsigned long array_count;
	/// Stores the coordinate system that this argument is declared within
	std::string space;
	/// Stores true iff this argument is an output (message passing) argument
	bool output;
	/// Stores the argument's default value as a string
	std::string default_value;

	// Enforce strict initialization
	argument(const std::string& Name, const std::string& Label, const std::string& Description, const storage_class_t StorageClass, const type_t Type, const extended_type_t ExtendedType, const unsigned long ArrayCount, const std::string& Space, const bool Output, const std::string& DefaultValue);
};

/// Encapsulates a Pixar RenderMan Shading Language shader
class shader
{
public:
	/// Enumerates shader types
	typedef enum
	{
		SURFACE,
		LIGHT,
		VOLUME,
		DISPLACEMENT,
		TRANSFORMATION,
		IMAGER,
	} type_t;

	/// Stores the shader file
	filesystem::path file_path;
	/// Stores the shader type
	type_t type;
	/// Stores the shader name
	std::string name;
	/// Stores the shader author(s)
	std::string authors;
	/// Stores the shader copyright
	std::string copyright;
	/// Stores a brief description of the shader
	std::string description;
	/// Stores the shader arguments
	typedef std::vector<argument> arguments_t;
	arguments_t arguments;

	shader(const type_t Type);
	shader(const filesystem::path& FilePath, const type_t Type, const std::string& Name, const std::string& Authors, const std::string& Copyright, const std::string& Description, const arguments_t Arguments);
};

/// A collection of shaders
typedef std::vector<shader> shaders_t;

bool operator==(const shader& LHS, const shader& RHS);
bool operator!=(const shader& LHS, const shader& RHS);
bool operator<(const shader& LHS, const shader& RHS);

// Serialization
std::ostream& operator<<(std::ostream& Stream, const argument::storage_class_t&);
std::istream& operator>>(std::istream& Stream, argument::storage_class_t&);
std::ostream& operator<<(std::ostream& Stream, const argument::type_t&);
std::istream& operator>>(std::istream& Stream, argument::type_t&);
std::ostream& operator<<(std::ostream& Stream, const argument::extended_type_t&);
std::istream& operator>>(std::istream& Stream, argument::extended_type_t&);
std::ostream& operator<<(std::ostream& Stream, const shader::type_t& Value);
std::istream& operator>>(std::istream& Stream, shader::type_t& Value);

/// Parses preprocessed RenderMan shading language source-code, returning shader data (note: could return an empty collection, SourcePath is for informational messages only)
shaders_t parse_source(std::istream& Stream, const filesystem::path& SourcePath);
/// Parses a shader metafile, returning shader data (note: could return an empty collection, SourcePath and MetafilePath are for informational messages only)
shaders_t parse_metafile(std::istream& Stream, const filesystem::path& SourcePath, const filesystem::path& MetafilePath);
/// Generates a shader metafile from a collection of shaders
void generate_metafile(std::ostream& Stream, const shaders_t Shaders);

} // namespace sl

} // namespace k3d

#endif // !K3DSDK_SL_H

