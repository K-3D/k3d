/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_UTILITIES_H
#define GIL_UTILITIES_H

#include "gil_config.hpp"
#include <functional>
#include <cmath>
#include <cstddef>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief  Various utilities not specific to the image library. Some are non-standard STL extensions or generic iterator adaptors
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
///
///
////////////////////////////////////////////////////////////////////////////////////////

namespace boost { namespace gil {

////////////////////////////////////////////////////////////////////////////////////////
//                           CLASS point2
///
/// \brief 2D point both axes of which have the same dimension type
/// \ingroup PointModel
/// Models: Point2DConcept
///
////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class point2 {
public:
    typedef T value_type;
    template <std::size_t D> struct axis { typedef value_type coord_t; };
    static const std::size_t num_dimensions=2;

    point2()                : x(0),     y(0)    {}
    point2(T newX, T newY)  : x(newX),  y(newY) {}
    point2(const point2& p) : x(p.x), y(p.y) {}
    ~point2() {}

    point2& operator=(const point2& p)            { x=p.x; y=p.y; return *this; }

    point2        operator<<(int shift)         const   { return point2(x<<shift,y<<shift); }
    point2        operator>>(int shift)         const   { return point2(x>>shift,y>>shift); }
    point2& operator+=(const point2& p)           { x+=p.x; y+=p.y; return *this; }
    point2& operator-=(const point2& p)           { x-=p.x; y-=p.y; return *this; }
    point2& operator/=(double t)                  { x/=t; y/=t; return *this; }

    const T& operator[](std::size_t i)          const   { return this->*mem_array[i]; }
          T& operator[](std::size_t i)                  { return this->*mem_array[i]; }

