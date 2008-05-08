#ifndef _CUDA_KERNELS_H_
#define _CUDA_KERNELS_H_

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
		
} 


#endif // #ifndef _CUDA_KERNELS_H_
