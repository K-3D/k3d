#ifndef MODULES_CGAL_CGAL_SIMPLIFYING_CONSTRUCTORS_H
#define MODULES_CGAL_CGAL_SIMPLIFYING_CONSTRUCTORS_H

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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/*
 * \file cgal_simplifying_constructors.h
 * \created Jan 2, 2009
 * \author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <CGAL/Homogeneous.h>
#include <CGAL/Origin.h>

namespace module
{

namespace cgal
{

/// Returns the greatest common divisor for the 4 given arguments
template<typename NT>
const NT gcd(const NT& X, const NT& Y, const NT& Z, const NT& W)
{
	const NT gcd_x = CGAL::gcd(X, W);
	const NT gcd_y = CGAL::gcd(Y, W);
	const NT gcd_z = CGAL::gcd(Z, W);
	const NT gcd_xy = CGAL::gcd(gcd_x, gcd_y);
	return CGAL::gcd(gcd_xy, gcd_z);
}

/// Replacement for the standard point constructor that finds the GCD of 4 homogeneous coordinates and simplifies them
template<typename K>
class Construct_point_3_simplify
{
	typedef typename K::RT         RT;
	typedef typename K::FT         FT;
	typedef typename K::Point_3    Point_3;
	typedef typename Point_3::Rep  Rep;
public:
	typedef Point_3          result_type;
	
	Rep // Point_3
	operator()(CGAL::Return_base_tag, CGAL::Origin o) const
	{ return Rep(o); }

	template < typename Tx, typename Ty, typename Tz >
	Rep // Point_3
	operator()(CGAL::Return_base_tag, const Tx& x, const Ty& y, const Tz& z) const
	{ return Rep(x, y, z); }

	Rep // Point_3
	operator()(CGAL::Return_base_tag, const FT& x, const FT& y, const FT& z) const
	{ return Rep(x, y, z); }

	Rep // Point_3
	operator()(CGAL::Return_base_tag, const RT& x, const RT& y, const RT& z, const RT& w) const
	{
		const RT f = gcd(x, y, z, w);
		return Rep(x/f, y/f, z/f, w/f);
	}
	
  Point_3
  operator()(CGAL::Origin o) const
  { return this->operator()(CGAL::Return_base_tag(), o); }

  template < typename Tx, typename Ty, typename Tz >
  Point_3
  operator()(const Tx& x, const Ty& y, const Tz& z) const
  { return this->operator()(CGAL::Return_base_tag(), x, y, z); }

  Point_3
  operator()(const FT& x, const FT& y, const FT& z) const
  { return this->operator()(CGAL::Return_base_tag(), x, y, z); }

  Point_3
  operator()(const RT& x, const RT& y, const RT& z, const RT& w) const
  { return this->operator()(CGAL::Return_base_tag(), x, y, z, w); }
};

/// Replacement for the standard vector constructor that finds the GCD of 4 homogeneous coordinates and simplifies them
template <typename K>
class Construct_vector_3_simplify
{
	typedef typename K::RT           RT;
	typedef typename K::FT           FT;
	typedef typename K::Direction_3  Direction_3;
	typedef typename K::Segment_3    Segment_3;
	typedef typename K::Ray_3        Ray_3;
	typedef typename K::Line_3       Line_3;
	typedef typename K::Vector_3     Vector_3;
	typedef typename K::Point_3      Point_3;
	typedef typename Vector_3::Rep   Rep;
public:
	typedef Vector_3         result_type;

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Point_3& p, const Point_3& q) const
	{
		const RT x = q.hx()*p.hw() - p.hx()*q.hw();
		const RT y = q.hy()*p.hw() - p.hy()*q.hw();
		const RT z = q.hz()*p.hw() - p.hz()*q.hw();
		const RT w = q.hw()*p.hw();
		const RT f = gcd(x, y, z, w);
		return Rep(x/f, y/f, z/f, w/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const CGAL::Origin&, const Point_3& q) const
	{
		const RT f = gcd(q.hx(), q.hy(), q.hz(), q.hw());
		return Rep(q.hx()/f, q.hy()/f, q.hz()/f, q.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Point_3& p, const CGAL::Origin& ) const
	{
		const RT f = gcd(p.hx(), p.hy(), p.hz(), p.hw());
		return Rep(-p.hx()/f, -p.hy()/f, -p.hz()/f, p.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Direction_3& d) const
	{ 
		Vector_3 v = d.rep().to_vector();
		const RT f = gcd(v.hx(), v.hy(), v.hz(), v.hw());
		return Rep(v.hx()/f, v.hy()/f, v.hz()/f, v.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Segment_3& s) const
	{ 
		Vector_3 v = s.rep().to_vector();
		const RT f = gcd(v.hx(), v.hy(), v.hz(), v.hw());
		return Rep(v.hx()/f, v.hy()/f, v.hz()/f, v.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Ray_3& r) const
	{ 
		Vector_3 v = r.rep().to_vector();
		const RT f = gcd(v.hx(), v.hy(), v.hz(), v.hw());
		return Rep(v.hx()/f, v.hy()/f, v.hz()/f, v.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Line_3& l) const
	{ 
		Vector_3 v = l.rep().to_vector();
		const RT f = gcd(v.hx(), v.hy(), v.hz(), v.hw());
		return Rep(v.hx()/f, v.hy()/f, v.hz()/f, v.hw()/f);
	}

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const CGAL::Null_vector&) const
	{ return Rep(RT(0), RT(0), RT(0), RT(1)); }

	template < typename Tx, typename Ty, typename Tz >
	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const Tx & x, const Ty & y, const Tz & z) const
	{ return Rep(x, y, z); }

	Rep // Vector_3
	operator()(CGAL::Return_base_tag, const RT& x, const RT& y, const RT& z, const RT& w) const
	{
		const RT f = gcd(x, y, z, w);
		return Rep(x/f, y/f, z/f, w/f);
	}


	Vector_3
	operator()( const Point_3& p, const Point_3& q) const
	{ return this->operator()(CGAL::Return_base_tag(), p, q); }

	Vector_3
	operator()( const CGAL::Origin& o, const Point_3& q) const
	{ return this->operator()(CGAL::Return_base_tag(), o, q); }

	Vector_3
	operator()( const Point_3& p, const CGAL::Origin& q) const
	{ return this->operator()(CGAL::Return_base_tag(), p, q); }

	Vector_3
	operator()( const Direction_3& d) const
	{ return this->operator()(CGAL::Return_base_tag(), d); }

	Vector_3
	operator()( const Segment_3& s) const
	{ return this->operator()(CGAL::Return_base_tag(), s); }

	Vector_3
	operator()( const Ray_3& r) const
	{ return this->operator()(CGAL::Return_base_tag(), r); }

	Vector_3
	operator()( const Line_3& l) const
	{ return this->operator()(CGAL::Return_base_tag(), l); }

	Vector_3
	operator()( const CGAL::Null_vector& n) const
	{ return this->operator()(CGAL::Return_base_tag(), n); }

	template < typename Tx, typename Ty, typename Tz >
	Vector_3
	operator()(const Tx & x, const Ty & y, const Tz & z) const
	{ return this->operator()(CGAL::Return_base_tag(), x, y, z); }

	Vector_3
	operator()( const RT& x, const RT& y, const RT& z, const RT& w) const
	{ return this->operator()(CGAL::Return_base_tag(), x, y, z, w); }
};

/// Base for a kernel using these simplifying constructors
template < typename K_, typename K_Base >
class simplifying_kernel_base
  : public K_Base::template Base<K_>::Type
{
  typedef typename K_Base::template Base<K_>::Type   OldK;
public:
  typedef K_                                Kernel;
  typedef Construct_point_3_simplify<K_>    Construct_point_3;
  typedef Construct_vector_3_simplify<K_>   Construct_vector_3;

  Construct_point_3
  construct_point_3_object() const
  { return Construct_point_3(); }
  
  Construct_vector_3
	construct_vector_3_object() const
	{ return Construct_vector_3(); }

  template < typename Kernel2 >
  struct Base { typedef simplifying_kernel_base<Kernel2, K_Base>  Type; };
};

/// Wrapper around the homogeneous kernel
template < typename RT_ >
struct simplifying_kernel
	: public CGAL::Type_equality_wrapper<
								simplifying_kernel_base<simplifying_kernel<RT_>, CGAL::Homogeneous<RT_> >,
								simplifying_kernel<RT_> >
{};

} // namespace cgal

} // namespace module


#endif // !MODULES_CGAL_CGAL_SIMPLIFYING_CONSTRUCTORS_H
