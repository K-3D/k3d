/*
  Copyright 2005-2007 Adobe Systems Incorporated
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_GRAY_H
#define GIL_GRAY_H

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Support for grayscale color space and variants
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on March 8, 2006
////////////////////////////////////////////////////////////////////////////////////////

#include "gil_config.hpp"
#include "utilities.hpp"
#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

namespace boost { namespace gil {

/// \ingroup ColorNameModel
/// \brief Gray
struct gray_color_t {};

/// \ingroup ColorSpaceModel
typedef mpl::vector1<gray_color_t> gray_t;

/// \ingroup LayoutModel
typedef layout<gray_t> gray_layout_t;

} }  // namespace boost::gil

#endif

