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
		\brief Implements k3dxml, an XML API designed to fit-in well with the Standard C++ Library
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iomanip.h"
#include "log.h"
#include "string_cast.h"
#include "xml.h"

#include <iostream>
#include <iterator>
#include <stack>
#include <stdexcept>

#if defined K3D_HAVE_EXPAT

	#include <expat.h>
	#define BACKEND_PARSER expat_parser
	
#elif defined K3D_HAVE_LIBXML2

	#include <libxml/parser.h>
	#define BACKEND_PARSER libxml2_parser
	
#else

	#error No XML parser defined ... did you forget to run configure?

#endif

namespace k3d
{

namespace xml
{

namespace detail
{

// We provide a set of iostream-compatible manipulators for formatting XML output

/// Returns true iff the XML should be serialized on a single line, otherwise false
long& single_line(std::ios& Stream)
{
	static const int index = std::ios::xalloc();
	return Stream.iword(index);
}

/// Inserts a "virtual" line break into a stream, the actual output is controlled by the state of the single_line flag
std::ostream& eol(std::ostream& Stream)
{
	if(!single_line(Stream))
		Stream << "\n";
	
	return Stream;
}

/// Inserts whitespace into a stream, proportional to its indentation level (we override the default to implement single_line behavior).
std::ostream& indentation(std::ostream& Stream)
{
	if(!single_line(Stream))
		Stream << std::string(k3d::current_indent(Stream), '\t'); 
	return Stream;
}

/// Returns the input string with leading and trailing whitespace removed
const std::string trim(const std::string& String)
{
	std::string::size_type string_size = String.size();

	std::string::size_type start = 0;
	for(; start != string_size; ++start)
	{
		if(!isspace(String[start]))
			break;
	}

	std::string::size_type end = string_size;
	for(; end != start; --end)
	{
		if(!isspace(String[end-1]))
			break;
	}

	return std::string(String.substr(start, end - start));
}

/// Returns the input string, with special characters encoded for XML
const std::string encode(const std::string& String)
{
	std::string result;
	result.reserve(String.size());

	for(std::string::size_type i = 0; i != String.size(); ++i)
	{
		if(String[i] == '&')
			result += "&amp;";
		else if(String[i] == '<')
			result += "&lt;";
		else if(String[i] == '>')
			result += "&gt;";
		else if(String[i] == '\'')
			result += "&apos;";
		else if(String[i] == '\"')
			result += "&quot;";
		else
			result += String[i];
	}

	return result;
}

#ifdef K3D_HAVE_EXPAT

/// Adaptor class that uses James Clark's expat library to parse a stream into an xml::element - replace this if you prefer some other parser
class expat_parser
{
public:
	typedef expat_parser this_t;

	expat_parser() :
		parser(XML_ParserCreate(0)),
		root(0)
	{
		XML_SetUserData(parser, this);
		XML_SetStartElementHandler(parser, raw_start_element_handler);
		XML_SetEndElementHandler(parser, raw_end_element_handler);
		XML_SetCharacterDataHandler(parser, raw_character_data_handler);
	}

	~expat_parser()
	{
		XML_ParserFree(parser);
	}

	const char* error_description()
	{
		static std::string buffer;
		buffer = XML_ErrorString(XML_GetErrorCode(parser)) + std::string(" line: ") + string_cast(XML_GetCurrentLineNumber(parser)) + std::string(" column: ") + string_cast(XML_GetCurrentColumnNumber(parser));
		
		return buffer.c_str();
	}

