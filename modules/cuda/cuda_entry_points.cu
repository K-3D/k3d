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

// cuda includes
#include <stdio.h>
#include <cutil.h>
#include <vector_types.h>
//include the kernels
#include "cuda_kernels.cu"

// define the externals
#include "cuda_entry_points.h"

/**
 * Initialize the timing info structure
 */
void initTimingInfo(timingInfo_t* tInfo, int numberOfEntries)
{
	(*tInfo).numEntries = numberOfEntries;
	(*tInfo).timings = (double*)malloc ( numberOfEntries*sizeof(double) );
	(*tInfo).labels = (char**)malloc ( numberOfEntries*sizeof(char*) );
	for ( int i = 0 ; i < numberOfEntries ; i++ )
	{
		(*tInfo).labels[i] = (char*) malloc ( 33*sizeof(char) );
	}	
} 

/**
 * Set the label of a given timing_info entry
 */
inline void setTimingInfoLabel(timingInfo_t* tInfo, int index, char* label)
{
	sprintf((*tInfo).labels[index], "%s", label);
}

/**
 * Set the timing value of a given timing_info entry
 */
inline void setTimingInfoTiming(timingInfo_t* tInfo, int index, double timingVal)
{
	(*tInfo).timings[index] = timingVal;
}

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
    		add_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, -value);
    		break;
        case CUDA_BITMAP_GAMMA:
            // execute the gamma kernel
            gamma_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, value);
    	default:
    		// unknown operation 
    		;
	}
		
    // check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
    // Make sure this function blocks until the calculation is complete
    cudaThreadSynchronize();
}

