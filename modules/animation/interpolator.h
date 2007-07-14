#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
		\brief Keyframe interpolator interface and default implementations
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <stdexcept>

namespace libk3danimation
{

/// Thrown when the interpolator does not have enough key data to give an interpolation
class insufficient_data_exception : public std::runtime_error
{
public:
	insufficient_data_exception() : std::runtime_error("Animation keyframe: insufficuent data to interpolate") {}
};

/// Base class for interpolation methods for keyframed animations
template<typename time_t, typename value_t>
class interpolator : public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	/// Stores the keyframe data
	typedef std::map<time_t, value_t> keyframes_t;
	
	interpolator(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base (Factory, Document) {}
	/// Calculate the interpolation value at Time based on Keyframes. Throws insufficient_data_exception if there aren't enough keyframes around Time
	virtual value_t interpolate(time_t Time, const keyframes_t& Keyframes) = 0;
	
	virtual ~interpolator() {} 
};

/// Implement linear interpolation
template <typename time_t, typename value_t>
class linear_interpolator :
	public interpolator<time_t, value_t>
{
	typedef std::map<time_t, value_t> keyframes_t;
public:
	linear_interpolator<time_t, value_t>(k3d::iplugin_factory& Factory, k3d::idocument& Document) : interpolator<time_t, value_t>(Factory, Document) {}
	virtual value_t interpolate(time_t Time, const keyframes_t& Keyframes)
	{
		time_t t_lower, t_upper;
		value_t v_lower, v_upper;
		typename keyframes_t::const_iterator found_key = Keyframes.lower_bound(Time);
		if (found_key == Keyframes.begin() && found_key->first == Time)
			return found_key->second; // exact match of first key
		if (found_key == Keyframes.begin() || found_key == Keyframes.end())
			throw insufficient_data_exception(); // no key before or after Time
		t_upper = found_key->first;
		v_upper = found_key->second;
		--found_key;
		t_lower = found_key->first;
		v_lower = found_key->second;
		return v_lower + (v_upper - v_lower)*(Time - t_lower)/(t_upper - t_lower);
	}
};

}

#endif /*INTERPOLATOR_H_*/
