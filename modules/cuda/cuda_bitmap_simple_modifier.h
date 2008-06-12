#ifndef LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H
#define LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H

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

#include <k3dsdk/bitmap_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/ipipeline_profiler.h>

// include the entry points as external definitions
#include "cuda_entry_points.h"

namespace module
{

namespace cuda
{

class cuda_bitmap_simple_modifier :
	public k3d::node,
	public k3d::bitmap_modifier<cuda_bitmap_simple_modifier>
{
	typedef k3d::node base;

public:
	cuda_bitmap_simple_modifier(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

protected:
    void start_profile_step()
    {
        document().pipeline_profiler().start_execution(*this, "");
    }    
    void stop_profile_step(const std::string& Task)
    {
        document().pipeline_profiler().finish_execution(*this, Task);
    }
	
	void bitmap_arithmetic(const k3d::bitmap& Input, k3d::bitmap& Output, const k3d::int32_t bitmapOperation, float value)
	{
		// intialize CUDA - should check for errors etc
		CUDA_initialize_device();
		const unsigned short* inputPixels = reinterpret_cast<const unsigned short*>(&(const_view(Input)[0]));
		unsigned short* outputPixels = reinterpret_cast<unsigned short*>(&(view(Output)[0]));
		k3d::int32_t sizeInBytes = Input.width()*Input.height()*8;
		
		start_profile_step();
		
		// allocate device storage for the image at 8 bits per pixel
		unsigned short *p_device_image;
		allocate_device_memory((void**)&p_device_image, sizeInBytes);
		// copy the data to the device		
		copy_from_host_to_device(p_device_image, inputPixels, sizeInBytes);  		
		stop_profile_step(PROFILE_STRING_HOST_TO_DEVICE);
		
		// perform the calculation
		start_profile_step();
		bitmap_arithmetic_kernel_entry(bitmapOperation, p_device_image, Input.width(), Input.height(), value);
		stop_profile_step(PROFILE_STRING_EXECUTE_KERNEL);
		
		// copy the data from the device
		start_profile_step();
		copy_from_device_to_host(outputPixels, p_device_image, sizeInBytes);
		stop_profile_step(PROFILE_STRING_DEVICE_TO_HOST);
		
		// free the memory allocated on the device
		free_device_memory(p_device_image);			
	} 
	
private:
	virtual void on_create_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		Output.recreate(Input.width(), Input.height());
	}

};

} // namespace cuda

} // namespace module

#endif // !LIBK3DBITMAP_SIMPLE_BITMAP_MODIFIER_H

