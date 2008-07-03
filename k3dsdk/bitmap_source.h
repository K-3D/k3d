#ifndef K3DSDK_BITMAP_SOURCE_H
#define K3DSDK_BITMAP_SOURCE_H

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
#include "ibitmap_source.h"
#include "k3d-i18n-config.h"

namespace k3d
{

template<typename derived_t>
class bitmap_source :
	public ibitmap_source
{
public:
	iproperty& bitmap_source_output()
	{
		return m_output_bitmap;
	}

	sigc::slot<void, ihint*> make_reset_bitmap_slot()
	{
		return m_output_bitmap.make_reset_slot();
	}

	sigc::slot<void, ihint*> make_update_bitmap_slot()
	{
		return sigc::mem_fun(*this, &bitmap_source<derived_t>::update_bitmap);
	}

protected:
	bitmap_source() :
		m_output_bitmap(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("output_bitmap")
			+ init_label(_("Output Bitmap"))
			+ init_description(_("Output bitmap"))
			+ init_slot(sigc::mem_fun(*this, &bitmap_source<derived_t>::create_bitmap)))
	{
	}

	k3d_data(bitmap*, immutable_name, change_signal, no_undo, demand_storage, no_constraint, read_only_property, no_serialization) m_output_bitmap;

private:
	void create_bitmap(bitmap& Bitmap)
	{
		on_create_bitmap(Bitmap);
		on_update_bitmap(Bitmap);
	}

	void update_bitmap(ihint* const Hint)
	{
		if(bitmap* const output_bitmap = m_output_bitmap.internal_value())
		{
			on_update_bitmap(*output_bitmap);
			m_output_bitmap.changed_signal().emit(Hint);
		}
	}

	virtual void on_create_bitmap(bitmap& Bitmap) = 0;
	virtual void on_update_bitmap(bitmap& Bitmap) = 0;
};

} // namespace k3d

#endif // !K3DSDK_BITMAP_SOURCE_H

