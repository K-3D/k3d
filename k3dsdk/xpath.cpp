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

struct match_context
{
	match_context(element& Tree, result_set& Results) :
		tree(Tree),
		results(Results)
	{
	}

	element& tree;
	result_set& results;
	string_t attribute_name;
};

struct match_root_path
{
	match_root_path(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char&) const
	{
		context.results.clear();
	}

	match_context& context;
};

struct match_absolute_path
{
	match_absolute_path(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const string_t name(Begin, End);
		if(name != context.tree.name)
			context.results.clear();
	}

	match_context& context;
};

struct match_child
{
	match_child(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const string_t name(Begin, End);

		result_set new_results;
		for(result_set::const_iterator anchor = context.results.begin(); anchor != context.results.end(); ++anchor)
		{
			for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
			{
				if(child->name == name)
					new_results.push_back(&*child);
			}
		}
		context.results = new_results;
	}

	match_context& context;
};

struct match_wildcard
{
	match_wildcard(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char*, const char*) const
	{
		result_set new_results;
		for(result_set::const_iterator anchor = context.results.begin(); anchor != context.results.end(); ++anchor)
		{
			for(element::elements_t::iterator child = (*anchor)->children.begin(); child != (*anchor)->children.end(); ++child)
			{
				new_results.push_back(&*child);
			}
		}
		context.results = new_results;
	}

	match_context& context;
};

struct match_attribute_name
{
	match_attribute_name(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		context.attribute_name = string_t(Begin, End);

		result_set new_results;
		for(result_set::const_iterator anchor = context.results.begin(); anchor != context.results.end(); ++anchor)
		{
			if(find_attribute(**anchor, context.attribute_name))
				new_results.push_back(*anchor);
		}
		context.results = new_results;
	}

	match_context& context;
};

struct match_attribute_value
{
	match_attribute_value(match_context& Context) :
		context(Context)
	{
	}

	void operator()(const char* Begin, const char* End) const
	{
		const string_t attribute_value = string_t(Begin, End);

		result_set new_results;
		for(result_set::const_iterator anchor = context.results.begin(); anchor != context.results.end(); ++anchor)
		{
			if(attribute_text(**anchor, context.attribute_name) == attribute_value)
				new_results.push_back(*anchor);
		}
		context.results = new_results;
	}

	match_context& context;
};

class grammar :
	public boost::spirit::grammar<grammar>
{
public:
	grammar(match_context& Context) :
		context(Context)
	{
		context.results.assign(1, &context.tree);
	}

	template<typename ScannerT>
	struct definition
	{
		definition(const grammar& self)
		{
			name = chset_p("_a-zA-Z") >> *chset_p("_a-zA-Z0-9");
			wildcard = ch_p('*');
			node_test = name[match_child(self.context)] | wildcard[match_wildcard(self.context)];
			attribute_test = '@' >> name[match_attribute_name(self.context)];
			attribute_value_test = '@' >> name[match_attribute_name(self.context)] >> ch_p('=') >> confix_p(ch_p('\''), (*anychar_p)[match_attribute_value(self.context)], ch_p('\''));
			predicate_expression = attribute_value_test | attribute_test;
			predicate = '[' >> predicate_expression >> ']';
			predicate_list = *predicate;
			step = node_test >> !predicate_list;
			root_path = ch_p('/')[match_root_path(self.context)];
			absolute_path = ch_p('/') >> name[match_absolute_path(self.context)] >> *('/' >> step);
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
	match_context& context;
};

result_set match(element& Tree, const string_t& Expression)
{
	// Special-case: an empty expression matches nothing
	if(Expression.empty())
		return result_set();

	result_set results;
	match_context context(Tree, results);
	if(!parse(Expression.c_str(), grammar(context), space_p).full)
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

