/*
 * screen_aa.sl -- RenderMan compatible shader for a metalic screen.
 *
 * DESCRIPTION:
 *   Makes a surface that looks like a metal screen.  Strips of metal run
 *   parallel to lines of s and t.  You can adjust the Ka, Kd, Ks, etc.
 *   to change the material appearance.  This texture antialiases pretty
 *   well, even with only one sample per pixel.
 *
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness, specularcolor - work just like the plastic shader
 *   frequency - how many cycles of screen in st space
 *   density - how much of each cycle is opaque?
 *
 * AUTHOR: written by Larry Gritz
 *
 * last modified  31 Jan 1994 by Larry Gritz
 *
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

#include "k3d_rmannotes.h"



surface k3d_screen_aa(float Ka = 1, Kd = 0.75, Ks = 0.4, roughness = 0.1;
		      color specularcolor = 1;
		      float density = 0.25, frequency = 20;)
{
  normal Nf;			/* Forward facing Normal vector */
  vector IN;			/* normalized incident vector */
  float d;			/* Density at the sample point */
  float ss, tt;			/* s,t, parameters in phase */
  float swidth, twidth, GWF, w, h;

  /* Compute a forward facing normal */
  IN = normalize(I);
  Nf = faceforward(normalize(N), I);

  /* Determine how wide in s-t space one pixel projects to */
  swidth = max(abs(Du(s) * du) + abs(Dv(s) * dv), MINFILTERWIDTH) * frequency;
  twidth = max(abs(Du(t) * du) + abs(Dv(t) * dv), MINFILTERWIDTH) * frequency;

  /* Figure out where in the pattern we are */
  ss = mod(frequency * s, 1);
  tt = mod(frequency * t, 1);

  /* Figure out where the strips are. Do some simple antialiasing. */
  GWF = density * 0.5;
  if(swidth >= 1)
    w = 1 - 2 * GWF;
  else
    w =
      clamp(boxstep(GWF - swidth, GWF, ss), max(1 - GWF / swidth, 0),
	    1) - clamp(boxstep(1 - GWF - swidth, 1 - GWF, ss), 0,
		       2 * GWF / swidth);
  if(twidth >= 1)
    h = 1 - 2 * GWF;
  else
    h =
      clamp(boxstep(GWF - twidth, GWF, tt), max(1 - GWF / twidth, 0),
	    1) - clamp(boxstep(1 - GWF - twidth, 1 - GWF, tt), 0,
		       2 * GWF / twidth);
  /* This would be the non-antialiased version:
   *    w = step (GWF,ss) - step(1-GWF,ss);
   *    h = step (GWF,tt) - step(1-GWF,tt);
   */
  d = 1 - w * h;

  Oi = d;
  if(d > 0)
    {
      Ci =
	Oi * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) +
	      specularcolor * Ks * specular(Nf, -IN, roughness));
    }
  else
    Ci = 0;
}
