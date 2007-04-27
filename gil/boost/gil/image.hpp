/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_IMAGE_H
#define GIL_IMAGE_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief Templated image
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on February 12, 2007
///
////////////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <memory>
#include "gil_config.hpp"
#include "image_view.hpp"
#include "metafunctions.hpp"
#include "algorithm.hpp"

namespace boost { namespace gil {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)     // conversion from 'gil::image<V,Alloc>::coord_t' to 'int', possible loss of data (visual studio compiler doesn't realize that the two types are the same)
#endif

////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup ImageModel PixelBasedModel
/// \brief container interface over image view. Models ImageConcept, PixelBasedConcept
/// 
/// A 2D container whose elements are pixels. It is templated over the pixel type, a boolean
/// indicating whether it should be planar, and an optional allocator.
///
/// Note that its element type does not have to be a pixel. \p image can be instantiated with any Regular element, 
/// in which case it models the weaker RandomAccess2DImageConcept and does not model PixelBasedConcept
///
////////////////////////////////////////////////////////////////////////////////////////

template <typename Pixel, bool IsPlanar, typename Alloc=std::allocator<unsigned char> >    
class image {
public:
    typedef typename Alloc::template rebind<unsigned char>::other allocator_type;
    typedef typename view_type_from_pixel<Pixel, IsPlanar>::type view_t;
    typedef typename view_t::const_t                 const_view_t;
    typedef typename view_t::point_t                 point_t;
    typedef typename view_t::coord_t                 coord_t;
    typedef typename view_t::value_type              value_type;
    typedef coord_t                                  x_coord_t;
    typedef coord_t                                  y_coord_t;

    const point_t&          dimensions()            const { return _view.dimensions(); }
    x_coord_t               width()                 const { return _view.width(); }
    y_coord_t               height()                const { return _view.height(); }

    explicit image(std::size_t alignment=1,
                   const Alloc alloc_in = Alloc()) : 
        _memory(0), _align(alignment), _alloc(alloc_in) {}

    // Create with dimensions and optional initial value and alignment
    image(const point_t& dimensions,
          std::size_t alignment=1) : _align(alignment) {
        create_with_own_data(dimensions);
        uninitialized_fill_pixels(_view,Pixel());
    }
    image(x_coord_t width, y_coord_t height,
          std::size_t alignment=1) : _align(alignment) {
        create_with_own_data(point_t(width,height));
        uninitialized_fill_pixels(_view,Pixel());
    }
    image(const point_t& dimensions, 
          const Pixel& p_in,
          std::size_t alignment,
          const Alloc alloc_in = Alloc())  :
        _align(alignment), _alloc(alloc_in) {
        create_with_own_data(dimensions);
        uninitialized_fill_pixels(_view, p_in);
    }
    image(x_coord_t width, y_coord_t height,
          const Pixel& p_in,
          std::size_t alignment,
          const Alloc alloc_in = Alloc())  :
        _align(alignment), _alloc(alloc_in) {
        create_with_own_data(point_t(width,height));
        uninitialized_fill_pixels(_view, p_in);
    }

    image(const image& img) :
        _align(img._align), _alloc(img._alloc) {
        create_with_own_data(img.dimensions());
        uninitialized_copy_pixels(img._view,_view);
    }

    template <typename P2, bool IP2, typename Alloc2>
    image(const image<P2,IP2,Alloc2>& img) : 
        _align(img._align), _alloc(img._alloc) {
        create_with_own_data(img.dimensions());
        uninitialized_copy_pixels(img._view,_view);
    }
    image& operator=(const image& img) {
        if (dimensions() == img.dimensions())
            copy_pixels(img._view,_view);
        else {
            image tmp(img);
            swap(tmp);
        }
        return *this;
    }

    template <typename Img>
    image& operator=(const Img& img) {
        if (dimensions() == img.dimensions())
            copy_pixels(img._view,_view);
        else {
            image tmp(img);
            swap(tmp);
        }
        return *this;
    }

    ~image() {
        _alloc.deallocate(_memory, 
                          total_allocated_size(_view.dimensions(),
                                               mpl::bool_<IsPlanar>()));
    }

    Alloc&       allocator() { return _alloc; }
    Alloc const& allocator() const { return _alloc; }

    void swap(image& img) { // required by MutableContainerConcept
        using std::swap;
        swap(_align, img._align);
        swap(_memory,img._memory);
        swap(_view,  img._view); 
        swap(_alloc, img._alloc);
    }    

    void recreate(const point_t& dims, std::size_t alignment=1) {
        if (dims!=_view.dimensions() || _align!=alignment) {
            image tmp(dims, alignment);
            swap(tmp);
        }
    }
    void recreate(x_coord_t width, y_coord_t height, std::size_t alignment=1) {
        recreate(point_t(width,height),alignment);
    }
    void recreate(const point_t& dims, 
                  const Pixel& p_in, std::size_t alignment) {
        if (dims!=_view.dimensions() || _align!=alignment) {
            image tmp(dims, p_in, alignment);
            swap(tmp);
        }
    }
    void recreate(x_coord_t width, y_coord_t height, 
                  const Pixel& p_in, std::size_t alignment) {
        recreate(point_t(width,height),p_in,alignment);
    }

