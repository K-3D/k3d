#ifndef _CUDA_KERNELS_H_
#define _CUDA_KERNELS_H_

#ifdef __DEVICE_EMULATION__
#include <stdio.h>
#endif

// declare the texture reference for Matrix multiplication
texture<float, 2> transformTexture;

// convert a half-float to a single precision float
__device__ float halfToFloat (unsigned short halfIn)
{
	int s = (halfIn >> 15) & 0x00000001;
    int e = (halfIn >> 10) & 0x0000001f;
    int m =  halfIn        & 0x000003ff;

	int val = 0;
	int done = 0;

    if (e == 0)
    {
		if (m == 0)
		{
	    	//
	    	// Plus or minus zero
	    	//
			val = s << 31;
			done = 1;
		}
		else
		{
	    	//
	    	// Denormalized number -- renormalize it
	    	//

	    	while (!(m & 0x00000400))
	    	{
				m <<= 1;
				e -=  1;
	    	}

	    	e += 1;
	    	m &= ~0x00000400;
		}
    }
    else if (e == 31)
    {
		if (m == 0)
		{
	    	//
	    	// Positive or negative infinity
	    	//

	    	val = (s << 31) | 0x7f800000;
	    	done = 1;
		}
		else
		{
	    	//
	    	// Nan -- preserve sign and significand bits
	    	//

	    	val = (s << 31) | 0x7f800000 | (m << 13);
	    	done = 1;
		}
    }

    //
    // Normalized number
    //
	if ( !done )
	{
		e = e + (127 - 15);
    	m = m << 13;

    	//
    	// Assemble s, e and m.
    	//
		val = (s << 31) | (e << 23) | m;
	}

	return __int_as_float (val);

}

// convert a single precision float to a half-float
__device__ unsigned short floatToHalf( float floatIn )
{
    //
    // Our floating point number, f, is represented by the bit
    // pattern in integer i.  Disassemble that bit pattern into
    // the sign, s, the exponent, e, and the significand, m.
    // Shift s into the position where it will go in in the
    // resulting half number.
    // Adjust e, accounting for the different exponent bias
    // of float and half (127 versus 15).
    //

	int floatBits = __float_as_int( floatIn );

    int s =  ((floatBits >> 16) & 0x00008000);
    int e = ((floatBits >> 23) & 0x000000ff) - (127 - 15);
    int m =   floatBits        & 0x007fffff;
    //
    // Now reassemble s, e and m into a half:
    //

    if (e <= 0)
    {
		if (e < -10)
		{
	    	//
	    	// E is less than -10.  The absolute value of f is
	    	// less than HALF_MIN (f may be a small normalized
	    	// float, a denormalized float or a zero).
	    	//
	    	// We convert f to a half zero.
	    	//

	    	return 0;
		}

		//
		// E is between -10 and 0.  F is a normalized float,
		// whose magnitude is less than HALF_NRM_MIN.
		//
		// We convert f to a denormalized half.
		//

		m = (m | 0x00800000) >> (1 - e);

		//
		// Round to nearest, round "0.5" up.
		//
		// Rounding may cause the significand to overflow and make
		// our number normalized.  Because of the way a half's bits
		// are laid out, we don't have to treat this case separately;
		// the code below will handle it correctly.
		//

		if (m &  0x00001000)
	    	m += 0x00002000;

		//
		// Assemble the half from s, e (zero) and m.
		//
		return s | (m >> 13);
    }
    else if (e == 0xff - (127 - 15))
    {
		if (m == 0)
		{
	    	//
	    	// F is an infinity; convert f to a half
	    	// infinity with the same sign as f.
	    	//
			return s | 0x7c00;
		}
		else
		{
	    	//
	    	// F is a NAN; produce a half NAN that preserves
	    	// the sign bit and the 10 leftmost bits of the
	    	// significand of f.
	    	//
			return s | 0x7c00 | (m >> 13);
		}
    }
    else
    {
		//
		// E is greater than zero.  F is a normalized float.
		// We try to convert f to a normalized half.
		//

		//
		// Round to nearest, round "0.5" up
		//

		if (m &  0x00001000)
		{
	    	m += 0x00002000;

	    	if (m & 0x00800000)
	    	{
				m =  0;		// overflow in significand,
				e += 1;		// adjust exponent
	    	}
		}

		//
		// Handle exponent overflow
		//

		if (e > 30)
		{
			return s | 0x7c00;	// if this returns, the half becomes an
	    	 // infinity with the same sign as f.
		}

		//
		// Assemble the half from s, e and m.
		//
		return s | (e << 10) | (m >> 13);
    }
}

