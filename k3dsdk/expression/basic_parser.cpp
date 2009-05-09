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

#include "fparser.h"
#include "basic_parser.h"

#include <k3dsdk/result.h>

namespace k3d
{

namespace expression
{

class basic_parser::implementation
{
public:
	FunctionParser basic_parser;
};

basic_parser::basic_parser() :
	m_implementation(new implementation())
{
}

basic_parser::~basic_parser()
{
	delete m_implementation;
}

void basic_parser::add_constant(const std::string& Name, double_t Value)
{
	return_if_fail(m_implementation->basic_parser.AddConstant(Name, Value));
}

void basic_parser::add_function(const std::string& Name, FunctionPtr Function, uint8_t ParameterCount)
{
	return_if_fail(m_implementation->basic_parser.AddFunction(Name, Function, ParameterCount));
}

bool_t basic_parser::parse(const std::string& Function, const std::string& Variables)
{
	return m_implementation->basic_parser.Parse(Function, Variables) <= -1;
}

const std::string basic_parser::last_parse_error()
{
	return m_implementation->basic_parser.ErrorMsg() ? m_implementation->basic_parser.ErrorMsg() : "";
}

void basic_parser::optimize()
{
	m_implementation->basic_parser.Optimize();
}

double_t basic_parser::evaluate(const double_t* Variables)
{
	return m_implementation->basic_parser.Eval(Variables);
}

} // namespace expression

} // namespace k3d

