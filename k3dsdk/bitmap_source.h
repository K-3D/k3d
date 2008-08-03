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
#include "hints.h"
#include "ibitmap_source.h"
#include "ipipeline_profiler.h"
#include "k3d-i18n-config.h"
#include "pointer_demand_storage.h"

namespace k3d
{

/// Boilerplate CRTP class for bitmap source objects that produce a k3d::bitmap* as output.
template<typename derived_t>
class bitmap_source :
	public ibitmap_source
{
public:
	iproperty& bitmap_source_output()
	{
		return m_output_bitmap;
	}

	/// Returns a slot that should be connected to input properties to signal that the output bitmap has changed.
	sigc::slot<void, ihint*> make_update_bitmap_slot()
	{
		return m_output_bitmap.make_slot();
	}

protected:
	bitmap_source() :
		m_output_bitmap(
			init_owner(owner())
			+ init_name("output_bitmap")
			+ init_label(_("Output Bitmap"))
			+ init_description(_("Output bitmap")))
	{
		m_output_bitmap.set_update_slot(sigc::mem_fun(*this, &bitmap_source<derived_t>::execute));
	}

	/// Stores the output bitmap, which is created on-demand.
	k3d_data(bitmap*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_bitmap;

private:
	inline derived_t& owner()
	{
		return *static_cast<derived_t*>(this);
	}

	/// Called whenever the output bitmap has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, bitmap& Bitmap)
	{
		bool_t resize_bitmap = false;
		bool_t assign_pixels = false;

		for(uint_t i = 0; i != Hints.size(); ++i)
		{
			// Input pixels changed, so all we have to do is reassign ours ...
			if(dynamic_cast<hint::bitmap_pixels_changed*>(Hints[i]))
			{
				assign_pixels = true;
			}
			// In every other case (bitmap_dimensions_changed, unknown hint, or no hint),
			// we must assume the worst and recreate everything from scratch ...
			else
			{
				resize_bitmap = true;
				assign_pixels = true;
				break;
			}
		}

		if(resize_bitmap)
		{
			owner().document().pipeline_profiler().start_execution(owner(), "Resize Bitmap");
			on_resize_bitmap(Bitmap);
			owner().document().pipeline_profiler().finish_execution(owner(), "Resize Bitmap");
		}

		if(assign_pixels)
		{
			owner().document().pipeline_profiler().start_execution(owner(), "Assign Pixels");
			on_assign_pixels(Bitmap);
			owner().document().pipeline_profiler().finish_execution(owner(), "Assign Pixels");
		}
	}

	/// Implement this in derived classes to set the size of the output bitmap.
	virtual void on_resize_bitmap(bitmap& Output) = 0;
	/// Implement this in derived classes to assign values to each output pixel.
	virtual void on_assign_pixels(bitmap& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_BITMAP_SOURCE_H

