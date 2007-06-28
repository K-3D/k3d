#ifndef K3DSDK_EXPRESSION_GRAMMAR_H
#define K3DSDK_EXPRESSION_GRAMMAR_H

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

#include <boost/spirit/core.hpp>
#include <cmath>
#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

namespace k3d
{

namespace expression
{

using namespace boost::spirit;

/// Defines a stack for holding intermediate values during parsing
typedef std::stack<double> stack_t;

/// Inserts a parsed numeric value onto the stack
class push_number
{
public:
	push_number(stack_t& Stack) :
		m_stack(Stack)
	{
	}

	void operator()(const double Constant) const
	{
		m_stack.push(Constant);
	}

private:
	stack_t& m_stack;
};

/// Inserts a constant numeric value onto the stack
class push_constant
{
public:
	push_constant(const double Constant, stack_t& Stack) :
		m_constant(Constant),
		m_stack(Stack)
	{
	}

	template<typename iterator_t>
	void operator()(iterator_t, iterator_t) const
	{
		m_stack.push(m_constant);
	}

private:
	const double m_constant;
	stack_t& m_stack;
};

/// Applies a unary function to the top of the stack
template<typename operator_t>
class unary_operator_t
{
public:
	explicit unary_operator_t(operator_t Operator, stack_t& Stack) :
		m_operator(Operator),
		m_stack(Stack)
	{
	}

	template<typename iterator_t>
	void operator()(iterator_t, iterator_t) const
	{
		const double rhs = m_stack.top(); m_stack.pop();
		m_stack.push(m_operator(rhs));
	}

private:
	operator_t m_operator;
	stack_t& m_stack;
};

/// Convenience factory function for instantiating unary_operator_t objects
template<typename operator_t>
unary_operator_t<operator_t> unary_operator(operator_t Operator, stack_t& Stack)
{
	return unary_operator_t<operator_t>(Operator, Stack);
}

/// Applies a binary function to the top two values of the stack
template<typename operator_t>
class binary_operator_t
{
public:
	explicit binary_operator_t(operator_t Operator, stack_t& Stack) :
		m_operator(Operator),
		m_stack(Stack)
	{
	}

	template<typename iterator_t>
	void operator()(iterator_t, iterator_t) const
	{
		const double rhs = m_stack.top(); m_stack.pop();
		const double lhs = m_stack.top(); m_stack.pop();
		m_stack.push(m_operator(lhs, rhs));
	}

private:
	operator_t m_operator;
	stack_t& m_stack;
};

/// Convenience factory function for instantiating binary_operator_t objects
template<typename operator_t>
binary_operator_t<operator_t> binary_operator(operator_t Operator, stack_t& Stack)
{
	return binary_operator_t<operator_t>(Operator, Stack);
}

/// Encapsulates a grammar for evaluating basic mathematics expressions
struct expression_grammar :
	public grammar<expression_grammar>
{
	expression_grammar(stack_t& Stack) :
		m_stack(Stack)
	{
	}

	template<typename ScannerT>
	struct definition
	{
		definition(const expression_grammar& self)
		{
			number = (real_p)[push_number(self.m_stack)];

			pi = (nocase_d[str_p("pi")])[push_constant(3.14159265358979323846, self.m_stack)];

			constant = number
				 | pi;

			primary_expression = constant
			                   | (ch_p('(') >> expression >> ch_p(')'))
					   | (ch_p('-') >> expression)[unary_operator(std::negate<double>(), self.m_stack)]
					   | (ch_p('+') >> expression);

			unary_function_arguments = ch_p('(') >> expression >> ch_p(')');

			binary_function_arguments = ch_p('(') >> expression >> ch_p(',') >> expression >> ch_p(')');

			builtins = primary_expression
				 | (str_p("abs") >> unary_function_arguments)[unary_operator((double(*)(double))&fabs, self.m_stack)]
				 | (str_p("acos") >> unary_function_arguments)[unary_operator((double(*)(double))&acos, self.m_stack)]
				 | (str_p("asin") >> unary_function_arguments)[unary_operator((double(*)(double))&asin, self.m_stack)]
				 | (str_p("atan") >> unary_function_arguments)[unary_operator((double(*)(double))&atan, self.m_stack)]
				 | (str_p("ceil") >> unary_function_arguments)[unary_operator((double(*)(double))&ceil, self.m_stack)]
				 | (str_p("cos") >> unary_function_arguments)[unary_operator((double(*)(double))&cos, self.m_stack)]
				 | (str_p("cosh") >> unary_function_arguments)[unary_operator((double(*)(double))&cosh, self.m_stack)]
				 | (str_p("exp") >> unary_function_arguments)[unary_operator((double(*)(double))&exp, self.m_stack)]
				 | (str_p("floor") >> unary_function_arguments)[unary_operator((double(*)(double))&floor, self.m_stack)]
				 | (str_p("log") >> unary_function_arguments)[unary_operator((double(*)(double))&::log, self.m_stack)]
				 | (str_p("log10") >> unary_function_arguments)[unary_operator((double(*)(double))&log10, self.m_stack)]
				 | (str_p("pow") >> binary_function_arguments)[binary_operator((double(*)(double, double))&pow, self.m_stack)]
				 | (str_p("sin") >> unary_function_arguments)[unary_operator((double(*)(double))&sin, self.m_stack)]
				 | (str_p("sinh") >> unary_function_arguments)[unary_operator((double(*)(double))&sinh, self.m_stack)]
				 | (str_p("sqrt") >> unary_function_arguments)[unary_operator((double(*)(double))&sqrt, self.m_stack)]
				 | (str_p("tan") >> unary_function_arguments)[unary_operator((double(*)(double))&tan, self.m_stack)]
				 | (str_p("tanh") >> unary_function_arguments)[unary_operator((double(*)(double))&tanh, self.m_stack)];

			multiplicative_expression = builtins >>
				*((ch_p('*') >> builtins)[binary_operator(std::multiplies<double>(), self.m_stack)]
				| (ch_p('/') >> builtins)[binary_operator(std::divides<double>(), self.m_stack)]);

			additive_expression = multiplicative_expression >>
				*((ch_p('+') >> multiplicative_expression)[binary_operator(std::plus<double>(), self.m_stack)]
				| (ch_p('-') >> multiplicative_expression)[binary_operator(std::minus<double>(), self.m_stack)]);

			expression = (additive_expression);
		}

		rule<ScannerT> const& start() const { return expression; }
		rule<ScannerT> number, pi, constant, primary_expression, unary_function_arguments, binary_function_arguments, builtins, multiplicative_expression, additive_expression, expression;
	};

	stack_t& m_stack;
};

} //namespace expression

} // namespace k3d

#endif // !K3DSDK_EXPRESSION_GRAMMAR_H

