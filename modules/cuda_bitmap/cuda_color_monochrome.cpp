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
#include <k3dsdk/high_res_timer.h>

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>

// include the entry points as external definitions
#include "../cuda_common/cuda_entry_points.h"

namespace module
{

namespace cuda_bitmap
{

/////////////////////////////////////////////////////////////////////////////
// cuda_color_monochrome

class cuda_color_monochrome :
	public simple_modifier
{
	typedef simple_modifier base;

public:
	cuda_color_monochrome(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_red_weight(init_owner(*this) + init_name("red_weight") + init_label(_("Red weight")) + init_description(_("Scale Red componnt value")) + init_value(0.3)),
		m_green_weight(init_owner(*this) + init_name("green_weight") + init_label(_("Green weight")) + init_description(_("Scale Green component value")) + init_value(0.59)),
		m_blue_weight(init_owner(*this) + init_name("blue_weight") + init_label(_("Blue weight")) + init_description(_("Scale Blue component value")) + init_value(0.11)),
		m_host_to_device_time(init_owner(*this) + init_name("host_to_device_time") + init_label(_("Host to device time")) + init_description(_("Timer for measuring host to device transfer time")) + init_value(0.0)),
		m_kernel_time(init_owner(*this) + init_name("kernel_time") + init_label(_("Kernel time")) + init_description(_("Timer for measuring kernel execution time")) + init_value(0.0)),
		m_device_to_host_time(init_owner(*this) + init_name("device_to_host_time") + init_label(_("Device to host time")) + init_description(_("Timer for measuring device to host transfer time")) + init_value(0.0))
	{
		m_red_weight.changed_signal().connect(make_update_bitmap_slot());
		m_green_weight.changed_signal().connect(make_update_bitmap_slot());
		m_blue_weight.changed_signal().connect(make_update_bitmap_slot());
	}


	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		k3d::timer timer;
        const unsigned short* inputPixels = reinterpret_cast<const unsigned short*>(&(const_view(Input)[0]));
        unsigned short* outputPixels = reinterpret_cast<unsigned short*>(&(view(Output)[0]));
        
        // intialize CUDA - should check for errors etc
        CUDA_initialize_device();
        
        // copy the data to the device
        start_profile_step();
        timer.restart();
        bitmap_copy_data_from_host_to_device(inputPixels, Input.width(), Input.height());
        m_host_to_device_time.set_value(timer.elapsed());
        stop_profile_step(PROFILE_STRING_HOST_TO_DEVICE);

        // perform the calculation
        start_profile_step();
        timer.restart();
        bitmap_color_monochrome_kernel_entry(Input.width(), Input.height(), (float)(m_red_weight.pipeline_value()), (float)(m_green_weight.pipeline_value()), (float)(m_blue_weight.pipeline_value()));
        m_kernel_time.set_value(timer.elapsed());
        stop_profile_step(PROFILE_STRING_EXECUTE_KERNEL);       

        // copy the data from the device
        start_profile_step();
        timer.restart();
        bitmap_copy_data_from_device_to_host(outputPixels, Input.width(), Input.height());
        m_device_to_host_time.set_value(timer.elapsed());
        stop_profile_step(PROFILE_STRING_DEVICE_TO_HOST);

        // cleanup the memory allocated on the device
        CUDA_cleanup();

	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cuda_color_monochrome,
			k3d::interface_list<k3d::ibitmap_source,
			k3d::interface_list<k3d::ibitmap_sink> > > factory(
				k3d::uuid(0xa51271ab, 0xfc43d4b9, 0xdec56aab, 0x35605d9c),
				"CUDABitmapColorMonochrome",
				_("Make any image black and white"),
				"CUDABitmap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_red_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_green_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blue_weight;
	// private member containers for timing measurements
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_host_to_device_time;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_kernel_time;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_device_to_host_time;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_color_monochrome

k3d::iplugin_factory& cuda_color_monochrome_factory()
{
	return cuda_color_monochrome::get_factory();
}

} // namespace cuda_bitmap

} // namespace module

