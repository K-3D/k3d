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

#include <k3dsdk/algebra.h>

namespace module
{

namespace animation
{

/// Thrown when the interpolator does not have enough key data to give an interpolation
class insufficient_data_exception : public std::runtime_error
{
public:
	insufficient_data_exception() : std::runtime_error("Animation keyframe: insufficuent data to interpolate") {}
};

/// Base class for interpolation methods for keyframed animations
template<typename time_t, typename value_t>
class interpolator : public k3d::node
{
	typedef k3d::node base;
public:
	/// Stores the keyframe data
	typedef std::map<time_t, value_t> keyframes_t;
	
	interpolator(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base (Factory, Document) {}
	/// Calculate the interpolation value at Time based on Keyframes. Throws insufficient_data_exception if there aren't enough keyframes around Time
	virtual value_t interpolate(time_t Time, const keyframes_t& Keyframes) = 0;
	
	virtual ~interpolator() {}
protected:
	/// Stores the keys and values of the key before and after Time in the non-const arguments
	void get_surrounding_keys(const time_t& Time, const keyframes_t& Keyframes, time_t& t_lower, time_t& t_upper, value_t& v_lower, value_t& v_upper)
	{
		typename keyframes_t::const_iterator found_key = Keyframes.lower_bound(Time);
		if (found_key == Keyframes.begin() && found_key->first == Time && !Keyframes.empty())
		{
			t_upper = found_key->first;
			v_upper = found_key->second;
			t_lower = t_upper;
			v_lower = v_upper;
			return;
		}
		if (found_key == Keyframes.begin() || found_key == Keyframes.end())
			throw insufficient_data_exception(); // no key before or after Time
		t_upper = found_key->first;
		v_upper = found_key->second;
		--found_key;
		t_lower = found_key->first;
		v_lower = found_key->second;
	}  
};


/// Implement linear interpolation
template <typename time_t, typename value_t>
class linear_interpolator :
	public interpolator<time_t, value_t>
{
	typedef interpolator<time_t, value_t> base;
public:
	linear_interpolator<time_t, value_t>(k3d::iplugin_factory& Factory, k3d::idocument& Document) : interpolator<time_t, value_t>(Factory, Document) {}
	virtual value_t interpolate(time_t Time, const typename base::keyframes_t& Keyframes)
	{
		time_t t_lower, t_upper;
		value_t v_lower, v_upper;
		get_surrounding_keys(Time, Keyframes, t_lower, t_upper, v_lower, v_upper); 
		return lerp(t_lower, t_upper, v_lower, v_upper, Time);
	}
protected:
	value_t lerp(const time_t& t_lower, const time_t& t_upper, const value_t& v_lower, const value_t& v_upper, const time_t& Time)
	{
		if (t_upper == t_lower)
			return v_lower;
		return v_lower + (v_upper - v_lower)*(Time - t_lower)/(t_upper - t_lower);
	}	
};

/// Specialization with correct interpolation of angles
template <typename time_t>
class linear_interpolator<time_t, k3d::matrix4> : public interpolator<time_t, k3d::matrix4> 
{
	typedef k3d::matrix4 value_t;
	typedef interpolator<time_t, value_t> base;
public:
	linear_interpolator<time_t, k3d::matrix4>(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base(Factory, Document) {}
	virtual value_t interpolate(time_t Time, const typename base::keyframes_t& Keyframes)
	{
		time_t t_lower, t_upper;
		value_t v_lower, v_upper;
		get_surrounding_keys(Time, Keyframes, t_lower, t_upper, v_lower, v_upper); 
		return lerp(t_lower, t_upper, v_lower, v_upper, Time);
	} 
protected:
	k3d::matrix4 lerp(const double& t_lower, const double& t_upper, const k3d::matrix4& v_lower, const k3d::matrix4& v_upper, const double& Time)
	{
		if (t_upper == t_lower)
			return v_lower;
		k3d::matrix4 rotation_l = k3d::extract_rotation(v_lower);
		k3d::matrix4 rotation_u = k3d::extract_rotation(v_upper);
		k3d::matrix4 norotation_l = v_lower * k3d::inverse(rotation_l);
		k3d::matrix4 norotation_u = v_upper * k3d::inverse(rotation_u);
		k3d::matrix4 norotation = norotation_l + (norotation_u - norotation_l)*(Time - t_lower)/(t_upper - t_lower);
		k3d::euler_angles angles_l(rotation_l, k3d::euler_angles::XYZstatic);
		k3d::euler_angles angles_u(rotation_u, k3d::euler_angles::XYZstatic);
		k3d::quaternion q_l(angles_l);
		k3d::quaternion q_u(angles_u);
		k3d::quaternion q_i = k3d::Slerp(q_l, q_u, (Time - t_lower)/(t_upper - t_lower));
		return k3d::matrix4(norotation * k3d::rotation3D(q_i));
	}
};

} // namespace animation

} // namespace module

#endif /*INTERPOLATOR_H_*/