__global__ void add_kernel (ushort4 *image_RGBA, int width, int height, float value)
{
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = halfToFloat((unsigned short)image_RGBA[idx].x) + value;
        pixelFloat.y = halfToFloat((unsigned short)image_RGBA[idx].y) + value;
        pixelFloat.z = halfToFloat((unsigned short)image_RGBA[idx].z) + value;

        image_RGBA[idx].x = floatToHalf(pixelFloat.x);
        image_RGBA[idx].y = floatToHalf(pixelFloat.y);
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
    }
	__syncthreads();
}

__global__ void multiply_kernel (ushort4 *image_RGBA, int width, int height, float value)
{
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = halfToFloat((unsigned short)image_RGBA[idx].x) * value;
        pixelFloat.y = halfToFloat((unsigned short)image_RGBA[idx].y) * value;
        pixelFloat.z = halfToFloat((unsigned short)image_RGBA[idx].z) * value;

        image_RGBA[idx].x = floatToHalf(pixelFloat.x);
        image_RGBA[idx].y = floatToHalf(pixelFloat.y);
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
    }
	__syncthreads();
}

/**
 * Apply gamma transform to each pixel value of an immage
 */
__global__ void gamma_kernel (ushort4 *image_RGBA, int width, int height, float value)
{
    const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = powf(halfToFloat((unsigned short)image_RGBA[idx].x), value);
        pixelFloat.y = powf(halfToFloat((unsigned short)image_RGBA[idx].y), value);
        pixelFloat.z = powf(halfToFloat((unsigned short)image_RGBA[idx].z), value);

        image_RGBA[idx].x = floatToHalf(pixelFloat.x);
        image_RGBA[idx].y = floatToHalf(pixelFloat.y);
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
    }
}

/**
 * Invert each color channel value:  out = max - in + min  with max = 1 and min = 0
 */
__global__ void invert_kernel (ushort4 *image_RGBA, int width, int height)
{
    #define MAX_HALF 1.0f

    const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = MAX_HALF - halfToFloat((unsigned short)image_RGBA[idx].x);
        pixelFloat.y = MAX_HALF - halfToFloat((unsigned short)image_RGBA[idx].y);
        pixelFloat.z = MAX_HALF - halfToFloat((unsigned short)image_RGBA[idx].z);

        image_RGBA[idx].x = floatToHalf(pixelFloat.x);
        image_RGBA[idx].y = floatToHalf(pixelFloat.y);
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
    }
}

/**
 * Create an alpha channel from the color difference
 */
__global__ void matte_color_diff_kernel (ushort4 *image_RGBA, int width, int height, float threshold)
{
    const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = halfToFloat((unsigned short)image_RGBA[idx].x);
        pixelFloat.y = halfToFloat((unsigned short)image_RGBA[idx].y);
        pixelFloat.z = fminf(pixelFloat.y, halfToFloat((unsigned short)image_RGBA[idx].z));

        if ( pixelFloat.z > threshold )
        {
            pixelFloat.w = 1 - pixelFloat.z + fmaxf(pixelFloat.x, pixelFloat.y);
        }
        else
        {
            pixelFloat.w = 1.0f;
        }

        // red and green channels are unchanged
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
        image_RGBA[idx].w = floatToHalf(pixelFloat.w);

    }
}

/**
 * Invert the alpha channel:  out = max - in + min  with max = 1 and min = 0
 */
__global__ void matte_invert_kernel (ushort4 *image_RGBA, int width, int height)
{
    #define MAX_HALF 1.0f

    const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float newAlpha;

        newAlpha = MAX_HALF - halfToFloat((unsigned short)image_RGBA[idx].w);

        image_RGBA[idx].w = floatToHalf(newAlpha);
    }
}

__global__ void color_monochrome_kernel ( ushort4 *image_RGBA, int width, int height, float redWeight, float greenWeight, float blueWeight)
{
	const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float monoValue;

        monoValue = halfToFloat(image_RGBA[idx].x) * redWeight
        		  + halfToFloat(image_RGBA[idx].y) * greenWeight
        		  + halfToFloat(image_RGBA[idx].z) * blueWeight;

        image_RGBA[idx].x = floatToHalf(monoValue);
        image_RGBA[idx].y = floatToHalf(monoValue);
        image_RGBA[idx].z = floatToHalf(monoValue);
    }
	__syncthreads();
}

