#ifndef THRESHOLD_NUMBER_H_
#define THRESHOLD_NUMBER_H_

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

#include <CGAL/basic.h>
#include <utility>
#include <cmath>
#include <limits>

CGAL_BEGIN_NAMESPACE

class threshold_number
{
public:
	threshold_number() : d(0.0), t(1e-10) {}
	threshold_number(double D) : d(D), t(1e-10) {}
	
	threshold_number& operator=(const double D) {this->d = D; return *this;}
	threshold_number& operator*=(const double D) {d *= D; return *this;}
	threshold_number& operator/=(const double D) {d/= D; return *this;}
	threshold_number& operator+=(const double D) {d+= D; return *this;}
	threshold_number& operator-=(const double D) {d-= D; return *this;}
	threshold_number& operator+=(const threshold_number& T) {d += T.d; return *this;}
	threshold_number& operator-=(const threshold_number& T) {d -= T.d; return *this;}
	threshold_number& operator*=(const threshold_number& T) {d *= T.d; return *this;}
	threshold_number& operator/=(const threshold_number& T) {d /= T.d; return *this;}

	double d;
	double t;
};

inline threshold_number operator-(const threshold_number& T)
{ return threshold_number(-T.d); }

inline threshold_number operator-(const threshold_number& T1, const threshold_number& T2)
{ return threshold_number(T1.d-T2.d); }

inline threshold_number operator+(const threshold_number& T1, const threshold_number& T2)
{ return threshold_number(T1.d+T2.d); }

inline threshold_number operator*(const threshold_number& T1, const threshold_number& T2)
{ return threshold_number(T1.d*T2.d); }

inline threshold_number operator/(const threshold_number& T1, const threshold_number& T2)
{ return threshold_number(T1.d/T2.d); }

inline bool operator==(const threshold_number& T1, const threshold_number& T2)
{ return std::abs(T1.d - T2.d) <= T1.t; }

inline bool operator!=(const threshold_number& T1, const threshold_number& T2)
{ return std::abs(T1.d - T2.d) > T1.t; }

inline bool operator<(const threshold_number& T1, const threshold_number& T2)
{ return (T1.d - T2.d) < -T1.t; }

inline bool operator>(const threshold_number& T1, const threshold_number& T2)
{ return (T1.d - T2.d) > T1.t; }

inline bool operator<=(const threshold_number& T1, const threshold_number& T2)
{ return T1 < T2 || T1 == T2; }

inline bool operator>=(const threshold_number& T1, const threshold_number& T2)
{ return T1 > T2 || T1 == T2; }

inline std::ostream& operator<<(std::ostream& Stream, const threshold_number& T)
{
	Stream << T.d;
	return Stream;
}

//////////////////////////////////
// CGAL required functions
//////////////////////////////////

template<> struct Number_type_traits<threshold_number> {
  typedef Tag_false  Has_gcd;
  typedef Tag_true   Has_division;
  typedef Tag_true   Has_sqrt;

  typedef Tag_false  Has_exact_ring_operations;
  typedef Tag_false  Has_exact_division;
  typedef Tag_false  Has_exact_sqrt;
};

inline
const double &
to_double(const threshold_number & T)
{ return T.d; }

inline 
std::pair<double,double>
to_interval(threshold_number T)
{ return std::make_pair(T.d,T.d);}

inline
threshold_number
sqrt(threshold_number T)
{ return threshold_number(std::sqrt(T.d)); }

inline
bool
is_valid(threshold_number T)
{ return (T == T); }

inline
bool
is_finite(threshold_number T)
{ return (T.d != std::numeric_limits<double>::infinity()) 
    && (-T.d != std::numeric_limits<double>::infinity())
    && is_valid(T); }

inline
io_Operator
io_tag(threshold_number)
{ return io_Operator(); }

CGAL_END_NAMESPACE

#endif /*THRESHOLD_NUMBER_H_*/
