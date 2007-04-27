#ifndef K3DSDK_BITMAP_MODIFIER_H
#define K3DSDK_BITMAP_MODIFIER_H

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

#include "data.h"
#include "i18n.h"
#include "ibitmap_sink.h"
#include "ibitmap_source.h"
#include "bitmap.h"

namespace k3d
{

template<typename base_t>
class bitmap_modifier :
	public base_t,
	public ibitmap_source,
	public ibitmap_sink
{
public:
	bitmap_modifier(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_bitmap(init_owner(*this) + init_name("input_bitmap") + init_label(_("Input Bitmap")) + init_description(_("Input bitmap")) + init_value<bitmap*>(0)),
		m_output_bitmap(init_owner(*this) + init_name("output_bitmap") + init_label(_("Output Bitmap")) + init_description(_("Output bitmap")) + init_slot(sigc::mem_fun(*this, &bitmap_modifier<base_t>::create_bitmap)))
	{
		m_input_bitmap.changed_signal().connect(make_reset_bitmap_slot());
	}

	iproperty& bitmap_source_output()
	{
		return m_output_bitmap;
	}

	iproperty& bitmap_sink_input()
	{
		return m_input_bitmap;
	}

	sigc::slot<void, iunknown*> make_reset_bitmap_slot()
	{
		return m_output_bitmap.make_reset_slot();
	}

	sigc::slot<void, iunknown*> make_update_bitmap_slot()
	{
		return sigc::mem_fun(*this, &bitmap_modifier<base_t>::update_bitmap);
	}

protected:
	k3d_data(bitmap*, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_input_bitmap;
	k3d_data(bitmap*, data::immutable_name, data::change_signal, data::no_undo, data::demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_bitmap;

private:
	void create_bitmap(bitmap& OutputBitmap)
	{
		if(const bitmap* const input_bitmap = m_input_bitmap.value())
		{
			on_create_bitmap(*input_bitmap, OutputBitmap);
			on_update_bitmap(*input_bitmap, OutputBitmap);
		}
	}

	void update_bitmap(iunknown* const Hint)
	{
		if(bitmap* const input_bitmap = m_input_bitmap.value())
		{
			if(bitmap* const output_bitmap = m_output_bitmap.internal_value())
			{
				on_update_bitmap(*input_bitmap, *output_bitmap);
				m_output_bitmap.changed_signal().emit(Hint);
			}
		}
	}

	virtual void on_create_bitmap(const bitmap& Input, bitmap& Output) = 0;
	virtual void on_update_bitmap(const bitmap& Input, bitmap& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_BITMAP_MODIFIER_H