extern "C" void bitmap_color_monochrome_kernel_entry(unsigned short* p_deviceImage, int width, int height, float redWeight, float greenWeight, float blueWeight)
{
	// allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));
	
	color_monochrome_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, redWeight, greenWeight, blueWeight);
    	
    // check if the kernel executed correctly
    CUT_CHECK_ERROR("Add Kernel execution failed");
    cudaThreadSynchronize();
	
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
	
	// initialize the timing info structure
	initTimingInfo(tInfo, 6);
    
    setTimingInfoLabel(tInfo, OTHER, "OTHER");
	setTimingInfoLabel(tInfo, CONVERT_PRE, "CONVERT_PRE");
	setTimingInfoLabel(tInfo, TO_DEVICE, "TO_DEVICE");
	setTimingInfoLabel(tInfo, EXECUTE, "EXECUTE");
	setTimingInfoLabel(tInfo, TO_HOST, "TO_HOST");
	setTimingInfoLabel(tInfo, CONVERT_POST, "CONVERT_POST");
		
	
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
    setTimingInfoTiming(tInfo, OTHER, cutGetTimerValue( timer));
		
	
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
	setTimingInfoTiming(tInfo, CONVERT_PRE, cutGetTimerValue( timer));	
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));		
	CUDA_SAFE_CALL ( cudaMemcpy(device_points, host_points_single_p, num_points*16, cudaMemcpyHostToDevice) );
	CUT_SAFE_CALL( cutStopTimer( timer));
	setTimingInfoTiming(tInfo, TO_DEVICE, cutGetTimerValue( timer));
		
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)(device_points), num_points);
	cudaThreadSynchronize();
	CUT_SAFE_CALL( cutStopTimer( timer));
	setTimingInfoTiming(tInfo, EXECUTE, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	CUDA_SAFE_CALL ( cudaMemcpy(host_points_single_p, device_points, num_points*16, cudaMemcpyDeviceToHost) );
	CUT_SAFE_CALL( cutStopTimer( timer));
	setTimingInfoTiming(tInfo, TO_HOST, cutGetTimerValue( timer));
		
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
	setTimingInfoTiming(tInfo, CONVERT_POST, cutGetTimerValue( timer));
		
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
	#define OTHER 0
	#define STREAM_CREATE 1
	#define PHASE_1 2
	#define PHASE_2 3
	#define STREAM_DESTROY 4
	
	// initialize the timing info structure
	initTimingInfo(tInfo, 5);
    
    setTimingInfoLabel(tInfo, OTHER, "OTHER");
	setTimingInfoLabel(tInfo, STREAM_CREATE, "STREAM_CREATE");
	setTimingInfoLabel(tInfo, PHASE_1, "CONVERT_TO_DEVICE_EXECUTE");
	setTimingInfoLabel(tInfo, PHASE_2, "TO_HOST_CONVERT");
	setTimingInfoLabel(tInfo, STREAM_DESTROY, "STREAM_DESTROY");
	
	// use a cuda timer for timing
	unsigned int timer = 0;
	CUT_SAFE_CALL( cutCreateTimer( &timer ) );
	
	CUT_SAFE_CALL( cutStartTimer( timer));
	// set the number of streams
	int nstreams = 4;
	
    float *device_points;
	// allocate the memory on the device - 16 bytes per point
	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);	
	
	// allocate pinned host memory to allow for asynchronous operations	
	float *host_points_single_p;
	allocate_pinned_host_memory ((void**)&host_points_single_p, num_points*sizeof(float)*4);
	
	int points_per_stream = num_points/nstreams;

	dim3 threads_per_block(32, 1);
	dim3 blocks_per_grid( iDivUp(points_per_stream, 32), 1);
	
	CUT_SAFE_CALL( cutStopTimer( timer));
    setTimingInfoTiming(tInfo, OTHER, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	// allocate and initialize an array of stream handles
    cudaStream_t *streams = (cudaStream_t*) malloc(nstreams * sizeof(cudaStream_t));
    for(int n = 0; n < nstreams; n++)
    	CUDA_SAFE_CALL( cudaStreamCreate(&(streams[n])) ); 
	
	CUT_SAFE_CALL( cutStopTimer( timer));
    setTimingInfoTiming(tInfo, STREAM_CREATE, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	for ( int n = 0; n < nstreams; n++ )
	{
		// Convert a subset of the data to floats 
		for (int point = n*points_per_stream; point < (n+1)*points_per_stream; ++point)
		{
			int float_index = (point)*4;
			int double_index = (point)*3;
			host_points_single_p[float_index] = (float)InputPoints[double_index];
			host_points_single_p[float_index+1] = (float)InputPoints[double_index+1];
			host_points_single_p[float_index+2] = (float)InputPoints[double_index+2];
			host_points_single_p[float_index+3] = (float)PointSelection[point];
		}
		
		// for each stream copy the data to the device and execute the kernel
		CUDA_SAFE_CALL ( cudaMemcpyAsync(device_points + n*points_per_stream*4, host_points_single_p + n*points_per_stream*4, points_per_stream*16, cudaMemcpyHostToDevice, streams[n]) );
		linear_transform_kernel <<< blocks_per_grid, threads_per_block, 0, streams[n] >>> ((float4*)(device_points + n*points_per_stream*4), points_per_stream);
	}
	CUT_SAFE_CALL( cutStopTimer( timer));
    setTimingInfoTiming(tInfo, PHASE_1, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	// copy the data back from the device and convert
	for ( int n = 0; n < nstreams; n++ )
	{
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
	CUT_SAFE_CALL( cutStopTimer( timer));
	setTimingInfoTiming(tInfo, PHASE_2, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	// release resources
	for(int n = 0; n < nstreams; n++)
	{
    	cudaStreamDestroy(streams[n]);
	}
	CUT_SAFE_CALL( cutStopTimer( timer));
	setTimingInfoTiming(tInfo, STREAM_DESTROY, cutGetTimerValue( timer));
	
	CUT_SAFE_CALL( cutResetTimer( timer));
	CUT_SAFE_CALL( cutStartTimer( timer));
	free_device_memory(device_points);
	free_pinned_host_memory ( host_points_single_p );
	CUT_SAFE_CALL( cutStopTimer( timer));
	(*tInfo).timings[OTHER] += cutGetTimerValue( timer);	
	CUT_SAFE_CALL ( cutDeleteTimer ( timer ));
}

extern "C" void subdivide_edges_split_point_calculator ( unsigned int* edge_indices, 
                                                         unsigned int num_edge_indices, 
                                                         float* points_and_selection,
                                                         unsigned int num_points,  
                                                         unsigned int* edge_point_indices,
                                                         unsigned int* clockwise_edge_indices,
                                                         float* new_points_and_selection,
                                                         int num_split_points )
{
    int threads_x = 64 / num_split_points;
    
    dim3 threads_per_block(threads_x, num_split_points);
    dim3 blocks_per_grid( iDivUp(num_points, threads_x), 1);
    
    printf("CUDA DEBUG: Calling split kernel\n");
    
    subdivide_edges_split_point_kernel<<< blocks_per_grid, threads_per_block >>> ( edge_indices, 
                                                                                   num_edge_indices, 
                                                                                   (float4*)points_and_selection, 
                                                                                   num_points,
                                                                                   (float4*)new_points_and_selection, 
                                                                                   edge_point_indices, 
                                                                                   clockwise_edge_indices, 
                                                                                   num_split_points );  
    
    // check if the kernel executed correctly
    //CUT_CHECK_ERROR("Kernel execution failed");
    
    cudaError_t last_error = cudaGetLastError();
    
    printf("CUDA ERROR: %s\n", cudaGetErrorString(last_error));
    
    cudaThreadSynchronize();
}

extern "C" void copy_2D_from_host_to_device_with_padding ( void* device_pointer, const void* host_pointer, size_t device_pitch, size_t host_pitch, size_t width_in_bytes, size_t rows )
{
    CUDA_SAFE_CALL ( cudaMemcpy2D(device_pointer, device_pitch, host_pointer, host_pitch, width_in_bytes, rows, cudaMemcpyHostToDevice) );
}

/**
 * Call thread synchronize to ensure consistency
 */
extern "C" void synchronize_threads ()
{
    cudaThreadSynchronize();    
}
