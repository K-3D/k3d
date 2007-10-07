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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include "algebra.h"
#include "array.h"
#include "inode.h"
#include "named_array_operations.h"
#include "normal3.h"
#include "point2.h"
#include "point3.h"
#include "result.h"
#include "typed_array.h"
#include "type_registry.h"
#include "vector2.h"
#include "vector3.h"

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <functional>

namespace k3d
{

////////////////////////////////////////////////////////////////////////////
// named_array_copier::implementation

class named_array_copier::implementation
{
public:
	implementation(const named_arrays& Source, named_arrays& Target)
	{
		named_arrays::const_iterator source = Source.begin();
		named_arrays::const_iterator source_end = Source.end();
		named_arrays::iterator target = Target.begin();
		named_arrays::iterator target_end = Target.end();
		for(; source != source_end && target != target_end ; ++source, ++target)
		{
			const std::string source_name = source->first;
			const std::string target_name = target->first;
			if(source_name != target_name)
			{
				log() << error << "arrays with mis-matched names will not be copied: [" << source_name << "] versus [" << target_name << "]" << std::endl;
				continue;
			}

			const array* source_array = source->second.get();
			array* target_array = target->second.get();
			if(typeid(*source_array) != typeid(*target_array))
			{
				log() << error << "array [" << source_name << "] with mis-matched types will not be copied: [" << demangle(typeid(*source_array)) << "] versus [" << demangle(typeid(*target_array)) << "]" << std::endl;
				continue;
			}

			if(create_copier<typed_array<k3d::bool_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::int8_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::int16_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::int32_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::int64_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::uint8_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::uint16_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::uint32_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::uint64_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::half_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::float_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::double_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::uint_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<k3d::string_t> >(source_array, target_array)) continue;
			if(create_copier<typed_array<point2> >(source_array, target_array)) continue;
			if(create_copier<typed_array<vector2> >(source_array, target_array)) continue;
			if(create_copier<typed_array<point3> >(source_array, target_array)) continue;
			if(create_copier<typed_array<normal3> >(source_array, target_array)) continue;
			if(create_copier<typed_array<vector3> >(source_array, target_array)) continue;
			if(create_copier<typed_array<point4> >(source_array, target_array)) continue;
			if(create_copier<typed_array<matrix4> >(source_array, target_array)) continue;
			if(create_copier<typed_array<inode*> >(source_array, target_array)) continue;

			log() << error << "unknown array type [" << demangle(typeid(*source_array)) << "] will not be copied" << std::endl;
		}

		for(; source != source_end; ++source)
			log() << error << "source array [" << source->first << "] has no corresponding target and will not be copied" << std::endl;

		for(; target != target_end; ++target)
			log() << error << "target array [" << target->first << "] has no corresponding source and will not receive data" << std::endl;
	}

	void push_back(const size_t Index)
	{
		std::for_each(copiers.begin(), copiers.end(), boost::bind(&array_copier::push_back, _1, Index));
	}

private:
	/// Abstract interface for concrete objects that provide array-copying operations
	class array_copier
	{
	public:
		/// Called to append a value (identified by index) from the source array to the target array
		virtual void push_back(const size_t Index) = 0;
	};

	/// Concrete implementation of array_copier that performs copying on concrete array types
	template<typename array_t>
	class typed_array_copier :
		public array_copier
	{
	public:
		typed_array_copier(const array_t& Source, array_t& Target) :
			source(Source),
			target(Target)
		{
		}

		void push_back(const size_t Index)
		{
			target.push_back(source[Index]);
		}

	private:
		const array_t& source;
		array_t& target;
	};

	template<typename array_t>
	bool create_copier(const array* Source, array* Target)
	{
		if(const array_t* const source = dynamic_cast<const array_t*>(Source))
		{
			if(array_t* const target = dynamic_cast<array_t*>(Target))
			{
				copiers.push_back(new typed_array_copier<array_t>(*source, *target));
				return true;
			}
		}

		return false;
	}

	/// Stores a collection of array_copier objects that handle copying between each source/target pair of arrays
	boost::ptr_vector<array_copier> copiers;
};

////////////////////////////////////////////////////////////////////////////
// named_array_copier

named_array_copier::named_array_copier(const named_arrays& Source, named_arrays& Target) :
	m_implementation(new implementation(Source, Target))
{
}

named_array_copier::~named_array_copier()
{
}

void named_array_copier::push_back(const size_t Index)
{
	m_implementation->push_back(Index);
}

} // namespace k3d

