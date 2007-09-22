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
#include "parser.h"

#include <k3dsdk/result.h>

namespace k3d
{

namespace function
{

class parser::implementation
{
public:
	FunctionParser parser;
};

parser::parser() :
	m_implementation(new implementation())
{
}

parser::~parser()
{
	delete m_implementation;
}

void parser::add_constant(const std::string& Name, double Value)
{
	return_if_fail(m_implementation->parser.AddConstant(Name, Value));
}

void parser::add_function(const std::string& Name, FunctionPtr Function, k3d::uint8_t ParameterCount)
{
	return_if_fail(m_implementation->parser.AddFunction(Name, Function, ParameterCount));
}

const bool parser::parse(const std::string& Function, const std::string& Variables)
{
	return m_implementation->parser.Parse(Function, Variables) <= -1;
}

const std::string parser::last_parse_error()
{
	return m_implementation->parser.ErrorMsg() ? m_implementation->parser.ErrorMsg() : "";
}

void parser::optimize()
{
	m_implementation->parser.Optimize();
}

double parser::evaluate(const double* Variables)
{
	return m_implementation->parser.Eval(Variables);
}

} // namespace function

} // namespace k3d

