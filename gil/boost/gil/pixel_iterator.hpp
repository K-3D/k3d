/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_PIXEL_ITERATOR_H
#define GIL_PIXEL_ITERATOR_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief pixel iterator support
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n May 16, 2006
///
////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iterator>
#include "gil_config.hpp"
#include "gil_concept.hpp"
#include "pixel.hpp"

namespace boost { namespace gil {

//forwarded declaration (as this file is included in step_iterator.hpp)
template <typename Iterator>
class byte_addressable_step_iterator;

template <typename Iterator> struct dynamic_x_step_type;

/// \brief metafunction predicate determining whether the given iterator is a plain one or an adaptor over another iterator.
/// Examples of adaptors are the step iterator and the dereference iterator adaptor.
template <typename It>
struct is_iterator_adaptor : public mpl::false_{};

/// \brief returns the base iterator for a given iterator adaptor. Provide an specialization when introducing new iterator adaptors
template <typename It>
struct iterator_adaptor_get_base;

/// \brief Changes the base iterator of an iterator adaptor. Provide an specialization when introducing new iterator adaptors
template <typename It, typename NewBaseIt>
struct iterator_adaptor_rebind;

/// \brief Returns the type of an iterator just like the input iterator, except operating over immutable values
template <typename It>
struct const_iterator_type;

// The default implementation when the iterator is a C pointer is to use the standard constness semantics
template <typename T> struct const_iterator_type<      T*> { typedef const T* type; };
template <typename T> struct const_iterator_type<const T*> { typedef const T* type; };

/// \brief Metafunction predicate returning whether the given iterator allows for changing its values
/// \ingroup GILIsMutable
template <typename It>
struct iterator_is_mutable{};

// The default implementation when the iterator is a C pointer is to use the standard constness semantics
template <typename T> struct iterator_is_mutable<      T*> : public mpl::true_{};
template <typename T> struct iterator_is_mutable<const T*> : public mpl::false_{};

/// \defgroup PixelIteratorModelInterleavedPtr Pointer to pixel<ChannelValue,Layout>
/// \ingroup PixelIteratorModel
/// \brief Iterators over interleaved pixels.
/// The pointer pixel<ChannelValue,Layout>* is used as an iterator over interleaved pixels. Models PixelIteratorConcept, HomogeneousPixelBasedConcept, HasDynamicXStepTypeConcept, ByteAdvanceableIteratorConcept



/////////////////////////////
//  HasDynamicXStepTypeConcept
/////////////////////////////

/// \ingroup PixelIteratorModelInterleavedPtr 
template <typename T, typename L>
struct dynamic_x_step_type<pixel<T,L>*> {
    typedef byte_addressable_step_iterator<pixel<T,L>*> type;
};

/// \ingroup PixelIteratorModelInterleavedPtr 
template <typename T, typename L>
struct dynamic_x_step_type<const pixel<T,L>*> {
    typedef byte_addressable_step_iterator<const pixel<T,L>*> type;
};


/////////////////////////////
//  PixelBasedConcept
/////////////////////////////

template <typename T, typename L>
struct color_space_type<pixel<T,L>*> {
    typedef typename L::color_space_t type;
};

template <typename T, typename L>
struct channel_mapping_type<pixel<T,L>*> {
    typedef typename L::channel_mapping_t type;
};

template <typename T, typename L>
struct is_planar<pixel<T,L>*> : public mpl::false_ {};

/////////////////////////////
//  HomogeneousPixelBasedConcept
/////////////////////////////

template <typename T, typename L>
struct channel_type<pixel<T,L>*> {
    typedef T type;
};

template <typename T, typename L> struct color_space_type<const pixel<T,L>*> : public color_space_type<pixel<T,L>*> {};
template <typename T, typename L> struct channel_mapping_type<const pixel<T,L>*> : public channel_mapping_type<pixel<T,L>*> {};
template <typename T, typename L> struct is_planar<const pixel<T,L>*> : public is_planar<pixel<T,L>*> {};
template <typename T, typename L> struct channel_type<const pixel<T,L>*> : public channel_type<pixel<T,L>*> {};

////////////////////////////////////////////////////////////////////////////////////////
///
/// Support for pixel iterator movement measured in raw bytes (as opposed to pixel type). \n
/// Necessary to handle image row alignment and channel plane alignment.
///
////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
//  ByteAdvanceableIteratorConcept
/////////////////////////////

template <typename P>
inline std::ptrdiff_t byte_step(const P*) { return sizeof(P); }

template <typename P>
inline std::ptrdiff_t byte_distance(const P* p1, const P* p2) { 
    return (gil_reinterpret_cast_c<const unsigned char*>(p2)-gil_reinterpret_cast_c<const unsigned char*>(p1)); 
}

template <typename P> P* byte_advanced(const P* p, std::ptrdiff_t byteDiff);

template <typename P>
inline void byte_advance(P* &p, std::ptrdiff_t byteDiff) { 
    p=(P*)((unsigned char*)(p)+byteDiff);
}

template <typename P>
inline P* byte_advanced(const P* p, std::ptrdiff_t byteDiff) {
    return (P*)((unsigned char*)(p)+byteDiff);
}


//  byte_advanced_ref
//  (shortcut to advancing a pointer by a given number of bytes and taking the reference in case the compiler is not smart enough)

template <typename P>
inline P& byte_advanced_ref(P* p, std::ptrdiff_t byteDiff) {
    return *byte_advanced(p,byteDiff);
}

} }  // namespace boost::gil

#endif
