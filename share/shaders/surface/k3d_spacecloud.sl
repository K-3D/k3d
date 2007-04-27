/*
 * TLSpaceCloud.sl - perform turbulence function to add more dimension to
 *    texture-map and try to make it not so obvious that it is a texture-map.
 *
 *
 * DESCRIPTION:
 *   Uses a pulse function to tapper off the edges of the texture
 *
 * PARAMETERS:
 *   txtFile -- texture map
 *   startPulse -- start of pulse function.
 *   endPulse -- end of pulse function
 *   fuzz -- amount to blur the edges of the pulse
 *   minAdjust -- amount that can be subtracted from value
 *   maxAdjust -- amount that can be added to the value
 *   maxOpacity -- maximin opacity for the surface
 *
 * HINTS:
 *  Only tested on rectanglar patch.
 *
 * AUTHOR: Tal Lancaster
 *
 *
 * HISTORY:
 *  Created: 6/1/95
 *  
 *  tal 3/2/97  -- Cleaned up code, removed many constants, added comments
 *  tal 2/23/97 -- Originally tried using fBm to create turbulence.  But
 *      I was never happy with the results.  So now am just using noise over
 *      u,v. 
 */
#include "k3d_noises.h"
#include "k3d_functions.h"

#define  MINFREQ 1.1
#define MAXFREQ 6




surface k3d_spacecloud(
  string txtFile = "";
  float startPulse = .2; /* .1 .2 .3 .01 */
  float endPulse = .9; /* .9 .8 .7 .8 */
  float afuzz = .1;
  float bfuzz = .2;
  float minAdjust = -.4;
  float maxAdjust = .4;
  float maxOpacity = .4;
)
{
  float value = 0;
  float f;
  color Ct;
  point PP;
  float freq, i, size;
  float adjust;
  float ss, tt;
  

  if (txtFile != "")
    Ct = color texture (txtFile, s, 1-t);
  else
    Ct = color (.3, .3, .3);

	PP = transform ("object", P);
  /* fractalsum */
  for (f = MINFREQ; f < MAXFREQ; f += 1)
    value += abs(snoise (PP * f))/f;

#if 0
#define  MINFREQ 1.1
#define MAXFREQ 6
	/* Old way */

	fBm (P, noiseScale, octaves, PP, freq, i, size, adjust);
	/*printf ("%.3f %.3f: adjust %.3f\n", s, t, adjust);*/
	
#else
	adjust = adjustNoise (u, v, minAdjust, maxAdjust);
	ss = s + adjust;
	tt = t + adjust;
	/* printf ("%.3f %.3f: %.3f %.3f\n", s, t, ss-s, tt-t); */
	
#endif
	Oi = value * smoothPulse2Fuzz (startPulse, endPulse, afuzz, afuzz, ss) * 
		smoothPulse2Fuzz (startPulse, endPulse, bfuzz, afuzz, tt);

	Oi *= maxOpacity;
	Ci = Ct * Oi * 1.75  /* saturate colors */;
}
