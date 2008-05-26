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
// cuda_multiply

class cuda_multiply :
	public simple_modifier
{
	typedef simple_modifier base;

public:
	cuda_multiply(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Multiplicand")) + init_description(_("Multiply each pixel component with this value")) + init_value(1.0)),
		m_host_to_device_time(init_owner(*this) + init_name("host_to_device_time") + init_label(_("Host to device time")) + init_description(_("Timer for measuring host to device transfer time")) + init_value(0.0)),
		m_kernel_time(init_owner(*this) + init_name("kernel_time") + init_label(_("Kernel time")) + init_description(_("Timer for measuring kernel execution time")) + init_value(0.0)),
		m_device_to_host_time(init_owner(*this) + init_name("device_to_host_time") + init_label(_("Device to host time")) + init_description(_("Timer for measuring device to host transfer time")) + init_value(0.0))
	{
		m_value.changed_signal().connect(make_update_bitmap_slot());
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
		bitmap_arithmetic_kernel_entry(CUDA_BITMAP_MULTIPLY, Input.width(), Input.height(), (float)(m_value.pipeline_value()));
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
		static k3d::document_plugin_factory<cuda_multiply,
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
	// private member containers for timing measurements
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_host_to_device_time;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_kernel_time;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, with_serialization) m_device_to_host_time;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_multiply

k3d::iplugin_factory& cuda_multiply_factory()
{
	return cuda_multiply::get_factory();
}

} // namespace cuda_bitmap

} // namespace module

