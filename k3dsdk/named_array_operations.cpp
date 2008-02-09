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

#include "array.h"
#include "named_array_operations.h"
#include "named_array_types.h"
#include "result.h"
#include "typed_array.h"
#include "type_registry.h"

#include <boost/bind.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/static_assert.hpp>

#include <functional>

namespace k3d
{

/// Returns the weighted sum of a random-access subset of array values
bool_t weighted_sum(const typed_array<bool_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	return Source[Indices[0]];
}

/// Returns the weighted sum of a random-access subset of array values
int8_t weighted_sum(const typed_array<int8_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	int8_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<int8_t>(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
int16_t weighted_sum(const typed_array<int16_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	int16_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<int16_t>(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
int32_t weighted_sum(const typed_array<int32_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	int32_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<int32_t>(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
int64_t weighted_sum(const typed_array<int64_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	int64_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<int64_t>(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
uint8_t weighted_sum(const typed_array<uint8_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	uint8_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<uint8_t>(Source[Indices[i]] * std::max(0.0, Weights[i]));
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
uint16_t weighted_sum(const typed_array<uint16_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	uint16_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<uint16_t>(Source[Indices[i]] * std::max(0.0, Weights[i]));
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
uint32_t weighted_sum(const typed_array<uint32_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	uint32_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<uint32_t>(Source[Indices[i]] * std::max(0.0, Weights[i]));
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
uint64_t weighted_sum(const typed_array<uint64_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	uint64_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += static_cast<uint64_t>(Source[Indices[i]] * std::max(0.0, Weights[i]));
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
half_t weighted_sum(const typed_array<half_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	half_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
float_t weighted_sum(const typed_array<float_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	float_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
double_t weighted_sum(const typed_array<double_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	double_t result = 0;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
string_t weighted_sum(const typed_array<string_t>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	return Source[Indices[0]];
}

/// Returns the weighted sum of a random-access subset of array values
color weighted_sum(const typed_array<color>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	color result;
	for(uint_t i = 0; i != Count; ++i)
		result = result + (Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
point2 weighted_sum(const typed_array<point2>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	point2 result;
	for(uint_t i = 0; i != Count; ++i)
		result += to_vector(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
vector2 weighted_sum(const typed_array<vector2>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	vector2 result;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
normal3 weighted_sum(const typed_array<normal3>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	normal3 result;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
point3 weighted_sum(const typed_array<point3>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	point3 result;
	for(uint_t i = 0; i != Count; ++i)
		result += to_vector(Source[Indices[i]] * Weights[i]);
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
vector3 weighted_sum(const typed_array<vector3>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	vector3 result;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
point4 weighted_sum(const typed_array<point4>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	point4 result;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
matrix4 weighted_sum(const typed_array<matrix4>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	matrix4 result;
	for(uint_t i = 0; i != Count; ++i)
		result += Source[Indices[i]] * Weights[i];
	return result;
}

/// Returns the weighted sum of a random-access subset of array values
inode* weighted_sum(const typed_array<inode*>& Source, const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	return Source[Indices[0]];
}

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

			if(!copier_factory::create_copier(source_array, target_array, copiers))
				log() << error << "array [" << source_name << "] with unknown type [" << demangle(typeid(*source_array)) << "] will not be copied" << std::endl;
		}

		for(; source != source_end; ++source)
			log() << error << "source array [" << source->first << "] has no corresponding target and will not be copied" << std::endl;

		for(; target != target_end; ++target)
			log() << error << "target array [" << target->first << "] has no corresponding source and will not receive data" << std::endl;
	}

	void push_back(const uint_t Index)
	{
		std::for_each(copiers.begin(), copiers.end(), boost::bind(&array_copier::push_back, _1, Index));
	}

	void push_back(const uint_t Count, const uint_t* Indices, const double_t* Weights)
	{
		std::for_each(copiers.begin(), copiers.end(), boost::bind(&array_copier::push_back, _1, Count, Indices, Weights));
	}

private:
	/// Abstract interface for concrete objects that provide array-copying operations
	class array_copier
	{
	public:
		/// Called to append a value (identified by index) from the source array to the target array
		virtual void push_back(const uint_t Index) = 0;
		/// Called to compute a weighted sum from the source array and append the result to the target array
		virtual void push_back(const uint_t Count, const uint_t* Indices, const double_t* Weights) = 0;
	};

	/// Defines storage for a collection of array copiers
	typedef boost::ptr_vector<array_copier> copiers_t;

	/// Helper class that instantiates array_copier objects based on the runtime type of source and target arrays
	class copier_factory
	{
	public:
		static const bool create_copier(const array* Source, array* Target, copiers_t& Copiers)
		{
			bool result = false;
			boost::mpl::for_each<named_array_types>(copier_factory(Source, Target, Copiers, result));
			return result;
		}

		template<typename T>
		void operator()(T)
		{
			if(created)
				return;

			if(const typed_array<T>* const typed_source = dynamic_cast<const typed_array<T>* >(source))
			{
				if(typed_array<T>* const typed_target = dynamic_cast<typed_array<T>* >(target))
				{
					copiers.push_back(new typed_array_copier<typed_array<T> >(*typed_source, *typed_target));
					created = true;
				}
			}
		}

	private:
		copier_factory(const array* Source, array* Target, copiers_t& Copiers, bool& Created) :
			source(Source),
			target(Target),
			copiers(Copiers),
			created(Created)
		{
		}

		/// Concrete array_copier implementation that is templated on the array type
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

			void push_back(const uint_t Index)
			{
				target.push_back(source[Index]);
			}

			void push_back(const uint_t Count, const uint_t* Indices, const double_t* Weights)
			{
				target.push_back(weighted_sum(source, Count, Indices, Weights));
			}

		private:
			const array_t& source;
			array_t& target;
		};

		const array* const source;
		array* const target;
		copiers_t& copiers;
		bool& created;
	};

	/// Stores a collection of array_copier objects that handle copying between each source/target pair of arrays
	copiers_t copiers;
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

void named_array_copier::push_back(const uint_t Index)
{
	m_implementation->push_back(Index);
}

void named_array_copier::push_back(const uint_t Count, const uint_t* Indices, const double_t* Weights)
{
	m_implementation->push_back(Count, Indices, Weights);
}

} // namespace k3d