__global__ void threshold_kernel ( ushort4 *image_RGBA, int width, int height, float redThreshold, float greenThreshold, float blueThreshold, float alphaThreshold)
{
    const int ix = blockDim.x * blockIdx.x + threadIdx.x;
    const int iy = blockDim.y * blockIdx.y + threadIdx.y;

    if(ix < width && iy < height)
    {
        // the first, second, third, and fourth fields can be accessed using x, y, z, and w
        const int idx = width * iy + ix;

        float4 pixelFloat;

        pixelFloat.x = fmaxf(halfToFloat((unsigned short)image_RGBA[idx].x), redThreshold);
        pixelFloat.y = fmaxf(halfToFloat((unsigned short)image_RGBA[idx].y), blueThreshold);
        pixelFloat.z = fmaxf(halfToFloat((unsigned short)image_RGBA[idx].z), greenThreshold);
        pixelFloat.w = fmaxf(halfToFloat((unsigned short)image_RGBA[idx].w), alphaThreshold);

        image_RGBA[idx].x = floatToHalf(pixelFloat.x);
        image_RGBA[idx].y = floatToHalf(pixelFloat.y);
        image_RGBA[idx].z = floatToHalf(pixelFloat.z);
        image_RGBA[idx].w = floatToHalf(pixelFloat.w);
    }
    __syncthreads();
}

__global__ void linear_transform_kernel ( float4 *points, int num_points )
{
	const int idx = blockDim.x * blockIdx.x + threadIdx.x;
	__shared__ float T[4][4];

	__shared__ float4 ThreadPoints[64];

	/*
	__syncthreads();

	if ( threadIdx.x < 1 )
	{
		T[0][0] = tex2D(transformTexture, 0, 0);
		T[1][0] = tex2D(transformTexture, 0, 1);
		T[2][0] = tex2D(transformTexture, 0, 2);
		T[3][0] = tex2D(transformTexture, 0, 3);
		T[0][1] = tex2D(transformTexture, 1, 0);
		T[1][1] = tex2D(transformTexture, 1, 1);
		T[2][1] = tex2D(transformTexture, 1, 2);
		T[3][1] = tex2D(transformTexture, 1, 3);
		T[0][2] = tex2D(transformTexture, 2, 0);
		T[1][2] = tex2D(transformTexture, 2, 1);
		T[2][2] = tex2D(transformTexture, 2, 2);
		T[3][2] = tex2D(transformTexture, 2, 3);
		T[0][3] = tex2D(transformTexture, 3, 0);
		T[1][3] = tex2D(transformTexture, 3, 1);
		T[2][3] = tex2D(transformTexture, 3, 2);
		T[3][3] = tex2D(transformTexture, 3, 3);
	}
	*/

    if ( threadIdx.x < 16 )
	{
		//const int row = idx >> 2;
		//const int col = idx & 0x3;
		T[idx >> 2][idx & 0x3] = tex2D(transformTexture, idx & 0x3, idx >> 2);
	}
	if ( blockDim.x < 16 )
	{
		for ( int i = blockDim.x ; i < 16 ; i++ )
		{
			T[i >> 2][i & 0x3] = tex2D(transformTexture, i & 0x3, i >> 2);
		}
	}
	__syncthreads();

	if ( idx < num_points )
	{
		float4 vt;
		ThreadPoints[threadIdx.x] = points[idx];
		vt.w = T[3][0]*ThreadPoints[threadIdx.x].x + T[3][1]*ThreadPoints[threadIdx.x].y + T[3][2]*ThreadPoints[threadIdx.x].z + T[3][3];

		vt.x = (T[0][0]*ThreadPoints[threadIdx.x].x + T[0][1]*ThreadPoints[threadIdx.x].y + T[0][2]*ThreadPoints[threadIdx.x].z + T[0][3])/vt.w;
		vt.y = (T[1][0]*ThreadPoints[threadIdx.x].x + T[1][1]*ThreadPoints[threadIdx.x].y + T[1][2]*ThreadPoints[threadIdx.x].z + T[1][3])/vt.w;
		vt.z = (T[2][0]*ThreadPoints[threadIdx.x].x + T[2][1]*ThreadPoints[threadIdx.x].y + T[2][2]*ThreadPoints[threadIdx.x].z + T[2][3])/vt.w;

		points[idx].x = ThreadPoints[threadIdx.x].x*(1 - ThreadPoints[threadIdx.x].w) + ThreadPoints[threadIdx.x].w*vt.x;
		points[idx].y = ThreadPoints[threadIdx.x].y*(1 - ThreadPoints[threadIdx.x].w) + ThreadPoints[threadIdx.x].w*vt.y;
		points[idx].z = ThreadPoints[threadIdx.x].z*(1 - ThreadPoints[threadIdx.x].w) + ThreadPoints[threadIdx.x].w*vt.z;
	}

	__syncthreads();

}

