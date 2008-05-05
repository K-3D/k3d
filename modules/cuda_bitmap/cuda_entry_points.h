#ifndef CUDA_ENTRY_POINTS_H
#define CUDA_ENTRY_POINTS_H

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
	\author Evan Lezar (evanlezar@gmail.com)
*/

// forward declaration of the entry functions
// split the entry functions for timing reasons
extern "C" void CUDA_initialize_device();
extern "C" void bitmap_copy_data_from_host_to_device(const unsigned short *input, int width, int height);
extern "C" void bitmap_add_entry(int width, int height, float value);
extern "C" void bitmap_copy_data_from_device_to_host(unsigned short *output, int width, int height);
extern "C" void CUDA_cleanup();

#endif // !CUDA_ENTRY_POINTS_H
