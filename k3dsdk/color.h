#ifndef K3DSDK_COLOR_H
#define K3DSDK_COLOR_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "almost_equal.h"
#include "basic_math.h"
#include "Half/half.h"
#include "result.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/io/ios_state.hpp>


namespace k3d
{

template<typename, typename>
class basic_alpha;
	
template<typename, typename>
class basic_luma;

template<typename, typename>
class basic_luma_alpha;

template<typename, typename>
class basic_rgb;

template<typename, typename>
class basic_rgba;

/////////////////////////////////////////////////////////////////////////////
// color_traits

/// Describes traits of a type used as a sample within a color specification
template<typename SampleType>
class color_traits
{
public:
	typedef SampleType sample_type;

	/// Returns the maximum value of a sample
	static sample_type minimum();
	/// Returns the minimum value of a sample
	static sample_type maximum();
	/// Returns the sample value corresponding to transparent alpha
	static sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static sample_type invert(const sample_type& Sample);
};

/////////////////////////////////////////////////////////////////////////////
// color_traits<boost::uint_t>

/// Specialization of color_traits for 8 bit samples
template<>
class color_traits<boost::uint8_t>
{
public:
	/// Defines the sample type for this specialization
	typedef boost::uint8_t sample_type;

	/// Returns the minimum value of a sample
	static inline sample_type minimum() { return 0; }
	/// Returns the maximum value of a sample
	static inline sample_type maximum() { return 255; }
	/// Returns the sample value corresponding to transparent alpha
	static inline sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static inline sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static inline sample_type invert(const sample_type& Sample) { return maximum() - Sample; }

	/// Converts a sample from a uint8_t
	static inline sample_type convert(const sample_type Sample) { return Sample; }
	/// Converts a sample from a half
	static inline sample_type convert(const half Sample) { return static_cast<sample_type>(clamp(Sample, half(0), half(1)) * maximum()); }
	/// Converts a sample from a float
	static inline sample_type convert(const float Sample) { return static_cast<sample_type>(clamp(Sample, float(0), float(1)) * maximum()); }
	/// Converts a sample from a double
	static inline sample_type convert(const double_t Sample) { return static_cast<sample_type>(clamp(Sample, double_t(0), double_t(1)) * maximum()); }
};

/////////////////////////////////////////////////////////////////////////////
// color_traits<boost::uint16_t>

/// Specialization of color_traits for 16 bit samples
template<>
class color_traits<boost::uint16_t>
{
public:
	/// Defines the sample type for this specialization
	typedef boost::uint16_t sample_type;

	/// Returns the minimum value of a sample
	static inline sample_type minimum() { return 0; }
	/// Returns the maximum value of a sample
	static inline sample_type maximum() { return 65535; }
	/// Returns the sample value corresponding to transparent alpha
	static inline sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static inline sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static inline sample_type invert(const sample_type& Sample) { return maximum() - Sample; }

	/// Converts a sample from a uint16_t
	static inline sample_type convert(const sample_type Sample) { return Sample; }
	/// Converts a sample from a half
	static inline sample_type convert(const half Sample) { return static_cast<sample_type>(clamp(Sample, half(0), half(1)) * maximum()); }
	/// Converts a sample from a float
	static inline sample_type convert(const float Sample) { return static_cast<sample_type>(clamp(Sample, float(0), float(1)) * maximum()); }
	/// Converts a sample from a double
	static inline sample_type convert(const double_t Sample) { return static_cast<sample_type>(clamp(Sample, double_t(0), double_t(1)) * maximum()); }
};

/////////////////////////////////////////////////////////////////////////////
// color_traits<half>

/// Specialization of color_traits for half samples
template<>
class color_traits<half>
{
public:
	/// Defines the sample type for this specialization
	typedef half sample_type;

	/// Returns the minimum value of a sample
	static inline sample_type minimum() { return 0; }
	/// Returns the maximum value of a sample
	static inline sample_type maximum() { return 1; }
	/// Returns the sample value corresponding to transparent alpha
	static inline sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static inline sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static inline sample_type invert(const sample_type& Sample) { return maximum() - Sample; }

