/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_CHANNEL_HPP
#define GIL_CHANNEL_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief Channel utilities
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on September 18, 2006
///
/// Definitions of standard GIL channel models
///
////////////////////////////////////////////////////////////////////////////////////////

#include <limits>
#include <cassert>
#include <boost/cstdint.hpp>
#include "gil_config.hpp"
#include "utilities.hpp"

namespace boost { namespace gil {


///////////////////////////////////////////
////  channel_traits
////  
////  \ingroup ChannelModel
////  \class channel_traits
////  \brief defines properties of channels, such as their range and associated types
////
////  The channel traits must be defined for every model of ChannelConcept
////  Default traits are provided. For built-in types the default traits use
////  built-in pointer and reference and the channel range is the physical 
////  range of the type. For classes, the default traits forward the associated types
////  and range to the class.
//// 
///////////////////////////////////////////

namespace detail {
    template <typename T, bool is_class> struct channel_traits_impl;

    /// \brief channel traits for custom class
    /// \ingroup ChannelModel
    template <typename T> 
    struct channel_traits_impl<T, true> {
        typedef typename T::value_type      value_type;
        typedef typename T::reference       reference;
        typedef typename T::pointer         pointer;
        typedef typename T::const_reference const_reference;
        typedef typename T::const_pointer   const_pointer;
        BOOST_STATIC_CONSTANT(bool, is_mutable=T::is_mutable);
        static value_type min_value() { return T::min_value(); }
        static value_type max_value() { return T::max_value(); }
    };

    /// \brief channel traits implementation for built-in scalar or floating point channel type
    /// \ingroup ChannelModel
    template <typename T> 
    struct channel_traits_impl<T, false> {
        typedef T           value_type;
        typedef T&          reference;
        typedef T*          pointer;
        typedef const T&    const_reference;
        typedef T const*    const_pointer;
        BOOST_STATIC_CONSTANT(bool, is_mutable=true);
        static value_type min_value() { return std::numeric_limits<T>::min(); }
        static value_type max_value() { return std::numeric_limits<T>::max(); }
    };

    /// \brief channel traits implementation for constant built-in scalar or floating point type
    /// \ingroup ChannelModel
    template <typename T> 
    struct channel_traits_impl<const T, false> : public channel_traits_impl<T, false> {
        typedef const T&    reference;
        typedef const T*    pointer;
        BOOST_STATIC_CONSTANT(bool, is_mutable=false);
    };
}

template <typename T>
struct channel_traits : public detail::channel_traits_impl<T, is_class<T>::value> {};

/// \ingroup ChannelModel
/// \brief Channel traits for C++ reference type - remove the reference
template <typename T> struct channel_traits<      T&> : public channel_traits<T> {};

/// \ingroup ChannelModel
/// \brief Channel traits for constant C++ reference type
template <typename T> struct channel_traits<const T&> : public channel_traits<T> {
    typedef typename channel_traits<T>::const_reference reference;
    typedef typename channel_traits<T>::const_pointer   pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable=false);
};

///////////////////////////////////////////
////
////  scoped_channel_value
////
///////////////////////////////////////////

/// \defgroup ScopedChannelValue scoped_channel_value
/// \ingroup ChannelModel
/// \brief A channel adaptor that modifies the range of the source channel. Models: ChannelValueConcept

/// \ingroup ScopedChannelValue
/// \brief A channel adaptor that modifies the range of the source channel. Models: ChannelValueConcept
template <typename BaseChannelValue,        // base channel (models ChannelValueConcept)
          typename MinVal, typename MaxVal> // classes with a static apply() function returning the minimum/maximum channel values
struct scoped_channel_value {
    typedef scoped_channel_value    value_type;
    typedef value_type&             reference;
    typedef value_type*             pointer;
    typedef const value_type&       const_reference;
    typedef const value_type*       const_pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable=channel_traits<BaseChannelValue>::is_mutable);

    static value_type min_value() { return MinVal::apply(); }
    static value_type max_value() { return MaxVal::apply(); }

    scoped_channel_value() {}
    scoped_channel_value(const scoped_channel_value& c) : _value(c._value) {}
    scoped_channel_value(BaseChannelValue val) : _value(val) {}

    scoped_channel_value& operator++() { ++_value; return *this; }
    scoped_channel_value& operator--() { --_value; return *this; }

