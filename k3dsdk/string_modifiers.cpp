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
		\brief Implements extended std::string functions
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "result.h"
#include "string_modifiers.h"

#include <cctype>

namespace k3d
{

const std::string replace_all(const std::string& Search, const std::string& Replacement, const std::string& Source)
{
	std::string result(Source);

	for(std::string::size_type pos = result.find(Search); pos != std::string::npos; pos = result.find(Search, pos + Replacement.size()))
		result.replace(pos, Search.size(), Replacement);

	return result;
}

const std::string right(const std::string& Text, std::string::size_type Count)
{
	assert_warning(Count <= Text.size());
	return std::string(Text.end() - Count, Text.end());
}

const std::string trim(const std::string& Text)
{
	return trimleft(trimright(Text));
}

const std::string trimleft(const std::string& Text)
{
	// Easiest case ...
	if(!Text.size())
		return std::string();

	std::string::size_type i;
	for(i = 0; i < Text.size(); ++i)
		if(!std::isspace(Text[i]))
			break;

	// No whitespace, so we're done ...
	if(i == 0)
		return std::string(Text);

	// All whitespace, so clear the string ...
	if(i == Text.size())
		return std::string();

	return right(Text, Text.size()-i);
}

const std::string trimright(const std::string& Text)
{
	// Easiest case ...
	if(!Text.size())
		return std::string();

	std::string::size_type i;
	for(i = Text.size()-1; i >= 0; --i)
		if(!std::isspace(Text[i]))
			break;

	// No whitespace, so we're done ...
	if(i == (Text.size()-1))
		return std::string(Text);

	// All whitespace, so return empty string ...
	if(i == std::string::size_type(-1))
		return std::string();

	return Text.substr(0, i+1);
}

} // namespace k3d


