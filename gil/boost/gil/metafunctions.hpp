/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_METAFUNCTIONS_HPP
#define GIL_METAFUNCTIONS_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief metafunctions that construct types or return type properties
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
///
/// \date 2005-2007 \n Last updated on February 6, 2007
///
////////////////////////////////////////////////////////////////////////////////////////

#include <iterator>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/type_traits.hpp>
#include "gil_config.hpp"
#include "gil_concept.hpp"

namespace boost { namespace gil {

// forward declarations
template <typename T, typename L> struct pixel;
template <typename T, typename C> struct planar_pixel_reference;
template <typename IC, typename C> struct planar_pixel_iterator;
template <typename I> class byte_addressable_step_iterator;
template <typename I> class byte_addressable_2d_locator;
template <typename L> class image_view;
template <typename Pixel, bool IsPlanar, typename Alloc> class image;
template <typename T> struct channel_type;
template <typename T> struct color_space_type;
template <typename T> struct channel_mapping_type;
template <typename It> struct is_iterator_adaptor;
template <typename It> struct iterator_adaptor_get_base;


//////////////////////////////////////////////////
///
///  TYPE ANALYSIS METAFUNCTIONS
///  Predicate metafunctions determining properties of GIL types
///
//////////////////////////////////////////////////


/// \defgroup GILIsBasic xxx_is_basic
/// \ingroup TypeAnalysis
/// \brief Determines if GIL constructs are basic. 
///    Basic constructs are the ones that can be generated with the type 
///    factory methods pixel_reference_type, iterator_type, locator_type, view_type and image_type
///    They can be mutable/immutable, planar/interleaved, step/nonstep. They must use GIL-provided models.

/// \brief Determines if a given pixel reference is basic
///    Basic references must use gil::pixel& (if interleaved), gil::planar_pixel_reference (if planar). They must use the standard constness rules. 
/// \ingroup GILIsBasic
template <typename PixelRef>        struct pixel_reference_is_basic                     : public mpl::false_ {};
template <typename T,  typename L>  struct pixel_reference_is_basic<      pixel<T,L>&>  : public mpl::true_ {};
template <typename T,  typename L>  struct pixel_reference_is_basic<const pixel<T,L>&>  : public mpl::true_ {};
template <typename TR, typename Cs> struct pixel_reference_is_basic<planar_pixel_reference<TR,Cs> > : public mpl::true_ {};
template <typename TR, typename Cs> struct pixel_reference_is_basic<const planar_pixel_reference<TR,Cs> > : public mpl::true_ {};


/// \brief Determines if a given pixel iterator is basic
///    Basic iterators must use gil::pixel (if interleaved), gil::planar_pixel_iterator (if planar) and gil::byte_addressable_step_iterator (if step). They must use the standard constness rules. 
/// \ingroup GILIsBasic
template <typename Iterator>
struct iterator_is_basic : public mpl::false_ {};
template <typename T, typename L>  // mutable   interleaved
struct iterator_is_basic<      pixel<T,L>*      > : public mpl::true_ {};
template <typename T, typename L>  // immutable interleaved
struct iterator_is_basic<const pixel<T,L>*      > : public mpl::true_ {};
template <typename T, typename Cs>  // mutable   planar
struct iterator_is_basic<planar_pixel_iterator<      T*,Cs> > : public mpl::true_ {};
template <typename T, typename Cs>    // immutable planar
struct iterator_is_basic<planar_pixel_iterator<const T*,Cs> > : public mpl::true_ {};
template <typename T, typename L>  // mutable   interleaved step
struct iterator_is_basic<byte_addressable_step_iterator<      pixel<T,L>*> > : public mpl::true_ {};
template <typename T, typename L>  // immutable interleaved step
struct iterator_is_basic<byte_addressable_step_iterator<const pixel<T,L>*> > : public mpl::true_ {};
template <typename T, typename Cs>  // mutable   planar step
struct iterator_is_basic<byte_addressable_step_iterator<planar_pixel_iterator<      T*,Cs> > > : public mpl::true_ {};
template <typename T, typename Cs>    // immutable planar step
struct iterator_is_basic<byte_addressable_step_iterator<planar_pixel_iterator<const T*,Cs> > > : public mpl::true_ {};


/// \ingroup GILIsBasic
/// \brief Determines if a given locator is basic. A basic locator is memory-based and has basic x_iterator and y_iterator
template <typename Loc> struct locator_is_basic : public mpl::false_ {};
template <typename Iterator> struct locator_is_basic<byte_addressable_2d_locator<byte_addressable_step_iterator<Iterator> > > : public iterator_is_basic<Iterator> {};

/// \ingroup GILIsBasic
/// \brief Basic views must be over basic locators
template <typename View> struct view_is_basic : public mpl::false_ {};
template <typename Loc> struct view_is_basic<image_view<Loc> > : public locator_is_basic<Loc> {};

/// \ingroup GILIsBasic
/// \brief Basic images must use basic views and std::allocator of char
template <typename Img> struct image_is_basic : public mpl::false_ {};
template <typename Pixel, bool IsPlanar, typename Alloc> struct image_is_basic<image<Pixel,IsPlanar,Alloc> > : public mpl::true_ {};


/// \defgroup GILIsStep xxx_is_step
/// \ingroup TypeAnalysis
/// \brief Determines if the given iterator/locator/view has a step that could be set dynamically

template <typename I> struct iterator_is_step;
namespace detail {
    template <typename It, bool IsBase, bool EqualsStepType> struct iterator_is_step_impl;
    // iterator that has the same type as its dynamic_x_step_type must be a step iterator
    template <typename It, bool IsBase> struct iterator_is_step_impl<It,IsBase,true> : public mpl::true_{};

