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
#include <vector_types.h>
//include the kernels
#include "cuda_kernels.cu"

// define the externals
#include "cuda_entry_points.h"

#ifdef K3D_API_WIN32

/// Retrieves a timestamp in seconds using the Win32 high performance counters
inline double nanotime()
{
	LARGE_INTEGER timestamp;
	LARGE_INTEGER frequency;
	if ( !(QueryPerformanceCounter(&timestamp) && QueryPerformanceFrequency(&frequency)) )
	{
		return 0.0;
	}

	return static_cast<double>(timestamp.QuadPart) / static_cast<double>(frequency.QuadPart);
}

#else // K3D_API_WIN32

/// Retrieves a timestamp in seconds using gettimeofday() for portable timing
inline double nanotime()
{
	timeval tv;
	gettimeofday(&tv, 0);

	return tv.tv_sec + static_cast<double>(tv.tv_usec) / 1000000;
}

#endif // !K3D_API_WIN32


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
 * Initialize the timing_info entry to the current time
 */
inline void startTimingInfoTimer (timingInfo_t* tInfo, int index)
{
	(*tInfo).timings[index] = nanotime();
}
/**
 * Set the timing value of a given timing_info entry to the elapsed time since it was started
 */
inline void measureTimingInfoTimer(timingInfo_t* tInfo, int index)
{
	(*tInfo).timings[index] = nanotime() - (*tInfo).timings[index];
}

/**
 * Integer division and rounding up
 */
int iDivUp(int a, int b)
{
	// if a is not divisible by b, return a/b + 1, else return a/b
	return ((a % b) != 0) ? (a / b + 1) : (a / b);
}

/**
 * Get the last CUDA error and display it if required
 */
extern "C" void checkLastCudaError ()
{
	cudaThreadSynchronize();
	cudaError_t error = cudaGetLastError();
	if ( error != cudaSuccess )
	{
		printf("CUDA ERROR: %s\n", cudaGetErrorString(error));
	}
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
            break;
        case CUDA_BITMAP_INVERT:
            // excute the bitmap invert kernel
            invert_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height);
            break;
        case CUDA_BITMAP_MATTE_COLORDIFF:
            matte_color_diff_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, value);
            break;
        case CUDA_BITMAP_MATTE_INVERT:
            matte_invert_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height);
            break;
    	default:
    		// unknown operation
    		;
	}

    // check if the kernel executed correctly
    checkLastCudaError();
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
    checkLastCudaError();
    cudaThreadSynchronize();

}

extern "C" void bitmap_threshold_kernel_entry(unsigned short* p_deviceImage, int width, int height, float redThreshold, float greenThreshold, float blueThreshold, float alphaThreshold)
{
    // allocate the blocks and threads
    dim3 threads_per_block(8, 8);
    dim3 blocks_per_grid( iDivUp(width, 8), iDivUp(height,8));

    threshold_kernel<<< blocks_per_grid, threads_per_block >>> ((ushort4*)p_deviceImage, width, height, redThreshold, greenThreshold, blueThreshold, alphaThreshold);

    // check if the kernel executed correctly
    checkLastCudaError();
    cudaThreadSynchronize();

}

extern "C" void copy_and_bind_texture_to_array( void** cudaArrayPointer, float* arrayData, int width, int height )
{
	// alocate a cudaArray to store the transformation matrix
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
	cudaArray* cu_array;
	cudaMallocArray( &cu_array, &channelDesc, width, height );
    cudaMemcpyToArray( cu_array, 0, 0, arrayData, width*height*sizeof(float), cudaMemcpyHostToDevice);

	// set texture parameters
    transformTexture.addressMode[0] = cudaAddressModeClamp;
    transformTexture.addressMode[1] = cudaAddressModeClamp;
    transformTexture.filterMode = cudaFilterModePoint;
    transformTexture.normalized = false;

	// Bind the array to the texture
    cudaBindTextureToArray( transformTexture, cu_array, channelDesc);

	*cudaArrayPointer = (void*)cu_array;
}

