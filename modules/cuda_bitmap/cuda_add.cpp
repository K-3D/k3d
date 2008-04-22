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

#include "simple_modifier.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

extern "C" void bitmap_add_entry(const unsigned short *input, unsigned short *output, int width, int height, float value);

namespace module
{

namespace cuda_bitmap
{

/////////////////////////////////////////////////////////////////////////////
// cuda_add

class cuda_add :
	public simple_modifier
{
	typedef simple_modifier base;

public:
	cuda_add(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Add value")) + init_description(_("Add value to each pixel's Red, Green and Blue component")) + init_value(0.0))
	{
		m_value.changed_signal().connect(make_update_bitmap_slot());
	}

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		const unsigned short* inputPixels = reinterpret_cast<const unsigned short*>(&(const_view(Input)[0]));
		unsigned short* outputPixels = reinterpret_cast<unsigned short*>(&(view(Output)[0]));
		
		bitmap_add_entry(inputPixels, outputPixels, Input.width(), Input.height(), (float)(m_value.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_add,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0x86ddee78, 0x074d46f7, 0x13bf758a, 0x8893b6c2),
				"CUDABitmapAdd",
				_("Add value to each pixel color component using the CUDA API"),
				"CUDA Bitmap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_value;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_add

k3d::iplugin_factory& cuda_add_factory()
{
	return cuda_add::get_factory();
}

} // namespace cuda_bitmap

} // namespace module