    T x,y;
private:
    // this static array of pointers to member variables makes operator[] safe and doesn't seem to exhibit any performance penalty
    static T point2<T>::* const mem_array[num_dimensions];
};

template <typename T>
T point2<T>::* const point2<T>::mem_array[point2<T>::num_dimensions] = { &point2<T>::x, &point2<T>::y };

/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bool operator==(const point2<T>& p1, const point2<T>& p2) { return (p1.x==p2.x && p1.y==p2.y); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bool operator!=(const point2<T>& p1, const point2<T>& p2) { return  p1.x!=p2.x || p1.y!=p2.y; }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
point2<T> operator+(const point2<T>& p1, const point2<T>& p2) { return point2<T>(p1.x+p2.x,p1.y+p2.y); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
point2<T> operator-(const point2<T>& p) { return point2<T>(-p.x,-p.y); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
point2<T> operator-(const point2<T>& p1, const point2<T>& p2) { return point2<T>(p1.x-p2.x,p1.y-p2.y); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
point2<double> operator/(const point2<T>& p, double t)      { return t==0 ? point2<double>(0,0):point2<double>(p.x/t,p.y/t); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
point2<T> operator*(const point2<T>& p, int t)      { return point2<T>(p.x*t,p.y*t); }

/// \ingroup PointModel
template <std::size_t K, typename T> GIL_FORCEINLINE
const T& axis_value(const point2<T>& p) { return p[K]; }

/// \ingroup PointModel
template <std::size_t K, typename T> GIL_FORCEINLINE
      T& axis_value(      point2<T>& p) { return p[K]; }

////////////////////////////////////////////////////////////////////////////////////////
///
///  Rounding of real numbers / points to integers / integer points
///
////////////////////////////////////////////////////////////////////////////////////////

inline int iround(float x ) { return static_cast<int>(x + (x < 0.0f ? -0.5f : 0.5f)); }
inline int iround(double x) { return static_cast<int>(x + (x < 0.0 ? -0.5 : 0.5)); }
inline int ifloor(float x ) { return static_cast<int>(std::floor(x)); }
inline int ifloor(double x) { return static_cast<int>(std::floor(x)); }
inline int iceil(float x )  { return static_cast<int>(std::ceil(x)); }
inline int iceil(double x)  { return static_cast<int>(std::ceil(x)); }

/// \ingroup PointAlgorithm
inline point2<int> iround(const point2<float >& p)  { return point2<int>(iround(p.x),iround(p.y)); }
/// \ingroup PointAlgorithm
inline point2<int> iround(const point2<double>& p)  { return point2<int>(iround(p.x),iround(p.y)); }
/// \ingroup PointAlgorithm
inline point2<int> ifloor(const point2<float >& p)  { return point2<int>(ifloor(p.x),ifloor(p.y)); }
/// \ingroup PointAlgorithm
inline point2<int> ifloor(const point2<double>& p)  { return point2<int>(ifloor(p.x),ifloor(p.y)); }
/// \ingroup PointAlgorithm
inline point2<int> iceil (const point2<float >& p)  { return point2<int>(iceil(p.x), iceil(p.y)); }
/// \ingroup PointAlgorithm
inline point2<int> iceil (const point2<double>& p)  { return point2<int>(iceil(p.x), iceil(p.y)); }

////////////////////////////////////////////////////////////////////////////////////////
///
///  computing size with alignment
///
////////////////////////////////////////////////////////////////////////////////////////

template <typename T> 
inline T align(T val, std::size_t alignment) { 
    return val+(alignment - val%alignment)%alignment; 
}

/// \brief Composes two dereference function objects. Similar to std::unary_compose but needs to pull some typedefs from the component types.  Models: PixelDereferenceAdaptorConcept
/// \ingroup PixelDereferenceAdaptorModel
///
template <typename D1, typename D2>
class deref_compose {
public:
    D1 _fn1;
    D2 _fn2;

    typedef deref_compose<typename D1::const_t, typename D2::const_t> const_t;
    typedef typename D1::value_type      value_type;
    typedef typename D1::reference       reference;
    typedef typename D1::const_reference const_reference;
    typedef typename D2::argument_type   argument_type;
    typedef typename D1::result_type     result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable=D1::is_mutable && D2::is_mutable);

    deref_compose() {}
    deref_compose(const D1& x, const D2& y) : _fn1(x), _fn2(y) {}
    deref_compose(const deref_compose& dc)  : _fn1(dc._fn1), _fn2(dc._fn2) {}
    template <typename _D1, typename _D2> deref_compose(const deref_compose<_D1,_D2>& dc) : _fn1(dc._fn1), _fn2(dc._fn2) {}

    result_type operator()(argument_type x) const { return _fn1(_fn2(x)); }
    result_type operator()(argument_type x)       { return _fn1(_fn2(x)); }
};

// reinterpret_cast is implementation-defined. Static cast is not.
template <typename OutPtr, typename In> GIL_FORCEINLINE
      OutPtr gil_reinterpret_cast(      In* p) { return static_cast<OutPtr>(static_cast<void*>(p)); }

template <typename OutPtr, typename In> GIL_FORCEINLINE
const OutPtr gil_reinterpret_cast_c(const In* p) { return static_cast<const OutPtr>(static_cast<const void*>(p)); }

namespace detail {

////////////////////////////////////////////////////////////////////////////////////////
///
///  \brief copy_n taken from SGI STL.
///
////////////////////////////////////////////////////////////////////////////////////////

template <class InputIter, class Size, class OutputIter>
std::pair<InputIter, OutputIter> _copy_n(InputIter first, Size count,
                                         OutputIter result,
                                         std::input_iterator_tag) {
   for ( ; count > 0; --count) {
      *result = *first;
      ++first;
      ++result;
   }
   return std::pair<InputIter, OutputIter>(first, result);
}

template <class RAIter, class Size, class OutputIter>
inline std::pair<RAIter, OutputIter>
_copy_n(RAIter first, Size count, OutputIter result, std::random_access_iterator_tag) {
   RAIter last = first + count;
   return std::pair<RAIter, OutputIter>(last, std::copy(first, last, result));
}

template <class InputIter, class Size, class OutputIter>
inline std::pair<InputIter, OutputIter>
_copy_n(InputIter first, Size count, OutputIter result) {
   return _copy_n(first, count, result, typename std::iterator_traits<InputIter>::iterator_category());
}

template <class InputIter, class Size, class OutputIter>
inline std::pair<InputIter, OutputIter>
copy_n(InputIter first, Size count, OutputIter result) {
    return detail::_copy_n(first, count, result);
}

/// \brief identity taken from SGI STL.
template <typename T> 
struct identity : public std::unary_function<T,T> {
    const T& operator()(const T& val) const { return val; }
};

/*************************************************************************************************/

/// \brief plus function object whose arguments may be of different type.
template <typename T1, typename T2>
struct plus_asymmetric : public std::binary_function<T1,T2,T1> {
    T1 operator()(T1 f1, T2 f2) const {
        return f1+f2;
    }
};

/*************************************************************************************************/

/// \brief operator++ wrapped in a function object
template <typename T>
struct inc : public std::unary_function<T,T> {
    T operator()(T x) const { return ++x; }
};

/*************************************************************************************************/

/// \brief operator-- wrapped in a function object
template <typename T>
struct dec : public std::unary_function<T,T> {
    T operator()(T x) const { return --x; }
};

/// \brief Returns the index corresponding to the first occurrance of a given given type in 
//         a given MPL RandomAccessSequence (or size if the type is not present)
template <typename Types, typename T>
struct type_to_index 
    : public mpl::distance<typename mpl::begin<Types>::type, 
                                  typename mpl::find<Types,T>::type>::type {};
} // namespace detail



/// \ingroup ColorSpaceAndLayoutModel
/// \brief Represents a color space and ordering of channels in memory
template <typename ColorSpace, typename ChannelMapping = mpl::range_c<int,0,mpl::size<ColorSpace>::value> >
struct layout {
    typedef ColorSpace      color_space_t;
    typedef ChannelMapping  channel_mapping_t;
};

/// \brief A version of swap that also works with reference proxy objects
template <typename Value, typename T1, typename T2> // where value_type<T1>  == value_type<T2> == Value
void swap_proxy(T1& left, T2& right) {
    Value tmp = left;
    left = right;
    right = tmp;
}


} }  // namespace boost::gil

#endif