extern "C" void free_CUDA_array ( void* cudaArrayPointer )
{
	cudaFreeArray((cudaArray*)cudaArrayPointer);
}

extern "C" void apply_linear_transform_to_point_data ( float *device_points, float *T_matrix, int num_points )
{
	dim3 threads_per_block(64, 1);
    dim3 blocks_per_grid( iDivUp(num_points, 64), 1);

	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)device_points, num_points);

	// check if the kernel executed correctly
    checkLastCudaError();
    cudaThreadSynchronize();
}

extern "C" void allocate_device_memory ( void** device_pointer, int size_in_bytes )
{
	cudaMalloc(device_pointer, size_in_bytes);
}

extern "C" void set_device_memory ( void* device_pointer, int value, int size_in_bytes )
{
	cudaMemset(device_pointer, value, size_in_bytes);
}

extern "C" void resize_device_memory_block ( void** new_device_pointer, void* current_device_pointer, int new_size_in_bytes, int old_size_in_bytes, char clear )
{
	if ( new_size_in_bytes == old_size_in_bytes )
	{
		*new_device_pointer = current_device_pointer;
	}
	else
	{
		cudaMalloc(new_device_pointer, new_size_in_bytes);
		cudaThreadSynchronize();

		if ( !clear )
		{
			if ( new_size_in_bytes < old_size_in_bytes )
			{
				cudaMemcpy(*new_device_pointer, current_device_pointer, new_size_in_bytes, cudaMemcpyDeviceToDevice);
			}
			else
			{
				cudaMemset( *new_device_pointer, 0, new_size_in_bytes );
				cudaThreadSynchronize();
				cudaMemcpy(*new_device_pointer, current_device_pointer, old_size_in_bytes, cudaMemcpyDeviceToDevice);
			}
			cudaThreadSynchronize();
		}
		cudaFree ( current_device_pointer );
	}

	if ( clear )
	{
		cudaMemset( *new_device_pointer, 0, new_size_in_bytes );
		cudaThreadSynchronize();
	}
	else
	{
		cudaThreadSynchronize();
	}

}

extern "C" void copy_from_host_to_device ( void* device_pointer, const void* host_pointer, int size_in_bytes )
{
	cudaMemcpy(device_pointer, host_pointer, size_in_bytes, cudaMemcpyHostToDevice);
}

extern "C" void copy_from_host_to_device_64_to_32_convert ( void* device_pointer, const void* host_pointer, int size_in_bytes )
{
    #define NUM_THREADS 64
    int num_uints = size_in_bytes/sizeof(unsigned int);
    uint2* pdev_uint_64;

    allocate_device_memory((void**)&pdev_uint_64, size_in_bytes*2);
    copy_from_host_to_device((void*)pdev_uint_64, (const void*)host_pointer, size_in_bytes*2);

    dim3 threads_per_block(NUM_THREADS, 1);
    dim3 blocks_per_grid( iDivUp(num_uints, NUM_THREADS), 1);

    convert_uint_64_to_32_kernel <<< blocks_per_grid, threads_per_block >>> ( pdev_uint_64, (unsigned int*) device_pointer, num_uints);

    checkLastCudaError();

    cudaThreadSynchronize();
    free_device_memory ( pdev_uint_64 );
}

extern "C" void copy_from_device_to_host ( void* host_pointer, const void* device_pointer, int size_in_bytes )
{
	cudaMemcpy(host_pointer, device_pointer, size_in_bytes, cudaMemcpyDeviceToHost);
}

extern "C" void copy_from_device_to_host_32_to_64_convert ( void* host_pointer, const void* device_pointer, int size_in_bytes )
{
	#define NUM_THREADS 64
	int num_uints = size_in_bytes/sizeof(unsigned int);
	uint2* pdev_uint_64;

	allocate_device_memory((void**)&pdev_uint_64, size_in_bytes*2);

	dim3 threads_per_block(NUM_THREADS, 1);
	dim3 blocks_per_grid( iDivUp(num_uints, NUM_THREADS), 1);

	convert_uint_32_to_64_kernel <<< blocks_per_grid, threads_per_block >>> ( pdev_uint_64, (unsigned int*) device_pointer, num_uints);

	checkLastCudaError();

	copy_from_device_to_host(host_pointer, (const void*)pdev_uint_64, size_in_bytes*2);

	cudaThreadSynchronize();
	free_device_memory ( pdev_uint_64 );
}