	void parse(xml::element& Root, std::istream& InputStream, const std::string& StreamName, progress& Progress)
	{
		root = &Root;

		while(!element_stack.empty())
			element_stack.pop();

		std::vector<char> buffer(1024);
		for(InputStream.read(&buffer[0], buffer.size()); InputStream; InputStream.read(&buffer[0], buffer.size()))
		{
			Progress.show_activity();
			if(XML_STATUS_ERROR == XML_Parse(parser, &buffer[0], InputStream.gcount(), false))
				throw std::runtime_error(error_description());
		}
		Progress.show_activity();
		if(XML_STATUS_ERROR == XML_Parse(parser, &buffer[0], InputStream.gcount(), true))
			throw std::runtime_error(error_description());
	}

private:
	void start_element_handler(const XML_Char* Name, const XML_Char** Attributes)
	{
		if(element_stack.empty())
		{
			root->name = Name;
			element_stack.push(root);
		}
		else
		{
			element_stack.top()->children.push_back(xml::element(Name));
			element_stack.push(&element_stack.top()->children.back());
		}


		if(Attributes)
		{
			for(unsigned long i = 0; Attributes[i]; i+=2)
				element_stack.top()->attributes.push_back(xml::attribute(Attributes[i], Attributes[i+1]));
		}
	}

	void end_element_handler(const XML_Char* Name)
	{
		if(!element_stack.empty())
		{
			element_stack.top()->text = trim(element_stack.top()->text);
			element_stack.pop();
		}
	}

	void character_data_handler(const XML_Char* Data, int Length)
	{
		element_stack.top()->text += std::string(Data, Length);
	}

	static void raw_start_element_handler(void* UserData, const XML_Char* Name, const XML_Char** Attributes)
	{
		reinterpret_cast<this_t*>(UserData)->start_element_handler(Name, Attributes);
	}

	static void raw_end_element_handler(void* UserData, const XML_Char* Name)
	{
		reinterpret_cast<this_t*>(UserData)->end_element_handler(Name);
	}

	static void raw_character_data_handler(void* UserData, const XML_Char* Data, int Length)
	{
		reinterpret_cast<this_t*>(UserData)->character_data_handler(Data, Length);
	}

	XML_Parser parser;
	xml::element* root;
	std::stack<xml::element*> element_stack;
};

#endif // K3D_HAVE_EXPAT

#ifdef K3D_HAVE_LIBXML2

/// Adaptor class that uses the Gnome libxml2 library to parse a stream into an xml::element - replace this if you prefer some other parser
class libxml2_parser
{
public:
	typedef libxml2_parser this_t;

	libxml2_parser() :
		root(0)
	{
		memset(&sax_handler, 0, sizeof(sax_handler));
		sax_handler.startElement = raw_start_element_handler;
		sax_handler.endElement = raw_end_element_handler;
		sax_handler.characters = raw_character_data_handler;
		sax_handler.warning = raw_warning_handler;
		sax_handler.error = raw_error_handler;
		sax_handler.fatalError = raw_fatal_error_handler;
	}

	void parse(k3d::xml::element& Root, std::istream& InputStream, const std::string& StreamName, k3d::xml::progress& Progress)
	{
		parser_context parser(xmlCreatePushParserCtxt(&sax_handler, this, 0, 0, StreamName.c_str()));
		xmlCtxtUseOptions(parser, XML_PARSE_NOENT);
		
		root = &Root;

		while(!element_stack.empty())
			element_stack.pop();

		std::vector<char> buffer(1024);
		for(InputStream.read(&buffer[0], buffer.size()); InputStream; InputStream.read(&buffer[0], buffer.size()))
		{
			Progress.show_activity();
			if(XML_ERR_OK != xmlParseChunk(parser, &buffer[0], InputStream.gcount(), false))
				throw std::runtime_error(error_description(parser));
		}
		Progress.show_activity();
		if(XML_ERR_OK != xmlParseChunk(parser, &buffer[0], InputStream.gcount(), true))
			throw std::runtime_error(error_description(parser));
	}

private:
	const char* error_description(xmlParserCtxtPtr Context)
	{
		std::ostringstream buffer;

		buffer << "XML parsing error";
		
		static std::string results;

		results = buffer.str();
		results.erase(std::remove(results.begin(), results.end(), '\n'), results.end());

		return results.c_str();
	}

	class parser_context
	{
	public:
		parser_context(xmlParserCtxtPtr Context) :
			context(Context)
		{
		}

