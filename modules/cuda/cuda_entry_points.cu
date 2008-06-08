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
    		break;
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
    cudaThreadSynchronize();
	
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

extern "C" void copy_and_bind_texture_to_array( void** cudaArrayPointer, float* arrayData, int width, int height )
{
	// alocate a cudaArray to store the transformation matrix
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	cudaArray* cu_array;
	CUDA_SAFE_CALL( cudaMallocArray( &cu_array, &channelDesc, width, height )); 
    CUDA_SAFE_CALL( cudaMemcpyToArray( cu_array, 0, 0, arrayData, width*height*sizeof(float), cudaMemcpyHostToDevice));
	
	// set texture parameters
    transformTexture.addressMode[0] = cudaAddressModeClamp;
    transformTexture.addressMode[1] = cudaAddressModeClamp;
    transformTexture.filterMode = cudaFilterModePoint;
    transformTexture.normalized = false;
	
	// Bind the array to the texture
    CUDA_SAFE_CALL( cudaBindTextureToArray( transformTexture, cu_array, channelDesc));
	
	*cudaArrayPointer = (void*)cu_array;
}

extern "C" void free_CUDA_array ( void* cudaArrayPointer )
{
	CUDA_SAFE_CALL(cudaFreeArray((cudaArray*)cudaArrayPointer));			
}

extern "C" void apply_linear_transform_to_point_data ( float *device_points, float *T_matrix, int num_points )
{
	dim3 threads_per_block(64, 1);
    dim3 blocks_per_grid( iDivUp(num_points, 64), 1);
	
	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)device_points, num_points);
	
	// check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
    cudaThreadSynchronize();
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

extern "C" void allocate_pinned_host_memory ( void** pointer_on_host, size_t size_in_bytes )  
{
	CUDA_SAFE_CALL(cudaMallocHost(pointer_on_host, size_in_bytes));	
}

extern "C" void free_pinned_host_memory ( void* pointer_on_host )  
{
	CUDA_SAFE_CALL(cudaFreeHost(pointer_on_host));	
}

extern "C" void test_stream_implementation ( double *InputPoints, double *PointSelection, float* host_points_single_p, int num_points )
{
	// set the number of streams
	int nstreams = 4;
    float *device_points;
	
	// allocate the memory on the device - 16 bytes per point
	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);	
		
		
	// allocate and initialize an array of stream handles
    cudaStream_t *streams = (cudaStream_t*) malloc(nstreams * sizeof(cudaStream_t));
    for(int i = 0; i < nstreams; i++)
    	CUDA_SAFE_CALL( cudaStreamCreate(&(streams[i])) ); 
	
	int points_per_stream = num_points/nstreams;
	
#ifdef __DEVICE_EMULATION__	
	printf("Num points : %d : Points per stream : %d\n", num_points, points_per_stream); 
	printf("device base : %x : host base : %x\n", device_points, host_points_single_p);
#endif
	
	dim3 threads_per_block(64, 1);
	dim3 blocks_per_grid( iDivUp(points_per_stream, 64), 1);
	
	for ( int n = 0; n < nstreams; n++ )
	{
		for (int point = n*points_per_stream; point < (n+1)*points_per_stream; ++point)
		{
			int float_index = (point)*4;
			int double_index = (point)*3;
			host_points_single_p[float_index] = (float)InputPoints[double_index];
			host_points_single_p[float_index+1] = (float)InputPoints[double_index+1];
			host_points_single_p[float_index+2] = (float)InputPoints[double_index+2];
			host_points_single_p[float_index+3] = (float)PointSelection[point];
		}
		
#ifdef __DEVICE_EMULATION__		
		printf("device adr : %x : host adr : %x : num bytes : %d \n", device_points + n*points_per_stream*4, host_points_single_p + n*points_per_stream*4, points_per_stream*16);
#endif

		CUDA_SAFE_CALL ( cudaMemcpyAsync(device_points + n*points_per_stream*4, host_points_single_p + n*points_per_stream*4, points_per_stream*16, cudaMemcpyHostToDevice, streams[n]) );
		linear_transform_kernel <<< blocks_per_grid, threads_per_block, 0, streams[n] >>> ((float4*)(device_points + n*points_per_stream*4), points_per_stream);
		//linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)(device_points + n*points_per_stream*16), points_per_stream);
		CUDA_SAFE_CALL ( cudaMemcpyAsync(host_points_single_p + n*points_per_stream*4, device_points + n*points_per_stream*4, points_per_stream*16, cudaMemcpyDeviceToHost, streams[n]) );
				
	}
	//for (int point = n*points_per_stream; point < (n+1)*points_per_stream; ++point)
	
	// release resources
	for(int i = 0; i < nstreams; i++)
	{
		cudaStreamSynchronize(streams[i]);
    	cudaStreamDestroy(streams[i]);
	}
	
	free_cuda_pointer(device_points);
}

