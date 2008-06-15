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

/**
 * Integer division and rounding up
 */
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
extern "C" void bitmap_arithmetic_kernel_entry(int operation, unsigned short* p_deviceImage, int width, int height, float value)
{
    // allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));

	switch ( operation )
	{
    	case CUDA_BITMAP_ADD:
    		// execute the add
    		add_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, value);
    		break;
    	case CUDA_BITMAP_MULTIPLY:
    		// execute the multiply kernel
    		multiply_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, value);
    		break;
    	case CUDA_BITMAP_SUBTRACT:
    		// execute the add kernel with value negated
    		add_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)d_image, width, height, -value);
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

extern "C" void free_device_memory ( void* device_pointer )
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

extern "C" void transform_points_synchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo )
{
	#define OTHER 0
	#define CONVERT_PRE 1
	#define TO_DEVICE 2
	#define EXECUTE 3
	#define TO_HOST 4
	#define CONVERT_POST 5

    // setup the timing info struct - stores time in s
	(*tInfo).numEntries = 6;
	(*tInfo).timings = (double*)malloc ( 6*sizeof(double) );
	(*tInfo).labels = (char**)malloc ( 6*sizeof(char*) );
	for ( int i = 0 ; i < 6 ; i++ )
	{
		(*tInfo).labels[i] = (char*) malloc ( 33*sizeof(char) );
	}
	sprintf((*tInfo).labels[OTHER], "%s", "OTHER");
	sprintf((*tInfo).labels[CONVERT_PRE], "%s", "CONVERT_PRE");
	sprintf((*tInfo).labels[TO_DEVICE], "%s", "TO_DEVICE");
	sprintf((*tInfo).labels[EXECUTE], "%s", "EXECUTE");
	sprintf((*tInfo).labels[TO_HOST], "%s", "TO_HOST");
	sprintf((*tInfo).labels[CONVERT_POST], "%s", "CONVERT_POST");
	
	// use a cuda timer for timing
	unsigned int timer = 0;
	CUT_SAFE_CALL( cutCreateTimer( &timer ) );
	
	CUT_SAFE_CALL( cutStartTimer( timer));
    float *device_points;
	
	// allocate the memory on the device - 16 bytes per point
	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);	
	
	// allocate pinned host memory to allow for asynchronous operations	
	float *host_points_single_p;
	allocate_pinned_host_memory ((void**)&host_points_single_p, num_points*sizeof(float)*4);
	
	dim3 threads_per_block(64, 1);
	dim3 blocks_per_grid( iDivUp(num_points, 64), 1);
	
	CUT_SAFE_CALL( cutStopTimer( timer));
    (*tInfo).timings[OTHER] = cutGetTimerValue( timer);
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	for (int point = 0; point < num_points; ++point)
	{
		int float_index = (point)*4;
		int double_index = (point)*3;
		host_points_single_p[float_index] = (float)InputPoints[double_index];
		host_points_single_p[float_index+1] = (float)InputPoints[double_index+1];
		host_points_single_p[float_index+2] = (float)InputPoints[double_index+2];
		host_points_single_p[float_index+3] = (float)PointSelection[point];
	}
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[CONVERT_PRE] = cutGetTimerValue( timer);	
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));		
	CUDA_SAFE_CALL ( cudaMemcpy(device_points, host_points_single_p, num_points*16, cudaMemcpyHostToDevice) );
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[TO_DEVICE] = cutGetTimerValue( timer);	
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)(device_points), num_points);
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[EXECUTE] = cutGetTimerValue( timer);	
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	CUDA_SAFE_CALL ( cudaMemcpy(host_points_single_p, device_points, num_points*16, cudaMemcpyDeviceToHost) );
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[TO_HOST] = cutGetTimerValue( timer);	
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	for (int point = 0; point < num_points; ++point)
	{
		int float_index = (point)*4;
		int double_index = (point)*3;
		OutputPoints[double_index] = host_points_single_p[float_index];
		OutputPoints[double_index+1] = host_points_single_p[float_index+1];
		OutputPoints[double_index+2] = host_points_single_p[float_index+2];
	}
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[CONVERT_POST] = cutGetTimerValue( timer);
	
	CUT_SAFE_CALL( cutResetTimer( timer));	
	CUT_SAFE_CALL( cutStartTimer( timer));
	free_device_memory(device_points);
	free_pinned_host_memory ( host_points_single_p );
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[OTHER] += cutGetTimerValue( timer);	
	CUT_SAFE_CALL ( cutDeleteTimer ( timer ));
}

extern "C" void transform_points_asynchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo )
{
	// set the number of streams
	int nstreams = 2;
	
	
    float *device_points;
	
	// allocate the memory on the device - 16 bytes per point
	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);	
	
	// allocate pinned host memory to allow for asynchronous operations	
	float *host_points_single_p;
	allocate_pinned_host_memory ((void**)&host_points_single_p, num_points*sizeof(float)*4);
	
	// allocate and initialize an array of stream handles
    cudaStream_t *streams = (cudaStream_t*) malloc(nstreams * sizeof(cudaStream_t));
    for(int n = 0; n < nstreams; n++)
    	CUDA_SAFE_CALL( cudaStreamCreate(&(streams[n])) ); 
	
	int points_per_stream = num_points/nstreams;

	dim3 threads_per_block(32, 1);
	dim3 blocks_per_grid( iDivUp(points_per_stream, 32), 1);
	
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

		CUDA_SAFE_CALL ( cudaMemcpyAsync(device_points + n*points_per_stream*4, host_points_single_p + n*points_per_stream*4, points_per_stream*16, cudaMemcpyHostToDevice, streams[n]) );
		linear_transform_kernel <<< blocks_per_grid, threads_per_block, 0, streams[n] >>> ((float4*)(device_points + n*points_per_stream*4), points_per_stream);
		//linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)(device_points + n*points_per_stream*16), points_per_stream);
		CUDA_SAFE_CALL ( cudaMemcpyAsync(host_points_single_p + n*points_per_stream*4, device_points + n*points_per_stream*4, points_per_stream*16, cudaMemcpyDeviceToHost, streams[n]) );
		
		// need to synchronize the streams so that the data is available to copy to the output points	
		cudaStreamSynchronize(streams[n]);		
		for (int point = n*points_per_stream; point < (n+1)*points_per_stream; ++point)
		{
			int float_index = (point)*4;
			int double_index = (point)*3;
			OutputPoints[double_index] = host_points_single_p[float_index];
			OutputPoints[double_index+1] = host_points_single_p[float_index+1];
			OutputPoints[double_index+2] = host_points_single_p[float_index+2];
		}
	
	}
	
	// release resources
	for(int n = 0; n < nstreams; n++)
	{
    	cudaStreamDestroy(streams[n]);
	}
	
	free_device_memory(device_points);
	free_pinned_host_memory ( host_points_single_p );
}

