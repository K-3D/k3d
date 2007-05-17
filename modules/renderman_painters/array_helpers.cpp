// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "array_helpers.h"
#include <k3dsdk/utility.h>

namespace libk3drendermanpainters
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// array_copier::implementation

class array_copier::implementation
{
public:
	implementation()
	{
	}

	~implementation()
	{
//		std::for_each(copiers.begin(), copiers.end(), k3d::delete_object());
//		std::for_each(destinations.begin(), destinations.end(), k3d::delete_object());
	}

	void add_arrays(const k3d::mesh::named_arrays& Sources)
	{
		for(k3d::mesh::named_arrays::const_iterator array_iterator = Sources.begin(); array_iterator != Sources.end(); ++array_iterator)
		{
			const std::string name = array_iterator->first;
			const k3d::array* const source = array_iterator->second.get();

			add_array(name, *source);
		}
	}

	void add_array(const std::string& Name, const k3d::array& Source)
	{
		if(create_copier<k3d::typed_array<double> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<std::string> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::point3> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::vector3> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::normal3> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::color> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::point4> >(Name, Source))
			return;
		if(create_copier<k3d::typed_array<k3d::matrix4> >(Name, Source))
			return;

		k3d::log() << error << k3d_file_reference << " array [" << Name << "] unknown type [" << typeid(Source).name() << "]" << std::endl;
	}

	void insert(const size_t Begin, const size_t End)
	{
		for(size_t i = 0; i != copiers.size(); ++i)
			copiers[i]->insert(Begin, End);
	}

	void copy_to(const k3d::ri::storage_class_t StorageClass, k3d::ri::parameter_list& Destination)
	{
		for(size_t i = 0; i != names.size(); ++i)
			Destination.push_back(k3d::ri::parameter(names[i], StorageClass, 1, destinations[i]));
	}

private:
	class copier_interface
	{
	public:
		virtual ~copier_interface() {}
		virtual void insert(const size_t Begin, const size_t End) = 0;
	};

	template<typename array_type>
	class copier :
		public copier_interface
	{
	public:
		copier(const array_type& Source, array_type& Destination) :
			source(Source),
			destination(Destination)
		{
		}

		void insert(const size_t Begin, const size_t End)
		{
			destination.insert(destination.end(), source.begin() + Begin, source.begin() + End);
		}

	private:
		const array_type& source;
		array_type& destination;
	};

	template<typename array_type>
	bool create_copier(const std::string& Name, const k3d::array& Source)
	{
		if(const array_type* const source = dynamic_cast<const array_type*>(&Source))
		{
			array_type* const destination = new array_type();

			names.push_back(Name);
			destinations.push_back(destination);
			copiers.push_back(new copier<array_type>(*source, *destination));

			return true;
		}

		return false;
	}

	std::vector<std::string> names;
	std::vector<k3d::array*> destinations;
	std::vector<copier_interface*> copiers;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// array_copier

array_copier::array_copier() :
	m_implementation(new implementation())
{
}

array_copier::~array_copier()
{
	delete m_implementation;
}

void array_copier::add_arrays(const k3d::mesh::named_arrays& Sources)
{
	m_implementation->add_arrays(Sources);
}

void array_copier::add_array(const std::string& Name, const k3d::array& Source)
{
	m_implementation->add_array(Name, Source);
}

void array_copier::insert(const size_t Begin, const size_t End)
{
	m_implementation->insert(Begin, End);
}

void array_copier::push_back(const size_t Index)
{
	m_implementation->insert(Index, Index + 1);
}

void array_copier::copy_to(const k3d::ri::storage_class_t StorageClass, k3d::ri::parameter_list& Destination)
{
	m_implementation->copy_to(StorageClass, Destination);
}

} // namespace libk3drendermanpainters

