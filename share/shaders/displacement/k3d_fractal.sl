/*********************************
 *AUTHOR: Ken Musgrave.
 *    Conversion to Shading Language and minor modifications by Fredrik Brännbacka.
 *
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1998.  ISBN 0-12-228730-4.
 *
 *********************************/
 #include "k3d_noises.h"

displacement
k3d_fractal(float H = 0.8, lacunarity = 2.5, octaves = 7, offset = 0.9, sharpness = 4, threshold = 12, Kt = 0.1)
{
	float result, signal, weight, i, exponent;
	point PP =transform("shader",P);
	normal Nn = normalize(N);	
	for( i=0; i<octaves; i += 1 ) {
       		/* First octaves */

       		if ( i == 0) {
          		signal = snoise2( PP );
          		if ( signal < 0.0 ) signal = -signal;
          		signal = offset - signal;
          		signal = pow( signal, sharpness );
                             /*This should give you a power function to control
                             sharpness of the ridges. Or you can just use the
                             original one -- signal *= signal;*/
          		result = signal;
          		weight = 1.0;
        	}else{
        	
          		exponent = pow( lacunarity, (-i*H) );
			/*
          		PP.x *= lacunarity;
          		PP.y *= lacunarity;
          		PP.z *= lacunarity;
			*/
			PP = PP * lacunarity;
          		/* weigh successive contributions by previous signal */
          		weight = signal * threshold;
          		weight = clamp(weight,0,1)    ;    		
          		signal = snoise2( PP );

          		/* get absolute value of signal*/
          		signal = abs(signal);

          		/* invert and translate*/
          		signal = offset - signal;

          		/* sharpen the ridge*/
          		signal = pow( signal, sharpness ); /* Or signal *= signal;*/

          		/* weight the contribution*/
          		signal *= weight;
          		result += signal * exponent;
       		}
	}

	P+= Nn*result*Kt;
  	N = calculatenormal(P);	
}
