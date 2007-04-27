/* I took wave's lead and renamed starfield to KMPuffyclouds.sl -- tal@cs.caltech.edu */

/* 
 *
 * Original name below 
 * puffyclouds.sl -- RenderMan compatible surface shader for puffy
 *                   clouds.
 *
 * DESCRIPTION:
 *    Makes nice looking cumulous clouds like you would see in the sky
 *    on a bright sunny day.  Works as a basic thresholded fBm.  Since
 *    this texture is generally used as a backdrop, it does not take
 *    lighting into account.  If you wanted a lit surface that looked like
 *    puffy clouds (like painted clouds on a wall), then it would be pretty
 *    easy to add the lighting.
 *
 * PARAMETERS:
 *    txtscale - overall scaling factor
 *    skycolor, cloudcolor - the obvious meanings
 *    octaves, omega, lambda - control the fractal appearance of the clouds
 *    threshold - fBm sum below this level is just blue sky
 *
 * ANTIALIASING:
 *    None, but should be easy to add antialiasing simply by adaptively
 *    setting the "octaves" parameter based on distance from eye point.
 *
 * AUTHOR:
 *    C language version by F. Kenton Musgrave
 *    Translation to RenderMan Shading Language by Larry Gritz.
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 *
 * HISTORY:
 *    ??? - original C language version by Ken Musgrave
 *    Apr 94 - translation to Shading Language by L. Gritz
 *
 * this file last updated 18 Apr 1994
 *
 */


#include "k3d_noises.h"

surface 
k3d_mysky (float Ka = 0, Kd = 0;
	     float txtscale = 1;
	     color skycolor = color(.15, .15, .6);
	     color cloudcolor = color(1,1,1);
	     float octaves = 8, omega = 0.5, lambda = 2;
	     float threshold = 0;
	     )
{
  float value;
  color Ct;      /* Color of the surface */
  point PP;      /* Surface point in shader space */
  float i, a, l, o;

  PP = txtscale * transform ("shader", P);

  /* Use fractional Brownian motion to compute a value for this point */
  /*  value = fBm (PP, omega, lambda, octaves); */
  value = 0;
  l = 1;  o = 1;  a = 0;
  for (i = 0;  i < octaves;  i += 1) {
      a += o * snoise (PP*l);
      l *= 2;  o *= omega;
    }
  value = a;

  Ct = mix (skycolor, cloudcolor, smoothstep (threshold, 1, value));

  /* Shade like matte, but use color Ct */
  Oi = 1;  
  Ci = Ct;  
}

