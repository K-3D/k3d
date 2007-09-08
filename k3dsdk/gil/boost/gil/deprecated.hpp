/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_DEPRECATED_HPP
#define GIL_DEPRECATED_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief Deprecated names
///        This file is provided as a courtesy to ease upgrading GIL client code.
///        Please make sure your code compiles when this file is not included. 
///
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 12, 2007
///
////////////////////////////////////////////////////////////////////////////////////////

#define planar_ptr          planar_pixel_iterator
#define planar_ref          planar_pixel_reference
#define membased_2d_locator byte_addressable_2d_locator
#define pixel_step_iterator byte_addressable_step_iterator
#define pixel_image_iterator iterator_from_2d

#define equal_channels      static_equal
#define copy_channels       static_copy
#define fill_channels       static_fill
#define generate_channels   static_generate
#define for_each_channel    static_for_each
#define transform_channels  static_transform
#define max_channel         static_max
#define min_channel         static_min

#define semantic_channel    semantic_at_c

template <typename Img>
void resize_clobber_image(Img& img, const typename Img::point_t& new_dims) {
    img.recreate(new_dims);
}

template <typename Img>
void resize_clobber_image(Img& img, const typename Img::x_coord_t& width, const typename Img::y_coord_t& height) {
    img.recreate(width,height);
}

template <typename T> typename T::x_coord_t get_width(const T& a) { return a.width(); }
template <typename T> typename T::y_coord_t get_height(const T& a) { return a.height(); }
template <typename T> typename T::point_t get_dimensions(const T& a) { return a.dimensions(); }
template <typename T> std::size_t get_num_channels(const T& a) { return a.num_channels(); }

#define GIL boost::gil
#define ADOBE_GIL_NAMESPACE_BEGIN  namespace boost { namespace gil {
#define ADOBE_GIL_NAMESPACE_END    } }


#endif