    scoped_channel_value operator++(int) { scoped_channel_value tmp=*this; this->operator++(); return tmp; }
    scoped_channel_value operator--(int) { scoped_channel_value tmp=*this; this->operator--(); return tmp; }

    template <typename Scalar2> scoped_channel_value& operator+=(Scalar2 v) { _value+=v; return *this; }
    template <typename Scalar2> scoped_channel_value& operator-=(Scalar2 v) { _value-=v; return *this; }
    template <typename Scalar2> scoped_channel_value& operator*=(Scalar2 v) { _value*=v; return *this; }
    template <typename Scalar2> scoped_channel_value& operator/=(Scalar2 v) { _value/=v; return *this; }

    scoped_channel_value& operator=(BaseChannelValue v) { _value=v; return *this; }
    operator BaseChannelValue() const { return _value; }
private:
    BaseChannelValue _value;
};

struct float_zero { static float apply() { return 0.0f; } };
struct float_one  { static float apply() { return 1.0f; } };


///////////////////////////////////////////
////
////  Support for sub-byte channels. These are integral channels whose value is contained in a range of bits inside an integral type
////
///////////////////////////////////////////

// It is necessary for packed channels to have their own value type. They cannot simply use an integral large enough to store the data. Here is why:
// - Any operation that requires returning the result by value will otherwise return the built-in integral type, which will have incorrect range
//   That means that after getting the value of the channel we cannot properly do channel_convert, channel_invert, etc.
// - Two channels are declared compatible if they have the same value type. That means that a packed channel is incorrectly declared compatible with an integral type
namespace detail {
    // returns the smallest fast unsigned integral type that has at least NumBits bits
    template <int NumBits>
    struct min_fast_uint : public mpl::if_c< (NumBits<8), 
            uint_least8_t, 
            typename mpl::if_c< (NumBits<16), uint_least16_t, 
                                                     uint_least32_t>::type > {};
}
/// \defgroup PackedChannelValueModel packed_channel_value
/// \ingroup ChannelModel
/// \brief Represents the value of an unsigned integral channel operating over a bit range. Models: ChannelValueConcept

/// \ingroup PackedChannelValueModel
/// \brief The value of a subbyte channel. Models: ChannelValueConcept
template <int NumBits>
class packed_channel_value {
    static const std::size_t num_values = 1<<NumBits;
public:
    typedef typename detail::min_fast_uint<NumBits>::type integer_t;

    typedef packed_channel_value   value_type;
    typedef value_type&            reference;
    typedef const value_type&      const_reference;
    typedef value_type*            pointer;
    typedef const value_type*      const_pointer;

    static value_type min_value() { return value_type(0); }
    static value_type max_value() { return value_type(num_values-1); }
    BOOST_STATIC_CONSTANT(bool, is_mutable=true);

    packed_channel_value() {}
    packed_channel_value(integer_t v) : _value(v % num_values) {}
    packed_channel_value(const packed_channel_value& v) : _value(v._value) {}
    template <typename Scalar> packed_channel_value(Scalar v) : _value(integer_t(v) % num_values) {}     // suppress GCC implicit conversion warnings in channel regression file 

    operator integer_t() const { return _value; }
private:
    integer_t _value;
};

namespace detail {
template <typename Derived, typename BitField, int NumBits, bool Mutable>
class packed_channel_reference_base {
protected:
    typedef typename mpl::if_c<Mutable,BitField&,const BitField&>::type data_ref_t;
    data_ref_t _data;

    static const BitField max_val    = (1<<NumBits) - 1;
public:
    typedef packed_channel_value<NumBits>   value_type;
    typedef const Derived                   reference;
    typedef value_type*                     pointer;
    typedef const value_type*               const_pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable=Mutable);

    static value_type min_value()       { return channel_traits<value_type>::min_value(); }
    static value_type max_value()       { return channel_traits<value_type>::max_value(); }

    typedef typename value_type::integer_t integer_t;

    packed_channel_reference_base(data_ref_t data) : _data(data) {}
    packed_channel_reference_base(const packed_channel_reference_base& ref) : _data(ref._data) {}
    const Derived& operator=(integer_t v) const { set(v); return derived(); }

    const Derived& operator++() const { set(get()+1); return derived(); }
    const Derived& operator--() const { set(get()-1); return derived(); }

    Derived operator++(int) const { Derived tmp=derived(); this->operator++(); return tmp; }
    Derived operator--(int) const { Derived tmp=derived(); this->operator--(); return tmp; }

