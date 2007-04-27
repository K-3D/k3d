/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_CHANNEL_ALGORITHM_HPP
#define GIL_CHANNEL_ALGORITHM_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief Channel algorithms
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on September 18, 2006
///
/// Definitions of standard GIL 8-bit, 16-bit, 32-bit channels
///
////////////////////////////////////////////////////////////////////////////////////////

#include "gil_config.hpp"
#include "channel.hpp"

namespace boost { namespace gil {

/**
\defgroup ChannelConvertAlgorithm channel_convert
\brief Converting from one channel type to another
\ingroup ChannelAlgorithm

Conversion is done as a simple linear mapping of one channel range to the other, 
such that the minimum/maximum value of the source maps to the minimum/maximum value of the destination.
One implication of this is that the value 0 of signed channels may not be preserved!

When creating new channel models, it is often a good idea to provide specializations for the channel conversion algorithms, for
example, for performance optimizations. If the new model is an integral type that can be signed, it is easier to define the conversion 
only for the unsigned type (\p channel_converter_unsigned) and provide specializations of \p channel_convert_to_unsigned and \p channel_convert_to_signed
to convert between the signed and unsigned type.
*/

/** 
\defgroup ChannelConvertUnsignedAlgorithm channel_converter_unsigned
\ingroup ChannelConvertAlgorithm
\brief Convert one unsigned/floating point channel to another. Converts both the channel type and range
 @{
 */

/// \brief This is the default implementation. Performance specializatons are provided
template <typename SrcChannelV, typename DstChannelV>     // Model ChannelValueConcept
struct channel_converter_unsigned : public std::unary_function<DstChannelV,SrcChannelV> {
    DstChannelV operator()(SrcChannelV src) const { 
        return DstChannelV(src / channel_range<SrcChannelV>() * channel_range<DstChannelV>()); 
    }
private:
    template <typename C>
    static double channel_range() {
        return double(channel_traits<C>::max_value()) - double(channel_traits<C>::min_value());
    }
};

/// \brief Converting a channel to itself - identity operation
template <typename T> struct channel_converter_unsigned<T,T> : public detail::identity<T> {};

/// \brief 8 bit <-> 16 bit channel conversion
template <> struct channel_converter_unsigned<bits16,bits8> : public std::unary_function<bits16,bits8> {
    bits8  operator()(bits16 x) const { return static_cast<bits8>(x/257); }
};
/// \brief 8 bit <-> 16 bit channel conversion
template <> struct channel_converter_unsigned<bits8,bits16> : public std::unary_function<bits8,bits16> {
    bits16 operator()(bits8  x) const { return static_cast<bits16>(x*257); }
};

/// \brief 8 bit <-> 32 bit channel conversion
template <> struct channel_converter_unsigned<bits32,bits8> : public std::unary_function<bits32,bits8> {
    bits8 operator()(bits32 x) const { return static_cast<bits8>(x/16843009); }
};
/// \brief 8 bit <-> 32 bit channel conversion
template <> struct channel_converter_unsigned<bits8,bits32> : public std::unary_function<bits8,bits32> {
    bits32 operator()(bits8 x) const { return static_cast<bits32>(x*16843009); }
};

/// \brief 8 bit <-> float0..1 channel conversion
template <> struct channel_converter_unsigned<bits32f,bits8> : public std::unary_function<bits32f,bits8> {
    bits8   operator()(bits32f x) const { return static_cast<bits8>(x*255+0.5f); }
};
/// \brief 8 bit <-> float0..1 channel conversion
template <> struct channel_converter_unsigned<bits8,bits32f> : public std::unary_function<bits8,bits32f> {
    bits32f operator()(bits8   x) const { return static_cast<bits32f>(x/255.0f); }
};

/// \brief 16 bit <-> float0..1 channel conversion
template <> struct channel_converter_unsigned<bits32f,bits16> : public std::unary_function<bits32f,bits16> {
    bits16  operator()(bits32f x) const { return static_cast<bits16>(x*65535+0.5f); }
};
/// \brief 16 bit <-> float0..1 channel conversion
template <> struct channel_converter_unsigned<bits16,bits32f> : public std::unary_function<bits16,bits32f> {
    bits32f operator()(bits16  x) const { return static_cast<bits32f>(x/65535.0f); }
};

/// \brief 16 bit <-> 32 bit channel conversion
template <> struct channel_converter_unsigned<bits32,bits16> : public std::unary_function<bits32,bits16> {
    bits16 operator()(bits32 x) const { return static_cast<bits16>(x/65537); }
};
/// \brief 16 bit <-> 32 bit channel conversion
template <> struct channel_converter_unsigned<bits16,bits32> : public std::unary_function<bits16,bits32> {
    bits32 operator()(bits16 x) const { return static_cast<bits32>(x*65537); }
};

/// \brief 32 bit <-> float channel conversion
template <> struct channel_converter_unsigned<bits32,bits32f> : public std::unary_function<bits32,bits32f> {
    bits32f operator()(bits32 x) const { 
        // unfortunately without an explicit check it is possible to get a round-off error. We must ensure that max_value of bits32 matches max_value of bits32f
        if (x>=channel_traits<bits32>::max_value()) return channel_traits<bits32f>::max_value();
        return float(x) / float(channel_traits<bits32>::max_value());
    }
};
/// \brief 32 bit <-> float channel conversion
template <> struct channel_converter_unsigned<bits32f,bits32> : public std::unary_function<bits32f,bits32> {
    bits32 operator()(bits32f x) const { 
        // unfortunately without an explicit check it is possible to get a round-off error. We must ensure that max_value of bits32 matches max_value of bits32f
        if (x>=channel_traits<bits32f>::max_value()) return channel_traits<bits32>::max_value();
        return bits32(x * channel_traits<bits32>::max_value() + 0.5f); 
    }
};
/// @} 

/** 
\defgroup ChannelConvertToUnsignedAlgorithm channel_convert_to_unsigned
\brief Convert signed channel to unsigned. By default it is an identity operation. Specializations are provided for signed integral channels
\ingroup ChannelConvertAlgorithm
 
@{
*/

/// \brief Converting from signed to unsigned integral channel. 
/// It is both a unary function, and a metafunction (thus requires the 'type' nested typedef, which equals result_type)
template <typename ChannelValue>     // Model ChannelValueConcept
struct channel_convert_to_unsigned : public detail::identity<ChannelValue> {
    typedef ChannelValue type;
};

/// \brief Converting from  8-bit signed to 8-bit unsigned integral channel
template <> struct channel_convert_to_unsigned<bits8s> : public std::unary_function<bits8s,bits8> { 
    typedef bits8 type;
    type operator()(bits8s  val) const { return val+128; } 
};

/// \brief Converting from 16-bit signed to 16-bit unsigned integral channel
template <> struct channel_convert_to_unsigned<bits16s> : public std::unary_function<bits16s,bits16> { 
    typedef bits16 type;
    type operator()(bits16s  val) const { return val+32768; } 
};

/// \brief Converting from 32-bit signed to 32-bit unsigned integral channel
template <> struct channel_convert_to_unsigned<bits32s> : public std::unary_function<bits32s,bits32> {
    typedef bits32 type;
    type operator()(bits32s x) const { return static_cast<bits32>(x+(1<<31)); }
};
/// @} 

/** 
\defgroup ChannelConvertToSignedAlgorithm channel_convert_to_signed
\brief Convert unsigned channel to signed. By default it is an identity operation. Specializations are provided for signed integral channels
\ingroup ChannelConvertAlgorithm
 
@{
*/

/// \brief Converting from unsigned to signed integral channel
/// It is both a unary function, and a metafunction (thus requires the 'type' nested typedef, which equals result_type)
template <typename ChannelValue>     // Model ChannelValueConcept
struct channel_convert_to_signed : public detail::identity<ChannelValue> {
    typedef ChannelValue type;
};

/// \brief Converting from 8-bit unsigned to 8-bit signed integral channel
template <> struct channel_convert_to_signed<bits8s> : public std::unary_function<bits8,bits8s> { 
    typedef bits8s type;
    type  operator()(bits8  val) const { return val-128; } 
};

/// \brief Converting from 16-bit unsigned to 16-bit signed integral channel
template <> struct channel_convert_to_signed<bits16s> : public std::unary_function<bits16,bits16s> { 
    typedef bits16s type;
    type operator()(bits16 val) const { return val-32768; } 
};

/// \brief Converting from 32-bit unsigned to 32-bit signed integral channel
template <> struct channel_convert_to_signed<bits32s> : public std::unary_function<bits32,bits32s> {
    typedef bits32s type;
    type operator()(bits32 x) const { return static_cast<bits32s>(x-(1<<31)); }
};
/// @} 

/// \ingroup ChannelConvertAlgorithm
/// \brief A unary function object converting between channel types
template <typename SrcChannelV, typename DstChannelV> // Model ChannelValueConcept
struct channel_converter : public std::unary_function<SrcChannelV,DstChannelV> {
    DstChannelV operator()(SrcChannelV src) const {
        typedef channel_convert_to_unsigned<SrcChannelV> to_unsigned;
        typedef channel_convert_to_signed<DstChannelV>   to_signed;
        typedef channel_converter_unsigned<typename to_unsigned::result_type, typename to_signed::argument_type> converter_unsigned;
        return to_signed()(converter_unsigned()(to_unsigned()(src))); 
    }
};

/// \ingroup ChannelConvertAlgorithm
/// \brief Converting from one channel type to another.
template <typename DstChannel, typename SrcChannel> // Model ChannelConcept (could be channel references)
inline typename channel_traits<DstChannel>::value_type channel_convert(SrcChannel src) {
    return channel_converter<typename channel_traits<SrcChannel>::value_type,
                             typename channel_traits<DstChannel>::value_type>()(src); 
}

/// \ingroup ChannelConvertAlgorithm
/// \brief Same as channel_converter, except it takes the destination channel by reference, which allows 
///        us to move the templates from the class level to the method level. This is important when invoking it
///        on heterogeneous pixels.
struct default_channel_converter {
    template <typename Ch1, typename Ch2>
    void operator()(const Ch1& src, Ch2& dst) const {
        dst=channel_convert<Ch2>(src);
    }
};

namespace detail {
    // fast integer division by 255
    inline uint32_t div255(uint32_t in) { uint32_t tmp=in+128; return (tmp + (tmp>>8))>>8; }

