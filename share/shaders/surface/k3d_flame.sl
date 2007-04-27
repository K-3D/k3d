/*
 * flame.sl -- RenderMan compatible surface shader for a flame-like texture.
 *
 * DESCRIPTION:
 *    Makes something that looks like fire.
 *
 * PARAMETERS:
 *    distortion - 
 *    chaosscale, chaosoffset, octaves - control the fBm
 *    flameheight, flameamplitude - scaling factors
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
 */

#include "k3d_noises.h"

surface k3d_flame(float distortion = 0;
		  float chaosscale = 1; 
		  float chaosoffset = 0;
		  float octaves = 7; 
		  float flameoffset = 0.0;
		  float flameamplitude = 2.0; 
		  float phase = 0.0
		  )
{
  point PP, PQ;
  float freq;
  float chaos, i, cmap;

  PP = point(s, t, phase);
  PQ = PP;
  PQ *= point(1, 1, exp(-ycomp(PP)));
  freq = 1;
  chaos = 0;
  for(i = 0; i < octaves; i += 1)
    {
      chaos += VLNoise(freq * PQ, distortion) / freq;
      freq *= 2;
    }

  chaos = abs(chaosscale * chaos + chaosoffset);
  cmap = 0.85 * chaos + flameoffset + (flameamplitude * ycomp(PP));
	color theflames = spline(cmap, 	color(0, 0, 0), \
		  														color(0, 0, 0), \
		  														color(27, 0, 0), \
		  														color(54, 0, 0), \
		  														color(81, 0, 0), \
		  														color(109, 0, 0), \
		  														color(136, 0, 0), \
		  														color(166, 5, 0), \
		  														color(189, 30, 0), \
																	color(211, 60, 0), \
		  														color(231, 91, 0), \
		  														color(238, 128, 0), \
		  														color(244, 162, 12), \
		  														color(248, 187, 58), \
		  														color(251, 209, 115), \
		  														color(254, 236, 210), \
		  														color(255, 241, 230), \
																	color(255, 241, 230));
  Ci = Cs * theflames / 255;

  Oi = 0.001;
}
