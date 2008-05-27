// cuda includes
#include <stdio.h>
#include <cutil.h>
#include <vector_types.h>
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

/// entry point for the CUDA version of the BitmapAdd BitmapSubtract and BitmapMultiply plugin
extern "C" void bitmap_arithmetic_kernel_entry(int operation, int width, int height, float value)
{
    // allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));

	switch ( operation )
	{
    	case CUDA_BITMAP_ADD:
    		// execute the add
    		add_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, value);
    		break;
    	case CUDA_BITMAP_MULTIPLY:
    		// execute the multiply kernel
    		multiply_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, value);
    		break;
    	case CUDA_BITMAP_SUBTRACT:
    		// execute the add kernel with value negated
    		add_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, -value);
    		break
    		;
    	default:
    		// unknown operation
    		;
	}
		
    // check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
    // Make sure this function blocks until the calculation is complete
    cudaThreadSynchronize();
}

extern "C" void bitmap_color_monochrome_kernel_entry(int width, int height, float redWeight, float greenWeight, float blueWeight)
{
	// allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));
	
	color_monochrome_kernel<<< blocks_per_grid, threads_per_block >>> (d_image, width, height, redWeight, greenWeight, blueWeight);
    	
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


extern "C" void apply_linear_transform_to_point_data ( float *device_points, float *device_matrix, int num_points )
{
	dim3 threads_per_block(64, 1);
    dim3 blocks_per_grid( iDivUp(num_points, 64), 1);
	
	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)device_points, (float4*)device_matrix, num_points);
	
	// check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");					
}

extern "C" void test_double_to_float_entry ( double *in, float *out, int num )
{
	dim3 threads_per_block(8, 1);
    dim3 blocks_per_grid( iDivUp(num, 8), 1);
	
	test_double_to_float<<< blocks_per_grid, threads_per_block >>> (in, out, num);
	
	// check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");		
}

extern "C" void allocate_device_memory ( void** device_pointer, int size_in_bytes )
{
	CUDA_SAFE_CALL(cudaMalloc(device_pointer, size_in_bytes));	
}

extern "C" void copy_from_host_to_device ( void* device_pointer, const void* host_pointer, int size_in_bytes )
{
	CUDA_SAFE_CALL(cudaMemcpy(device_pointer, host_pointer, size_in_bytes, cudaMemcpyHostToDevice));	
}

extern "C" void copy_from_device_to_host ( void* host_pointer, const void* device_pointer, int size_in_bytes )
{
	CUDA_SAFE_CALL(cudaMemcpy(host_pointer, device_pointer, size_in_bytes, cudaMemcpyDeviceToHost));	
}

extern "C" void free_cuda_pointer ( void* device_pointer )
{
	CUDA_SAFE_CALL(cudaFree(device_pointer));			
}