extern "C" void copy_from_device_to_device ( void* device_dest_pointer, const void* device_source_pointer, int size_in_bytes )
{
    cudaMemcpy(device_dest_pointer, device_source_pointer, size_in_bytes, cudaMemcpyDeviceToDevice);
}
extern "C" void free_device_memory ( void* device_pointer )
{
	cudaFree(device_pointer);
}

extern "C" void allocate_pinned_host_memory ( void** pointer_on_host, int size_in_bytes )
{
	cudaMallocHost(pointer_on_host, size_in_bytes);
}

extern "C" void free_pinned_host_memory ( void* pointer_on_host )
{
	cudaFreeHost(pointer_on_host);
}

extern "C" void transform_points_synchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo )
{
	#define SETUP 0
	#define CONVERT_PRE 1
	#define TO_DEVICE 2
	#define EXECUTE 3
	#define TO_HOST 4
	#define CONVERT_POST 5
	#define CLEANUP 6

	// initialize the timing info structure
	initTimingInfo(tInfo, 7);

    setTimingInfoLabel(tInfo, SETUP, "SETUP");
	setTimingInfoLabel(tInfo, CONVERT_PRE, "CONVERT_PRE");
	setTimingInfoLabel(tInfo, TO_DEVICE, "TO_DEVICE");
	setTimingInfoLabel(tInfo, EXECUTE, "EXECUTE");
	setTimingInfoLabel(tInfo, TO_HOST, "TO_HOST");
	setTimingInfoLabel(tInfo, CONVERT_POST, "CONVERT_POST");
	setTimingInfoLabel(tInfo, CLEANUP, "CLEANUP");

	startTimingInfoTimer ( tInfo, SETUP );
    float *device_points;

	// allocate the memory on the device - 16 bytes per point
	allocate_device_memory((void**)&device_points, num_points*sizeof(float)*4);

	// allocate pinned host memory to allow for asynchronous operations
	float *host_points_single_p;
	allocate_pinned_host_memory ((void**)&host_points_single_p, num_points*sizeof(float)*4);

	dim3 threads_per_block(64, 1);
	dim3 blocks_per_grid( iDivUp(num_points, 64), 1);

	measureTimingInfoTimer( tInfo, SETUP );



    startTimingInfoTimer (tInfo, CONVERT_PRE);
	for (int point = 0; point < num_points; ++point)
	{
		int float_index = (point)*4;
		int double_index = (point)*3;
		host_points_single_p[float_index] = (float)InputPoints[double_index];
		host_points_single_p[float_index+1] = (float)InputPoints[double_index+1];
		host_points_single_p[float_index+2] = (float)InputPoints[double_index+2];
		host_points_single_p[float_index+3] = (float)PointSelection[point];
	}
	measureTimingInfoTimer (tInfo, CONVERT_PRE);


	startTimingInfoTimer (tInfo, TO_DEVICE);
	cudaMemcpy(device_points, host_points_single_p, num_points*16, cudaMemcpyHostToDevice);
	synchronize_threads();
	measureTimingInfoTimer (tInfo, TO_DEVICE);


	startTimingInfoTimer (tInfo, EXECUTE);
	linear_transform_kernel <<< blocks_per_grid, threads_per_block >>> ((float4*)(device_points), num_points);
	cudaThreadSynchronize();
	measureTimingInfoTimer ( tInfo, EXECUTE );

	startTimingInfoTimer ( tInfo, TO_HOST );
	cudaMemcpy(host_points_single_p, device_points, num_points*16, cudaMemcpyDeviceToHost);
	measureTimingInfoTimer ( tInfo, TO_HOST );

	startTimingInfoTimer ( tInfo, CONVERT_POST );
	for (int point = 0; point < num_points; ++point)
	{
		int float_index = (point)*4;
		int double_index = (point)*3;
		OutputPoints[double_index] = host_points_single_p[float_index];
		OutputPoints[double_index+1] = host_points_single_p[float_index+1];
		OutputPoints[double_index+2] = host_points_single_p[float_index+2];
	}
	measureTimingInfoTimer(tInfo, CONVERT_POST);

	startTimingInfoTimer ( tInfo, CLEANUP );
	free_device_memory(device_points);
	free_pinned_host_memory ( host_points_single_p );
	measureTimingInfoTimer ( tInfo, CLEANUP );
}

