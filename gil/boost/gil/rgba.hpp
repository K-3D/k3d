/*
  Copyright 2005-2007 Adobe Systems Incorporated
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_RGBA_H
#define GIL_RGBA_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Support for RGBA color space and variants
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 12, 2007
////////////////////////////////////////////////////////////////////////////////////////

#include "gil_config.hpp"
#include <boost/mpl/contains.hpp>
#include "rgb.hpp"
#include "planar_pixel_iterator.hpp"

namespace boost { namespace gil {

/// \ingroup ColorNameModel
/// \brief Alpha
struct alpha_t {};    

/// \ingroup ColorSpaceModel
typedef mpl::vector4<red_t,green_t,blue_t,alpha_t> rgba_t;

/// \ingroup LayoutModel
typedef layout<rgba_t> rgba_layout_t;
/// \ingroup LayoutModel
typedef layout<rgba_t, mpl::vector4_c<int,2,1,0,3> > bgra_layout_t;
/// \ingroup LayoutModel
typedef layout<rgba_t, mpl::vector4_c<int,1,2,3,0> > argb_layout_t;
/// \ingroup LayoutModel
typedef layout<rgba_t, mpl::vector4_c<int,3,2,1,0> > abgr_layout_t;

/// \ingroup ImageViewConstructors
/// \brief from raw RGBA planar data
template <typename IC>
inline
typename type_from_x_iterator<planar_pixel_iterator<IC,rgba_t> >::view_t
planar_rgba_view(int width, int height,
                 IC r, IC g, IC b, IC a,
                 std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,rgba_t> >::view_t RVIEW;
    return RVIEW(width, height,
                 typename RVIEW::locator(planar_pixel_iterator<IC,rgba_t>(r,g,b,a),
                                         rowsize_in_bytes));
}

} }  // namespace boost::gil

#endif
