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

#include <CGAL/number_type_basic.h>

#include <utility>
#include <cmath>
#include <math.h> // for nextafter
#include <limits>

CGAL_BEGIN_NAMESPACE

class threshold_number
{
public:
	threshold_number() : d(0.0), t(1e-8) {}
	threshold_number(double D) : d(D), t(1e-8) {}
	
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

template<>
class Is_valid< threshold_number >
  : public Unary_function< threshold_number, bool > {
  public :
    bool operator()( const threshold_number& x ) const {
      return (x == x);
    }
};

template <> class Algebraic_structure_traits< threshold_number >
  : public Algebraic_structure_traits_base< threshold_number,
                                            Field_with_kth_root_tag >  {
  public:
    typedef Tag_false            Is_exact;
    typedef Tag_true             Is_numerical_sensitive;

    class Sqrt
      : public Unary_function< Type, Type > {
      public:
        Type operator()( const Type& x ) const {
          return CGAL_CLIB_STD::sqrt( x.d );
        }
    };

    class Kth_root
      : public Binary_function<int, Type, Type> {
      public:
        Type operator()( int k,
                                        const Type& x) const {
          CGAL_precondition_msg( k > 0, "'k' must be positive for k-th roots");
          return CGAL_CLIB_STD::pow(x.d, 1.0 / static_cast<double>(k));
        }
    };
    
    class Gcd
        : public Binary_function< Type, Type,
                                Type > {
    public:
        Type operator()( const Type& x,
                const Type& y ) const {
            return threshold_number(1.0);

        }

        Type operator()( const Type& x,
                                        const int& y ) const {
          return threshold_number(1.0);
        }

        Type operator()( const int& x,
                                        const Type& y ) const {
          return threshold_number(1.0);
        }
    };

};

template <> class Real_embeddable_traits< threshold_number >
  : public Real_embeddable_traits_base< threshold_number > {
public:
    
  class Is_finite
    : public Unary_function< Type, bool > {
    public :
      bool operator()( const Type& x ) const {
        return (x.d != std::numeric_limits<Type>::infinity())
            && (-x.d != std::numeric_limits<Type>::infinity())
            && is_valid(x.d);
    }
  };
    
  class To_double 
    : public Unary_function< Type, double > {
    public:      
      //! the function call.
      double operator()( const Type& x ) const {
        return x.d;
      }
  };
    
  class To_interval 
    : public Unary_function< Type, std::pair< double, double > > {
    public:      
        //! the function call.
      std::pair<double, double> operator()( const Type& x ) const {

        return std::pair<double,double>( x.d, x.d );
      }
  };
                                                                  
};
 
inline
bool
is_integer(threshold_number T)
{
  return CGAL::is_finite(T.d) && (std::ceil(T.d) == T.d);
}

template <>
class Fraction_traits< threshold_number > {
public:
    typedef threshold_number Type;
    typedef ::CGAL::Tag_false Is_fraction;
    typedef threshold_number Numerator_type;
    typedef threshold_number Denominator_type;
    typedef Algebraic_structure_traits< threshold_number >::Gcd Common_factor;
    class Decompose {
    public:
        typedef threshold_number first_argument_type;
        typedef threshold_number& second_argument_type;
        typedef threshold_number& third_argument_type;
        void operator () (const threshold_number& rat, threshold_number& num,threshold_number& den) {
          num = threshold_number(rat);
          den = threshold_number(1.0);
				}
    };
};


CGAL_END_NAMESPACE

#endif /*THRESHOLD_NUMBER_H_*/
