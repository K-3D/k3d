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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Evan Lezar (evanlezar@gmail.com)
*/

#include "cuda_bitmap_simple_modifier.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

namespace module
{

namespace cuda
{

/////////////////////////////////////////////////////////////////////////////
// cuda_bitmap_subtract

class cuda_bitmap_subtract :
	public cuda_bitmap_simple_modifier
{
	typedef cuda_bitmap_simple_modifier base;

public:
	cuda_bitmap_subtract(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Subtract value")) + init_description(_("Add value to each pixel's Red, Green and Blue component")) + init_value(0.0))
	{
		m_value.changed_signal().connect(make_update_bitmap_slot());
	}

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
        bitmap_arithmetic(Input, Output, CUDA_BITMAP_SUBTRACT, (float)(m_value.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_bitmap_subtract,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0xd85e4b52, 0xfd48fa70, 0x20c5cc93, 0x08ceb52f),
				"CUDABitmapSubtract",
				_("Subtract value from each pixel color component using the CUDA API"),
				"CUDABitmap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_value;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_subtract

k3d::iplugin_factory& cuda_bitmap_subtract_factory()
{
	return cuda_bitmap_subtract::get_factory();
}

} // namespace cuda

} // namespace module

