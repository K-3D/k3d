/*
  Copyright 2005-2007 Adobe Systems Incorporated
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_CMYK_H
#define GIL_CMYK_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Support for CMYK color space and variants
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on September 18, 2006
////////////////////////////////////////////////////////////////////////////////////////

#include "gil_config.hpp"
#include "metafunctions.hpp"
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

namespace boost { namespace gil {


/// \addtogroup ColorNameModel
/// \{

/// \brief Cyan
struct cyan_t {};    

/// \brief Magenta
struct magenta_t {};

/// \brief Yellow
struct yellow_t {}; 

/// \brief Black
struct black_t {};
/// \}

/// \ingroup ColorSpaceModel
typedef mpl::vector4<cyan_t,magenta_t,yellow_t,black_t>  cmyk_t;

/// \ingroup LayoutModel
typedef layout<cmyk_t> cmyk_layout_t;

/// \ingroup ImageViewConstructors
/// \brief from raw CMYK planar data
template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,cmyk_t> >::view_t planar_cmyk_view(int width, int height, IC c, IC m, IC y, IC k, std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,cmyk_t> >::view_t RVIEW;
    return RVIEW(width, height, typename RVIEW::locator(planar_pixel_iterator<IC,cmyk_t>(c,m,y,k), rowsize_in_bytes));
}

} }  // namespace gil

#endif
