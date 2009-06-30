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

#include <boost/spirit.hpp>
using namespace boost::spirit;

namespace k3d
{

namespace xml
{

namespace xpath
{

struct action
{
	action(result_set& Results) :
		results(Results)
	{
	}

	result_set& results;
};

struct match_root_path :
	public action
{
	match_root_path(result_set& Results) :
		action(Results)
	{
	}

	void operator()(const char&) const
	{
		results.clear();
	}
};

struct match_absolute_path :
	public action
{
	match_absolute_path(element& Tree, result_set& Results) :
		action(Results),
		tree(Tree)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const std::string name(Begin, End);
		if(name != tree.name)
			results.clear();
	}

	element& tree;
};

struct match_child :
	public action
{
	match_child(result_set& Results) :
		action(Results)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const std::string name(Begin, End);

		result_set new_results;
		for(result_set::const_iterator anchor = results.begin(); anchor != results.end(); ++anchor)
		{
			for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
			{
				if(child->name == name)
					new_results.push_back(&*child);
			}
		}
		results = new_results;
	}
};

struct match_wildcard :
	public action
{
	match_wildcard(result_set& Results) :
		action(Results)
	{
	}

	void operator()(const char*, const char*) const
	{
		result_set new_results;
		for(result_set::const_iterator anchor = results.begin(); anchor != results.end(); ++anchor)
		{
			for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
			{
				new_results.push_back(&*child);
			}
		}
		results = new_results;
	}
};

struct match_attribute :
	public action
{
	match_attribute(result_set& Results) :
		action(Results)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const std::string name(Begin, End);

		result_set new_results;
		for(result_set::const_iterator anchor = results.begin(); anchor != results.end(); ++anchor)
		{
			if(find_attribute(**anchor, name))
				new_results.push_back(*anchor);
		}
		results = new_results;
	}
};

class grammar :
	public boost::spirit::grammar<grammar>
{
public:
	grammar(element& Tree, result_set& Results) :
		tree(Tree),
		results(Results)
	{
		results.assign(1, &Tree);
	}

	template<typename ScannerT>
	struct definition
	{
		definition(const grammar& self)
		{
			name = chset_p("_a-zA-Z") >> *chset_p("_a-zA-Z0-9");
			wildcard = ch_p('*');
			node_test = name[match_child(self.results)] | wildcard[match_wildcard(self.results)];
			attribute_test = '@' >> name[match_attribute(self.results)];
			attribute_value_test = '@' >> name >> '=' >> name;
			predicate_expression = attribute_test | attribute_value_test;
			predicate = '[' >> predicate_expression >> ']';
			predicate_list = *predicate;
			step = node_test >> !predicate_list;
			root_path = ch_p('/')[match_root_path(self.results)];
			absolute_path = ch_p('/') >> name[match_absolute_path(self.tree, self.results)] >> *('/' >> step);
			relative_path = step >> *('/' >> step);
			expression = relative_path | absolute_path | root_path;
		}

		rule<ScannerT> const& start() const
		{
			return expression;
		}

		rule<ScannerT> name, wildcard, node_test, attribute_test, attribute_value_test, predicate_expression, predicate, predicate_list, step, root_path, absolute_path, relative_path, expression;
	};

private:
	element& tree;
	result_set& results;
};

result_set match(element& Tree, const string_t& Expression)
{
	// Special-case: an empty expression matches nothing
	if(Expression.empty())
		return result_set();

	result_set results;
	if(!parse(Expression.c_str(), grammar(Tree, results), space_p).full)
	{
		k3d::log() << error << "Not a valid XPath expression: " << Expression << std::endl;
		return result_set();
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

