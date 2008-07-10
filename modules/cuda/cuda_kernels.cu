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
__global__ void subdivide_edges_split_point_kernel ( unsigned int* edge_indices, 
                                                     unsigned int num_edge_indices, 
                                                     float4* points_and_selection,
                                                     unsigned int num_points,
                                                     float4* new_points_and_selection, 
                                                     unsigned int* edge_point_indices,
                                                     unsigned int* clockwise_edge_indices,
                                                     int num_split_points )
{
    
    unsigned int edge_index_index = (blockIdx.x * blockDim.x) + threadIdx.x;
    int split_index = (blockIdx.y * blockDim.y) + threadIdx.y;
    
    if ( edge_index_index < num_edge_indices )
    {
        
        unsigned int edge_index = edge_indices[edge_index_index];
        unsigned int p_index = edge_point_indices[edge_index];
        unsigned int new_point_index = edge_index_index*num_split_points + split_index;
        
        #ifdef __DEVICE_EMULATION__
            printf("Edge Index Index: %d\n", edge_index_index);
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
        
        new_points_and_selection[new_point_index].x = p0.x + (split_index + 1)*p1.x;
        new_points_and_selection[new_point_index].y = p0.y + (split_index + 1)*p1.y;
        new_points_and_selection[new_point_index].z = p0.z + (split_index + 1)*p1.z;
        new_points_and_selection[new_point_index].w = 1;
        
    } 
      
}

#endif // #ifndef _CUDA_KERNELS_H_

