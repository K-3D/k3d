#ifndef K3DSDK_COLOR_SOURCE_H
#define K3DSDK_COLOR_SOURCE_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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

#include "color.h"
#include "data.h"
#include "icolor_source.h"
#include "value_demand_storage.h"

namespace k3d
{

template<typename derived_t>
class color_source :
	public icolor_source
{
public:
	iproperty& color_source_output()
	{
		return m_output_color;
	}

	/// Returns a slot that should be connected to input properties to signal that the output has changed
	sigc::slot<void, ihint*> make_update_color_slot()
	{
		return m_output_color.make_slot();
	}

protected:
	color_source() :
		m_output_color(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("output_color")
			+ init_label(_("Output Color"))
			+ init_description(_("Output color"))
			+ init_value(k3d::color(1, 1, 1)))
	{
		m_output_color.set_update_slot(sigc::mem_fun(*this, &color_source<derived_t>::execute));
	}

private:
	k3d_data(color, data::immutable_name, data::change_signal, data::no_undo, data::value_demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_color;

	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, color& Output)
	{
		// We can safely ignore any hints ...
		on_update_color(Output);
	}

	virtual void on_update_color(color& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_COLOR_SOURCE_H