    template <typename Scalar2> const Derived& operator+=(Scalar2 v) const { set(get()+v); return derived(); }
    template <typename Scalar2> const Derived& operator-=(Scalar2 v) const { set(get()-v); return derived(); }
    template <typename Scalar2> const Derived& operator*=(Scalar2 v) const { set(get()*v); return derived(); }
    template <typename Scalar2> const Derived& operator/=(Scalar2 v) const { set(get()/v); return derived(); }

    operator integer_t() const { return get(); }
    BitField* operator &() const {return &_data;}
private:
    void set(integer_t value) const {     // can this be done faster??
        const integer_t num_values = max_val+1;
        this->derived().set_unsafe(((value % num_values) + num_values) % num_values); 
    }
    integer_t get() const { return derived().get(); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
}   // namespace detail

/// \defgroup PackedChannelReferenceModel packed_channel_reference
/// \ingroup ChannelModel
/// \brief Represents a reference proxy to a channel operating over a bit range whose offset is fixed at compile time. Models ChannelConcept

template <typename BitField,       // An integral type (the data type containing the bits of interest). Models unsigned integer
          int FirstBit, int NumBits,// Defines the sequence of bits in the data value that contain the channel 
          bool Mutable>             // true if the reference is mutable 
class packed_channel_reference;

/// \ingroup PackedChannelReferenceModel
/// \brief A constant subbyte channel reference whose bit offset is fixed at compile time. Models ChannelConcept
template <typename BitField, int FirstBit, int NumBits>
class packed_channel_reference<BitField,FirstBit,NumBits,false> 
   : public detail::packed_channel_reference_base<packed_channel_reference<BitField,FirstBit,NumBits,false>,BitField,NumBits,false> {
    typedef detail::packed_channel_reference_base<packed_channel_reference<BitField,FirstBit,NumBits,false>,BitField,NumBits,false> parent_t;
    static const BitField channel_mask = parent_t::max_val<<FirstBit;
    typedef typename parent_t::integer_t  integer_t;

    typedef packed_channel_reference<BitField,FirstBit,NumBits,true> mutable_reference;
    friend class packed_channel_reference<BitField,FirstBit,NumBits,true>;
public:
    typedef const packed_channel_reference const_reference;

    explicit packed_channel_reference(const BitField& data) : parent_t(data) {}
    packed_channel_reference(const packed_channel_reference& ref) : parent_t(ref._data) {}
    packed_channel_reference(const mutable_reference& ref) : parent_t(ref._data) {}

    integer_t get() const { return (this->_data&(parent_t::max_val<<FirstBit)) >> FirstBit; }
};

/// \ingroup PackedChannelReferenceModel
/// \brief A mutable subbyte channel reference whose bit offset is fixed at compile time. Models ChannelConcept
template <typename BitField, int FirstBit, int NumBits>
class packed_channel_reference<BitField,FirstBit,NumBits,true> 
   : public detail::packed_channel_reference_base<packed_channel_reference<BitField,FirstBit,NumBits,true>,BitField,NumBits,true> {
    typedef detail::packed_channel_reference_base<packed_channel_reference<BitField,FirstBit,NumBits,true>,BitField,NumBits,true> parent_t;
    static const BitField channel_mask = parent_t::max_val<<FirstBit;

    typedef typename parent_t::integer_t  integer_t;
    friend class packed_channel_reference<BitField,FirstBit,NumBits,false>;
public:
    typedef packed_channel_reference<BitField,FirstBit,NumBits,false> const_reference;

    explicit packed_channel_reference(BitField& data) : parent_t(data) {}
    packed_channel_reference(const packed_channel_reference& ref) : parent_t(ref._data) {}

    const packed_channel_reference& operator=(integer_t value) const { assert(value<=parent_t::max_val); set_unsafe(value); return *this; }
    const packed_channel_reference& operator=(const packed_channel_reference& ref) const { set_from_reference(ref._data); return *this; }
    const packed_channel_reference& operator=(const const_reference& ref)          const { set_from_reference(ref._data); return *this; }

    integer_t get() const { return (this->_data&channel_mask) >> FirstBit; }
    void set_unsafe(integer_t value)              const { this->_data = (this->_data & ~channel_mask) | (value<<FirstBit); }
private:
    void set_from_reference(const BitField& data) const { this->_data = (this->_data & ~channel_mask) | (data & channel_mask); }
};

} }  // namespace boost::gil

