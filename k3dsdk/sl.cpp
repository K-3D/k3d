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
		\brief Implements the Shading Language library, which returns information (including parsed and cached metadata) about a RenderMan Shading Language shader
		\author Tim Shead (tshead@k-3d.com)
*/

#include "path.h"
#include "result.h"
#include "sl.h"
#include "xml.h"
using namespace k3d::xml;

#include <boost/lexical_cast.hpp>

#include <cassert>
#include <iostream>

namespace
{

// Define some character constants for parsing ...
static const char singlequote = '\'';
static const char doublequote = '"';
static const char escape = '\\';
static const char openbrace = '{';
static const char closebrace = '}';
static const char openparen = '(';
static const char closeparen = ')';
static const char openbracket = '[';
static const char closebracket = ']';
static const char equals = '=';
static const char comma = ',';
static const char semicolon = ';';

/// A string token
typedef std::string token_t;

/// Stores a collection of tokens
typedef std::vector<token_t> token_collection_t;

/////////////////////////////////////////////////////////////////////////////
// write_token

/// Stores a parsed token
void write_token(token_collection_t& Output, token_t& OutputToken)
{
	if(OutputToken.size())
		Output.push_back(OutputToken);

	OutputToken.clear();
}

/////////////////////////////////////////////////////////////////////////////
// parse_stream

/// Parses a stream into a series of whitespace-delimited tokens.
/// Handles string constants and special characters.
void parse_stream(std::istream& Stream, token_collection_t& Output)
{
	// Our input stream ...
	std::ostringstream stream;

	Stream.get(*stream.rdbuf(), '\0');
	stream << '\0';

	// Convert it to a string buffer ...
	std::string buffer = stream.str();
	//stream.freeze(false);

	// Loop through the buffer ...
	char stringliteral = 0;

	token_t token;

	for(token_t::iterator c = buffer.begin(); c != buffer.end(); c++)
	{
		if(stringliteral)
		{
			if((*c) == stringliteral && ((*(c-1)) != escape))
			{
				token += *c;
				write_token(Output, token);
				stringliteral = 0;
			}
			else if((*c) == stringliteral && ((*(c-1)) == escape))
			{
				token.erase(token.end()-1);
				token += *c;
			}
			else
			{
				token += *c;
			}
		}
		else if(isspace(*c))
		{
			if(stringliteral)
				token += *c;
			else
				write_token(Output, token);
		}
		else
		{
			switch(*c)
			{
				case singlequote:
				case doublequote:
					stringliteral = *c;
					token += *c;
					break;

				case openparen:
				case closeparen:
				case openbracket:
				case closebracket:
				case openbrace:
				case closebrace:
				case equals:
				case comma:
				case semicolon:
					write_token(Output, token);
					token = *c;
					write_token(Output, token);
					break;

				default:
					token += *c;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// is_output

/// Tests a token to see if it's the "output" token used for light -> shader message passing
bool is_output(const token_t& InputToken)
{
	return InputToken == "output" ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
// is_storage

/// Tests a token to see if it's a storage type
bool is_storage(const token_t& InputToken)
{
	static const char* types[] =
	{
		"uniform", "varying"
	};

	for(unsigned long i = 0; i < sizeof(types) / sizeof(const char*); i++)
	{
		if(InputToken == types[i])
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// is_argument_type

/// Tests a token to see if it's an argument type
bool is_argument_type(const token_t& InputToken)
{
	static const char* types[] =
	{
		"float",
		"string",
		"point",
		"vector",
		"normal",
		"hpoint",
		"matrix",
		"color"
	};

	for(unsigned long i = 0; i < sizeof(types) / sizeof(const char*); i++)
	{
		if(InputToken == types[i])
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// is_string

/// Tests a token to see if it's a string type
bool is_string(const token_t& InputToken)
{
	if((InputToken.substr(0, 1) == "\'") || (InputToken.substr(0, 1) == "\""))
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// parse_arguments

class point3
{
public:
	/// Stores the vector values
	double n[3];

	point3()
	{
		n[0] = n[1] = n[2] = 0.0;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const point3& RHS)
	{
		Stream << RHS.n[0] << " " << RHS.n[1] << " " << RHS.n[2];
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, point3& RHS)
	{
		Stream >> RHS.n[0];
		RHS.n[1] = RHS.n[2] = RHS.n[0];
		Stream >> RHS.n[1] >> RHS.n[2];

		return Stream;
	}
};

/// Parses tokens to pick out all of the shader arguments ...
k3d::sl::shader::arguments_t parse_arguments(token_collection_t& Arguments, const k3d::filesystem::path& SourcePath)
{
	k3d::sl::shader::arguments_t results;

	try
	{
		bool output = false;
		k3d::sl::argument::storage_class_t storage_class = k3d::sl::argument::UNIFORM;
		k3d::sl::argument::type_t type = k3d::sl::argument::FLOAT;
		k3d::sl::argument::extended_type_t extended_type = k3d::sl::argument::EX_FLOAT;

		for(token_collection_t::const_iterator token = Arguments.begin(); token != Arguments.end(); ++token)
		{
			// Look to see if this is an output variable, part of the light message-passing system ...
			if(is_output(*token))
			{
				output = true;
				++token;
			}

			// Check for a storage class ...
			if(is_storage(*token))
			{
				storage_class = boost::lexical_cast<k3d::sl::argument::storage_class_t>(*token);
				++token;
			}

			// Check for an argument type ...
			if(is_argument_type(*token))
			{
				type = boost::lexical_cast<k3d::sl::argument::type_t>(*token);
				switch(type)
				{
					case k3d::sl::argument::FLOAT:
						extended_type = k3d::sl::argument::EX_FLOAT;
						break;
					case k3d::sl::argument::STRING:
						extended_type = k3d::sl::argument::EX_STRING;
						break;
					case k3d::sl::argument::POINT:
						extended_type = k3d::sl::argument::EX_POINT;
						break;
					case k3d::sl::argument::VECTOR:
						extended_type = k3d::sl::argument::EX_VECTOR;
						break;
					case k3d::sl::argument::NORMAL:
						extended_type = k3d::sl::argument::EX_NORMAL;
						break;
					case k3d::sl::argument::HPOINT:
						extended_type = k3d::sl::argument::EX_HPOINT;
						break;
					case k3d::sl::argument::MATRIX:
						extended_type = k3d::sl::argument::EX_MATRIX;
						break;
					case k3d::sl::argument::COLOR:
						extended_type = k3d::sl::argument::EX_COLOR;
						break;
				}

				++token;
			}

			// Whatever's left over has to be the argument name ...
			const std::string name = *token;
			++token;

			// Check to see if this argument is an array ...
			unsigned long array_size = 1;
			if(*token == "[")
			{
				++token;
				array_size = boost::lexical_cast<unsigned long>(*token);
				++token;
				return_val_if_fail(*token == "]", results);
				++token;
			}

			// Sanity check for the (required) default value ...
			return_val_if_fail(*token == "=", results);
			++token;

			// Some types syntactically use the type name when declaring constants - we can safely skip these ...
			if(is_argument_type(*token))
				++token;

			// If the next token is a string, and the argument isn't of string type, it must be a color/point/vector space ...
			std::string space = "";
			if(is_string(*token) && (type != k3d::sl::argument::STRING))
			{
				space = *token;
				++token;
			}

			// Next, we start accumulating the argument default ...
			bool same_type = false;
			unsigned long depth = 0;
			std::string default_value = "";
			for( ; token != Arguments.end(); ++token)
			{
				if((*token == ")") && (0 == depth))
				{
					break;
				}
				else if((*token == "(") || (*token == "{"))
				{
					default_value += *token;
					++depth;
				}
				else if((*token == "}") || (*token == ")"))
				{
					default_value += *token;
					--depth;
				}
				else if(*token == ";")
				{
					break;
				}
				else if((*token == ",") && (0 == depth))
				{
					same_type = true;
					break;
				}
				else
				{
					default_value += *token;
				}
			}

			// Massage our "array" types for consistency ...
			if((type == k3d::sl::argument::COLOR) || (type == k3d::sl::argument::POINT) || (type == k3d::sl::argument::VECTOR) || (type == k3d::sl::argument::NORMAL))
			{
				std::istringstream ibuffer(default_value);
				point3 temp;
				ibuffer >> temp;

				std::ostringstream obuffer;
				obuffer << temp;
				default_value = obuffer.str();
			}

			// Get rid of excess quote marks and whitespace in strings ...
			if(default_value.size())
			{
				if(is_string(default_value))
					default_value = default_value.substr(1, default_value.size()-2);
			}

			if(space.size())
			{
				if(is_string(space))
					space = space.substr(1, space.size()-2);
			}

			// These can't be extracted from SL code
			const std::string label;
			const std::string description;

			// Push the new argument into our results ...
			results.push_back(k3d::sl::argument(name, label, description, storage_class, type, extended_type, array_size, space, output, default_value));

			// Clear our type information, unless our arguments were separated by commas ...
			if(!same_type)
			{
				output = false;
				storage_class = k3d::sl::argument::UNIFORM;
				type = k3d::sl::argument::FLOAT;
				extended_type = k3d::sl::argument::EX_FLOAT;
			}

			// Make sure we don't overrun the end of tokens ...
			if(token == Arguments.end())
				break;
		}
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Exception parsing arguments for [ " << SourcePath.native_console_string() << " ]: " << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "Unknown exception parsing arguments for [ " << SourcePath.native_console_string() << " ]" << std::endl;
	}

	return results;
}

/////////////////////////////////////////////////////////////////////////////
// parse_shader

/// Parses tokens looking for shaders, and returns the results ...
k3d::sl::shaders_t parse_shaders(token_collection_t& InputTokens, const k3d::filesystem::path& SourcePath)
{
	k3d::sl::shaders_t results;

	try
	{
		// Find the beginning of a shader (by looking for a shader type) ...
		std::vector<std::string> shader_types;
		shader_types.push_back("light");
		shader_types.push_back("surface");
		shader_types.push_back("volume");
		shader_types.push_back("displacement");
		shader_types.push_back("transformation");
		shader_types.push_back("imager");

		const token_collection_t::const_iterator type_token = std::find_first_of(InputTokens.begin(), InputTokens.end(), shader_types.begin(), shader_types.end());
		return_val_if_fail(type_token != InputTokens.end(), results);

		// The shader name should be the next token ...
		const token_collection_t::const_iterator name_token = type_token + 1;
		return_val_if_fail(name_token != InputTokens.end(), results);

		// We now have the beginning of our shader arguments ...
		token_collection_t::const_iterator start_token = name_token + 1;
		return_val_if_fail(start_token != InputTokens.end(), results);
		return_val_if_fail(*start_token == "(", results);

		// Look for the end of the shader arguments ... since some arguments use parentheses in their declarations,
		// we have to maintain a count of nested parenthesis to figure out when we're done ...
		token_collection_t::const_iterator end_token = start_token;
		unsigned long depth = 1;
		do
		{
			++end_token;

			if(*end_token == "(")
				++depth;
			else if(*end_token == ")")
				--depth;
		}
		while(depth);

		// We now have all the argument tokens ...
		token_collection_t arguments;
		std::copy(start_token + 1, end_token, std::back_inserter(arguments));

		// Add the new shader to the collection ...
		const k3d::sl::shader::type_t type = boost::lexical_cast<k3d::sl::shader::type_t>(*type_token);
		const std::string name = *name_token;

		results.push_back(k3d::sl::shader(SourcePath, type, name, "", "", "", parse_arguments(arguments, SourcePath)));
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Exception parsing shaders for [ " << SourcePath.native_console_string() << " ]: " << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "Unknown exception parsing shaders for [ " << SourcePath.native_console_string() << " ]" << std::endl;
	}

	return results;
}

} // namespace

namespace k3d
{

namespace sl
{

/////////////////////////////////////////////////////////////////////////////////////////////
// argument

argument::argument(const std::string& Name, const std::string& Label, const std::string& Description, const storage_class_t StorageClass, const type_t Type, const extended_type_t ExtendedType, const unsigned long ArrayCount, const std::string& Space, const bool Output, const std::string& DefaultValue) :
	name(Name),
	label(Label),
	description(Description),
	storage_class(StorageClass),
	type(Type),
	extended_type(ExtendedType),
	array_count(ArrayCount),
	space(Space),
	output(Output),
	default_value(DefaultValue)
{
	// Sanity checks ...
	assert(name.size());
}

/////////////////////////////////////////////////////////////////////////////////////////////
// shader

shader::shader(const type_t Type) :
	type(Type),
	name("null")
{
};

shader::shader(const filesystem::path& FilePath, const type_t Type, const std::string& Name, const std::string& Authors, const std::string& Copyright, const std::string& Description, const arguments_t Arguments) :
	file_path(FilePath),
	type(Type),
	name(Name),
	authors(Authors),
	copyright(Copyright),
	description(Description),
	arguments(Arguments)
{
	assert(name.size());
}

bool operator==(const shader& LHS, const shader& RHS)
{
	return LHS.name == RHS.name;
}

bool operator!=(const shader& LHS, const shader& RHS)
{
	return !(LHS == RHS);
}

bool operator<(const shader& LHS, const shader& RHS)
{
	return LHS.name < RHS.name;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// serialization

std::ostream& operator<<(std::ostream& Stream, const argument::storage_class_t& Value)
{
	switch(Value)
	{
		case argument::UNIFORM:
			Stream << "uniform";
			break;
		case argument::VARYING:
			Stream << "varying";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, argument::storage_class_t& Value)
{
	std::string text;
	Stream >> text;

	if(text == "uniform")
		Value = argument::UNIFORM;
	else if(text == "varying")
		Value = argument::VARYING;
	else
	{
		log() << error << "Unknown Shading Language Storage Class: [" << text << "]" << std::endl;
	}

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const argument::type_t& Value)
{
	switch(Value)
	{
		case argument::FLOAT:
			Stream << "float";
			break;
		case argument::STRING:
			Stream << "string";
			break;
		case argument::POINT:
			Stream << "point";
			break;
		case argument::VECTOR:
			Stream << "vector";
			break;
		case argument::NORMAL:
			Stream << "normal";
			break;
		case argument::HPOINT:
			Stream << "hpoint";
			break;
		case argument::MATRIX:
			Stream << "matrix";
			break;
		case argument::COLOR:
			Stream << "color";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, argument::type_t& Value)
{
	std::string text;
	Stream >> text;

	if(text == "float")
		Value = argument::FLOAT;
	else if(text == "string")
		Value = argument::STRING;
	else if(text == "point")
		Value = argument::POINT;
	else if(text == "vector")
		Value = argument::VECTOR;
	else if(text == "normal")
		Value = argument::NORMAL;
	else if(text == "hpoint")
		Value = argument::HPOINT;
	else if(text == "matrix")
		Value = argument::MATRIX;
	else if(text == "color")
		Value = argument::COLOR;
	else
	{
		log() << error << "Unknown Shading Language Type: [" << text << "]" << std::endl;
	}

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const argument::extended_type_t& Value)
{
	switch(Value)
	{
		case argument::EX_FLOAT:
			Stream << "float";
			break;
		case argument::EX_TIME:
			Stream << "time";
			break;
		case argument::EX_ANGLE:
			Stream << "angle";
			break;
		case argument::EX_DISTANCE:
			Stream << "distance";
			break;
		case argument::EX_AREA:
			Stream << "area";
			break;
		case argument::EX_VOLUME:
			Stream << "volume";
			break;
		case argument::EX_MASS:
			Stream << "mass";
			break;
		case argument::EX_FORCE:
			Stream << "force";
			break;
		case argument::EX_PRESSURE:
			Stream << "pressure";
			break;
		case argument::EX_STRING:
			Stream << "string";
			break;
		case argument::EX_TEXTURE:
			Stream << "texture";
			break;
		case argument::EX_SPACE:
			Stream << "space";
			break;
		case argument::EX_POINT:
			Stream << "point";
			break;
		case argument::EX_VECTOR:
			Stream << "vector";
			break;
		case argument::EX_NORMAL:
			Stream << "normal";
			break;
		case argument::EX_HPOINT:
			Stream << "hpoint";
			break;
		case argument::EX_MATRIX:
			Stream << "matrix";
			break;
		case argument::EX_COLOR:
			Stream << "color";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, argument::extended_type_t& Value)
{
	std::string text;
	Stream >> text;

	if(text == "float")
		Value = argument::EX_FLOAT;
	else if(text == "time")
		Value = argument::EX_TIME;
	else if(text == "angle")
		Value = argument::EX_ANGLE;
	else if(text == "distance")
		Value = argument::EX_DISTANCE;
	else if(text == "area")
		Value = argument::EX_AREA;
	else if(text == "volume")
		Value = argument::EX_VOLUME;
	else if(text == "mass")
		Value = argument::EX_MASS;
	else if(text == "force")
		Value = argument::EX_FORCE;
	else if(text == "pressure")
		Value = argument::EX_PRESSURE;
	else if(text == "string")
		Value = argument::EX_STRING;
	else if(text == "texture")
		Value = argument::EX_TEXTURE;
	else if(text == "space")
		Value = argument::EX_SPACE;
	else if(text == "point")
		Value = argument::EX_POINT;
	else if(text == "vector")
		Value = argument::EX_VECTOR;
	else if(text == "normal")
		Value = argument::EX_NORMAL;
	else if(text == "hpoint")
		Value = argument::EX_HPOINT;
	else if(text == "matrix")
		Value = argument::EX_MATRIX;
	else if(text == "color")
		Value = argument::EX_COLOR;
	else
	{
		log() << error << "Unknown Extended Shading Language Type: [" << text << "]" << std::endl;
	}

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const shader::type_t& Value)
{
	switch(Value)
	{
		case shader::SURFACE:
			Stream << "surface";
			break;
		case shader::LIGHT:
			Stream << "light";
			break;
		case shader::VOLUME:
			Stream << "volume";
			break;
		case shader::DISPLACEMENT:
			Stream << "displacement";
			break;
		case shader::TRANSFORMATION:
			Stream << "transformation";
			break;
		case shader::IMAGER:
			Stream << "imager";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, shader::type_t& Value)
{
	std::string text;
	Stream >> text;

	if(text == "surface")
		Value = shader::SURFACE;
	else if(text == "light")
		Value = shader::LIGHT;
	else if(text == "volume")
		Value = shader::VOLUME;
	else if(text == "displacement")
		Value = shader::DISPLACEMENT;
	else if(text == "transformation")
		Value = shader::TRANSFORMATION;
	else if(text == "imager")
		Value = shader::IMAGER;
	else
	{
		log() << error << "Unknown Shading Language Shader Type: [" << text << "]" << std::endl;
	}

	return Stream;
}

shaders_t parse_source(std::istream& Stream, const filesystem::path& SourcePath)
{
	token_collection_t tokens;
	parse_stream(Stream, tokens);

	return parse_shaders(tokens, SourcePath);
}

shaders_t parse_metafile(std::istream& Stream, const filesystem::path& SourcePath, const filesystem::path& MetafilePath)
{
	shaders_t results;

	element xml_metafile("k3dml");
	try
	{
		hide_progress progress;
		parse(xml_metafile, Stream, MetafilePath.native_utf8_string().raw(), progress);
	}
	catch(std::exception& e)
	{
		log() << error << "Exception parsing metafile [" << MetafilePath.native_console_string() << "]: " << e.what() << std::endl;
		return results;
	}

	// Make sure it's a <k3dml> document ...
	return_val_if_fail(xml_metafile.name == "k3dml", results);

	// Get the first child, which should be our <shaders> collection ...
	element* const xml_shaders = find_element(xml_metafile, "shaders");
	return_val_if_fail(xml_shaders, results);

	// Iterate through the children, which should be <shader> elements ...
	for(element::elements_t::iterator xml_shader = xml_shaders->children.begin(); xml_shader != xml_shaders->children.end(); ++xml_shader)
	{
		try
		{
			if(xml_shader->name != "shader")
				continue;

			// Extract the shader name ...
			const std::string name = attribute_text(*xml_shader, "name");
			if(name.empty())
				continue;

			const std::string authors = element_text(*xml_shader, "authors");
			const std::string copyright = element_text(*xml_shader, "copyright");
			const std::string description = element_text(*xml_shader, "description");
			const shader::type_t type = boost::lexical_cast<shader::type_t>(attribute_text(*xml_shader, "type"));

			// Extract shader arguments ...
			shader::arguments_t arguments;
			for(element::elements_t::iterator xml_argument = xml_shader->children.begin(); xml_argument != xml_shader->children.end(); ++xml_argument)
			{
				if(xml_argument->name != "argument")
					continue;

				// Extract the argument name ...
				const std::string name = attribute_text(*xml_argument, "name");
				if(name.empty())
					continue;

				const std::string label = attribute_text(*xml_argument, "label");
				const std::string description = attribute_text(*xml_argument, "description");

				const argument::storage_class_t storage_class = boost::lexical_cast<argument::storage_class_t>(attribute_text(*xml_argument, "storage_class"));
				const argument::type_t type = boost::lexical_cast<argument::type_t>(attribute_text(*xml_argument, "type"));
				const argument::extended_type_t extended_type = boost::lexical_cast<argument::extended_type_t>(attribute_text(*xml_argument, "extended_type"));
				const unsigned long array_count = boost::lexical_cast<unsigned long>(attribute_text(*xml_argument, "array_count"));
				const std::string space = attribute_text(*xml_argument, "space");
				const bool output = attribute_text(*xml_argument, "output") == "true" ? true : false;
				const std::string default_value = attribute_text(*xml_argument, "default_value");

				arguments.push_back(argument(name, label, description, storage_class, type, extended_type, array_count, space, output, default_value));
			}

			// Insert the shader into the collection ...
			results.push_back(shader(SourcePath, type, name, authors, copyright, description, arguments));
		}
		catch(std::exception& e)
		{
			log() << error << "Exception loading metafile [ " << MetafilePath.native_console_string() << " ]: " << e.what() << std::endl;
		}
	}

	return results;
}

void generate_metafile(std::ostream& Stream, const shaders_t Shaders)
{
	element xml_document("k3dml");
	element& xml_shaders = xml_document.append(element("shaders"));

	for(shaders_t::const_iterator shader = Shaders.begin(); shader != Shaders.end(); ++shader)
	{
		element& xml_shader = xml_shaders.append(element("shader"));
		xml_shader.append(attribute("name", shader->name));
		xml_shader.append(attribute("type", boost::lexical_cast<std::string>(shader->type)));

		xml_shader.append(element("description", shader->description));
		xml_shader.append(element("authors", shader->authors));
		xml_shader.append(element("copyright", shader->copyright));

		for(shader::arguments_t::const_iterator argument = shader->arguments.begin(); argument != shader->arguments.end(); ++argument)
		{
			element& xml_argument = xml_shader.append(element("argument"));
			xml_argument.append(attribute("name", argument->name));
			xml_argument.append(attribute("label", argument->label));
			xml_argument.append(attribute("description", argument->description));
			xml_argument.append(attribute("storage_class", boost::lexical_cast<std::string>(argument->storage_class)));
			xml_argument.append(attribute("type", boost::lexical_cast<std::string>(argument->type)));
			xml_argument.append(attribute("extended_type", boost::lexical_cast<std::string>(argument->extended_type)));
			xml_argument.append(attribute("array_count", boost::lexical_cast<std::string>(argument->array_count)));
			xml_argument.append(attribute("space", argument->space));
			xml_argument.append(attribute("output", argument->output ? "true" : "false"));
			xml_argument.append(attribute("default_value", argument->default_value));
		}
	}

	Stream << k3d::xml::declaration() << xml_document << std::endl;
}

} // namespace sl

} // namespace k3d