extern "C" void transform_points_asynchronous ( double *InputPoints, double *PointSelection, double *OutputPoints, int num_points, timingInfo_t* tInfo )
{
	#define SETUP 0
	#define STREAM_CREATE 1
	#define PHASE_1 2
	#define PHASE_2 3
	#define STREAM_DESTROY 4
	#define DEV_CLEANUP 5

	// initialize the timing info structure
	initTimingInfo(tInfo, 6);

    setTimingInfoLabel(tInfo, SETUP, "SETUP");
	setTimingInfoLabel(tInfo, STREAM_CREATE, "STREAM_CREATE");
	setTimingInfoLabel(tInfo, PHASE_1, "CONVERT_TO_DEVICE_EXECUTE");
	setTimingInfoLabel(tInfo, PHASE_2, "TO_HOST_CONVERT");
	setTimingInfoLabel(tInfo, STREAM_DESTROY, "STREAM_DESTROY");
	setTimingInfoLabel(tInfo, DEV_CLEANUP, "CLEANUP");



	startTimingInfoTimer ( tInfo, SETUP);
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
    measureTimingInfoTimer(tInfo, SETUP);

	startTimingInfoTimer ( tInfo, STREAM_CREATE);
	// allocate and initialize an array of stream handles
    cudaStream_t *streams = (cudaStream_t*) malloc(nstreams * sizeof(cudaStream_t));
    for(int n = 0; n < nstreams; n++)
    	cudaStreamCreate(&(streams[n]));
    measureTimingInfoTimer(tInfo, STREAM_CREATE);

    startTimingInfoTimer ( tInfo, PHASE_1);
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
		cudaMemcpyAsync(device_points + n*points_per_stream*4, host_points_single_p + n*points_per_stream*4, points_per_stream*16, cudaMemcpyHostToDevice, streams[n]);
		linear_transform_kernel <<< blocks_per_grid, threads_per_block, 0, streams[n] >>> ((float4*)(device_points + n*points_per_stream*4), points_per_stream);
	}
    measureTimingInfoTimer(tInfo, PHASE_1);

    startTimingInfoTimer ( tInfo, PHASE_2);
	// copy the data back from the device and convert
	for ( int n = 0; n < nstreams; n++ )
	{
		cudaMemcpyAsync(host_points_single_p + n*points_per_stream*4, device_points + n*points_per_stream*4, points_per_stream*16, cudaMemcpyDeviceToHost, streams[n]);
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
	measureTimingInfoTimer(tInfo, PHASE_2);

	startTimingInfoTimer ( tInfo, STREAM_DESTROY);
	// release resources
	for(int n = 0; n < nstreams; n++)
	{
    	cudaStreamDestroy(streams[n]);
	}
	measureTimingInfoTimer(tInfo, STREAM_DESTROY);

	startTimingInfoTimer ( tInfo, DEV_CLEANUP);
	free_device_memory(device_points);
	free_pinned_host_memory ( host_points_single_p );
	measureTimingInfoTimer(tInfo, DEV_CLEANUP);
}

