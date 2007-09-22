#ifndef K3DSDK_FUNCTION_PARSER_PARSER_H
#define K3DSDK_FUNCTION_PARSER_PARSER_H

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

namespace function
{

/// Provides a parser that can execute functions with C-like syntax, returning a double result
class parser
{
public:
	parser();
	~parser();

	/// Adds a named constant that can be used in expressions
	void add_constant(const std::string& Name, double Value);

	typedef double (*FunctionPtr)(const double*);
	/// Adds a function that can be used in expressions
	void add_function(const std::string& Name, FunctionPtr Function, k3d::uint8_t ParameterCount);

	/// Parse an expression, returning true on success, otherwise false.
	/// If parsing fails, Error will contain a human-readable string describing the reason.
	const bool parse(const std::string& Function, const std::string& Variables);
	/// Returns a human-readable string describing the most recent parse error
	const std::string last_parse_error();

	/// Run an optimizer on the currently parsed expression, for (potentially) faster execution
	void optimize();

	/// Evaluate the expression with the given variable values, returning the result
	double evaluate(const double* Variables);


private:
	parser(const parser&);
	parser& operator=(const parser&);

	class implementation;
	implementation* const m_implementation;
};

} // namespace function

} // namespace k3d

#endif // !K3DSDK_FUNCTION_PARSER_PARSER-H