    // base iterator can never be a step iterator
    template <typename It> struct iterator_is_step_impl<It,true,false> : public mpl::false_{};

    // for an iterator adaptor, see if its base is step
    template <typename It> struct iterator_is_step_impl<It,false,false> 
        : public iterator_is_step<typename iterator_adaptor_get_base<It>::type>{};
}

/// \ingroup GILIsStep
/// \brief Determines if the given iterator has a step that could be set dynamically
template <typename I> struct iterator_is_step 
    : public detail::iterator_is_step_impl<I, 
        !is_iterator_adaptor<I>::type::value,
        is_same<I,typename dynamic_x_step_type<I>::type>::value >{};

/// \ingroup GILIsStep
/// \brief Determines if the given locator has a horizontal step that could be set dynamically
template <typename L> struct locator_is_step_in_x : public iterator_is_step<typename L::x_iterator> {}; 

/// \ingroup GILIsStep
/// \brief Determines if the given locator has a vertical step that could be set dynamically
template <typename L> struct locator_is_step_in_y : public iterator_is_step<typename L::y_iterator> {}; 

/// \ingroup GILIsStep
/// \brief Determines if the given view has a horizontal step that could be set dynamically
template <typename V> struct view_is_step_in_x : public locator_is_step_in_x<typename V::xy_locator> {}; 

/// \ingroup GILIsStep
/// \brief Determines if the given view has a vertical step that could be set dynamically
template <typename V> struct view_is_step_in_y : public locator_is_step_in_y<typename V::xy_locator> {}; 

/// \defgroup GILIsMutable xxx_is_mutable
/// \ingroup TypeAnalysis
/// \brief Determines if the given pixel reference/iterator/locator/view is mutable (i.e. its pixels can be changed)

/// \ingroup GILIsMutable
/// \brief Determines if the given pixel reference is mutable (i.e. its channels can be changed)
///
/// Note that built-in C++ references obey the const qualifier but reference proxy classes do not.
template <typename R>               struct pixel_reference_is_mutable : public mpl::bool_<remove_reference<R>::type::is_mutable> {};
template <typename T,  typename L>  struct pixel_reference_is_mutable<      pixel<T,L>&> : public mpl::true_ {};
template <typename T,  typename L>  struct pixel_reference_is_mutable<const pixel<T,L>&> : public mpl::false_ {};
template <typename TR, typename Cs> struct pixel_reference_is_mutable<      planar_pixel_reference<TR,Cs> > : public mpl::bool_< channel_traits<TR>::is_mutable > {};
template <typename TR, typename Cs> struct pixel_reference_is_mutable<const planar_pixel_reference<TR,Cs> > : public mpl::bool_< channel_traits<TR>::is_mutable > {};

/// \ingroup GILIsMutable
/// \brief Determines if the given locator is mutable (i.e. its pixels can be changed)
template <typename L> struct locator_is_mutable : public iterator_is_mutable<typename L::x_iterator> {};
/// \ingroup GILIsMutable
/// \brief Determines if the given view is mutable (i.e. its pixels can be changed)
template <typename V> struct view_is_mutable : public iterator_is_mutable<typename V::x_iterator> {};


/// \brief Determines if the given object is a GIL pixel reference (as opposed to a pixel value or anything else). Custom models of references must provide specializations.
/// \ingroup TypeAnalysis
template <typename R> struct is_pixel_reference : public pixel_reference_is_basic<R> {};

//////////////////////////////////////////////////
///
///  TYPE FACTORY METAFUNCTIONS
///  Metafunctions returning GIL types from other GIL types
///
//////////////////////////////////////////////////

/// \defgroup TypeFactoryFromElements xxx_type
/// \ingroup TypeFactory
/// \brief Returns the type of a homogeneous GIL construct given its elements (channel, layout, whether it is planar, step, mutable, etc.)

/// \defgroup TypeFactoryFromPixel xxx_type_from_pixel
/// \ingroup TypeFactory
/// \brief Returns the type of a GIL construct given its pixel type, whether it is planar, step, mutable, etc.

/// \defgroup TypeFactoryDerived derived_xxx_type
/// \ingroup TypeFactory
/// \brief Returns the type of a homogeneous GIL construct given a related construct by changing some of its properties

/// \ingroup TypeFactoryFromElements
/// \brief Returns the type of a homogeneous pixel reference given the channel type, layout, whether it operates on planar data and whether it is mutable
template <typename T, typename L, bool IsPlanar=false, bool IsMutable=true> struct pixel_reference_type{};
template <typename T, typename L> struct pixel_reference_type<T,L,false,true > { typedef pixel<T,L>& type; };
template <typename T, typename L> struct pixel_reference_type<T,L,false,false> { typedef const pixel<T,L>& type; };
template <typename T, typename L> struct pixel_reference_type<T,L,true,true> { typedef const planar_pixel_reference<typename channel_traits<T>::reference,typename color_space_type<L>::type> type; };       // TODO: Assert M=identity
template <typename T, typename L> struct pixel_reference_type<T,L,true,false> { typedef const planar_pixel_reference<typename channel_traits<T>::const_reference,typename color_space_type<L>::type> type; };// TODO: Assert M=identity

/// \ingroup TypeFactoryFromPixel
/// \brief Returns the type of a pixel iterator given the pixel type, whether it operates on planar data, whether it is a step iterator, and whether it is mutable
template <typename Pixel, bool IsPlanar=false, bool IsStep=false, bool IsMutable=true> struct iterator_type_from_pixel{};
template <typename Pixel> struct iterator_type_from_pixel<Pixel,false,false,true > { typedef Pixel* type; };
template <typename Pixel> struct iterator_type_from_pixel<Pixel,false,false,false> { typedef const Pixel* type; };
template <typename Pixel> struct iterator_type_from_pixel<Pixel,true,false,true> { 
    typedef planar_pixel_iterator<typename channel_traits<typename channel_type<Pixel>::type>::pointer,typename color_space_type<Pixel>::type> type; 
};
template <typename Pixel> struct iterator_type_from_pixel<Pixel,true,false,false> { 
    typedef planar_pixel_iterator<typename channel_traits<typename channel_type<Pixel>::type>::const_pointer,typename color_space_type<Pixel>::type> type; 
};

/// \ingroup TypeFactoryFromElements
/// \brief Returns the type of a homogeneous iterator given the channel type, layout, whether it operates on planar data, whether it is a step iterator, and whether it is mutable
template <typename T, typename L, bool IsPlanar=false, bool IsStep=false, bool IsMutable=true> struct iterator_type{};
template <typename T, typename L> struct iterator_type<T,L,false,false,true > { typedef pixel<T,L>* type; };
template <typename T, typename L> struct iterator_type<T,L,false,false,false> { typedef const pixel<T,L>* type; };
template <typename T, typename L> struct iterator_type<T,L,true,false,true> { typedef planar_pixel_iterator<T*,typename L::color_space_t> type; };               // TODO: Assert M=identity
template <typename T, typename L> struct iterator_type<T,L,true,false,false> { typedef planar_pixel_iterator<const T*,typename L::color_space_t> type; };        // TODO: Assert M=identity
template <typename T, typename L, bool IsPlanar, bool IsMutable> struct iterator_type<T,L,IsPlanar,true,IsMutable> { 
    typedef byte_addressable_step_iterator<typename iterator_type<T,L,IsPlanar,false,IsMutable>::type> type; 
};

/// \brief Given a pixel iterator defining access to pixels along a row, returns the types of the corresponding built-in step_iterator, xy_locator, image_view
/// \ingroup TypeFactory
template <typename XIterator> 
struct type_from_x_iterator {
    typedef byte_addressable_step_iterator<XIterator>    step_iterator_t;
    typedef byte_addressable_2d_locator<step_iterator_t> xy_locator_t;
    typedef image_view<xy_locator_t>                     view_t;
};


/// \ingroup TypeFactoryFromElements
/// \brief Returns the type of a homogeneous locator given the channel type, layout, whether it operates on planar data and whether it has a step horizontally
template <typename T, typename L, bool IsPlanar=false, bool IsStepX=false, bool IsMutable=true> 
struct locator_type {
    typedef typename type_from_x_iterator<typename iterator_type<T,L,IsPlanar,IsStepX,IsMutable>::type>::xy_locator_type type;
};

/// \ingroup TypeFactoryFromElements
/// \brief Returns the type of a homogeneous view given the channel type, layout, whether it operates on planar data and whether it has a step horizontally
template <typename T, typename L, bool IsPlanar=false, bool IsStepX=false, bool IsMutable=true> 
struct view_type {
    typedef typename type_from_x_iterator<typename iterator_type<T,L,IsPlanar,IsStepX,IsMutable>::type>::view_t type;
};

/// \ingroup TypeFactoryFromElements
/// \brief Returns the type of a homogeneous image given the channel type, layout, and whether it operates on planar data
template <typename T, typename L, bool IsPlanar=false, typename Alloc=std::allocator<unsigned char> > 
struct image_type {
    typedef image<pixel<T,L>, IsPlanar, Alloc> type;
};

/// \ingroup TypeFactoryFromPixel
/// \brief Returns the type of a view the pixel type, whether it operates on planar data and whether it has a step horizontally
template <typename Pixel, bool IsPlanar=false, bool IsStepX=false, bool IsMutable=true> 
struct view_type_from_pixel {
    typedef typename type_from_x_iterator<typename iterator_type_from_pixel<Pixel,IsPlanar,IsStepX,IsMutable>::type>::view_t type;
};


/// \brief Constructs a pixel reference type from a source pixel reference type by changing some of the properties.
/// \ingroup TypeFactoryDerived
///  Use use_default for the properties of the source view that you want to keep
template <typename Ref, typename T=use_default, typename L=use_default, typename IsPlanar=use_default, typename IsMutable=use_default>
class derived_pixel_reference_type {
    typedef typename remove_reference<Ref>::type pixel_t;
    typedef typename  mpl::if_<is_same<T, use_default>, typename channel_type<pixel_t>::type,     T >::type channel_t;
    typedef typename  mpl::if_<is_same<L, use_default>, 
        layout<typename color_space_type<pixel_t>::type, typename channel_mapping_type<pixel_t>::type>, L>::type           layout_t;
    static const bool mut   =mpl::if_<is_same<IsMutable,use_default>, pixel_reference_is_mutable<Ref>, IsMutable>::type::value;
    static const bool planar=mpl::if_<is_same<IsPlanar,use_default>,  is_planar<pixel_t>,  IsPlanar>::type::value;
public:
    typedef typename pixel_reference_type<channel_t, layout_t, planar, mut>::type type;
};

/// \brief Constructs a pixel iterator type from a source pixel iterator type by changing some of the properties.
/// \ingroup TypeFactoryDerived
///  Use use_default for the properties of the source view that you want to keep
template <typename Iterator, typename T=use_default, typename L=use_default, typename IsPlanar=use_default, typename IsStep=use_default, typename IsMutable=use_default>
class derived_iterator_type {
    typedef typename  mpl::if_<is_same<T ,use_default>, typename channel_type<Iterator>::type,     T >::type channel_t;
    typedef typename  mpl::if_<is_same<L,use_default>, 
        layout<typename color_space_type<Iterator>::type, typename channel_mapping_type<Iterator>::type>, L>::type layout_t;