extern "C" void subdivide_edges_split_point_calculator ( unsigned int* pdev_edge_indices,
                                                        unsigned int num_edge_indices,
                                                        float* pdev_points_and_selection,
                                                        unsigned int num_input_points,
                                                        unsigned int* pdev_edge_point_indices,
                                                        unsigned int* pdev_clockwise_edge_indices,
                                                        int num_split_points )
{
    // allocate device memory for the edge_indices
    if ( num_edge_indices > 0 & num_split_points > 0 & num_input_points > 0 )
    {
        int threads_x = 512 / num_split_points;

        dim3 threads_per_block(threads_x, num_split_points);
        dim3 blocks_per_grid( iDivUp(num_edge_indices, threads_x), 1);
        subdivide_edges_split_point_kernel<<< blocks_per_grid, threads_per_block >>> ( pdev_edge_indices,
                                                                                       num_edge_indices,
                                                                                       (float4*)pdev_points_and_selection,
                                                                                       num_input_points,
                                                                                       (float4*)(pdev_points_and_selection + num_input_points*4),
                                                                                       pdev_edge_point_indices,
                                                                                       pdev_clockwise_edge_indices,
                                                                                       num_split_points );

        checkLastCudaError();
    }
}

extern "C" void subdivide_edges_update_indices_entry (unsigned int* pdev_input_edge_point_indices,
                                                      unsigned int* pdev_input_clockwise_edge_point_indices,
                                                      unsigned int num_host_edges,
                                                      unsigned int* pdev_output_edge_point_indices,
                                                      unsigned int* pdev_output_clockwise_edge_point_indices,
                                                      unsigned int* pdev_edge_index_map,
                                                      int num_edge_maps)
{
    int threads_x = 512;
    dim3 threads_per_block(threads_x, 1);
    dim3 blocks_per_grid( iDivUp(num_edge_maps, threads_x), 1);


    subdivide_edges_update_edge_indices_kernel<<< blocks_per_grid, threads_per_block >>>
                                              ( pdev_output_edge_point_indices,
                                                pdev_output_clockwise_edge_point_indices,
                                                pdev_input_edge_point_indices,
                                                pdev_input_clockwise_edge_point_indices,
                                                pdev_edge_index_map,
                                                num_edge_maps );

    checkLastCudaError();
}

extern "C" void subdivide_edges_update_loop_first_edges_entry (
                                                        unsigned int* pdev_ouput_loop_first_edges,
                                                        unsigned int num_loops,
                                                        unsigned int* pdev_edge_index_map,
                                                        int num_edge_maps
                                                            )
{
    int threads_x = 64;
    dim3 threads_per_block(threads_x, 1);
    dim3 blocks_per_grid( iDivUp(num_loops, threads_x), 1);

    subdivide_edges_update_loop_first_edges_kernel<<< blocks_per_grid, threads_per_block >>>
                                              ( pdev_ouput_loop_first_edges,
                                                num_loops,
                                                pdev_edge_index_map );

    checkLastCudaError();
}

extern "C" void subdivide_edges_split_edges_entry ( unsigned int* pdev_output_edge_point_indices,
                                                    unsigned int* pdev_output_clockwise_edge_point_indices,
                                                    unsigned int* pdev_input_clockwise_edge_point_indices,
                                                    unsigned int* pdev_edge_index_map,
                                                    unsigned int* pdev_edge_indices,
                                                    unsigned int num_edge_indices,
                                                    int num_split_points,
                                                    unsigned int* pdev_first_midpoint,
                                                    unsigned int* pdev_companions,
                                                    unsigned char* pdev_boundary_edges
                                                    )
{
    int threads_x = 512 / num_split_points;
    dim3 threads_per_block(threads_x, num_split_points);
    dim3 blocks_per_grid( iDivUp(num_edge_indices, threads_x), 1);

    subdivide_edges_split_edges_kernel<<< blocks_per_grid, threads_per_block >>>
                                                   (pdev_output_edge_point_indices,
                                                    pdev_output_clockwise_edge_point_indices,
                                                    pdev_input_clockwise_edge_point_indices,
                                                    pdev_edge_index_map,
                                                    pdev_edge_indices,
                                                    num_edge_indices,
                                                    num_split_points,
                                                    pdev_first_midpoint,
                                                    pdev_companions,
                                                    pdev_boundary_edges
                                                    );
    checkLastCudaError();
}

