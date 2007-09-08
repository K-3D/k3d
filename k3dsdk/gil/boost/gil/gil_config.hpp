/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_CONFIG_HPP
#define GIL_CONFIG_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief GIL configuration file
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
///
////////////////////////////////////////////////////////////////////////////////////////

#define GIL_VERSION 2.0

#ifdef _DEBUG
#    define GIL_FORCEINLINE inline
#else
#ifdef NDEBUG
#if   defined(_MSC_VER)
#    define GIL_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#    define GIL_FORCEINLINE inline __attribute__ ((always_inline))
#else
#    define GIL_FORCEINLINE inline
#endif
#else
#    define GIL_FORCEINLINE inline
#endif
#endif


#endif
