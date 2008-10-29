#ifndef K3DSDK_EXPRESSION_BASIC_PARSER_H
#define K3DSDK_EXPRESSION_BASIC_PARSER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <k3dsdk/types.h>

namespace k3d
{

namespace expression
{

/// Provides a parser that can execute functions with C-like syntax, returning a double result
/** \note The implementation is provided by http://iki.fi/warp/FunctionParser */
class basic_parser
{
public:
	basic_parser();
	~basic_parser();

	/// Adds a named constant that can be used in expressions
	void add_constant(const std::string& Name, double_t Value);

	typedef double_t (*FunctionPtr)(const double_t*);
	/// Adds a function that can be used in expressions
	void add_function(const std::string& Name, FunctionPtr Function, uint8_t ParameterCount);

	/// Parse an expression, returning true on success, otherwise false.
	/// If parsing fails, Error will contain a human-readable string describing the reason.
	const bool_t parse(const std::string& Function, const std::string& Variables);
	/// Returns a human-readable string describing the most recent parse error
	const std::string last_parse_error();

	/// Run an optimizer on the currently parsed expression, for (potentially) faster execution
	void optimize();

	/// Evaluate the expression with the given variable values, returning the result
	double_t evaluate(const double_t* Variables);


private:
	basic_parser(const basic_parser&);
	basic_parser& operator=(const basic_parser&);

	class implementation;
	implementation* const m_implementation;
};

} // namespace expression

} // namespace k3d

#endif // !K3DSDK_EXPRESSION_BASIC_PARSER_H