	/// Converts a sample from a uint8_t
	static inline sample_type convert(const boost::uint8_t Sample) { return static_cast<sample_type>(Sample) / 255.0f; }
	/// Converts a sample from a uint16_t
	static inline sample_type convert(const boost::uint16_t Sample) { return static_cast<sample_type>(Sample) / 65535.0f; }
	/// Converts a sample from a half
	static inline sample_type convert(const sample_type Sample) { return Sample; }
	/// Converts a sample from a float
	static inline sample_type convert(const float Sample) { return static_cast<sample_type>(Sample); }
	/// Converts a sample from a double
	static inline sample_type convert(const double_t Sample) { return static_cast<sample_type>(static_cast<float>(Sample)); }
};

/////////////////////////////////////////////////////////////////////////////
// color_traits<float>

/// Specialization of color_traits for float samples
template<>
class color_traits<float>
{
public:
	/// Defines the sample type for this specialization
	typedef float sample_type;

	/// Returns the minimum value of a sample
	static inline sample_type minimum() { return 0; }
	/// Returns the maximum value of a sample
	static inline sample_type maximum() { return 1; }
	/// Returns the sample value corresponding to transparent alpha
	static inline sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static inline sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static inline sample_type invert(const sample_type& Sample) { return maximum() - Sample; }

	/// Converts a sample from a uint8_t
	static inline sample_type convert(const boost::uint8_t Sample) { return static_cast<sample_type>(Sample) / 255.0f; }
	/// Converts a sample from a half
	static inline sample_type convert(const half Sample) { return Sample; }
	/// Converts a sample from a float
	static inline sample_type convert(const sample_type Sample) { return static_cast<sample_type>(Sample); }
	/// Converts a sample from a double
	static inline sample_type convert(const double_t Sample) { return static_cast<sample_type>(Sample); }
};

/////////////////////////////////////////////////////////////////////////////
// color_traits<double>

/// Specialization of color_traits for double samples
template<>
class color_traits<double_t>
{
public:
	/// Defines the sample type for this specialization
	typedef double_t sample_type;

	/// Returns the minimum value of a sample
	static inline sample_type minimum() { return 0; }
	/// Returns the maximum value of a sample
	static inline sample_type maximum() { return 1; }
	/// Returns the sample value corresponding to transparent alpha
	static inline sample_type transparent() { return minimum(); }
	/// Returns the sample value corresponding to opaque alpha
	static inline sample_type opaque() { return maximum(); }
	/// Inverts the value of a sample
	static inline sample_type invert(const sample_type& Sample) { return maximum() - Sample; }

	/// Converts a sample from a uint8_t
	static inline sample_type convert(const boost::uint8_t Sample) { return static_cast<sample_type>(Sample) / 255.0; }
	/// Converts a sample from a half
	static inline sample_type convert(const half Sample) { return Sample; }
	/// Converts a sample from a float
	static inline sample_type convert(const float Sample) { return Sample; }
	/// Converts a sample from a double
	static inline sample_type convert(const sample_type Sample) { return Sample; }
};

/////////////////////////////////////////////////////////////////////////////
// basic_hsv

/// Encapsulates storage for an HSV color sample
class basic_hsv
{
public:
	typedef double_t sample_type;
	typedef color_traits<sample_type> sample_traits;
	typedef basic_hsv this_type;

	/// Default constructor sets all samples to zero
	basic_hsv() :
		hue(sample_traits::minimum()),
		saturation(sample_traits::minimum()),
		value(sample_traits::minimum())
	{
	}

	/// Constructor that takes hue, saturation, and value samples
	basic_hsv(const sample_type Hue, const sample_type Saturation, const sample_type Value) :
		hue(Hue),
		saturation(Saturation),
		value(Value)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_hsv(const basic_luma<ForeignType, ForeignTraits>& RHS) :
		hue(sample_traits::minimum()),
		saturation(sample_traits::minimum()),
		value(sample_traits::convert(RHS.luma))
	{
	}

	template<typename ForeignType>
	basic_hsv(const ForeignType& RHS)
	{
		const sample_type red = sample_traits::convert(RHS.red);
		const sample_type green = sample_traits::convert(RHS.green);
		const sample_type blue = sample_traits::convert(RHS.blue);

		const sample_type maxcomponent = std::max(std::max(red, green), blue);
		const sample_type mincomponent = std::min(std::min(red, green), blue);
		const sample_type difference = maxcomponent - mincomponent;

		value = maxcomponent;

		saturation = maxcomponent ? difference / maxcomponent : sample_traits::minimum();

		if(saturation != sample_traits::minimum())
		{
			const sample_type reddistance = (maxcomponent - red) / difference;
			const sample_type greendistance = (maxcomponent - green) / difference;
			const sample_type bluedistance = (maxcomponent - blue) / difference;

			if(red == maxcomponent)
			{
				hue = bluedistance - greendistance;
			}
			else if(green == maxcomponent)
			{
				hue = 2 + reddistance - bluedistance;
			}
			else
			{
				hue = 4 + greendistance - reddistance;
			}

			hue *= 60;
			while(hue < 0)
				hue += 360;
			while(hue >= 360)
				hue -= 360;
		}
		else
		{
			hue = sample_traits::minimum();
		}
	}