extern "C" void copy_2D_from_host_to_device_with_padding ( void* device_pointer, const void* host_pointer, int device_pitch, int host_pitch, int width_in_bytes, int rows )
{
    cudaMemcpy2D(device_pointer, device_pitch, host_pointer, host_pitch, width_in_bytes, rows, cudaMemcpyHostToDevice);
}

/**
 * Call thread synchronize to ensure consistency
 */
extern "C" void synchronize_threads ()
{
    cudaThreadSynchronize();
}

extern "C" void set_selection_value_entry ( float* points_and_selection, float selection_value, int num_points )
{
    int numThreads = 64;

    dim3 threads_per_block(numThreads, 1);
    dim3 blocks_per_grid( iDivUp(num_points, numThreads), 1);

    set_selection_value_kernel <<< blocks_per_grid, threads_per_block >>> ( (float4*)points_and_selection, selection_value, num_points );

    checkLastCudaError();

    cudaThreadSynchronize();
}

extern "C" void find_companion_kernel_entry ( unsigned char* pdev_boundary_edges,
											  unsigned int* pdev_adjacent_edge_indices,
											  const int num_edges,
											  const unsigned int* pdev_edge_point_indices,
											  const unsigned int* pdev_clockwise_edges_point_indices,
											  const unsigned int* pdev_first_edges,
											  const unsigned int* pdev_valences,
											  const unsigned int* pdev_point_edges )
{
	#define NUM_THREADS 64

	dim3 threads_per_block(NUM_THREADS, 1);
	dim3 blocks_per_grid( iDivUp(num_edges, NUM_THREADS), 1);

	// initialize the values for the boundary edges to true and the adjacent edge list to zeros
	cudaMemset(pdev_boundary_edges, 1, num_edges*sizeof(unsigned char));
	cudaMemset(pdev_adjacent_edge_indices, 0, num_edges*sizeof(unsigned int));
	cudaThreadSynchronize();

	find_companion_kernel <<< blocks_per_grid, threads_per_block >>> ( pdev_boundary_edges,
																	   pdev_adjacent_edge_indices,
																	   num_edges,
																	   pdev_edge_point_indices,
																	   pdev_clockwise_edges_point_indices,
																	   pdev_first_edges,
																	   pdev_valences,
																	   pdev_point_edges );

	checkLastCudaError();

	cudaThreadSynchronize();


}

extern "C" int create_vertex_valence_lookup_kernel_entry (
																unsigned int* pdev_valence,
																const unsigned int* pdev_edge_point_indices,
																int num_edges
																)
{
	int host_valence_size = 0;
	int* pdev_valence_size;
	allocate_device_memory((void**)&pdev_valence_size, sizeof(int));

	// intialize the valence data on the device - set to zero
	cudaMemset((void*)pdev_valence, 0, num_edges*2*sizeof(unsigned int));

	// the following runs serially

	dim3 threads_per_block(1, 1);
	dim3 blocks_per_grid(1, 1);

	cudaThreadSynchronize();
	create_vertex_valence_lookup_kernel<<< blocks_per_grid, threads_per_block >>> ( pdev_valence, pdev_edge_point_indices, pdev_valence_size, num_edges );
	cudaThreadSynchronize();
	checkLastCudaError();

	copy_from_device_to_host((void*)&host_valence_size, (const void*)pdev_valence_size, sizeof(int));


	free_device_memory((void*)pdev_valence_size);

	return host_valence_size;
}