		~parser_context()
		{
			xmlFreeParserCtxt(context);
		}

		operator xmlParserCtxtPtr()
		{
			return context;
		}
		
	private:
		xmlParserCtxtPtr context;
	};

	// This may-or-may-not-be safe-to-do, but at least it's all in one place
	static const char* utf8_to_char(const xmlChar* RHS)
	{
		return reinterpret_cast<const char*>(RHS);
	}

	void start_element_handler(const xmlChar* Name, const xmlChar** Attributes)
	{
		if(element_stack.empty())
		{
			root->name = utf8_to_char(Name);
			element_stack.push(root);
		}
		else
		{
			element_stack.top()->children.push_back(k3d::xml::element(utf8_to_char(Name)));
			element_stack.push(&element_stack.top()->children.back());
		}

		if(Attributes)
		{
			for(unsigned long i = 0; Attributes[i]; i+=2)
				element_stack.top()->attributes.push_back(k3d::xml::attribute(utf8_to_char(Attributes[i]), utf8_to_char(Attributes[i+1])));
		}
	}

	void end_element_handler(const xmlChar* Name)
	{
		if(!element_stack.empty())
		{
			element_stack.top()->text = trim(element_stack.top()->text);
			element_stack.pop();
		}
	}

	void character_data_handler(const xmlChar* Data, int Length)
	{
		element_stack.top()->text += std::string(utf8_to_char(Data), Length);
	}

	static void raw_start_element_handler(void* UserData, const xmlChar* Name, const xmlChar** Attributes)
	{
		reinterpret_cast<this_t*>(UserData)->start_element_handler(Name, Attributes);
	}

	static void raw_end_element_handler(void* UserData, const xmlChar* Name)
	{
		reinterpret_cast<this_t*>(UserData)->end_element_handler(Name);
	}

	static void raw_character_data_handler(void* UserData, const xmlChar* Data, int Length)
	{
		reinterpret_cast<this_t*>(UserData)->character_data_handler(Data, Length);
	}

	static void raw_warning_handler(void* UserData, const char* Message, ...)
	{
		va_list args;
		va_start(args, Message);

		std::string buffer(256, '\0');
		vsnprintf(&buffer[0], buffer.size(), Message, args);
		buffer.resize(strlen(buffer.c_str()));
		
		log() << warning << buffer << std::flush;
		
		va_end(args);
	}

	static void raw_error_handler(void* UserData, const char* Message, ...)
	{
		va_list args;
		va_start(args, Message);
		
		std::string buffer(256, '\0');
		vsnprintf(&buffer[0], buffer.size(), Message, args);
		buffer.resize(strlen(buffer.c_str()));
		
		log() << error << buffer << std::flush;
		
		va_end(args);
	}

	static void raw_fatal_error_handler(void* UserData, const char* Message, ...)
	{
		va_list args;
		va_start(args, Message);
		
		std::string buffer(256, '\0');
		vsnprintf(&buffer[0], buffer.size(), Message, args);
		buffer.resize(strlen(buffer.c_str()));
		
		log() << critical << buffer << std::flush;
		
		va_end(args);
	}

