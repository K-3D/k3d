#ifndef K3DSDK_BITMAP_SINK_H
#define K3DSDK_BITMAP_SINK_H

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

#include "bitmap.h"
#include "data.h"
#include "ibitmap_sink.h"
#include "k3d-i18n-config.h"

namespace k3d
{

template<typename derived_t>
class bitmap_sink :
	public ibitmap_sink
{
public:
	iproperty& bitmap_sink_input()
	{
		return m_input_bitmap;
	}

	sigc::slot<void, iunknown*> make_input_changed_slot()
	{
		return sigc::mem_fun(*this, &bitmap_sink<derived_t>::input_changed);
	}

protected:
	bitmap_sink() :
		m_input_bitmap(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("input_bitmap")
			+ init_label(_("Input Bitmap"))
			+ init_description(_("Input bitmap"))
			+ init_value<bitmap*>(0))
	{
		m_input_bitmap.changed_signal().connect(make_input_changed_slot());
	}

	k3d_data(bitmap*, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_input_bitmap;

private:
	void input_changed(k3d::iunknown* Hint)
	{
		on_input_changed(Hint);
	}

	virtual void on_input_changed(k3d::iunknown*) = 0;
};

} // namespace k3d

#endif // !K3DSDK_BITMAP_SINK_H

