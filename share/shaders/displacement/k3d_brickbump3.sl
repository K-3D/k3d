/*
 * brickbump.sl -- displacement shader for bricks.
 *
 * DESCRIPTION:
 *   Makes displacements for a wall of bricks.  This is the companion
 *   shader to the surface "brick" shader.  The parameters work exactly
 *   the same.  Of course, you can use it with any surface shader, and
 *   in fact matte or plastic gives those nice white cinder block walls.
 *   However, if you do use it with "brick", the parameters MUST match,
 *   or your bricks will look very strange.
 * 
 * PARAMETERS:
 *    brickwidth                Width of a brick (in st space)
 *    brickheight               Height of a brick (in st space)
 *    mortarthickness           Thickness of the mortar (in st space)
 *    rowvary                   How much does each row shift?
 *    jagged                    How much do bricks deviate from squares?
 *    pitting                   The amplitude of the "pits" on the face of
 *                                 the bricks.
 *    pockfrequency             The st frequency of the pits.
 *    groovedepth               The depth of the grooves between bricks.
 *
 * AUTHOR: written by Larry Gritz, 1992
 *
 * HISTORY:
 *      28 May 1992 -- written by lg for the "Timbre Trees" video (saucer)
 *      12 Jan 1994 -- recoded by lg in correct shading language.
 *
 * last modified  12 Jan 1994 by Larry Gritz
 */

/* note from Larry:

  You may note the companion shaders "brick" and "brickbump".

  They're meant to go together, but I like to use the brickbump
  displacement shader with the matte surface shader.  With appropriate
  parameters, it looks *exactly* like those painted cinderblock walls
  (we have a wall in our lab like this).  
  ~gritz/brick.tif.

  I think good parameters for this look are:

  Displacement "brickbump" "brickwidth" 0.5 "brickheight" 0.25 
	       "mortarthickness" 0.02 "pitting" 0.015 "pockfrequency" 12
   	       "groovedepth" 0.015
*/
#include "k3d_functions.h"
#include "k3d_noises.h"

displacement
k3d_brickbump3 ( float jagged = 0.006;
            float brickwidth = .25, brickheight = .08;
	    float mortarthickness = .01;
	    float rowvary = .25, pitting = 0.01;
	    float pockfrequency = 10, groovedepth = 0.01; )
{
#define BMWIDTH (brickwidth+mortarthickness)
#define BMHEIGHT (brickheight+mortarthickness)
#define MWF (mortarthickness*0.5/BMWIDTH)
#define MHF (mortarthickness*0.5/BMHEIGHT)
  point PP2;
  float sbrick, tbrick, w, h;
  float scoord, tcoord, ss, tt;
  float fact, disp;

  scoord = s;  tcoord = t;

  /* Make the shapes of the bricks vary just a bit */
  PP2 = point noise (s/BMWIDTH, t/BMHEIGHT);
  scoord = s + jagged * xcomp (PP2);
  tcoord = t + jagged * ycomp (PP2);

  ss = scoord / BMWIDTH;
  tt = tcoord / BMHEIGHT;

  /* shift alternate rows */
  if (mod (tt*0.5, 1) > 0.5)
      ss += 0.5;

  tbrick = floor (tt);   /* which brick row? */
  /* Shift the columns randomly by row */
  ss += rowvary * (noise (tbrick+0.5) - 0.5);

  sbrick = floor (ss);   /* which brick column? */
  ss -= sbrick;          /* Now ss and tt are coords within the brick */
  tt -= tbrick;

  fact = 1;
  disp = 0;
  if (tt < MHF) {
      /* We're in the top horizontal groove */
      disp = groovedepth * (sqr((tt)/MHF) - 1);
    }
  if (tt > (1.0-MHF)) {
      /* Bottom horizontal groove */
      disp = groovedepth * (sqr((1-tt)/MHF) - 1);
    }
  if (ss < MWF) {
      disp = 0.75 * groovedepth * (sqr(ss/MWF) - 1);
    }
  if (ss > (1.0-MWF)) {
      disp = 0.75 * groovedepth * (sqr((1-ss)/MWF) - 1);
    }

  fact = smoothstep (0, 1.3*MHF, tt) - smoothstep (1.0-1.3*MHF, 1, tt);
  fact *= (smoothstep (0, 1.3*MWF, ss) - smoothstep (1.0-1.3*MWF, 1, ss));
  fact = pitting * (0.75 * fact + 0.25);
  disp -= fact * pow(noise ((ss+sbrick)*pockfrequency/BMHEIGHT,
		            (tt+tbrick)*pockfrequency/BMWIDTH), 0.25);

  P += disp * normalize(N);
  N = calculatenormal (P);

}
