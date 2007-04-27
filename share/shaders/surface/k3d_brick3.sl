/*
 * brick.sl -- Surface shader for a bricks.
 *
 * DESCRIPTION:
 *   Makes a wall of bricks.  Need more be said?  OK.  It makes a good
 *   looking staggered brick masonry.  It is especially convincing when
 *   used in conjunction with the "brickbump" displacement shader (and
 *   identical parameters).  Every other row of bricks is staggered.
 *   The staggering isn't exact, however, and this variance is controlled
 *   by the "rowvary" parameter.
 * 
 * PARAMETERS:
 *    Ka, Kd			The usual
 *    brickcolor, mortarcolor	Pretty obvious (default is red bricks)
 *    brickvary                 How much does the brick color vary from
 *     				   brick to brick?
 *    brickwidth                Width of a brick (in st space)
 *    brickheight               Height of a brick (in st space)
 *    mortarthickness           Thickness of the mortar (in st space)
 *    rowvary                   How much does each row shift?
 *    jagged                    How much do bricks deviate from squares?
 *
 * AUTHOR: written by Larry Gritz, 1992
 * 	This shader is very similar to (and based upon) brick shaders
 *	by Darwyn Peachey.
 *
 * 
 * HISTORY:
 *      28 May 1992 -- written by lg for the "Timbre Trees" video (saucer)
 *      12 Jan 1994 -- recoded by lg in correct shading language.
 *
 * last modified  12 Jan 1994 by Larry Gritz
 */
#include "k3d_noises.h"


surface
k3d_brick3 ( float Ka = 1, Kd = 1;
	color brickcolor = color "rgb" (.6,.1,.1);
	color mortarcolor = color "rgb" (.6,.6,.6);
        float jagged = 0.006, brickvary = 0.3;
        float brickwidth = .25, brickheight = .08;
        float mortarthickness = .01;
        float rowvary = .25; )
{
#define BMWIDTH (brickwidth+mortarthickness)
#define BMHEIGHT (brickheight+mortarthickness)
#define MWF (mortarthickness*0.5/BMWIDTH)
#define MHF (mortarthickness*0.5/BMHEIGHT)
#define boxstep(a,b,x) (clamp(((x)-(a))/((b)-(a)),0,1))
#define MINFILTERWIDTH 1.0e-7
  color bcolor, Ct;
  point PP2, Nf;
  float sbrick, tbrick, w, h;
  float scoord, tcoord, ss, tt;
  float swidth, twidth;
  float Nfactor;

  /* Determine how wide in s-t space one pixel projects to */
  swidth = max (abs(Du(s)*du) + abs(Dv(s)*dv), MINFILTERWIDTH);
  twidth = max (abs(Du(t)*du) + abs(Dv(t)*dv), MINFILTERWIDTH);

  Nf = faceforward (normalize(N),I);

  /* Make the shapes of the bricks vary just a bit */
  PP2 = point noise (s/BMWIDTH, t/BMHEIGHT);
  scoord = s + jagged * xcomp (PP2);
  tcoord = t + jagged * ycomp (PP2);

  ss = scoord / BMWIDTH;   /* Determine which brick the point is in */
  tt = tcoord / BMHEIGHT;  /*                   "                   */
  swidth /= BMWIDTH;
  twidth /= BMHEIGHT;

  /* shift alternate rows */
  if (mod (tt*0.5, 1) > 0.5)
      ss += 0.5;

  tbrick = floor (tt);   /* which brick row? */
  /* Shift the columns randomly by row */
  ss += rowvary * (noise (tbrick+0.5) - 0.5);

  sbrick = floor (ss);   /* which brick column? */
  ss -= sbrick;          /* Now ss and tt are coords within the brick */
  tt -= tbrick;

  /* Choose a color for the surface */
  if (swidth >= 1)
      w = 1 - 2*MWF;
  else w = clamp (boxstep(MWF-swidth,MWF,ss), max(1-MWF/swidth,0), 1)
	 - clamp (boxstep(1-MWF-swidth,1-MWF,ss), 0, 2*MWF/swidth);

  if (twidth >= 1)
      h = 1 - 2*MHF;
  else h = clamp (boxstep(MHF-twidth,MHF,tt), max(1-MHF/twidth,0),1)
	 - clamp (boxstep(1-MHF-twidth,1-MHF,tt), 0, 2*MHF/twidth);

  /* Choose a brick color that varies from brick to brick */
  bcolor = brickcolor * (1 + (brickvary * snoise (tbrick+(100*sbrick)+0.5)));

  Ct = mix (mortarcolor, bcolor, w*h);

  Oi = Os;
  Ci = Os * Ct * (Ka * ambient() + Kd*diffuse(Nf));
}
