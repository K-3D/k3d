// cuda includes
#include <cutil.h>
#include <vector_types.h>
#include <stdio.h>
//include the kernels
#include "cuda_kernels.cu"

// define the externals
#include "cuda_entry_points.h"

// pointer to the image in device memory
static ushort4 *d_image = 0;
unsigned int input_size = 0;

// integer division and rounding up
int iDivUp(int a, int b)
{
	// if a is not divisible by b, return a/b + 1, else return a/b
	return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

extern "C" void CUDA_initialize_device()
{
	// initialise the device
	CUT_DEVICE_INIT();
}
extern "C" void bitmap_copy_data_from_host_to_device(const unsigned short *input, int width, int height)
{
	// width and height is the number of pixels - each pixel is 4*16bits = 8bytes
	input_size = 8 * width * height;
    
    // allocate the memory on the device    
    CUDA_SAFE_CALL(cudaMalloc((void**)&d_image, input_size));

    // copy the data to the device
    CUDA_SAFE_CALL(cudaMemcpy(d_image, input, input_size, cudaMemcpyHostToDevice));	
	
}

/// entry point for the CUDA version of the BitmapAdd plugin
extern "C" void bitmap_add_entry(int width, int height, float value)
{
    // allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));

    // execute the kernel
    add_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, value);
    
    // check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
}

extern "C" void bitmap_copy_data_from_device_to_host(unsigned short *output, int width, int height)
{
	// copy the results to the output image
    CUDA_SAFE_CALL(cudaMemcpy(output, d_image, input_size, cudaMemcpyDeviceToHost));
}

extern "C" void CUDA_cleanup()
{
	// cleanup memory
    CUDA_SAFE_CALL(cudaFree(d_image));	
}