namespace std {
// We are forced to define swap inside std namespace because on some platforms (Visual Studio 8) STL calls swap qualified.
// swap with 'left bias': 
// - swap between proxy and anything
// - swap between value type and proxy
// - swap between proxy and proxy

/// \ingroup PackedChannelReferenceModel
/// \brief swap for packed_channel_reference
template <typename BF, int FB, int NB, bool M, typename R> inline
void swap(boost::gil::packed_channel_reference<BF,FB,NB,M> x, R& y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_channel_reference<BF,FB,NB,M>::value_type>(x,y); 
}


/// \ingroup PackedChannelReferenceModel
/// \brief swap for packed_channel_reference
template <typename BF, int FB, int NB, bool M> inline
void swap(typename boost::gil::packed_channel_reference<BF,FB,NB,M>::value_type& x, boost::gil::packed_channel_reference<BF,FB,NB,M> y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_channel_reference<BF,FB,NB,M>::value_type>(x,y); 
}


/// \ingroup PackedChannelReferenceModel
/// \brief swap for packed_channel_reference
template <typename BF, int FB, int NB, bool M> inline
void swap(boost::gil::packed_channel_reference<BF,FB,NB,M> x, boost::gil::packed_channel_reference<BF,FB,NB,M> y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_channel_reference<BF,FB,NB,M>::value_type>(x,y); 
}
}   // namespace std

namespace boost { namespace gil {



/// \defgroup PackedChannelDynamicReferenceModel packed_dynamic_channel_reference
/// \ingroup ChannelModel
/// \brief Represents a reference proxy to a channel operating over a bit range whose offset is specified at run time. Models ChannelConcept

template <typename BitField,       // An integral type (the data type containing the bits of interest). Models unsigned integer
          int NumBits,              // Defines the sequence of bits in the data value that contain the channel 
          bool Mutable>             // true if the reference is mutable 
class packed_dynamic_channel_reference;

/// \brief Models a constant subbyte channel reference whose bit offset is a runtime parameter. Models ChannelConcept
///        Same as packed_channel_reference, except that the offset is a runtime parameter
/// \ingroup PackedChannelDynamicReferenceModel
template <typename BitField, int NumBits> 
class packed_dynamic_channel_reference<BitField,NumBits,false>
   : public detail::packed_channel_reference_base<packed_dynamic_channel_reference<BitField,NumBits,false>,BitField,NumBits,false> {
    typedef detail::packed_channel_reference_base<packed_dynamic_channel_reference<BitField,NumBits,false>,BitField,NumBits,false> parent_t;
    int _first_bit;

    typedef packed_dynamic_channel_reference<BitField,NumBits,true>   mutable_reference;
    typedef typename parent_t::integer_t  integer_t;
public:
    typedef packed_dynamic_channel_reference<BitField,NumBits,false>   const_reference;

    packed_dynamic_channel_reference(typename parent_t::data_ref_t data, int first_bit) : parent_t(data), _first_bit(first_bit) {}
    packed_dynamic_channel_reference(const packed_dynamic_channel_reference& ref) : parent_t(ref._data), _first_bit(ref._first_bit) {}
    packed_dynamic_channel_reference(const mutable_reference& ref) : parent_t(ref._data), _first_bit(ref._first_bit) {}

    integer_t get() const { 
        const BitField channel_mask = parent_t::max_val<<_first_bit;
        return (this->_data&channel_mask) >> _first_bit; 
    }
};

/// \brief Models a mutable subbyte channel reference whose bit offset is a runtime parameter. Models ChannelConcept
///        Same as packed_channel_reference, except that the offset is a runtime parameter
/// \ingroup PackedChannelDynamicReferenceModel
template <typename BitField, int NumBits> 
class packed_dynamic_channel_reference<BitField,NumBits,true>
   : public detail::packed_channel_reference_base<packed_dynamic_channel_reference<BitField,NumBits,true>,BitField,NumBits,true> {
    typedef detail::packed_channel_reference_base<packed_dynamic_channel_reference<BitField,NumBits,true>,BitField,NumBits,true> parent_t;
    int _first_bit;

    typedef typename parent_t::integer_t  integer_t;
public:
    typedef packed_dynamic_channel_reference<BitField,NumBits,false>   const_reference;

    packed_dynamic_channel_reference(typename parent_t::data_ref_t data, int first_bit) : parent_t(data), _first_bit(first_bit) {}
    packed_dynamic_channel_reference(const packed_dynamic_channel_reference& ref) : parent_t(ref._data), _first_bit(ref._first_bit) {}

    const packed_dynamic_channel_reference& operator=(integer_t value) const { assert(value<=parent_t::max_val); set_unsafe(value); return *this; }
    const packed_dynamic_channel_reference& operator=(const packed_dynamic_channel_reference& ref) const {  set_unsafe(ref.get()); return *this; }
    const packed_dynamic_channel_reference& operator=(const const_reference& ref)                  const {  set_unsafe(ref.get()); return *this; }

    integer_t get() const { 
        const BitField channel_mask = parent_t::max_val<<_first_bit;
        return (this->_data&channel_mask) >> _first_bit; 
    }
    void set_unsafe(integer_t value) const { 
        const BitField channel_mask = parent_t::max_val<<_first_bit;
        this->_data = (this->_data & ~channel_mask) | value<<_first_bit; 
    }
};
} }  // namespace boost::gil

