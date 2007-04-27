/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_PLANAR_PTR_H
#define GIL_PLANAR_PTR_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief planar pixel pointer class
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 12, 2007
///
////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>
#include "gil_config.hpp"
#include "pixel.hpp"
#include "step_iterator.hpp"

namespace boost { namespace gil {

//forward declaration (as this file is included in planar_pixel_reference.hpp)
template <typename ChannelReference, typename ColorSpace> 
struct planar_pixel_reference;

/// \defgroup ColorBaseModelPlanarPtr planar_pixel_iterator 
/// \ingroup ColorBaseModel
/// \brief A homogeneous color base whose element is a channel iterator. Models HomogeneousColorBaseValueConcept
/// This class is used as an iterator to a planar pixel.

/// \defgroup PixelIteratorModelPlanarPtr planar_pixel_iterator
/// \ingroup PixelIteratorModel
/// \brief An iterator over planar pixels. Models PixelIteratorConcept, HomogeneousPixelBasedConcept, ByteAdvanceableIteratorConcept, HasDynamicXStepTypeConcept

////////////////////////////////////////////////////////////////////////////////////////
/// \brief An iterator over planar pixels. Models HomogeneousColorBaseConcept, PixelIteratorConcept, HomogeneousPixelBasedConcept, ByteAdvanceableIteratorConcept, HasDynamicXStepTypeConcept
///
/// Planar pixels have channel data that is not consecutive in memory.
/// To abstract this we use classes to represent references and pointers to planar pixels.
/// 
/// \ingroup PixelIteratorModelPlanarPtr ColorBaseModelPlanarPtr PixelBasedModel
template <typename ChannelPtr, typename ColorSpace>
struct planar_pixel_iterator : public iterator_facade<planar_pixel_iterator<ChannelPtr,ColorSpace>,
                                                      pixel<typename std::iterator_traits<ChannelPtr>::value_type,layout<ColorSpace> >,
                                                      random_access_traversal_tag,
                                                      const planar_pixel_reference<typename std::iterator_traits<ChannelPtr>::reference,ColorSpace> >,
                               public detail::homogeneous_color_base<ChannelPtr,layout<ColorSpace>,mpl::size<ColorSpace>::value > {
private:
    typedef iterator_facade<planar_pixel_iterator<ChannelPtr,ColorSpace>,
                            pixel<typename std::iterator_traits<ChannelPtr>::value_type,layout<ColorSpace> >,
                            random_access_traversal_tag,
                            const planar_pixel_reference<typename std::iterator_traits<ChannelPtr>::reference,ColorSpace> > parent_t;
    typedef detail::homogeneous_color_base<ChannelPtr,layout<ColorSpace>,mpl::size<ColorSpace>::value> color_base_parent_t;
    typedef typename std::iterator_traits<ChannelPtr>::value_type channel_t;
public:
    typedef typename parent_t::value_type                 value_type;
    typedef typename parent_t::reference                  reference;
    typedef typename parent_t::difference_type            difference_type;

    planar_pixel_iterator() : color_base_parent_t(0) {} 
    planar_pixel_iterator(bool) {}        // constructor that does not fill with zero (for performance)

    planar_pixel_iterator(const ChannelPtr& v0, const ChannelPtr& v1) : color_base_parent_t(v0,v1) {}
    planar_pixel_iterator(const ChannelPtr& v0, const ChannelPtr& v1, const ChannelPtr& v2) : color_base_parent_t(v0,v1,v2) {}
    planar_pixel_iterator(const ChannelPtr& v0, const ChannelPtr& v1, const ChannelPtr& v2, const ChannelPtr& v3) : color_base_parent_t(v0,v1,v2,v3) {}
    planar_pixel_iterator(const ChannelPtr& v0, const ChannelPtr& v1, const ChannelPtr& v2, const ChannelPtr& v3, const ChannelPtr& v4) : color_base_parent_t(v0,v1,v2,v3,v4) {}

    template <typename IC1,typename C1> 
    planar_pixel_iterator(const planar_pixel_iterator<IC1,C1>& ptr) : color_base_parent_t(ptr) {}


    /// Copy constructor and operator= from pointers to compatible planar pixels or planar pixel references.
    /// That allow constructs like pointer = &value or pointer = &reference
    /// Since we should not override operator& that's the best we can do.
    template <typename P> 
    planar_pixel_iterator(P* pix) : color_base_parent_t(pix, true) {
        function_requires<PixelsCompatibleConcept<P,value_type> >();
    }

    struct address_of { template <typename T> T* operator()(T& t) { return &t; } };
    template <typename P> 
    planar_pixel_iterator& operator=(P* pix) {
        function_requires<PixelsCompatibleConcept<P,value_type> >();
        static_transform(*pix,*this, address_of());

        // PERFORMANCE_CHECK: Compare to this:
        //this->template semantic_at_c<0>()=&pix->template semantic_at_c<0>();
        //this->template semantic_at_c<1>()=&pix->template semantic_at_c<1>();
        //this->template semantic_at_c<2>()=&pix->template semantic_at_c<2>();
        return *this;
    }

    /// For some reason operator[] provided by iterator_facade returns a custom class that is convertible to reference
    /// We require our own reference because it is registered in iterator_traits
    reference operator[](difference_type d)       const { return byte_advanced_ref(*this,d*sizeof(channel_t));}

    reference operator->()                        const { return **this; }

    // PERFORMANCE_CHECK: Remove?
    bool operator< (const planar_pixel_iterator& ptr)   const { return at_c<0>(*this)< at_c<0>(ptr); }
    bool operator!=(const planar_pixel_iterator& ptr)   const { return at_c<0>(*this)!=at_c<0>(ptr); }
private:
    friend class boost::iterator_core_access;

    void increment()            { static_transform(*this,*this,detail::inc<ChannelPtr>()); }
    void decrement()            { static_transform(*this,*this,detail::dec<ChannelPtr>()); }
    void advance(ptrdiff_t d)   { static_transform(*this,*this,std::bind2nd(detail::plus_asymmetric<ChannelPtr,ptrdiff_t>(),d)); }
    reference dereference() const { return this->template deref<reference>(); }

    ptrdiff_t distance_to(const planar_pixel_iterator& it) const { return at_c<0>(it)-at_c<0>(*this); }
    bool equal(const planar_pixel_iterator& it) const { return at_c<0>(*this)==at_c<0>(it); }
};

namespace detail {
    template <typename IC> struct channel_iterator_is_mutable : public mpl::true_ {};
    template <typename T>  struct channel_iterator_is_mutable<const T*> : public mpl::false_ {};
}

template <typename IC, typename C> 
struct const_iterator_type<planar_pixel_iterator<IC,C> > { 
private:
    typedef typename std::iterator_traits<IC>::value_type channel_t;
public:
    typedef planar_pixel_iterator<typename channel_traits<channel_t>::const_pointer,C> type; 
};

// The default implementation when the iterator is a C pointer is to use the standard constness semantics
template <typename IC, typename C> 
struct iterator_is_mutable<planar_pixel_iterator<IC,C> > : public detail::channel_iterator_is_mutable<IC> {};

/////////////////////////////
//  HomogeneousPixelBasedConcept
/////////////////////////////

template <typename IC, typename C>
struct color_space_type<planar_pixel_iterator<IC,C> > {
    typedef C type;
};

template <typename IC, typename C>
struct channel_mapping_type<planar_pixel_iterator<IC,C> > : public channel_mapping_type<typename planar_pixel_iterator<IC,C>::value_type> {};

template <typename IC, typename C>
struct is_planar<planar_pixel_iterator<IC,C> > : public mpl::true_ {};

template <typename IC, typename C>
struct channel_type<planar_pixel_iterator<IC,C> > {
    typedef typename std::iterator_traits<IC>::value_type type;
};

/////////////////////////////
//  ByteAdvanceableIteratorConcept
/////////////////////////////

template <typename IC, typename C>
inline std::ptrdiff_t byte_step(const planar_pixel_iterator<IC,C>&) { return sizeof(typename std::iterator_traits<IC>::value_type); }

template <typename IC, typename C>
inline std::ptrdiff_t byte_distance(const planar_pixel_iterator<IC,C>& p1, const planar_pixel_iterator<IC,C>& p2) { 
    return byte_distance(at_c<0>(p1),at_c<0>(p2)); 
}

template <typename IC>
struct byte_advance_fn {
    byte_advance_fn(std::ptrdiff_t byte_diff) : _byte_diff(byte_diff) {}
    IC operator()(const IC& p) const { return byte_advanced(p,_byte_diff); }

