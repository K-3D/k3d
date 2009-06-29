// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/result.h>
#include <k3dsdk/xml.h>
#include <k3dsdk/xpath.h>

#include <boost/tokenizer.hpp>

namespace k3d
{

namespace xml
{

namespace xpath
{

static result_set match_wildcard(const result_set& Anchors)
{
	result_set results;

	for(result_set::const_iterator anchor = Anchors.begin(); anchor != Anchors.end(); ++anchor)
	{
		for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
		{
			results.push_back(&*child);
		}
	}

	return results;
}

static result_set match_child(const result_set& Anchors, const string_t& Name)
{
	result_set results;

	for(result_set::const_iterator anchor = Anchors.begin(); anchor != Anchors.end(); ++anchor)
	{
		for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
		{
			if(child->name == Name)
				results.push_back(&*child);
		}
	}

	return results;
}

result_set match(element& Tree, const string_t& Expression)
{
	// Tokenize the XPath expression ...
	boost::tokenizer<boost::char_separator<char> > tokenizer(Expression, boost::char_separator<char>("", "/", boost::drop_empty_tokens));
	std::vector<string_t> tokens(tokenizer.begin(), tokenizer.end());

	// Special-case: an empty expression matches nothing
	if(tokens.empty())
		return result_set();

	// Special-case: if this is an absolute path, match the root node
	if(tokens.front() == "/")
	{
		tokens.erase(tokens.begin());

		if(tokens.empty())
			return result_set();

		if(tokens.front() != Tree.name)
			return result_set();

		tokens.erase(tokens.begin());
	}

	// We begin by matching the root node of the tree ...
	result_set results(1, &Tree);

	// Iterate over the remaining tokens, refining our results ...
	for(k3d::uint_t i = 0; i != tokens.size(); ++i)
	{
		if(tokens[i] == "/")
		{
		}
		else if(tokens[i] == "*")
		{
			results = match_wildcard(results);
		}
		else
		{
			results = match_child(results, tokens[i]);
		}
	}

	return results;
}

} // namespace xpath

} // namespace xml

} // namespace k3d

std::ostream& operator<<(std::ostream& Stream, const k3d::xml::xpath::result_set& RHS)
{
	for(k3d::xml::xpath::result_set::const_iterator result = RHS.begin(); result != RHS.end(); ++result)
		Stream << **result << "\n";

	return Stream;
}