	/// Serialization
	friend std::ostream& operator<<(std::ostream& Stream, const basic_hsv& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.hue << " " << RHS.saturation << " " << RHS.value;
		return Stream;
	}

	/// Deserialization
	friend std::istream& operator>>(std::istream& Stream, basic_hsv& RHS)
	{
		Stream >> RHS.hue >> RHS.saturation >> RHS.value;
		return Stream;
	}

	sample_type hue;
	sample_type saturation;
	sample_type value;
};

/////////////////////////////////////////////////////////////////////////////
// basic_alpha

/// Encapsulates storage for an alpha color sample
template<typename SampleType, typename SampleTraits = color_traits<SampleType> >
class basic_alpha
{
public:
	typedef SampleType sample_type;
	typedef SampleTraits sample_traits;
	typedef basic_alpha<sample_type, sample_traits> this_type;

	/// Default constructor sets all samples to zero
	basic_alpha() :
		alpha(sample_traits::transparent())
	{
	}

	/// Straightforward constructor that initializes samples
	basic_alpha(const sample_type Alpha) :
		alpha(Alpha)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_alpha(const basic_alpha<ForeignType, ForeignTraits>& RHS) :
		alpha(sample_traits::convert(RHS.Alpha))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_alpha(const basic_rgb<ForeignType, ForeignTraits>& RHS) :
		alpha(sample_traits::opaque())
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_alpha(const basic_rgba<ForeignType, ForeignTraits>& RHS) :
		alpha(sample_traits::convert(RHS.alpha))
	{
	}

	basic_alpha(const basic_hsv& RHS) :
		alpha(sample_traits::opaque())
	{
	}

	/// Serialization
	friend std::ostream& operator<<(std::ostream& Stream, const basic_alpha<sample_type, sample_traits>& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.alpha;
		return Stream;
	}

	/// Deserialization
	friend std::istream& operator>>(std::istream& Stream, basic_alpha<sample_type, sample_traits>& RHS)
	{
		Stream >> RHS.alpha;
		return Stream;
	}

	sample_type alpha;
};

/////////////////////////////////////////////////////////////////////////////
// basic_luma

/// Encapsulates storage for a luma color sample
template<typename SampleType, typename SampleTraits = color_traits<SampleType> >
class basic_luma
{
public:
	typedef SampleType sample_type;
	typedef SampleTraits sample_traits;
	typedef basic_luma<sample_type, sample_traits> this_type;

	/// Default constructor sets all samples to zero
	basic_luma() :
		luma(sample_traits::minimum())
	{
	}

	/// Straightforward constructor that initializes samples
	basic_luma(const sample_type Luma) :
		luma(Luma)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma(const basic_luma<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(RHS.luma))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma(const basic_rgb<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(std::max(RHS.red, std::max(RHS.green, RHS.blue))))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma(const basic_rgba<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(std::max(RHS.red, std::max(RHS.green, RHS.blue))))
	{
	}

	basic_luma(const basic_hsv& RHS) :
		luma(sample_traits::convert(RHS.value))
	{
	}

	/// Serialization
	friend std::ostream& operator<<(std::ostream& Stream, const basic_luma<sample_type, sample_traits>& RHS)
	{ 
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.luma;
		return Stream;
	}

	/// Deserialization
	friend std::istream& operator>>(std::istream& Stream, basic_luma<sample_type, sample_traits>& RHS)
	{
		Stream >> RHS.luma;
		return Stream;
	}

	sample_type luma;
};

/////////////////////////////////////////////////////////////////////////////
// basic_luma_alpha

/// Encapsulates storage for a luma color sample with alpha
template<typename SampleType, typename SampleTraits = color_traits<SampleType> >
class basic_luma_alpha
{
public:
	typedef SampleType sample_type;
	typedef SampleTraits sample_traits;
	typedef basic_luma_alpha<sample_type, sample_traits> this_type;

	/// Default constructor sets all samples to zero
	basic_luma_alpha() :
		luma(sample_traits::minimum()),
		alpha(sample_traits::opaque())
	{
	}

	/// Straightforward constructor that initializes samples
	basic_luma_alpha(const sample_type Luma) :
		luma(Luma),
		alpha(sample_traits::opaque())
	{
	}

