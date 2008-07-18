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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace cuda
{

/////////////////////////////////////////////////////////////////////////////
// cuda_multiply

class cuda_bitmap_multiply :
	public cuda_bitmap_simple_modifier
{
	typedef cuda_bitmap_simple_modifier base;

public:
	cuda_bitmap_multiply(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Multiplicand")) + init_description(_("Multiply each pixel component with this value")) + init_value(1.0))
	{
		m_value.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(make_update_bitmap_slot()));
	}


	void on_assign_pixels(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		bitmap_arithmetic(Input, Output, CUDA_BITMAP_MULTIPLY, (float)(m_value.pipeline_value()));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_bitmap_multiply,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0xf2b5cdd6, 0x574d0d53, 0x8053ff81, 0x8612d328),
				"CUDABitmapMultiply",
				_("Multiply each each pixel color component by a specified value using the CUDA API"),
				"CUDABitmap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_value;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_bitmap_multiply

k3d::iplugin_factory& cuda_bitmap_multiply_factory()
{
	return cuda_bitmap_multiply::get_factory();
}

} // namespace cuda

} // namespace module