/**
 * Kernel for calculating the coordinates of the new points along the specified edges.
 *
 */
__global__ void subdivide_edges_split_point_kernel ( const unsigned int* first_midpoint,
																											const unsigned char* has_midpoint,
                                                     float4* points_and_selection,
                                                     unsigned int num_points,
                                                     unsigned int* edge_point_indices,
                                                     unsigned int* clockwise_edge_indices,
                                                     const float* edge_selection,
                                                     const unsigned int* companions,
                                                     const unsigned char* boundary_edges,
                                                     const unsigned int* edge_faces,
                                                     int num_split_points,
                                                     const int num_edges)
{
    unsigned int edge_index = (blockIdx.x * blockDim.x) + threadIdx.x;
    int split_index = (blockIdx.y * blockDim.y) + threadIdx.y;

    const unsigned char boundary = boundary_edges[edge_index];
    const unsigned int companion = companions[edge_index];
    if ( edge_index < num_edges  && has_midpoint[edge_index] && (boundary || (!boundary && (edge_faces[companion] > edge_faces[edge_index] || !edge_selection[companion]))))
    {
        unsigned int p_index = edge_point_indices[edge_index];
        unsigned int new_point_index = first_midpoint[edge_index] + split_index;

        #ifdef __DEVICE_EMULATION__
            printf("Split Index: %d\n", split_index);
            printf("Edge Index: %d\n", edge_index);
            printf("Clockwise Edge Index: %d\n", clockwise_edge_indices[edge_index]);
            printf("Point Index: %d\n", edge_point_indices[edge_index]);
            printf("Clockwise Point Index: %d\n", edge_point_indices[clockwise_edge_indices[edge_index]]);
            printf("New Point Index: %d\n", new_point_index);
        #endif


        float4 p0 = points_and_selection[p_index];
        float4 p1 = points_and_selection[edge_point_indices[clockwise_edge_indices[edge_index]]];

        #ifdef __DEVICE_EMULATION__
            printf("P_0:%d: (%f, %f, %f)\n", p_index, p0.x, p0.y, p0.z);
            printf("P_1:%d: (%f, %f, %f)\n", edge_point_indices[clockwise_edge_indices[edge_index]], p1.x, p1.y, p1.z);
        #endif

        p1.x = (p1.x - p0.x) / (num_split_points + 1);
        p1.y = (p1.y - p0.y) / (num_split_points + 1);
        p1.z = (p1.z - p0.z) / (num_split_points + 1);

        #ifdef __DEVICE_EMULATION__
            printf("P_delta:(%f, %f, %f)\n", p1.x, p1.y, p1.z);
        #endif

        points_and_selection[new_point_index].x = p0.x + (split_index + 1)*p1.x;
        points_and_selection[new_point_index].y = p0.y + (split_index + 1)*p1.y;
        points_and_selection[new_point_index].z = p0.z + (split_index + 1)*p1.z;
        points_and_selection[new_point_index].w = 1;

    }

}

__global__ void subdivide_edges_update_edge_indices_kernel ( unsigned int* output_edge_point_indices,
                                                        unsigned int* output_clockwise_edge_point_indices,
                                                        unsigned int* input_edge_point_indices,
                                                        unsigned int* input_clockwise_edge_point_indices,
                                                        unsigned int* edge_index_map,
                                                        int num_edge_maps)
{
    unsigned int edge_index_index = (blockIdx.x * blockDim.x) + threadIdx.x;

    if ( edge_index_index < num_edge_maps )
    {
        unsigned int out_edge_index = edge_index_map[edge_index_index];

        #ifdef __DEVICE_EMULATION__
            printf("Edge: %d : Mapped Edge : %d \n", edge_index_index, out_edge_index);
        #endif

        output_edge_point_indices[out_edge_index] = input_edge_point_indices[edge_index_index];
        output_clockwise_edge_point_indices[out_edge_index] = edge_index_map[input_clockwise_edge_point_indices[edge_index_index]];

        #ifdef __DEVICE_EMULATION__
            printf("Input Edge Point: %d : Input CW Edge Point : %d \n", input_edge_point_indices[edge_index_index], input_clockwise_edge_point_indices[edge_index_index]);
        #endif
    }
}

