// cuda includes
#include <cutil.h>
#include <vector_types.h>
//include the kernels
#include "cuda_kernels.cu"

#include <stdio.h>

// forward declaration of the entry functions
extern "C" void bitmap_add_entry(const unsigned short *input, unsigned short *output, int width, int height, float value);

// integer division and rounding up
int iDivUp(int a, int b)
{
	// if a is not divisible by b, return a/b + 1, else return a/b
	return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

extern "C" void bitmap_add_entry(const unsigned short *input, unsigned short *output, int width, int height, float value)
{
	// initialise the device
	CUT_DEVICE_INIT();
	
 	// width and height is the number of pixels - each pixel is 4*16bits = 8bytes
	const unsigned int input_size = 8 * width * height;
    
    // pointer to the image on the device
    ushort4 *d_image;
    // allocate the memory on the device    
    CUDA_SAFE_CALL(cudaMalloc((void**)&d_image, input_size));
    
    // copy the data to the device
    CUDA_SAFE_CALL(cudaMemcpy(d_image, input, input_size, cudaMemcpyHostToDevice));
    
    // allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));

    // execute the kernel
    add_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, value);
    
    // check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
	
	
	//CUDA_SAFE_CALL(cudaMemcpy(output, input, input_size, cudaMemcpyHostToHost));
	 // copy the results to the output image
    CUDA_SAFE_CALL(cudaMemcpy(output, d_image, input_size, cudaMemcpyDeviceToHost));

    // cleanup memory
    CUDA_SAFE_CALL(cudaFree(d_image));
	
    	
}

