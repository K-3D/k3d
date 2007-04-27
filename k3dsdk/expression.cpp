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

#include "expression_grammar.h"
#include "expression.h"

namespace k3d
{

namespace expression
{

bool parse(std::istream& Stream, double&Value)
{
	std::string buffer;
	std::copy(std::istreambuf_iterator<char>(Stream), std::istreambuf_iterator<char>(), std::back_inserter(buffer));

	return parse(buffer, Value);
}

bool parse(const std::string& Buffer, double& Value)
{
	std::stack<double> stack;

	if(!parse(Buffer.c_str(), expression_grammar(stack), space_p).full)
		return false;

	assert(!stack.empty());
	Value = stack.top();

	return true;
}

} // namespace expression

} // namespace k3d