__global__ void subdivide_edges_update_loop_first_edges_kernel (
                                                        unsigned int* pdev_ouput_loop_first_edges,
                                                        int num_loops,
                                                        unsigned int* edge_index_map
                                                        )
{
    unsigned int loop_index = (blockIdx.x * blockDim.x) + threadIdx.x;

    if ( loop_index < num_loops )
    {
        pdev_ouput_loop_first_edges[loop_index] = edge_index_map[pdev_ouput_loop_first_edges[loop_index]];
    }
}

__global__ void subdivide_edges_split_edges_kernel (unsigned int* output_edge_point_indices,
                                                    unsigned int* output_clockwise_edge_point_indices,
                                                    unsigned int* input_clockwise_edge_point_indices,
                                                    const float* edge_selection,
                                                    unsigned int* edge_index_map,
                                                    unsigned int* pdev_first_midpoint,
                                                    unsigned char* has_midpoint,
                                                    int num_split_points,
                                                    unsigned int* pdev_companions,
                                                    unsigned char* pdev_boundary_edges,
                                                    const unsigned int num_edges,
                                                    const unsigned int* edge_faces
                                                    )

{
    unsigned int edge = (blockIdx.x * blockDim.x) + threadIdx.x;
    int split_index = (blockIdx.y * blockDim.y) + threadIdx.y;
    
    if ( edge < num_edges  && has_midpoint[edge])
    {
        unsigned int old_clockwise = input_clockwise_edge_point_indices[edge];
        unsigned int new_edge = edge_index_map[edge] + 1 + split_index;
        
        const unsigned int companion = pdev_companions[edge];
        const unsigned char boundary = pdev_boundary_edges[edge];

        if(edge_selection[edge] && (boundary || (!boundary && (edge_faces[edge] < edge_faces[companion] || !edge_selection[companion]))))
        {
        	output_edge_point_indices[new_edge] = pdev_first_midpoint[edge] + split_index;
        }
        else
        {
        	output_edge_point_indices[new_edge] = pdev_first_midpoint[edge] - split_index + num_split_points - 1;
        }
        output_clockwise_edge_point_indices[new_edge - 1] = new_edge;

        if ( split_index == 0 )
        {
            output_clockwise_edge_point_indices[new_edge + num_split_points - 1] = edge_index_map[old_clockwise];
        }

    }
}

__global__ void convert_uint_64_to_32_kernel ( uint2* p_uint_64, unsigned int* p_uint_32, int num_ints )
{
    const int int_index = (blockDim.x * blockIdx.x) + threadIdx.x;

    if ( int_index < num_ints )
    {
        // set the 32bit unsigned int to the lower 32bits of the 64bit unsigned int
        p_uint_32[int_index] = p_uint_64[int_index].x;
    }
}

__global__ void convert_uint_32_to_64_kernel ( uint2* p_uint_64, unsigned int* p_uint_32, int num_ints )
{
    const int int_index = (blockDim.x * blockIdx.x) + threadIdx.x;

    if ( int_index < num_ints )
    {
        // zero the upper 32bits and equate the lower 32
        p_uint_64[int_index].y = 0;
        p_uint_64[int_index].x = p_uint_32[int_index];
    }
}


__global__ void set_selection_value_kernel ( float4* points_and_selection, float selection_value, int num_points )
{
    const int index = (blockDim.x * blockIdx.x) + threadIdx.x;
    if ( index < num_points )
    {
        points_and_selection[index].w = selection_value;
    }
}

/**
 * Kernel for companion computation
 */
__global__ void find_companion_kernel(unsigned char* boundary_edges,
									  unsigned int* adjacent_edge_indices,
									  const int num_edges,
									  const unsigned int* edge_point_indices,
									  const unsigned int* clockwise_edges_point_indices,
									  const unsigned int* first_edges,
									  const unsigned int* valences,
									  const unsigned int* point_edges)
{
	const int edge_index = (blockDim.x * blockIdx.x) + threadIdx.x;

	if ( edge_index < num_edges )
	{
		const unsigned int vertex1 = edge_point_indices[edge_index];
		const unsigned int vertex2 = edge_point_indices[clockwise_edges_point_indices[edge_index]];

		const unsigned int first_index = first_edges[vertex2];
		const unsigned int last_index = first_index + valences[vertex2];
		for ( unsigned int i = first_index; i != last_index; ++i )
		{
			const unsigned int companion = point_edges[i];
			if ( edge_point_indices[clockwise_edges_point_indices[companion]] == vertex1 )
			{
				boundary_edges[edge_index] = 0;
				adjacent_edge_indices[edge_index] = companion;
				break;
			}
		}
	}

}


