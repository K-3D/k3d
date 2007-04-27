/*
  Copyright 2005-2007 Adobe Systems Incorporated
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_PACKED_PIXEL_H
#define GIL_PACKED_PIXEL_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief A model of a heterogeneous pixel whose channels are bit ranges. For example 16-bit RGB in '565' format
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on September 28, 2006
///
////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/front.hpp>
#include "gil_config.hpp"
#include "pixel.hpp"

namespace boost { namespace gil {

/// \defgroup ColorBaseModelPackedPixel heterogeneous_packed_pixel 
/// \ingroup ColorBaseModel
/// \brief A heterogeneous color base whose elements are reference proxies to channels in a pixel. Models ColorBaseValueConcept. This class is used to model packed pixels, such as 16-bit packed RGB.

/// \defgroup PixelModelPackedPixel heterogeneous_packed_pixel 
/// \ingroup PixelModel
/// \brief A heterogeneous pixel used to represent packed pixels with non-byte-aligned channels. Models PixelValueConcept

/// \ingroup ColorBaseModelPackedPixel PixelModelPackedPixel PixelBasedModel
/// \brief Heterogeneous pixel value whose channel references can be constructed from the pixel data and their index. Models ColorBaseValueConcept, PixelValueConcept, PixelBasedConcept
/// Typical use for this is a model of a packed pixel (like 565 RGB)
template <typename PixelData, typename ChannelRefVec, typename Layout>
struct heterogeneous_packed_pixel {
    PixelData _data;

    // required by ColorBaseConcept, MutableColorBaseConcept
    typedef Layout layout_t;
    template <int K> struct kth_element_type           : public mpl::at_c<ChannelRefVec,K> {};
    template <int K> struct kth_element_reference_type : public mpl::at_c<ChannelRefVec,K> {};
    template <int K> struct kth_element_const_reference_type {
        typedef typename channel_traits<typename mpl::at_c<ChannelRefVec,K>::type>::const_reference type;    
    };

    typedef heterogeneous_packed_pixel          value_type;
    typedef value_type&                         reference;
    typedef const value_type&                   const_reference;

    BOOST_STATIC_CONSTANT(bool, is_mutable = channel_traits<typename mpl::front<ChannelRefVec>::type>::is_mutable);

    heterogeneous_packed_pixel(){}
    heterogeneous_packed_pixel(const PixelData& data) : _data(data) {}

    // Construct from another compatible pixel type
    heterogeneous_packed_pixel(const heterogeneous_packed_pixel& p) : _data(p._data) {}
    template <typename P> heterogeneous_packed_pixel(const P& p)                    { check_compatible<P>(); static_copy(p,*this); }   

    template <typename P> heterogeneous_packed_pixel& operator=(const P& p)        { check_compatible<P>(); static_copy(p,*this); return *this; } 
    heterogeneous_packed_pixel& operator=(const heterogeneous_packed_pixel& p)     { _data=p._data; return *this; }

    template <typename P> bool                        operator==(const P& p) const { check_compatible<P>(); return static_equal(*this,p); }
    template <typename P> bool                        operator!=(const P& p) const { return !(*this==p); }

private:
    template <typename Pixel> static void check_compatible() { gil_function_requires<PixelsCompatibleConcept<Pixel,heterogeneous_packed_pixel> >(); }
};

/// \brief Metafunction predicate that flags heterogeneous_packed_pixel as a model of PixelConcept. Required by PixelConcept
/// \ingroup PixelModelPackedPixel
template <typename PixelData, typename ChannelRefVec, typename Layout>  
struct is_pixel<heterogeneous_packed_pixel<PixelData,ChannelRefVec,Layout> > : public mpl::true_{};

/// \brief mutable at_c required by MutableColorBaseConcept
/// \ingroup ColorBaseModelPackedPixel
template <int K, typename P, typename C, typename L> inline
typename heterogeneous_packed_pixel<P,C,L>::template kth_element_reference_type<K>::type 
at_c(heterogeneous_packed_pixel<P,C,L>& p) { 
    return typename heterogeneous_packed_pixel<P,C,L>::template kth_element_reference_type<K>::type(p._data); 
}

/// \brief constant at_c required by ColorBaseConcept
/// \ingroup ColorBaseModelPackedPixel
template <int K, typename P, typename C, typename L> inline
typename heterogeneous_packed_pixel<P,C,L>::template kth_element_const_reference_type<K>::type 
at_c(const heterogeneous_packed_pixel<P,C,L>& p) { 
    return typename heterogeneous_packed_pixel<P,C,L>::template kth_element_const_reference_type<K>::type(p._data);
}

/// \brief Specifies the color space type of a heterogeneous packed pixel. Required by PixelBasedConcept
/// \ingroup PixelModelPackedPixel
template <typename P, typename C, typename Layout>
struct color_space_type<heterogeneous_packed_pixel<P,C,Layout> > {
    typedef typename Layout::color_space_t type;
}; 

/// \brief Specifies the channel mapping of a heterogeneous packed pixel. Required by PixelBasedConcept
/// \ingroup PixelModelPackedPixel
template <typename P, typename C, typename Layout>
struct channel_mapping_type<heterogeneous_packed_pixel<P,C,Layout> > {
    typedef typename Layout::channel_mapping_t type;
}; 

/// \brief Specifies that the heterogeneous packed pixel is not planar. Required by PixelBasedConcept
/// \ingroup PixelModelPackedPixel
template <typename P, typename C, typename Layout>
struct is_planar<heterogeneous_packed_pixel<P,C,Layout> > : mpl::false_ {}; 

////////////////////////////////////////////////////////////////////////////////
///
/// Support for interleaved iterators over packed pixel
///
////////////////////////////////////////////////////////////////////////////////

/// \defgroup PixelIteratorModelPackedInterleavedPtr Pointer to heterogeneous_packed_pixel<P,CR,Layout>
/// \ingroup PixelIteratorModel
/// \brief Iterators over interleaved pixels.
/// The pointer heterogeneous_packed_pixel<P,CR,Layout>* is used as an iterator over interleaved pixels of packed format. Models PixelIteratorConcept, HasDynamicXStepTypeConcept, ByteAdvanceableIteratorConcept

template <typename P, typename C, typename L>  
struct iterator_is_mutable<heterogeneous_packed_pixel<P,C,L>*> : public mpl::bool_<heterogeneous_packed_pixel<P,C,L>::is_mutable> {};
template <typename P, typename C, typename L>  
struct iterator_is_mutable<const heterogeneous_packed_pixel<P,C,L>*> : public mpl::false_ {};

/////////////////////////////
//  HasDynamicXStepTypeConcept
/////////////////////////////

/// \ingroup PixelIteratorModelPackedInterleavedPtr 
template <typename P, typename C, typename L>
struct dynamic_x_step_type<heterogeneous_packed_pixel<P,C,L>*> {
    typedef byte_addressable_step_iterator<heterogeneous_packed_pixel<P,C,L>*> type;
};

/// \ingroup PixelIteratorModelPackedInterleavedPtr 
template <typename P, typename C, typename L>
struct dynamic_x_step_type<const heterogeneous_packed_pixel<P,C,L>*> {
    typedef byte_addressable_step_iterator<const heterogeneous_packed_pixel<P,C,L>*> type;
};


/////////////////////////////
//  PixelBasedConcept
/////////////////////////////

template <typename P, typename C, typename L>
struct color_space_type<heterogeneous_packed_pixel<P,C,L>*> : public color_space_type<heterogeneous_packed_pixel<P,C,L> > {};

template <typename P, typename C, typename L>
struct channel_mapping_type<heterogeneous_packed_pixel<P,C,L>*> : public channel_mapping_type<heterogeneous_packed_pixel<P,C,L> > {};

template <typename P, typename C, typename L>
struct is_planar<heterogeneous_packed_pixel<P,C,L>*> : public is_planar<heterogeneous_packed_pixel<P,C,L> > {};


template <typename P, typename C, typename L>
struct color_space_type<const heterogeneous_packed_pixel<P,C,L>*> : public color_space_type<heterogeneous_packed_pixel<P,C,L> > {};

template <typename P, typename C, typename L>
struct channel_mapping_type<const heterogeneous_packed_pixel<P,C,L>*> : public channel_mapping_type<heterogeneous_packed_pixel<P,C,L> > {};

template <typename P, typename C, typename L>
struct is_planar<const heterogeneous_packed_pixel<P,C,L>*> : public is_planar<heterogeneous_packed_pixel<P,C,L> > {};


} }  // namespace boost::gil

#endif
