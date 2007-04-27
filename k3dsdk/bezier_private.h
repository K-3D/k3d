#ifndef K3DSDK_BEZIER_PRIVATE_H
#define K3DSDK_BEZIER_PRIVATE_H

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
		\brief Implements internal Bezier computation functions
		\author Tim Shead (tshead@k-3d.com)
*/


namespace k3d
{

namespace detail
{

/// Compile-time computation of an integer factorial
template<int n>
struct factorial
{
	enum { result = n * factorial<n-1>::result };
};

template<>
struct factorial<0>
{
	enum { result = 1 };
};

template<int n, int i>
double bernstein_basis(const double t)
{
	return (static_cast<double>(factorial<n>::result) / static_cast<double>(factorial<i>::result * factorial<n-i>::result)) * std::pow(t, i) * std::pow(1-t, n-i);
}

template<int n, int i, typename value_t, typename iterator_t>
struct bezier_implementation
{
	value_t value(double t, iterator_t B)
	{
		value_t a(bernstein_basis<n, i>(t) * (*B));
		value_t b(bezier_implementation<n, i-1, value_t, iterator_t>().value(t, --B));
		return a + b;
	}
};

template<int n, typename value_t, typename iterator_t>
struct bezier_implementation<n, 0, value_t, iterator_t>
{
	value_t value(double t, iterator_t B)
	{
		return bernstein_basis<n, 0>(t) * (*B);
	}
};

} // namespace detail

} // namespace k3d

#endif // !K3DSDK_BEZIER_PRIVATE_H