/**
 * Kernel for looking up the valence of edges points
 */
__global__ void create_vertex_valence_lookup_kernel ( unsigned int* valence, const unsigned int* edge_point_indices, int* num_valence, int num_edges )
{
	int tmp_num_valence = 0;
	unsigned int point_index = 0;

	for ( unsigned int edge_index = 0; edge_index < num_edges ; edge_index++ )
	{
		point_index = edge_point_indices[edge_index];
		valence[point_index] += 1;
		tmp_num_valence = max(tmp_num_valence, (point_index+1));
	}

	*num_valence = tmp_num_valence;
}

/**
 * Kernel to calculate first edges in edge_adjacency lookup
 */
__global__ void calculate_first_edges_kernel ( unsigned int* first_edges, const unsigned int* valences, int num_points )
{
	unsigned int count = 0;

	for ( unsigned int point = 0; point < num_points; ++point)
	{
		first_edges[point] = count;
		count += valences[point];
	}

}
__global__ void calculate_first_edges_block_kernel ( unsigned int* first_edges, const unsigned int* valences, int num_edges )
{
	// use shared memory to store both the result per block as well as the initial block values
	__shared__ extern uint2 shared_first[];

	unsigned int edge_index = ( blockDim.x * blockIdx.x) + threadIdx.x;

	if ( edge_index < num_edges )
	{
		shared_first[threadIdx.x].x = valences[edge_index];
		shared_first[0].y = 0;

		if ( edge_index > 0 )
		{
			shared_first[threadIdx.x].y = valences[edge_index-1];
		}
	}

	__syncthreads();

	if ( edge_index < num_edges )
	{
		for ( int i = 1; i < blockDim.x ; i++ )
		{
			if ( threadIdx.x >= i )
			{
				shared_first[threadIdx.x].x += shared_first[threadIdx.x - i].y;
			}
		}
	}

	__syncthreads();
	if ( edge_index < num_edges )
	{
		first_edges[edge_index] = shared_first[threadIdx.x].x;
	}
	__syncthreads();

}

__global__ void calculate_first_edges_update_kernel ( unsigned int* first_edges, int num_edges )
{

	__shared__ extern unsigned int shared_first_edge[];
	__shared__ unsigned int update_val;

	unsigned int edge_index = ( blockDim.x * blockIdx.x) + threadIdx.x;

#ifdef __DEVICE_EMULATION__
	printf("calculate_first_edges_update_kernel: %u of %d blah\n", edge_index, num_edges);
#endif

	if ( edge_index < num_edges )
	{
		shared_first_edge[threadIdx.x] = first_edges[edge_index];
	}

	if ( threadIdx.x == 0 )
	{
		update_val = 0;
		for ( int k = 1 ; k <= ( blockIdx.x ) ; k++ )
		{
			update_val += first_edges[k*blockDim.x -1];
		}
	}
	__syncthreads();


	if ( edge_index < num_edges )
	{
		shared_first_edge[threadIdx.x] += update_val;
	}

	__syncthreads();

	if ( edge_index < num_edges )
	{
		first_edges[edge_index] = shared_first_edge[threadIdx.x];
	}

}

/**
 * Kernel for calculating point edges
 */
__global__ void calculate_point_edges_kernel ( unsigned int* point_edges, unsigned int* found_edges, const unsigned int* edge_point_indices, const unsigned int* first_edges, int num_edges)
{
	unsigned int point_index = 0;
	for ( unsigned int edge_index = 0 ; edge_index < num_edges ; ++edge_index )
	{
		point_index = edge_point_indices[edge_index];
		point_edges[first_edges[point_index] + found_edges[point_index]] = edge_index;
		++found_edges[point_index];
	}
}

/**
 * Kernel to get the first loop index as well as the loop count for a given face
 */
__global__ void get_loop_index_and_count_kernel (
											unsigned int* index_and_counts,
											const unsigned int* face_first_loops,
											const unsigned int* face_loop_counts,
											int face_index )
{
	index_and_counts[0] = face_first_loops[face_index];
	index_and_counts[1] = face_loop_counts[face_index];
}


/**
 * Count the total number of edges each face will have after applying the edge subdivision, and the number of new points added per face
 */

