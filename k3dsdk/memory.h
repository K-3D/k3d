#ifndef K3DSDK_MEMORY_H
#define K3DSDK_MEMORY_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

namespace k3d
{

namespace memory
{

/// CRTP base-class for classes that should cache instances for re-use
template<typename T>
class cache
{
public:
	inline static void* operator new(size_t size)
	{
		if(!m_available)
		{
			m_available = reinterpret_cast<link*>(malloc(sizeof(T)));
			m_available->next = 0;
		}

		void* result = m_available;
		m_available = m_available->next;
		return result;
	}

	inline void operator delete(void* instance)
	{
		reinterpret_cast<link*>(instance)->next = m_available;
		m_available = reinterpret_cast<link*>(instance);
	}

	static const unsigned long total_allocations()
	{
		return m_total_allocations;
	}

private:
	/// Provides a singly-linked list of memory available for use
	struct link
	{
		link* next;
	};

	/// Stores the singly-linked list of memory available for use
	static link* m_available;
	/// Keeps a running-count of how many allocations have been made (the "high-water mark")
	static unsigned long m_total_allocations;
};

#define k3d_instantiate_memory_cache(type) \
	template<> k3d::memory::cache<type>::link* k3d::memory::cache<type>::m_available = 0; \
	template<> unsigned long k3d::memory::cache<type>::m_total_allocations = 0;

} // namespace memory

} // namespace k3d

#endif // !K3DSDK_MEMORY_H


