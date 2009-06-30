#ifndef K3DSDK_XML_H
#define K3DSDK_XML_H

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
		\brief Declares the public API of k3dxml, an XML API designed to fit-in well with the Standard C++ Library
		\author Tim Shead (tshead@k-3d.com)
*/

#include "string_cast.h"

#include <iosfwd>
#include <sstream>
#include <vector>

namespace k3d
{

namespace xml
{

/// Encapsulates an XML attribute with name and value
class attribute
{
public:
	attribute() {}
	attribute(const std::string& Name, const std::string& Value) : name(Name), value(Value) {}
	attribute(const std::string& Name, const char* Value) : name(Name), value(Value) {}

	template<typename T>
	attribute(const std::string& Name, const T& Value) : name(Name), value(string_cast(Value)) {}

	/// Stores the attribute name
	std::string name;
	/// Stores the attribute value
	std::string value;
};

/// Encapsulates an XML element with name, text, child attributes, and child elements
class element
{
public:
	element(){}

	element(const std::string& Name) : name(Name) {}
	
	template<typename T1>	
	element(const std::string& Name, const T1& A1) : name(Name) { push_back(A1); }

	template<typename T1, typename T2>
	element(const std::string& Name, const T1& A1, const T2& A2) : name(Name) { push_back(A1); push_back(A2); }

	template<typename T1, typename T2, typename T3>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3) : name(Name) { push_back(A1); push_back(A2); push_back(A3); }
	
	template<typename T1, typename T2, typename T3, typename T4>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); }
	
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9);}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10);}

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10, const T11& A11) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10); push_back(A11); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10, const T11& A11, const T12& A12) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10); push_back(A11); push_back(A12); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10, const T11& A11, const T12& A12, const T13& A13) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10); push_back(A11); push_back(A12); push_back(A13); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10, const T11& A11, const T12& A12, const T13& A13, const T14& A14) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10); push_back(A11); push_back(A12); push_back(A13); push_back(A14); }

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
	element(const std::string& Name, const T1& A1, const T2& A2, const T3& A3, const T4& A4, const T5& A5, const T6& A6, const T7& A7, const T8& A8, const T9& A9, const T10& A10, const T11& A11, const T12& A12, const T13& A13, const T14& A14, const T15& A15) : name(Name) { push_back(A1); push_back(A2); push_back(A3); push_back(A4); push_back(A5); push_back(A6); push_back(A7); push_back(A8); push_back(A9); push_back(A10); push_back(A11); push_back(A12); push_back(A13); push_back(A14); push_back(A15); }

	void push_back(const std::string& Value) { text += Value; }
	void push_back(const attribute& Value) { attributes.push_back(Value); }
	void push_back(const element& Value) { children.push_back(Value); }
	
	/// Searches for a child element by name, creating it if it isn't found
	element& safe_element(const std::string& Name);
	/// Searches for a child element, creating it from the match if it isn't found
	element& safe_element(const element& Match);
	/// Searches for a child element, creating it from the prototype if it isn't found
	element& safe_element(const element& Match, const element& Prototype);

	/// Appends a child attribute, returning a reference to the new child for convenience
	attribute& append(const attribute& Child);
	/// Appends a child element, returning a reference to the new child for convenience
	element& append(const element& Child);

	/// Stores the element name
	std::string name;
	/// Stores element text
	std::string text;

	/// Defines a collection of attributes
	typedef std::vector<attribute> attributes_t;
	/// Stores child attributes
	attributes_t attributes;
	/// Defines a collection of child elements
	typedef std::vector<element> elements_t;
	/// Stores child elements
	elements_t children;
};

class same_name
{
public:
	same_name(const std::string& Name) : name(Name) {}

	template<typename T>
	bool operator()(const T& Val) { return Val.name == name; }

private:
	const std::string name;
};

/// Sets an attribute on a child element, overwriting any existing attribute value
attribute& set_attribute(element& Element, const attribute& Child);
/*
/// Appends a child attribute to an element, returning a reference to the new child for convenience
attribute& append(element& Element, const attribute& Child);
/// Appends a child element to an element, returning a reference to the new child for convenience
element& append(element& Element, const element& Child);
*/

/// Searches for a child attribute by name, returning the child if found, else NULL
const attribute* find_attribute(const element& Element, const std::string& AttributeName);
/// Searches for a child attribute by name, returning the child if found, else NULL
attribute* find_attribute(element& Element, const std::string& AttributeName);
/// Searches for a child element by name, returning the child if found, else NULL
const element* find_element(const element& Element, const std::string& ElementName);
/// Searches for a child element by name, returning the child if found, else NULL
element* find_element(element& Element, const std::string& ElementName);
/// Searches for a child element by name, returning the child if found, else NULL
element* find_element(element& Element, const std::string& ElementName);

/// Returns the text value of an attribute as a string by name, or a default value if the attribute can't be found
const std::string attribute_text(const element& Element, const std::string& AttributeName, const std::string& DefaultValue = std::string());
/// Returns the text value of an attribute, converted to a specific type, or a default value if the attribute can't be found
template<typename T>
const T attribute_value(const element& Element, const std::string& AttributeName, const T& DefaultValue)
{
	const attribute* const a = find_attribute(Element, AttributeName);
	if(a)
		return k3d::from_string<T>(a->value, DefaultValue);
	
	return DefaultValue;
}

/// Removes an attribute from an element, returning its string value if it exists, or empty string.
const std::string remove_attribute(element& Element, const std::string& AttributeName);

/// Returns the text value of a child element as a string by name, or a default value if the child element can't be found
const std::string element_text(const element& Element, const std::string& ElementName, const std::string& DefaultValue = std::string());

/// iostream-compatible manipulator that controls whether subsequent XML will be serialized with-or-without line breaks
struct single_line
{
	single_line(bool Enable = true);
	bool enable;
};
std::ostream& operator<<(std::ostream& Stream, const single_line& RHS);

/// Inserts an XML declaration <?xml version="1.0" ?> into a stream
struct declaration
{
};
std::ostream& operator<<(std::ostream& Stream, const declaration& RHS);

/// Serializes an XML attribute to a stream
std::ostream& operator<<(std::ostream& Stream, const attribute& RHS);
/// Serializes an XML document to a stream
std::ostream& operator<<(std::ostream& Stream, const element& RHS);
/// Extracts an XML document from a stream.  Throws std::runtime_error if there are any problems.
std::istream& operator>>(std::istream& Stream, element& RHS);

/// Abstract interface used to provide feedback during long parsing operations
class progress
{
public:
	virtual void show_activity() = 0;

protected:
	progress() {}
	progress(const progress&) {}
	progress& operator=(const progress&) { return *this; }
	virtual ~progress() {}
};

/// Convenience implementation of the progress interface for use when feedback is not required (Null Object Pattern)
class hide_progress :
	public progress
{
public:
	void show_activity() {}
};

/// Parses an XML document from a stream.  Throws std::runtime_error if there are any problems.
void parse(element& Root, std::istream& InputStream, const std::string& StreamName, progress& Progress);

} // namespace xml

} // namespace k3d

#endif // !K3DSDK_XML_H