__global__ void subdivide_edges_count_components_per_face(
													const unsigned int* face_first_loops,
													const int num_faces,
													const unsigned int* face_loop_counts,
													const unsigned int* loop_first_edges,
													const unsigned int* clockwise_edges,
													const float* edge_selection,
													const unsigned int* companions,
													const unsigned char* boundary_edges,
													const int split_point_count,
													const unsigned int* edge_faces,
													unsigned int* face_edge_counts,
													unsigned int* face_split_point_counts
		)
{
	unsigned int face_index = (blockIdx.x * blockDim.x) + threadIdx.x;	
	if(face_index >= num_faces)
		return;
	
	unsigned int edge_count = 0;
	unsigned int new_point_count = 0;
	
	unsigned int loop_begin = face_first_loops[face_index];
	unsigned int loop_end = loop_begin + face_loop_counts[face_index];
	for(unsigned int loop_index = loop_begin; loop_index != loop_end; ++loop_index)
	{
		unsigned int first_edge_index = loop_first_edges[loop_index];
		
		for( unsigned int edge_index = first_edge_index; 1 ; )
		{
			unsigned int companion = companions[edge_index]; 
			++edge_count;
	
			edge_count += split_point_count * (edge_selection[edge_index] || (!boundary_edges[edge_index] && edge_selection[companion]));
			unsigned int found_points = 0;
			if(edge_selection[edge_index] && (boundary_edges[edge_index] || edge_faces[edge_index] < edge_faces[companion] || !edge_selection[companion]))
				found_points = split_point_count;
			//const unsigned int found_points = split_point_count * (edge_selection[edge_index] && !(!boundary_edges[edge_index] && (edge_faces[edge_index] > edge_faces[companion] || !edge_selection[companion])));
			new_point_count += found_points;
			
#ifdef __DEVICE_EMULATION__
	printf("adding %d points for edge %d\n", found_points, edge_index);
#endif
						
			edge_index = clockwise_edges[edge_index];
			if (edge_index == first_edge_index)
				break;
		}
	}
	
	face_edge_counts[face_index] = edge_count;
	face_split_point_counts[face_index] = new_point_count;
}

/**
 *
 */
__global__ void subdivide_edges_edge_index_calculator_kernel (
											unsigned int* first_midpoint,
											unsigned char* has_midpoint,
											unsigned int* index_map,
											const unsigned int* face_first_loops,
											int num_faces,
											const unsigned int* face_loop_counts,
											const unsigned int* loop_first_edges,
											const unsigned int* clockwise_edges,
											const float* edge_selection,
											const unsigned int* companions,
											const unsigned char* boundary_edges,
											const unsigned int* edge_faces,
											const unsigned int* face_edge_counts,
											const unsigned int* face_split_point_counts,
											int split_point_count,
											int first_new_point_index
											 )
{

	unsigned int face_index = (blockIdx.x * blockDim.x) + threadIdx.x;
	
	if(face_index >= num_faces)
		return;
		
	// initialize the edge count to the cumulative edge count of the previous faces
	unsigned int edge_count = face_index == 0 ? 0 : face_edge_counts[face_index - 1];
	// initialise the new midpoint point index to the cumulative new point count of the previous faces
	unsigned int midpoint_index = face_index == 0 ? first_new_point_index : first_new_point_index + face_split_point_counts[face_index - 1];

	unsigned int loop_begin = face_first_loops[face_index];
	unsigned int loop_end = loop_begin + face_loop_counts[face_index];
	for(unsigned int loop_index = loop_begin; loop_index != loop_end; ++loop_index)
	{
		unsigned int first_edge_index = loop_first_edges[loop_index];
		for( unsigned int edge_index = first_edge_index; 1 ; )
		{
			index_map[edge_index] = edge_count;
			const unsigned int companion = companions[edge_index];
			const char boundary = boundary_edges[edge_index];
			
			// The midpoint was/is being/will be calculated in another kernel, but we still need to update the edge index counter for this face
			if(!boundary && edge_selection[companion] && (!edge_selection[edge_index] || edge_faces[edge_index] > edge_faces[companion]))
			{
				edge_count += split_point_count;
			}
	
			// Get the midpoint index, and set it for both ourselves and the companion edge if we are the kernel for the first face of the edge-companion pair
			if(edge_selection[edge_index] && (boundary || (!boundary && (edge_faces[edge_index] < edge_faces[companion] || !edge_selection[companion]))))
			{
				first_midpoint[edge_index] = midpoint_index;
				has_midpoint[edge_index] = 1;
				if(!boundary)
				{
					first_midpoint[companion] = midpoint_index;
					has_midpoint[companion] = 1;
				}
				midpoint_index += split_point_count;
				edge_count += split_point_count;
			}
	
			edge_index = clockwise_edges[edge_index];
			++edge_count;
			if (edge_index == first_edge_index)
				break;
		}
	}
}