	basic_luma_alpha(const sample_type Luma, const sample_type Alpha) :
		luma(Luma),
		alpha(Alpha)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma_alpha(const basic_luma_alpha<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(RHS.luma)),
		alpha(sample_traits::convert(RHS.alpha))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma_alpha(const basic_rgb<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(std::max(RHS.red, std::max(RHS.green, RHS.blue)))),
		alpha(sample_traits::opaque())
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_luma_alpha(const basic_rgba<ForeignType, ForeignTraits>& RHS) :
		luma(sample_traits::convert(std::max(RHS.red, std::max(RHS.green, RHS.blue)))),
		alpha(sample_traits::convert(RHS.alpha))
	{
	}

	basic_luma_alpha(const basic_hsv& RHS) :
		luma(sample_traits::convert(RHS.value)),
		alpha(sample_traits::opaque())
	{
	}

	/// Serialization
	friend std::ostream& operator<<(std::ostream& Stream, const basic_luma_alpha<sample_type, sample_traits>& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.luma << RHS.alpha;
		return Stream;
	}

	/// Deserialization
	friend std::istream& operator>>(std::istream& Stream, basic_luma_alpha<sample_type, sample_traits>& RHS)
	{
		Stream >> RHS.luma >> RHS.alpha;
		return Stream;
	}

	sample_type luma;
	sample_type alpha;
};

/////////////////////////////////////////////////////////////////////////////
// basic_rgb

/// Encapsulates storage for an RGB color sample
template<typename SampleType, typename SampleTraits = color_traits<SampleType> >
class basic_rgb
{
public:
	typedef SampleType sample_type;
	typedef SampleTraits sample_traits;
	typedef basic_rgb<sample_type, sample_traits> this_type;

	/// Default constructor sets all samples to zero
	basic_rgb() :
		red(sample_traits::minimum()),
		green(sample_traits::minimum()),
		blue(sample_traits::minimum())
	{
	}

	/// Constructor that takes red, green, and blue samples
	basic_rgb(const sample_type Red, const sample_type Green, const sample_type Blue) :
		red(Red),
		green(Green),
		blue(Blue)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgb(const basic_alpha<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.alpha)),
		green(sample_traits::convert(RHS.alpha)),
		blue(sample_traits::convert(RHS.alpha))
	{
	}
	
	template<typename ForeignType, typename ForeignTraits>
	basic_rgb(const basic_luma<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.luma)),
		green(sample_traits::convert(RHS.luma)),
		blue(sample_traits::convert(RHS.luma))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgb(const basic_rgb<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.red)),
		green(sample_traits::convert(RHS.green)),
		blue(sample_traits::convert(RHS.blue))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgb(const basic_rgba<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.red)),
		green(sample_traits::convert(RHS.green)),
		blue(sample_traits::convert(RHS.blue))
	{
	}

	basic_rgb(const basic_hsv& RHS)
	{
		// Easiest case - saturation is zero
		if(0 == RHS.saturation)
		{
			red = green = blue = sample_traits::convert(RHS.value);
			return;
		}

		const double_t h = RHS.hue / 60;
		const double_t i = floor(h);
		const double_t f = h - i;
		const double_t p = RHS.value * (1 - RHS.saturation);
		const double_t q = RHS.value * (1 - (RHS.saturation * f));
		const double_t t = RHS.value * (1 - (RHS.saturation * (1 - f)));

		if(0.0 == i)
		{
			red = sample_traits::convert(RHS.value);
			green = sample_traits::convert(t);
			blue = sample_traits::convert(p);
		}
		else if(1.0 == i)
		{
			red = sample_traits::convert(q);
			green = sample_traits::convert(RHS.value);
			blue = sample_traits::convert(p);
		}
		else if(2.0 == i)
		{
			red = sample_traits::convert(p);
			green = sample_traits::convert(RHS.value);
			blue = sample_traits::convert(t);
		}
		else if(3.0 == i)
		{
			red = sample_traits::convert(p);
			green = sample_traits::convert(q);
			blue = sample_traits::convert(RHS.value);
		}
		else if(4.0 == i)
		{
			red = sample_traits::convert(t);
			green = sample_traits::convert(p);
			blue = sample_traits::convert(RHS.value);
		}
		else if(5.0 == i)
		{
			red = sample_traits::convert(RHS.value);
			green = sample_traits::convert(p);
			blue = sample_traits::convert(q);
		}
	}

	const double_t* data() const
	{
		return &red;
	}

	friend bool operator==(const this_type& LHS, const this_type& RHS)
	{
		return LHS.red == RHS.red && LHS.green == RHS.green && LHS.blue == RHS.blue;
	}

	friend bool operator!=(const this_type& LHS, const this_type& RHS)
	{
		return !(LHS == RHS);
	}

	friend const this_type operator+(const this_type& LHS, const this_type& RHS)
	{
		return this_type(LHS.red + RHS.red, LHS.green + RHS.green, LHS.blue + RHS.blue);
	}
	
	friend const this_type operator-(const this_type& LHS, const this_type& RHS)
	{
		return this_type(LHS.red - RHS.red, LHS.green - RHS.green, LHS.blue - RHS.blue);
	}
	
	friend const this_type operator*(const this_type& LHS, const double_t RHS)
	{
		return this_type(LHS.red * RHS, LHS.green * RHS, LHS.blue * RHS);
	}

	friend const this_type operator*(const double_t LHS, const this_type& RHS)
	{
		return RHS * LHS;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const basic_rgb<sample_type, sample_traits>& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.red << " " << RHS.green << " " << RHS.blue;
		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, basic_rgb<sample_type, sample_traits>& RHS)
	{
		Stream >> RHS.red >> RHS.green >> RHS.blue;
		return Stream;
	}

	sample_type red;
	sample_type green;
	sample_type blue;
};

