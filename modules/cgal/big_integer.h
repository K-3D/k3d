#ifndef MODULES_CGAL_THRESHOLD_NUMBER_H
#define MODULES_CGAL_THRESHOLD_NUMBER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Number type that provides comparison operators that have a tolerance specified by a threshold
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <CGAL/Gmpz.h>

#include <k3dsdk/types.h>

#include <boost/integer_traits.hpp>

CGAL_BEGIN_NAMESPACE

typedef Gmpz exact_t; // exact type
typedef k3d::int64_t integer_t;
typedef boost::integer_traits<integer_t> traits;

class big_integer
{
public:
	big_integer() : n(0), overflow(false) {}
	big_integer(const double D)
	{
		if(D > traits::const_max || D < traits::const_min)
		{
			overflow = true;
			exact = exact_t(D);
		}
		else
		{
			n = D;
			overflow = false;
		}
	}
	big_integer(const int N) : n(N), overflow(false)
	{
	}
	
	big_integer& operator+=(const big_integer& N)
	{
		n += N.n;
		return *this;
	}
	big_integer& operator-=(const big_integer& N)
	{
		n -= N.n;
		return *this;
	}
	big_integer& operator*=(const big_integer& N)
	{
		n *= N.n;
		return *this;
	}
	big_integer& operator/=(const big_integer& N)
	{
		n /= N.n;
		return *this;
	}

	exact_t exact;
	integer_t n;
	k3d::bool_t overflow;
};

inline big_integer operator-(const big_integer& N)
{ return big_integer(-N.n); }

inline big_integer operator-(const big_integer& N1, const big_integer& N2)
{
	return big_integer(N1.n - N2.n);
}

inline big_integer operator+(const big_integer& N1, const big_integer& N2)
{
	return big_integer(N1.n + N2.n);
}

inline big_integer operator*(const big_integer& N1, const big_integer& N2)
{
	return big_integer(N1.n * N2.n);
}

inline big_integer operator/(const big_integer& N1, const big_integer& N2)
{
	return big_integer(N1.n / N2.n);
}

inline bool operator==(const big_integer& N1, const big_integer& N2)
{ return N1.n == N2.n; }

inline bool operator!=(const big_integer& N1, const big_integer& N2)
{ return N1.n != N2.n; }

inline bool operator<(const big_integer& N1, const big_integer& N2)
{ return N1.n < N2.n; }

inline bool operator>(const big_integer& N1, const big_integer& N2)
{ return N1.n > N2.n; }

inline bool operator<=(const big_integer& N1, const big_integer& N2)
{ return N1.n <= N2.n; }

inline bool operator>=(const big_integer& N1, const big_integer& N2)
{ return N1.n >= N2.n; }

inline std::ostream& operator<<(std::ostream& Stream, const big_integer& N)
{
	Stream << N.n;
	return Stream;
}

CGAL_END_NAMESPACE

#endif // !MODULES_CGAL_THRESHOLD_NUMBER_H
