/*
 * checks_aa.sl -- RenderMan compatible shader for checks.
 *
 * DESCRIPTION:
 *   Makes a checkered surface, fully antialiased!  This sucker should
 *   look great even at only one sample per pixel.
 * 
 * PARAMETERS:
 *   Ka, Kd	    	work just like the matte shader
 *   color1, color2     these are the colors which make the pattern
 *   frequency		determines the frequency (in s-t space) of the checks
 *
 *
 * AUTHOR: written by Larry Gritz
 *
 * HISTORY:
 *      27 Jan 1994 -- written by lg
 *
 * last modified 27 Jan 1994 by Larry Gritz
 */

surface
k3d_antialiasedchecks (float Ka = 1, Kd = 1, frequency = 10;
	   color color1 = 0, color2 = 1; )
{
  point Nf;             /* Forward facing surface normal */
  float smod, tmod;     /* Texture position within the pattern */
  color checkcolor;     /* Color of the checks */
  float x, y;           /* Used to determine pattern */
  float swidth, twidth, sfuzz, tfuzz;  /* Antialiasing */
  float Nfactor;        /* Multiplicative factor for AA due to normal */
  float fuzzmax;        /* max of (sfuzz, tfuzz) */

  Nf = faceforward (normalize(N), I);

  /* Determine how wide in s-t space one pixel projects to */
  swidth = abs(Du(s)*du) + abs(Dv(s)*dv);
  twidth = abs(Du(t)*du) + abs(Dv(t)*dv);

  /* Figure out amount of fuzziness, taking normal into account */
  Nfactor = abs (Nf . I) / (length(Nf) * length(I));
  sfuzz = .5 * swidth * frequency / Nfactor;
  tfuzz = .5 * twidth * frequency / Nfactor;
  fuzzmax = max (sfuzz, tfuzz);

  /* Get the place in the pattern where we're sampling */
  smod = mod (s*frequency, 1);
  tmod = mod (t*frequency, 1);

  /* If the filter width is small enough, compute the pattern color */
  if (fuzzmax <= 0.5) {
      x = ((smoothstep (.5,.5+sfuzz,smod)) + (1 - smoothstep (0,sfuzz,smod)));
      y = ((smoothstep (.5,.5+tfuzz,tmod)) + (1 - smoothstep (0,tfuzz,tmod)));
      checkcolor = mix (color1, color2, x*y + (1-x)*(1-y));
      /* Gradually fade in the average color when we get close to the limit */
      Ci = mix (checkcolor, (color1+color2)/2, smoothstep (.125, .5, fuzzmax));
    }
  else { /* otherwise, only use the average color */
      Ci = (color1 + color2) / 2;
    }

  /* Use the matte reflectance formula */
  Oi = Os;
  Ci *= Os * (Ka*ambient() + Kd*diffuse(Nf));
}