    view_t       _view;      // contains pointer to the pixels, the image size and ways to navigate pixels
private:
    unsigned char* _memory;
    std::size_t _align;
    allocator_type _alloc;

    std::size_t total_allocated_size(const point_t& dimensions,mpl::false_) const {
        std::size_t row_bytes=align(dimensions.x*sizeof(value_type),_align);
        return row_bytes*dimensions.y+_align-1;
    }
    std::size_t total_allocated_size(const point_t& dimensions,mpl::true_) const {
        std::size_t row_bytes=align(dimensions.x*sizeof(typename channel_type<view_t>::type),_align);
        std::size_t plane_bytes=row_bytes*dimensions.y;
        return plane_bytes*num_channels<view_t>::value+_align-1;
    }

    void create_with_own_data(const point_t& dimensions) { 
        create_with_own_data_(dimensions,mpl::bool_<IsPlanar>());
    }
    
    void create_with_own_data_(const point_t& dimensions, mpl::false_) {
        std::size_t row_bytes=align(dimensions.x*sizeof(value_type),_align);
        _memory=_alloc.allocate(total_allocated_size(dimensions,mpl::false_()));
        unsigned char* tmp=(unsigned char*)align((std::size_t)_memory,_align);
        try {
            _view=view_t(dimensions,typename view_t::locator(typename view_t::x_iterator(tmp),row_bytes));
        } catch(...) {
            _alloc.deallocate(_memory, total_allocated_size(dimensions,mpl::false_()));
            throw;
        }
    }
    void create_with_own_data_(const point_t& dimensions, mpl::true_) {
        std::size_t row_bytes=align(dimensions.x*sizeof(typename channel_type<view_t>::type),_align);
        std::size_t plane_bytes=row_bytes*dimensions.y;
        _memory=_alloc.allocate(total_allocated_size(dimensions,mpl::true_()));
        unsigned char* tmp=(unsigned char*)align((std::size_t)_memory,_align);
        try {
            typename view_t::x_iterator first; 
            for (int i=0; i<num_channels<view_t>::value; ++i)
                dynamic_at_c(first,i) = (typename channel_type<view_t>::type*)(tmp + plane_bytes*i);
            _view=view_t(dimensions, typename view_t::locator(first, dimensions.x*sizeof(typename channel_type<view_t>::type)));
        } catch(...) {
            _alloc.deallocate(_memory, total_allocated_size(dimensions,mpl::true_()));
            throw;
        }
    }
    
};

template <typename Pixel, bool IsPlanar, typename Alloc>
void swap(image<Pixel, IsPlanar, Alloc>& im1,image<Pixel, IsPlanar, Alloc>& im2) {
    im1.swap(im2); 
}

template <typename Pixel1, bool IsPlanar1, typename Alloc1, typename Pixel2, bool IsPlanar2, typename Alloc2>
bool operator==(const image<Pixel1,IsPlanar1,Alloc1>& im1,const image<Pixel2,IsPlanar2,Alloc2>& im2) {
    if ((void*)(&im1)==(void*)(&im2)) return true;
    if (const_view(im1).dimensions()!=const_view(im2).dimensions()) return false;
    return equal_pixels(const_view(im1),const_view(im2));
}
template <typename Pixel1, bool IsPlanar1, typename Alloc1, typename Pixel2, bool IsPlanar2, typename Alloc2>
bool operator!=(const image<Pixel1,IsPlanar1,Alloc1>& im1,const image<Pixel2,IsPlanar2,Alloc2>& im2) {return !(im1==im2);}

///@{
/// \name view, const_view
/// \brief Get an image view from an image

/// \ingroup ImageModel

/// \brief Returns the non-constant-pixel view of an image
template <typename Pixel, bool IsPlanar, typename Alloc> inline 
const typename image<Pixel,IsPlanar,Alloc>::view_t& view(image<Pixel,IsPlanar,Alloc>& img) { return img._view; }

/// \brief Returns the constant-pixel view of an image
template <typename Pixel, bool IsPlanar, typename Alloc> inline 
const typename image<Pixel,IsPlanar,Alloc>::const_view_t const_view(const image<Pixel,IsPlanar,Alloc>& img) { 
    return static_cast<const typename image<Pixel,IsPlanar,Alloc>::const_view_t>(img._view); 
}
///@}

/////////////////////////////
//  PixelBasedConcept
/////////////////////////////

template <typename Pixel, bool IsPlanar, typename Alloc>
struct channel_type<image<Pixel,IsPlanar,Alloc> > : public channel_type<Pixel> {}; 

template <typename Pixel, bool IsPlanar, typename Alloc>
struct color_space_type<image<Pixel,IsPlanar,Alloc> >  : public color_space_type<Pixel> {};

template <typename Pixel, bool IsPlanar, typename Alloc>
struct channel_mapping_type<image<Pixel,IsPlanar,Alloc> > : public channel_mapping_type<Pixel> {};

template <typename Pixel, bool IsPlanar, typename Alloc>
struct is_planar<image<Pixel,IsPlanar,Alloc> > : public mpl::bool_<IsPlanar> {};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} }  // namespace boost::gil

#endif