	xmlSAXHandler sax_handler;
	k3d::xml::element* root;
	std::stack<k3d::xml::element*> element_stack;
};

#endif // K3D_HAVE_LIBXML2

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// single_line

single_line::single_line(bool Enable) :
	enable(Enable)
{
}

std::ostream& operator<<(std::ostream& Stream, const single_line& RHS)
{
	detail::single_line(Stream) = RHS.enable;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// declaration

std::ostream& operator<<(std::ostream& Stream, const declaration& RHS)
{
	Stream << "<?xml version=\"1.0\" ?>" << detail::eol;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// element

element& element::safe_element(const std::string& Name)
{
	
	for(element::elements_t::iterator child = children.begin(); child != children.end(); ++child)
	{
		if(child->name == Name)
			return *child;
	}

	return append(element(Name));
}

element& element::safe_element(const element& Match)
{
	return safe_element(Match, Match);
}

element& element::safe_element(const element& Match, const element& Prototype)
{
	for(element::elements_t::iterator child = children.begin(); child != children.end(); ++child)
	{
		if(child->name != Match.name)
			continue;
			
		element::attributes_t::const_iterator search_attribute;
		for(search_attribute = Match.attributes.begin(); search_attribute != Match.attributes.end(); ++search_attribute)
		{
			const attribute* const a = find_attribute(*child, search_attribute->name);
			if(!a)
				break;

			if(a->value != search_attribute->value)
				break;
		}
			
		if(search_attribute != Match.attributes.end())
			continue;
			
		return *child;
	}

	return append(Prototype);
}

attribute& element::append(const attribute& Child)
{
	attributes.push_back(Child);
	return attributes.back();
}

element& element::append(const element& Child)
{
	children.push_back(Child);
	return children.back();
}

const attribute* find_attribute(const element& Element, const std::string& AttributeName)
{
	for(element::attributes_t::const_iterator child = Element.attributes.begin(); child != Element.attributes.end(); ++child)
	{
		if(child->name == AttributeName)
			return &(*child);
	}

	return 0;
}

attribute* find_attribute(element& Element, const std::string& AttributeName)
{
	for(element::attributes_t::iterator child = Element.attributes.begin(); child != Element.attributes.end(); ++child)
	{
		if(child->name == AttributeName)
			return &(*child);
	}

	return 0;
}

const element* find_element(const element& Element, const std::string& ElementName)
{
	for(element::elements_t::const_iterator child = Element.children.begin(); child != Element.children.end(); ++child)
	{
		if(child->name == ElementName)
			return &(*child);
	}

	return 0;
}

element* find_element(element& Element, const std::string& ElementName)
{
	for(element::elements_t::iterator child = Element.children.begin(); child != Element.children.end(); ++child)
	{
		if(child->name == ElementName)
			return &(*child);
	}

	return 0;
}

attribute& set_attribute(element& Element, const attribute& Attribute)
{
	attribute* const a = find_attribute(Element, Attribute.name);
	if(a)
	{
		a->value = Attribute.value;
		return *a;
	}

	return Element.append(Attribute);
}

const std::string attribute_text(const element& Element, const std::string& AttributeName, const std::string& DefaultValue)
{
	const attribute* child = find_attribute(Element, AttributeName);
	return child ? child->value : DefaultValue;
}

const std::string element_text(const element& Element, const std::string& ElementName, const std::string& DefaultValue)
{
	const element* child = find_element(Element, ElementName);
	return child ? child->text : DefaultValue;
}

std::ostream& operator<<(std::ostream& Stream, const attribute& RHS)
{
	Stream << RHS.name << "=\"" << detail::encode(RHS.value) << "\"";
	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const element& RHS)
{
	Stream << detail::indentation << "<" << RHS.name;

	for(element::attributes_t::const_iterator a = RHS.attributes.begin(); a != RHS.attributes.end(); ++a)
		Stream << " " << *a;
	
	if(RHS.children.empty() && RHS.text.empty())
	{
		Stream << "/>" << detail::eol;
		return Stream;
	}

	Stream << ">" << detail::encode(RHS.text);

	if(RHS.children.size())
	{
		Stream << detail::eol << push_indent;
		std::copy(RHS.children.begin(), RHS.children.end(), std::ostream_iterator<element>(Stream));
		Stream << pop_indent << detail::indentation;
	}

	Stream << "</" << RHS.name << ">" << detail::eol;

	return Stream;
}

std::istream& operator>>(std::istream& Stream, element& RHS)
{
	hide_progress p;
	parse(RHS, Stream, "", p);
	return Stream;
}

void parse(element& Root, std::istream& InputStream, const std::string& StreamName, progress& Progress)
{
	detail::BACKEND_PARSER parser;
	parser.parse(Root, InputStream, StreamName, Progress);
}

} // namespace xml

} // namespace k3d

