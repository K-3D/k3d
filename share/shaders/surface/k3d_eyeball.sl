/*
 * eyeball.sl -- RenderMan compatible shader for an eyeball.
 *
 * DESCRIPTION:
 *   Makes a plastic-like surface which looks like an eyeball.  It's meant
 *   for use on a sphere.  The center of the pupil is at the "north pole",
 *   i.e. where the t parameter is 1.  The colors of the pupil, iris, white
 *   part (eyeball), and blood vessels can be set individually.  Fractal
 *   functions are used for the veining and the iris mottling.
 * 
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness, specularcolor - work just like the plastic shader
 *   iriscolor - color of the iris
 *   eyeballcolor - color of the white part of the eyeball
 *   bloodcolor - color of the blood vessels
 *   pupilcolor - color of the pupil (opening)
 *   pupilsize - size of pupil (in "t" space)
 *   irissize - size of iris (in "t" space), must be larger than pupilsize
 *   bloodshot - controls how bloodshot the eye is (0=no blood, 1=very ugly)
 *   veinfreq, veinlevel - control the formation of the blood vessels
 *   index - set between 0 and 1, lets you use this shader to generate
 *           non-identical eyeballs.
 *
 * ANTIALIASING: basic antialiasing of the boundaries between tissue types
 *               is performed.
 *
 * AUTHOR: written by Larry Gritz
 *
 * HISTORY:
 *      Nov 1991 - crude written of "eye" by lg for Herman's eyes for
 *                 "Graphic Violence".  Original version hard coded in C.
 *      Dec 1993 - "eye" modified by lg to clean up a bit.
 *      10 Jan 1994 - recoded by lg in correct shading language.
 *      28 Jun 94 (lg) - revamped to add veins and iris mottling, renamed
 *                       "eyeball"
 *       7 Jan 95 (wave) - changed name to LGEyeBall for namespace reasons...
 *       8 Jan 95 (wave) - changed Ciris line to fix bug Larry figured out and changed defaults
 *       27 Feb 95 (wave) - changed PO line to fix bug Larry figured out to actually *use* index
 *
 * last modified  8 Jan 95 by Michael B. Johnson (wave)
 */

#include "k3d_noises.h" 
#include "k3d_rmannotes.h"

surface
k3d_eyeball (float Ka = .75, Kd = 0.75, Ks = 0.4, roughness = 0.1;
	 color specularcolor = 1;
	 color iriscolor = color (.135289, .084323, .372417);
	 color irisoutercolor = color (.403882, .343944, .68276);
	 color irisinnercolor = color (.065142, .040605, .179311);
	 color eyeballcolor = color(1,1,1);
	 color bloodcolor = color(.8,.05,.05);
	 color pupilcolor = 0;
	 float pupilsize = 0.05, irissize = 0.12;
	 float bloodshot = 1.0;
	 float veinfreq = 8, veinlevel = 4;
	 float index = 0;
        )
{
  color Ct;
  point Nf;
  point PP, PO;
  float i, turb, newturb, freq, f2;
  float displayed, newdisp;
  color Cball, Ciris;
  float irisstat, pupilstat;
  float bloody, tt;
  float ks, rough;
  float twidth, cutoff;

  /* Calculate an appropriate filter width for antialiasing */
  twidth = max (abs(Du(t)*du) + abs(Dv(t)*dv), MINFILTERWIDTH);
  PO = transform ("object", P) + index;

  /* Figure out where we are in the eyeball.  Use the following variables:
   * irisstat: 0 inside the iris/white boundary, 1 outside
   * pupilstat: 0 inside the pupil/iris boundary, 1 outside
   * bloody: how potentially bloody it is (fade as we get away from iris)
   */
  tt = 1-t;
  irisstat = smoothstep (irissize, irissize+twidth, tt);
  pupilstat = smoothstep (pupilsize, pupilsize+twidth, tt);
  bloody = bloodshot * (smoothstep (-irissize, 2.5*irissize, tt));

  /* If we're somewhere in the white part and it's potentially bloody,
   * then calculate the veining pattern.  Otherwise, just use the color
   * of the whites.  The veining pattern is essentially summed zero sets
   * of turbulence functions.  Some stretching is done to get it to look
   * just right.
   */
  if (irisstat * bloody > 0.001) {
      turb = bloody;  freq = veinfreq;
      displayed = 0;
      for (i = 1;  (i <= veinlevel) && (turb > 0.1);  i += 1) {
	  newturb = 1 - abs (snoise(PO*freq + point(0,0,20*freq)));
	  newdisp = pow (smoothstep (.85, 1, newturb), 10);
	  displayed += (1-displayed) * newdisp * smoothstep (.1, .85, turb * turb);
	  turb *= newturb;
	  freq *= 2;
        }
      Cball = mix (eyeballcolor, bloodcolor, smoothstep(0,.75,displayed));
    }
  else Cball = eyeballcolor;

  Ciris = mix (iriscolor, irisoutercolor, smoothstep (irissize*.8, irissize, tt));
  /* If we're somewhere in the iris, calculate the iris pattern, which is
   * just a stretched turbulence function.
   */
  if (irisstat < 0.9999 && pupilstat > 0.0001) {
      turb = 0;  freq = 1;  f2 = 30;
      for (i = 1;  i <= 4;  i += 1) {
	  turb += snoise (PO*f2 + point(0,0,20*f2)) / freq;
	  freq *= 2;  f2 *= 2;
        }
      Ciris *= (1-clamp(turb/2,0,1));
    }

  /* OK, now calculate a surface texture color (Ct) based on where we are
   * and what patterns we calculated.
   */
  Ct = mix (Ciris, Cball, irisstat);
  Ct = mix (pupilcolor, Ct, pupilstat);

  /* Make the eye a little glossier on the iris and pupil */
  ks = Ks * (1+2*(1-irisstat));
  rough = roughness * (1-.75*(1-irisstat));

  /* Now shade like plastic, but using our calculated surface color and
   * our modified values for roughness and Ks.
   */
  Oi = Os;
  Nf = faceforward (normalize(N),I);
  Ci = Os * ( Ct * (Ka*ambient() + Kd*diffuse(Nf)) +
	      specularcolor * ks*specular(Nf,-normalize(I),rough));
}