/**
 * Execute a single step in the iterative calculation of a cumulative sum. The algorithm
 * has to as long as step_size < size
 * \param input The input array
 * \param output The result, which is partial as long as step_size < size
 * \param size The number of elements in input and output
 * \param step_size Equal to 2^(step_number), with step_number starting at 0
 */
__global__ void cumulative_sum_step(const unsigned int* input, unsigned int* output, const int size, const int step_size)
{
	const unsigned int index = (blockIdx.x * blockDim.x) + threadIdx.x;
	
	if(index >= size)
		return;
	
	char to_calculate = step_size <= index; // true if the number at index still needs calculating
	output[index] = input[index] + input[(index - step_size) * to_calculate] * to_calculate;
}

/**
 * Stores the face each edge belongs to
 */
__global__ void face_per_edge_kernel(
													const unsigned int* face_first_loops,
													const int num_faces,
													const unsigned int* face_loop_counts,
													const unsigned int* loop_first_edges,
													const unsigned int* clockwise_edges,
													unsigned int* edge_faces)
{
	unsigned int face_index = (blockIdx.x * blockDim.x) + threadIdx.x;	
	if(face_index >= num_faces)
		return;

	unsigned int loop_begin = face_first_loops[face_index];
	unsigned int loop_end = loop_begin + face_loop_counts[face_index];
	for(unsigned int loop_index = loop_begin; loop_index != loop_end; ++loop_index)
	{
		unsigned int first_edge_index = loop_first_edges[loop_index];
		
		for( unsigned int edge_index = first_edge_index; 1 ; )
		{
			edge_faces[edge_index] = face_index;
			
			edge_index = clockwise_edges[edge_index];
			if (edge_index == first_edge_index)
				break;
		}
	}
}

/**
 * Kernel to construct the mesh structure of the polyhedra
 */
__global__ void create_grid_structure_kernel (
												unsigned int* face_first_loops,
												unsigned int* face_loop_count,
												unsigned int* loop_first_edge,
												uint4* edge_point,
												uint4* clockwise_edge,
												unsigned int rows,
												unsigned int columns
												)
{
	unsigned int row_index = ( blockIdx.x * blockDim.x ) + threadIdx.x;
	unsigned int col_index = ( blockIdx.y * blockDim.y ) + threadIdx.y;

	if ( ( row_index < rows ) && (col_index < columns) )
	{
		unsigned int face_number = row_index * columns + col_index;
		face_first_loops[face_number] = face_number;
		face_loop_count[face_number] = 1;
		loop_first_edge[face_number] = 4*face_number;

		edge_point[face_number].x = face_number + row_index;
		edge_point[face_number].y = face_number + row_index + 1;
		edge_point[face_number].z = face_number + row_index + columns + 2;
		edge_point[face_number].w = face_number + row_index + columns + 1;

		clockwise_edge[face_number].x = 4*face_number + 1;
		clockwise_edge[face_number].y = 4*face_number + 2;
		clockwise_edge[face_number].z = 4*face_number + 3;
		clockwise_edge[face_number].w = 4*face_number;

	}

}

/**
 * Kernel to calculate the point coordinates of a poly_grid
 */
__global__ void calculate_grid_points_kernel ( float4 *point_and_selection,
												float3 x,
												float3 y,
												unsigned int rows,
												unsigned int columns
												)
{
	unsigned int row_index = ( blockIdx.x * blockDim.x ) + threadIdx.x;
	unsigned int col_index = ( blockIdx.y * blockDim.y ) + threadIdx.y;

	if ( ( row_index <= rows ) && (col_index <= columns) )
	{
		float row_percent = 0.5 - (float)row_index / (float)rows;
		float col_percent = (float)col_index/(float)columns - 0.5;

		unsigned int point_index = row_index*(columns+1) + col_index;

		point_and_selection[point_index].x = col_percent * x.x + row_percent * y.x;
		point_and_selection[point_index].y = col_percent * x.y + row_percent * y.y;
		point_and_selection[point_index].z = col_percent * x.z + row_percent * y.z;
		point_and_selection[point_index].w = 0;
	}

}

#endif // #ifndef _CUDA_KERNELS_H_