    std::ptrdiff_t _byte_diff;
};

template <typename IC, typename C>
inline void byte_advance(planar_pixel_iterator<IC,C>& p, std::ptrdiff_t byte_diff) { 
    static_transform(p, p, byte_advance_fn<IC>(byte_diff));
}

template <typename IC, typename C>
inline planar_pixel_iterator<IC,C> byte_advanced(const planar_pixel_iterator<IC,C>& p, std::ptrdiff_t byteDiff) {
    planar_pixel_iterator<IC,C> ret=p;
    byte_advance(ret, byteDiff);
    return ret;
}

// Advancing a planar pixel iterator by a given number of bytes and taking the reference
template <typename ChannelPtr, typename ColorSpace>
inline planar_pixel_reference<typename std::iterator_traits<ChannelPtr>::reference,ColorSpace> 
    byte_advanced_ref(const planar_pixel_iterator<ChannelPtr,ColorSpace>& ptr, std::ptrdiff_t diff) {
    return planar_pixel_reference<typename std::iterator_traits<ChannelPtr>::reference,ColorSpace>(ptr, diff);
}

/////////////////////////////
//  HasDynamicXStepTypeConcept
/////////////////////////////

template <typename IC, typename C>
struct dynamic_x_step_type<planar_pixel_iterator<IC,C> > {
    typedef byte_addressable_step_iterator<planar_pixel_iterator<IC,C> > type;
};

} }  // namespace boost::gil

#endif