    static const bool mut   =mpl::if_<is_same<IsMutable,use_default>, iterator_is_mutable<Iterator>, IsMutable>::type::value;
    static const bool planar=mpl::if_<is_same<IsPlanar,use_default>,         is_planar<Iterator>,  IsPlanar>::type::value;
    static const bool step  =mpl::if_<is_same<IsStep  ,use_default>,  iterator_is_step<Iterator>,    IsStep>::type::value;
public:
    typedef typename iterator_type<channel_t, layout_t, planar, step, mut>::type type;
};

/// \brief Constructs an image view type from a source view type by changing some of the properties.
/// \ingroup TypeFactoryDerived
///  Use use_default for the properties of the source view that you want to keep
template <typename View, typename T=use_default, typename L=use_default, typename IsPlanar=use_default, typename StepX=use_default, typename IsMutable=use_default>
class derived_view_type {
    typedef typename  mpl::if_<is_same<T ,use_default>, typename channel_type<View>::type, T>::type channel_t;
    typedef typename  mpl::if_<is_same<L,use_default>, 
        layout<typename color_space_type<View>::type, typename channel_mapping_type<View>::type>, L>::type layout_t;
    static const bool mut   =mpl::if_<is_same<IsMutable,use_default>, view_is_mutable<View>, IsMutable>::type::value;
    static const bool planar=mpl::if_<is_same<IsPlanar,use_default>,  is_planar<View>,  IsPlanar>::type::value;
    static const bool step  =mpl::if_<is_same<StepX ,use_default>,  view_is_step_in_x<View>,StepX>::type::value;
public:
    typedef typename view_type<channel_t, layout_t, planar, step, mut>::type type;
};

/// \brief Constructs a homogeneous image type from a source image type by changing some of the properties.
/// \ingroup TypeFactoryDerived
///  Use use_default for the properties of the source image that you want to keep
template <typename Image, typename T=use_default, typename L=use_default, typename IsPlanar=use_default>
class derived_image_type {
    typedef typename  mpl::if_<is_same<T ,use_default>, typename channel_type<Image>::type,     T >::type channel_t;
    typedef typename  mpl::if_<is_same<L,use_default>, 
        layout<typename color_space_type<Image>::type, typename channel_mapping_type<Image>::type>, L>::type layout_t;
    static const bool planar=mpl::if_<is_same<IsPlanar,use_default>,  is_planar<Image>,  IsPlanar>::type::value;
public:
    typedef typename image_type<channel_t, layout_t, planar>::type type;
};




} }  // namespace boost::gil

#endif
