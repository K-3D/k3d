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

/** \file
	\author Evan Lezar (evanlezar@gmail.com)\
    \date 10 July 2008
*/

#include "cuda_bitmap_simple_modifier.h" 

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>

namespace module
{

namespace cuda
{
	
/////////////////////////////////////////////////////////////////////////////
// cuda_bitmap_threshold

class cuda_bitmap_threshold :
	public cuda_bitmap_simple_modifier
{
	typedef cuda_bitmap_simple_modifier base;

public:
	cuda_bitmap_threshold(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_red_threshold(init_owner(*this) + init_name("red_threshold") + init_label(_("Red threshold")) + init_description(_("Clamp Red channel to threshold")) + init_value(0.0)),
        m_green_threshold(init_owner(*this) + init_name("green_threshold") + init_label(_("Green threshold")) + init_description(_("Clamp Green channel to threshold")) + init_value(0.0)),
        m_blue_threshold(init_owner(*this) + init_name("blue_threshold") + init_label(_("Blue threshold")) + init_description(_("Clamp Blue channel to threshold")) + init_value(0.0)),
        m_alpha_threshold(init_owner(*this) + init_name("alpha_threshold") + init_label(_("Alpha threshold")) + init_description(_("Clamp Alpha channel to threshold")) + init_value(0.0))
    {
		m_red_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(make_update_bitmap_slot()));
		m_green_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(make_update_bitmap_slot()));
		m_blue_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(make_update_bitmap_slot()));
		m_alpha_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::bitmap_pixels_changed> >(make_update_bitmap_slot()));
    }


	void on_assign_pixels(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		const unsigned short* inputPixels = reinterpret_cast<const unsigned short*>(&(const_view(Input)[0]));
        unsigned short* outputPixels = reinterpret_cast<unsigned short*>(&(view(Output)[0]));
        unsigned short *p_device_image;
        k3d::int32_t sizeInBytes = Input.width()*Input.height()*8;
        
        // copy the data to the device
        start_profile_step();
        allocate_device_memory((void**)&p_device_image, sizeInBytes);
        copy_from_host_to_device(p_device_image, inputPixels, sizeInBytes);
        stop_profile_step(PROFILE_STRING_HOST_TO_DEVICE);

        // perform the calculation
        start_profile_step();
        bitmap_threshold_kernel_entry(p_device_image, Input.width(), Input.height(), 
                                            (float)(m_red_threshold.pipeline_value()), 
                                            (float)(m_green_threshold.pipeline_value()), 
                                            (float)(m_blue_threshold.pipeline_value()), 
                                            (float)(m_alpha_threshold.pipeline_value()));
                                            
        stop_profile_step(PROFILE_STRING_EXECUTE_KERNEL);       

        // copy the data from the device
        start_profile_step();
        copy_from_device_to_host(outputPixels, p_device_image, sizeInBytes);
        stop_profile_step(PROFILE_STRING_DEVICE_TO_HOST);

        // free the memory allocated on the device
        free_device_memory(p_device_image);

	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_bitmap_threshold,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0xee5346f6, 0x0747fb8d, 0x524333a8, 0xe92dfee7),
				"CUDABitmapThreshold",
				_("Cut off chanels at a certain value using the CUDA API"),
				"CUDABitmap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_red_threshold;
    k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_green_threshold;
    k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blue_threshold;
    k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_alpha_threshold;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_bitmap_threshold

k3d::iplugin_factory& cuda_bitmap_threshold_factory()
{
	return cuda_bitmap_threshold::get_factory();
}

} // namespace cuda

} // namespace module