    // fast integer divison by 32768
    inline uint32_t div32768(uint32_t in) { return (in+16384)>>15; }
}

/// \defgroup ChannelMultiplyAlgorithm channel_multiply
/// \ingroup ChannelAlgorithm
/// \brief Multiplying unsigned channel values of the same type. Performs scaled multiplication result = a * b / max_value
/// @{

/// \brief This is the default implementation. Performance specializatons are provided
template <typename ChannelValue>
struct channel_multiplier_unsigned : public std::binary_function<ChannelValue,ChannelValue,ChannelValue> {
    ChannelValue operator()(ChannelValue a, ChannelValue b) const {
        return ChannelValue(a / double(channel_traits<ChannelValue>::max_value()) * b);
    }
};

/// \brief Specialization of channel_multiply for 8-bit unsigned channels
template<> struct channel_multiplier_unsigned<bits8> : public std::binary_function<bits8,bits8,bits8> {
    bits8 operator()(bits8 a, bits8 b) const { return bits8(detail::div255(uint32_t(a) * uint32_t(b))); }
};

/// \brief Specialization of channel_multiply for 16-bit unsigned channels
template<> struct channel_multiplier_unsigned<bits16> : public std::binary_function<bits16,bits16,bits16> {
    bits16 operator()(bits16 a, bits16 b) const { return bits16((uint32_t(a) * uint32_t(b))/65535); }
};

/// \brief Specialization of channel_multiply for float 0..1 channels
template<> struct channel_multiplier_unsigned<bits32f> : public std::binary_function<bits32f,bits32f,bits32f> {
    bits32f operator()(bits32f a, bits32f b) const { return a*b; }
};

/// \brief A function object to multiply two channels. result = a * b / max_value
template <typename ChannelValue>
struct channel_multiplier : public std::binary_function<ChannelValue, ChannelValue, ChannelValue> {
    ChannelValue operator()(ChannelValue a, ChannelValue b) const {
        typedef channel_convert_to_unsigned<ChannelValue> to_unsigned;
        typedef channel_convert_to_signed<ChannelValue>   to_signed;
        typedef channel_multiplier_unsigned<typename to_unsigned::result_type> multiplier_unsigned;
        return to_signed()(multiplier_unsigned()(to_unsigned()(a), to_unsigned()(b))); 
    }
};

/// \brief A function multiplying two channels. result = a * b / max_value
template <typename Channel> // Models ChannelConcept (could be a channel reference)
inline typename channel_traits<Channel>::value_type channel_multiply(Channel a, Channel b) { 
    return channel_multiplier<typename channel_traits<Channel>::value_type>()(a,b);
}
/// @} 

/// \defgroup ChannelInvertAlgorithm channel_invert
/// \ingroup ChannelAlgorithm
/// \brief Returns the inverse of a channel. result = max_value - x + min_value

/// \brief Default implementation. Provide overloads for performance
/// \ingroup ChannelInvertAlgorithm channel_invert
template <typename Channel> // Models ChannelConcept (could be a channel reference)
inline typename channel_traits<Channel>::value_type channel_invert(Channel x) { 
    return channel_traits<Channel>::max_value()-x + channel_traits<Channel>::min_value(); 
}

} }  // namespace boost::gil

#endif