namespace std {
// We are forced to define swap inside std namespace because on some platforms (Visual Studio 8) STL calls swap qualified.
// swap with 'left bias': 
// - swap between proxy and anything
// - swap between value type and proxy
// - swap between proxy and proxy


/// \ingroup PackedChannelDynamicReferenceModel
/// \brief swap for packed_dynamic_channel_reference
template <typename BF, int NB, bool M, typename R> inline
void swap(boost::gil::packed_dynamic_channel_reference<BF,NB,M> x, R& y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_dynamic_channel_reference<BF,NB,M>::value_type>(x,y); 
}


/// \ingroup PackedChannelDynamicReferenceModel
/// \brief swap for packed_dynamic_channel_reference
template <typename BF, int NB, bool M> inline
void swap(typename boost::gil::packed_dynamic_channel_reference<BF,NB,M>::value_type& x, boost::gil::packed_dynamic_channel_reference<BF,NB,M> y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_dynamic_channel_reference<BF,NB,M>::value_type>(x,y); 
}


/// \ingroup PackedChannelDynamicReferenceModel
/// \brief swap for packed_dynamic_channel_reference
template <typename BF, int NB, bool M> inline
void swap(boost::gil::packed_dynamic_channel_reference<BF,NB,M> x, boost::gil::packed_dynamic_channel_reference<BF,NB,M> y) { 
    boost::gil::swap_proxy<typename boost::gil::packed_dynamic_channel_reference<BF,NB,M>::value_type>(x,y); 
}
}   // namespace std

namespace boost { namespace gil {
///////////////////////////////////////////
////
////  Built-in channel models
////
///////////////////////////////////////////

/// \defgroup bits8 bits8
/// \ingroup ChannelModel
/// \brief 8-bit unsigned integral channel type. Models ChannelValueConcept

/// \ingroup bits8
typedef uint8_t  bits8;

/// \defgroup bits16 bits16
/// \ingroup ChannelModel
/// \brief 16-bit unsigned integral channel type. Models ChannelValueConcept

/// \ingroup bits16
typedef uint16_t bits16;

/// \defgroup bits32 bits32
/// \ingroup ChannelModel
/// \brief 32-bit unsigned integral channel type. Models ChannelValueConcept

/// \ingroup bits32
typedef uint32_t bits32;

/// \defgroup bits8s bits8s
/// \ingroup ChannelModel
/// \brief 8-bit signed integral channel type. Models ChannelValueConcept

/// \ingroup bits8s
typedef int8_t   bits8s;

/// \defgroup bits16s bits16s
/// \ingroup ChannelModel
/// \brief 16-bit signed integral channel type. Models ChannelValueConcept

/// \ingroup bits16s
typedef int16_t  bits16s;

/// \defgroup bits32s bits32s
/// \ingroup ChannelModel
/// \brief 32-bit signed integral channel type. Models ChannelValueConcept

/// \ingroup bits32s
typedef int32_t  bits32s;

/// \defgroup bits32f bits32f
/// \ingroup ChannelModel
/// \brief 32-bit floating point channel type with range [0.0f ... 1.0f]. Models ChannelValueConcept

/// \ingroup bits32f
typedef scoped_channel_value<float,float_zero,float_one> bits32f;

} }  // namespace boost::gil


#endif
