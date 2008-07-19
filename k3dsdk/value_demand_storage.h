#ifndef K3DSDK_VALUE_DEMAND_STORAGE_H
#define K3DSDK_VALUE_DEMAND_STORAGE_H

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

#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace k3d
{

namespace data
{

/////////////////////////////////////////////////////////////////////////////
// value_demand_storage

/// Storage policy that computes a value on-demand
template<typename value_t, typename signal_policy_t>
class value_demand_storage :
	public signal_policy_t
{
	// This policy only works for data stored by-value
	BOOST_STATIC_ASSERT((!boost::is_pointer<value_t>::value));

public:
	typedef value_demand_storage<value_t, signal_policy_t> this_t;
	typedef std::vector<ihint*> pending_hints_t;

	/// Set the slot that will be called to bring the underlying data up-to-date
	void set_update_slot(const sigc::slot<void, const pending_hints_t&, value_t&>& Slot)
	{
		m_update_slot = Slot;
		update();
	}

	/// Returns a slot that will invoke the update() method
	sigc::slot<void, ihint*> make_slot()
	{
		return sigc::mem_fun(*this, &this_t::update);
	}

	/// Schedule an update for the value the next time it's read
	void update(ihint* const Hint = 0)
	{
		m_pending_hints.push_back(Hint ? Hint->clone() : static_cast<ihint*>(0));
		signal_policy_t::set_value(Hint);
	}

	/// Accesses the underlying data, updating it if necessary
	const value_t& internal_value()
	{
		if(!m_pending_hints.empty())
		{
			// Create a temporary copy of pending hints in-case we are updated while executing ...
			const pending_hints_t pending_hints(m_pending_hints);
			m_update_slot(pending_hints, m_value);
			
			std::for_each(m_pending_hints.begin(), m_pending_hints.end(), delete_object());
			m_pending_hints.clear();
		}

		return m_value;
	}

protected:
	template<typename init_t>
	value_demand_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_value(Init.value())
	{
	}

	~value_demand_storage()
	{
		std::for_each(m_pending_hints.begin(), m_pending_hints.end(), delete_object());
	}

private:
	/// Storage for this policy's value
	value_t m_value;
	/// Stores a slot that will be called to bring this policy's value up-to-date
	sigc::slot<void, const pending_hints_t&, value_t&> m_update_slot;
	/// Stores a collection of pending hints to be updated
	pending_hints_t m_pending_hints;
};

} // namespace data

} // namespace k3d

#endif // !K3DSDK_VALUE_DEMAND_STORAGE_H

