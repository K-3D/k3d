#ifndef K3DSDK_STRING_SOURCE_H
#define K3DSDK_STRING_SOURCE_H

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

#include "data.h"
#include "istring_source.h"
#include "value_demand_storage.h"

namespace k3d
{

template<typename derived_t>
class string_source :
	public istring_source
{
public:
	iproperty& string_source_output()
	{
		return m_output_string;
	}

	sigc::slot<void, ihint*> make_update_string_slot()
	{
		return m_output_string.make_slot();
	}

protected:
	string_source() :
		m_output_string(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("output_string")
			+ init_label(_("Output String"))
			+ init_description("Output string")
			+ init_value(string_t()))
	{
		m_output_string.set_update_slot(sigc::mem_fun(*this, &string_source<derived_t>::execute));
	}

private:
	k3d_data(std::string, data::immutable_name, data::change_signal, data::no_undo, data::value_demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_string;

	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, string_t& Output)
	{
		// We can safely ignore any hints ...
		on_update_string(Output);
	}

	virtual void on_update_string(string_t& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_STRING_SOURCE_H