/////////////////////////////////////////////////////////////////////////////
// basic_rgba

/// Encapsulates storage for an RGBA color sample
template<typename SampleType, typename SampleTraits = color_traits<SampleType> >
class basic_rgba
{
public:
	typedef SampleType sample_type;
	typedef SampleTraits sample_traits;
	typedef basic_rgba<sample_type, sample_traits> this_type;

	/// Default constructor sets all samples to zero
	basic_rgba() :
		red(sample_traits::minimum()),
		green(sample_traits::minimum()),
		blue(sample_traits::minimum()),
		alpha(sample_traits::opaque())
	{
	}

	/// Constructor that takes red, green, and blue samples, and sets alpha opaque
	basic_rgba(const sample_type Red, const sample_type Green, const sample_type Blue) :
		red(Red),
		green(Green),
		blue(Blue),
		alpha(sample_traits::opaque())
	{
	}

	/// Constructor that takes red, green, blue, and alpha samples
	basic_rgba(const sample_type Red, const sample_type Green, const sample_type Blue, const sample_type Alpha) :
		red(Red),
		green(Green),
		blue(Blue),
		alpha(Alpha)
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgba(const basic_luma<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.luma)),
		green(sample_traits::convert(RHS.luma)),
		blue(sample_traits::convert(RHS.luma)),
		alpha(sample_traits::opaque())
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgba(const basic_luma_alpha<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.luma)),
		green(sample_traits::convert(RHS.luma)),
		blue(sample_traits::convert(RHS.luma)),
		alpha(sample_traits::convert(RHS.alpha))
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgba(const basic_rgb<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.red)),
		green(sample_traits::convert(RHS.green)),
		blue(sample_traits::convert(RHS.blue)),
		alpha(sample_traits::opaque())
	{
	}

	template<typename ForeignType, typename ForeignTraits>
	basic_rgba(const basic_rgba<ForeignType, ForeignTraits>& RHS) :
		red(sample_traits::convert(RHS.red)),
		green(sample_traits::convert(RHS.green)),
		blue(sample_traits::convert(RHS.blue)),
		alpha(sample_traits::convert(RHS.alpha))
	{
	}

	/// Serialization
	friend std::ostream& operator<<(std::ostream& Stream, const basic_rgba<sample_type, sample_traits>& RHS)
	{
		boost::io::ios_flags_saver stream_state(Stream);
		Stream << std::setprecision(17) << RHS.red << " " << RHS.green << " " << RHS.blue << " " << RHS.alpha;
		return Stream;
	}

	/// Deserialization
	friend std::istream& operator>>(std::istream& Stream, basic_rgba<sample_type, sample_traits>& RHS)
	{
		Stream >> RHS.red >> RHS.green >> RHS.blue >> RHS.alpha;
		return Stream;
	}

	sample_type red;
	sample_type green;
	sample_type blue;
	sample_type alpha;
};

/// Convenience typedef for our "standard" color type
typedef basic_rgb<double_t> color;

/// Specialization of almost_equal that tests two color objects for near-equality
template<>
class almost_equal<color>
{
	typedef color T;
public:
	almost_equal(const boost::uint64_t Threshold) : test(Threshold) { }
	inline bool_t operator()(const T& A, const T& B) const
	{
		return test(A.red, B.red) && test(A.green, B.green) && test(A.blue, B.blue);
	}

private:
	const almost_equal<double_t> test;
};

} // namespace k3d

#endif // !K3DSDK_COLOR_H