extern "C" void calculate_first_edge_entry ( unsigned int* pdev_first_edge, const unsigned int* pdev_valences, int num_edges )
{
	int numThreads = 64;
	dim3 threads_per_block(numThreads, 1);
	dim3 blocks_per_grid( iDivUp(num_edges, numThreads), 1);

	calculate_first_edges_block_kernel<<< blocks_per_grid, threads_per_block, blocks_per_grid.x*2*sizeof(unsigned int) >>> ( pdev_first_edge, pdev_valences, num_edges );
	cudaThreadSynchronize();
	calculate_first_edges_update_kernel<<< blocks_per_grid, threads_per_block, blocks_per_grid.x*2*sizeof(unsigned int) >>> ( pdev_first_edge, num_edges );
	cudaThreadSynchronize();
	checkLastCudaError();

}

extern "C" void calculate_point_edges_entry (  unsigned int* pdev_point_edges, unsigned int* pdev_found_edges, const unsigned int* pdev_edge_point_indices, const unsigned int* pdev_first_edges, int num_edges, int num_points )
{
	// initialize the point edges
	cudaMemset((void*)pdev_point_edges, 0, num_points*sizeof(unsigned int));
	cudaMemset((void*)pdev_found_edges, 0, num_edges*sizeof(unsigned int));

	// runs serially
	dim3 threads_per_block(1, 1);
	dim3 blocks_per_grid(1, 1);
	cudaThreadSynchronize();
	calculate_point_edges_kernel<<<blocks_per_grid, threads_per_block>>> (pdev_point_edges, pdev_found_edges, pdev_edge_point_indices, pdev_first_edges, num_edges);
	cudaThreadSynchronize();
	checkLastCudaError();
}

extern "C" unsigned int edge_index_calculator_entry (
													unsigned int* pdev_edge_list,
													unsigned int* phost_edge_list_size,
													unsigned int* pdev_first_midpoint,
													unsigned char* pdev_has_midpoint,
													unsigned int* pdev_index_map,
													const unsigned int* pdev_face_first_loops,
													const unsigned int* pdev_face_loop_counts,
													const unsigned int* pdev_loop_first_edges,
													const unsigned int* pdev_clockwise_edges,
													const float* pdev_edge_selection,
													const unsigned int* pdev_companions,
													const unsigned char* pdev_boundary_edges,
													int split_point_count,
													int num_faces,
													int first_new_point_index
													)
{
	unsigned int host_sizes[4];
	unsigned int* pdev_sizes;
	allocate_device_memory((void**)&pdev_sizes, 4*sizeof(unsigned int));
	set_device_memory((void*)pdev_sizes, 0, 4*sizeof(unsigned int));
	// runs serially
	dim3 threads_per_block(1, 1);
	dim3 blocks_per_grid(1, 1);
	cudaThreadSynchronize();

	for ( int face_count = 0; face_count < num_faces ; face_count++ )
	{
		//printf("face %d of %d\n", face_count+1, num_faces);

		get_loop_index_and_count_kernel <<< blocks_per_grid, threads_per_block >>> (
												pdev_sizes+2,
												pdev_face_first_loops,
												pdev_face_loop_counts,
												face_count );
		checkLastCudaError();
		copy_from_device_to_host((void*)(&host_sizes), (const void*)pdev_sizes, 4*sizeof(unsigned int));
		checkLastCudaError();

		for ( int loop_index = host_sizes[2]; loop_index < host_sizes[2] + host_sizes[3]; loop_index++ )
		{
			//printf("loop %d of %d\n", loop_index+1, host_sizes[2] + host_sizes[3]);
			edge_index_calculator_kernel <<< blocks_per_grid, threads_per_block >>> (
												pdev_edge_list,
												pdev_sizes,
												pdev_first_midpoint,
												pdev_has_midpoint,
												pdev_index_map,
												pdev_loop_first_edges,
												pdev_clockwise_edges,
												pdev_edge_selection,
												pdev_companions,
												pdev_boundary_edges,
												split_point_count,
												first_new_point_index,
												loop_index
												 );
			checkLastCudaError();

		}
	}

	copy_from_device_to_host((void*)(&host_sizes), (const void*)pdev_sizes, 2*sizeof(unsigned int));
	cudaThreadSynchronize();
	free_device_memory ( pdev_sizes );

	*phost_edge_list_size = host_sizes[1];
	return host_sizes[0];
}
