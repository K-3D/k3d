#ifndef K3DSDK_PIPELINE_DATA_H
#define K3DSDK_PIPELINE_DATA_H

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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "types.h"
#include <boost/shared_ptr.hpp>

namespace k3d
{

template<typename T>
class pipeline_data_traits
{
public:
	static T* create()
	{
		return new T();
	}

	static T* clone(const T& Other)
	{
		return new T(Other);
	}
};

template<typename T>
class pipeline_data
{
public:
	typedef T value_type;
	typedef T element_type;

	pipeline_data() :
		originator(false)
	{
	}

	pipeline_data(const pipeline_data<T>& Other) :
		storage(Other.storage),
		originator(false)
	{
	}

	pipeline_data(T* Other) :
		storage(Other),
		originator(true)
	{
	}

	T& create()
	{
		storage.reset(pipeline_data_traits<T>::create());
		originator = storage.get() ? true : false;
		return *storage;
	}

	T& create(T* Instance)
	{
		storage.reset(Instance);
		originator = storage.get() ? true : false;
		return *storage;
	}

	template<typename Y>
	T& create(Y* Instance)
	{
		storage.reset(Instance);
		originator = storage.get() ? true : false;
		return *storage;
	}

	void reset()
	{
		storage.reset();
		originator = false;
	}

	const T& operator*() const
	{
		return *storage;
	}

	const T* operator->() const
	{
		return storage.operator->();
	}

	const T* get() const
	{
		return storage.get();
	}

	T& writable()
	{
		if(originator)
			return *storage;

		storage.reset(pipeline_data_traits<T>::clone(*storage));
		originator = true;
		return *storage;
	}

	long use_count() const
	{
		return storage.use_count();
	}

	pipeline_data& operator=(const pipeline_data& Other)
	{
		storage = Other.storage;
		originator = false;
	}

	operator bool() const
	{
		return storage;
	}

	bool operator!() const
	{
		return !storage;
	}

private:
	typedef boost::shared_ptr<T> storage_type;

	storage_type storage;
	bool_t originator;
};

} // namespace k3d

#endif // !K3DSDK_PIPELINE_DATA_H

