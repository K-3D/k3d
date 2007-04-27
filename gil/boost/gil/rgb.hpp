/*
  Copyright 2005-2007 Adobe Systems Incorporated
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_RGB_H
#define GIL_RGB_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Support for RGB color space and variants
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on September 18, 2006
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>
#include "gil_config.hpp"
#include "metafunctions.hpp"
#include "planar_pixel_iterator.hpp"

namespace boost { namespace gil {

/// \addtogroup ColorNameModel
/// \{

/// \brief Red
struct red_t {};    

/// \brief Green
struct green_t {};

/// \brief Blue
struct blue_t {}; 
/// \}

/// \ingroup ColorSpaceModel
typedef mpl::vector3<red_t,green_t,blue_t> rgb_t;

/// \ingroup LayoutModel
typedef layout<rgb_t> rgb_layout_t;
/// \ingroup LayoutModel
typedef layout<rgb_t, mpl::vector3_c<int,2,1,0> > bgr_layout_t;

/// \ingroup ImageViewConstructors
/// \brief from raw RGB planar data
template <typename IC>
inline
typename type_from_x_iterator<planar_pixel_iterator<IC,rgb_t> >::view_t
planar_rgb_view(int width, int height,
                IC r, IC g, IC b,
                std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,rgb_t> >::view_t RVIEW;
    return RVIEW(width, height,
                 typename RVIEW::locator(planar_pixel_iterator<IC,rgb_t>(r,g,b),
                                         rowsize_in_bytes));
}

} }  // namespace boost::gil

#endif
